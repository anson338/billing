#pragma once
#include "logger.hpp"
#include "server_config.hpp"
#include <asio.hpp>
#include <mysql.h>
#include <map>
#include <memory>
#include <vector>
#include "request_handler.hpp"
using asio::ip::tcp;

class BillingServer
{
public:
	typedef void(*reqHandler)(tcp::socket& client,std::shared_ptr<std::vector<char>> response, const asio::error_code& ec);
	BillingServer();
	BillingServer(bool mask);
	~BillingServer();
	void run();
	void stop();
#ifdef OPEN_SERVER_DEBUG
	void sendTestData();
#endif
	friend class ClientConnection;
private:
	ServerConfig config;
	asio::io_service ioService;
	tcp::endpoint serverEndpoint;
	std::shared_ptr<tcp::acceptor> acceptor;
	std::shared_ptr<MYSQL> mysql;
	bool stopMask;
	void startAccept();
	bool testConnect();
	std::map<unsigned char, std::shared_ptr<RequestHandler>> handlers;
	void sendClientRequest(tcp::socket& socket, std::vector<char>& dataBytes,reqHandler respHandler);
#ifdef OPEN_SERVER_DEBUG
	std::shared_ptr<tcp::socket> proxySocket;
#endif
};