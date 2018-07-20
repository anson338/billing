#pragma once
#include "logger.hpp"
#include "server_config.hpp"
#include <asio.hpp>
#include <mysql.h>
#include <map>
#include <memory>
#include <vector>
#include <functional>
#include "request_handler.hpp"

class BillingServer
{
public:
	typedef std::function<void(std::shared_ptr<std::vector<char>> response, const asio::error_code& ec)> reqHandler;
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
	asio::ip::tcp::endpoint serverEndpoint;
	std::shared_ptr<asio::ip::tcp::acceptor> acceptor;
	std::shared_ptr<MYSQL> mysql;
	bool stopMask;
	void startAccept();
	bool testConnect();
	std::shared_ptr<AccountModel> accountModel;
	std::map<unsigned char, std::shared_ptr<RequestHandler>> handlers;
	void sendClientRequest(asio::ip::tcp::socket& socket, std::vector<char>& dataBytes,reqHandler respHandler);
	void loadHandler(std::shared_ptr<RequestHandler> handler);
#ifdef OPEN_SERVER_DEBUG
#ifdef OPEN_PROXY_DEBUG
	std::shared_ptr<asio::ip::tcp::socket> proxySocket;
#endif
#endif
};