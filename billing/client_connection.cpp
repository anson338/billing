#include "inc/client_connection.hpp"
#include "inc/billing_server.hpp"
#include <hex_tool.hpp>
using asio::ip::tcp;
using std::vector;
using std::string;

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
	Logger::write("client connected");
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
		Logger::write("stop server read");
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
			if (error == asio::error::eof) {
				Logger::write(string("client disconnect"));
			}
			else {
				Logger::write(string("read client error: ") + error.message());
			}
#endif
		}
		else {
			//开启下一次读取
			this->readFromClient();
			if (size > 0) {
#ifdef OPEN_SERVER_DEBUG
				string inputDebugStr;
				bytesToHexDebug(request->begin(), request->begin() + size, inputDebugStr);
				Logger::write("client request");
				Logger::write(inputDebugStr);
#endif
				this->processRequest(request, size);
			}
		}
	});

}

void ClientConnection::processRequest(std::shared_ptr<vector<char>> request, std::size_t size)
{
	//加入缓冲区
	this->cacheBuffer.insert(this->cacheBuffer.end(), request->begin(), request->begin() + size);
	//判断是否为命令
	if (this->cacheBuffer.size() >= 4) {
		string commandHex;
		bytesToHex(this->cacheBuffer.begin(), this->cacheBuffer.begin() + 4, commandHex);
		//close命令
		if (commandHex.compare("00000000") == 0) {
			//响应close命令
			auto selfPointer(shared_from_this());
			asio::async_write(socket,
				asio::buffer("---"),
				[this, selfPointer](const asio::error_code& error, std::size_t size) {
				if (!error) {
					//
					this->server->stopMask = true;
					Logger::write("get command : stop");
					this->server->ioService.stop();
					Logger::write("billing server stoped");
				}
				this->writeHandler(error, size);
			});
			return;
		}
	}
	BillingData requestData(this->cacheBuffer);
	string hexStr;
	while (requestData.isDataValid()) {

		unsigned char requestType = requestData.getPayloadType();
		auto it = server->handlers.find(requestType);
		if (it != server->handlers.end()) {
			this->processRequest(it->second, requestData);
		}
		else {
			vector<char> payloadTypeBytes(1, requestType);
			bytesToHex(payloadTypeBytes, hexStr);
			Logger::write(string("[error]unkown BillingData type: 0x") + hexStr);
			requestData.doDump(hexStr);
			Logger::write(hexStr);
#ifdef OPEN_SERVER_DEBUG
#ifdef OPEN_PROXY_DEBUG
			this->callProxyServer(request, size, requestData, [this](std::shared_ptr<std::vector<char>> response, const asio::error_code& ec) {

				if (ec) {
					Logger::write(string("send proxy data failed: ") + ec.message());
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
					//调用proxy处理
					asio::async_write(socket,
						asio::buffer(*response),
						[this](const asio::error_code& error, std::size_t size) {
						this->writeHandler(error, size);
					});
				}

			});
#else
			Logger::write("can not process type : 0x" + hexStr);
			requestData.doDump(hexStr);
			Logger::write(hexStr);
#endif
#endif
		}
		requestData = BillingData(this->cacheBuffer);
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
void ClientConnection::callProxyServer(std::shared_ptr<std::vector<char>> request, std::size_t size, BillingData & requestData, proxyRespHandler respHandler)
{
	string proxyDebugStr;
	requestData.doDump(proxyDebugStr);
	Logger::write("===send data to proxy===");
	Logger::write(proxyDebugStr);
	this->server->sendClientRequest(*(this->server->proxySocket), *request, size, respHandler);
}
#endif
#endif
