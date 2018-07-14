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
	typedef void(*reqHandler)(std::shared_ptr<std::vector<char>> response);
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
	void BillingServer::syncConnect(tcp::endpoint& remotePoint, tcp::socket& socket, asio::error_code& ec);
	void sendClientRequest(tcp::socket& socket, asio::error_code& ec,std::vector<char>& dataBytes, reqHandler respHandler);
};