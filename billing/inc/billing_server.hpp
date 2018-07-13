#pragma once
#include "server_config.hpp"
#include "logger.hpp"
#include <asio.hpp>
#include <mysql.h>
using asio::ip::tcp;
#include <map>
#include <memory>
#include "request_handler.hpp"

class BillingServer
{
public:
	BillingServer();
	~BillingServer();
	void run();
	void stop();
#ifdef OPEN_SERVER_DEBUG
	void sendTestData();
	void readTestData(tcp::socket& socket);
#endif
	friend class ClientConnection;
private:
	ServerConfig config;
	asio::io_service ioService;
	tcp::endpoint serverEndpoint;
	tcp::acceptor acceptor;
	MYSQL mysql;
	bool stopMask;
	void startAccept();
	bool testConnect();
	std::map<unsigned char, std::shared_ptr<RequestHandler>> handlers;
};