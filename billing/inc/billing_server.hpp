#pragma once
#include "server_config.hpp"
#include "logger.hpp"
#include <asio.hpp>
#include <mysql.h>
using asio::ip::tcp;

class BillingServer
{
public:
	BillingServer();
	~BillingServer();
	void run();
	void stop();
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
};