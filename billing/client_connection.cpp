#include "inc/client_connection.hpp"
#include "inc/billing_server.hpp"

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
	else {
		this->readFromClient();
	}
}

void ClientConnection::readFromClient()
{
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
		//¶ÁÈ¡´íÎó
		if (error && (error != asio::error::eof)) {
#ifdef OPEN_SERVER_DEBUG
			Logger::write(string("read client error ")+error.message());
#endif
		}
		else {
#ifdef OPEN_SERVER_DEBUG
			Logger::write(string("request data\r\n") + *request);
#endif
			//·µ»ØÏìÓ¦
			this->writeResponse();
		}
	});

}

void ClientConnection::writeResponse()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("write client start");
#endif
	auto selfPointer(shared_from_this());
	auto resp = std::make_shared<string>("HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 11\r\n"
		"Connection: keep-alive\r\n"
		"Server: liuguang/server\r\n\r\n"
		"hello world");
	socket.async_send(
		asio::buffer(*resp),
		[this, selfPointer, resp](const asio::error_code& error, std::size_t size) {
		Logger::write("write client end");
		Logger::write(string("response data\r\n") + *resp);
		this->writeHandler(error, size);
	}
	);
}