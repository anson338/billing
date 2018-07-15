#include "inc/client_connection.hpp"
#include "inc/billing_server.hpp"
#include <iostream>
#include "inc/hex_tool.hpp"
using std::cout;
using std::endl;
#include <set>

ClientConnection::ClientConnection(BillingServer * s, asio::io_service& io) :server(s), socket(io) {
#ifdef OPEN_SERVER_DEBUG
	Logger::write("client construct");
#endif
}

ClientConnection::~ClientConnection()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("client destroy");
#endif
}

tcp::socket & ClientConnection::getSocket()
{
	return socket;
}
void ClientConnection::acceptHandler(const asio::error_code & error)
{

#ifdef OPEN_SERVER_DEBUG
	Logger::write("connection entered");
	Logger::write("prepare accept new");
#endif
	server->startAccept();
	if (error) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write(string("accept error ") + error.message());
#endif
	}
	else {
#ifdef OPEN_SERVER_DEBUG
#endif
		string clientIp = socket.remote_endpoint().address().to_string();
		std::set<string> allowIps = this->server->config.getAllowIps();
		if (!allowIps.empty()) {
			//判断IP是否在白名单内
			if (allowIps.find(clientIp) == allowIps.end()) {
				socket.close();
#ifdef OPEN_SERVER_DEBUG
				Logger::write(string("ip: ") + clientIp + " is not allowed to connect to billing server");
#endif
				return;
			}
		}
		socket.set_option(asio::socket_base::keep_alive(true));
		this->readFromClient();
	}
}

void ClientConnection::writeHandler(const asio::error_code & error, std::size_t size)
{
	if (error) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write(string("write error ") + error.message());
#endif
	}
}

void ClientConnection::readFromClient()
{
	if (this->server->stopMask) {
		cout << "stop server read" << endl;
	}
	auto request = std::make_shared<vector<char>>(260);
	request->resize(260);
	auto selfPointer(shared_from_this());
	socket.async_receive(
		asio::buffer(*request),
		[this, selfPointer, request](const asio::error_code& error, std::size_t size) {
		//读取错误
		if (error) {
#ifdef OPEN_SERVER_DEBUG
			Logger::write(string("read client error: ") + error.message());
#endif
		}
		else {
			//开启下一次读取
			this->readFromClient();
			if (size > 0) {
				if (size < request->capacity()) {
					//移除尾部多余空间
					request->resize(size);
				}
#ifdef OPEN_SERVER_DEBUG
				//string inputDebugStr;
				//bytesToHexDebug(*request, inputDebugStr);
				//Logger::write("client request");
				//Logger::write(inputDebugStr);
#endif
				this->processRequest(request, size);
			}
		}
	});

}

void ClientConnection::processRequest(std::shared_ptr<vector<char>> request, std::size_t size)
{
	//判断是否为命令
	if (request->size() == 4) {
		string commandHex;
		bytesToHex(*request, commandHex);
		//close命令
		if (commandHex.compare("00000000") == 0) {
			//响应close命令
			auto selfPointer(shared_from_this());
			asio::async_write(socket,
				asio::buffer(*request),
				[this, selfPointer](const asio::error_code& error, std::size_t size) {
				if (!error) {
					//
					this->server->stopMask = true;
					cout << "get command : stop" << endl;
					this->server->ioService.stop();
				}
				this->writeHandler(error, size);
			});
			return;
		}
	}
	BillingData requestData(*request);
	if (requestData.isDataValid()) {

		unsigned char requestType = requestData.getPayloadType();
		string hexStr;
		vector<char> payloadTypeBytes(1, requestType);
		bytesToHex(payloadTypeBytes, hexStr);
		auto it = server->handlers.find(requestType);
		if (it != server->handlers.end()) {
			this->processRequest((*it).second, requestData);
		}
		else {
			cout << "[error]unkown BillingData type: 0x" << hexStr << endl;
#ifdef OPEN_SERVER_DEBUG
#ifdef OPEN_PROXY_DEBUG
			this->callProxyServer(request, requestData);
#else
			Logger::write("can not process type : 0x" + hexStr);
			requestData.doDump(hexStr);
			Logger::write(hexStr);
#endif
#endif
		}
	}
	else {
		cout << "not valid BillingData" << endl;
	}
}

void ClientConnection::processRequest(std::shared_ptr<RequestHandler> handler, BillingData & requestData)
{
	BillingData responseData;
	handler->processRequest(requestData, responseData);
	if (responseData.isDataValid()) {
		auto selfPointer(shared_from_this());
		auto resp = std::make_shared<vector<char>>();
		responseData.packData(*resp);
		asio::async_write(socket,
			asio::buffer(*resp),
			[this, selfPointer, resp](const asio::error_code& error, std::size_t size) {
			this->writeHandler(error, size);
		});
	}
}

#ifdef OPEN_SERVER_DEBUG
#ifdef OPEN_PROXY_DEBUG
void ClientConnection::callProxyServer(std::shared_ptr<vector<char>> request, BillingData& requestData) {
	string proxyDebugStr;
	requestData.doDump(proxyDebugStr);
	Logger::write("===send data to proxy===");
	Logger::write(proxyDebugStr);
	this->server->sendClientRequest(*(this->server->proxySocket), *request, [](tcp::socket& client, std::shared_ptr<std::vector<char>> response, const asio::error_code& ec) {
		if (ec) {
			cout << "send proxy data failed: " << ec.message() << endl;
		}
		else {
			Logger::write("===get proxy data===");
			BillingData proxyData(*response);
			string proxyDebugStr1;
			proxyData.doDump(proxyDebugStr1);
			Logger::write(proxyDebugStr1);
			Logger::write("===full data========");
			bytesToHexDebug(*response, proxyDebugStr1);
			Logger::write(proxyDebugStr1);
			Logger::write("===end proxy data===");
		}
	});
}
#endif
#endif
