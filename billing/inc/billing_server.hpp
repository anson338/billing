#pragma once
#include "server_config.hpp"
#include "logger.hpp"
#include <asio.hpp>
using asio::ip::tcp;

class BillingServer
{
public:
	BillingServer();
	~BillingServer();
	void run();
	friend class ClientConnection;
private:
	ServerConfig config;
	asio::io_service ioService;
	tcp::acceptor acceptor;
	void startAccept();
};