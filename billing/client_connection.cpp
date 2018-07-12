#include "inc/client_connection.hpp"
#include "inc/billing_server.hpp"
#include <iostream>
#include "inc/hex_tool.hpp"
using std::cout;
using std::endl;

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
#ifdef OPEN_SERVER_DEBUG
	Logger::write("read client start");
#endif
	auto request = std::make_shared<string>();
	request->resize(500);
	auto selfPointer(shared_from_this());
	socket.async_receive(
		asio::buffer(*request),
		[this, selfPointer, request](const asio::error_code& error, std::size_t size) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("read client end");
#endif
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
				//移除尾部多余空间
				request->resize(size);
				this->processRequest(request, size);
			}
		}
	});

}

void ClientConnection::processRequest(std::shared_ptr<string> request, std::size_t size)
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write(string("request data[length:") + std::to_string(size) + "]\r\n<<" + *request + ">>");
	string requestStr;
	charsToHex(request->c_str(), requestStr);
	std::size_t hexLength = requestStr.length(), i;
	string hexDebug(29, '=');
	for (i = 0; i < hexLength; i++) {
		if (i % 2 == 0) {
			if (i % 20 == 0) {
				hexDebug += "\r\n";
			}
			else {
				hexDebug += " ";
			}
		}
		hexDebug.append(1, requestStr[i]);
	}
	hexDebug += "\r\n";
	hexDebug += string(29, '=');
	Logger::write(hexDebug);
#endif
	//判断是否为命令
	if (request->compare("stop") == 0) {
		this->server->stopMask = true;
		cout << "get command : stop" << endl;
		this->server->ioService.stop();
		return;
	}
	auto selfPointer(shared_from_this());
	auto resp = std::make_shared<string>("HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 11\r\n"
		"Connection: keep-alive\r\n"
		"Server: liuguang/server\r\n\r\n"
		"hello world");
#ifdef OPEN_SERVER_DEBUG
	Logger::write("write client start");
#endif
	socket.async_send(
		asio::buffer(*resp),
		[this, selfPointer, resp](const asio::error_code& error, std::size_t size) {

#ifdef OPEN_SERVER_DEBUG
		Logger::write("write client end");
		Logger::write(string("response data\r\n") + *resp);
#endif
		this->writeHandler(error, size);
	}
	);
}