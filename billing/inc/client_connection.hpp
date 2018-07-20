#pragma once

#include "logger.hpp"
#include <asio.hpp>
#include "request_handler.hpp"
#include "billing_data.hpp"
#include <vector>
#include <functional>

class BillingServer;
class ClientConnection : public std::enable_shared_from_this<ClientConnection>
{
public:
	typedef std::function<void(std::shared_ptr<std::vector<char>> response, const asio::error_code& ec)> proxyRespHandler;
	ClientConnection(BillingServer* s, asio::io_service& io);
	~ClientConnection();
	asio::ip::tcp::socket& getSocket();
	void acceptHandler(const asio::error_code& error);
	void writeHandler(const asio::error_code& error, std::size_t size);

private:
	BillingServer * server;
	asio::ip::tcp::socket socket;
	void readFromClient();
	void processRequest(std::shared_ptr<std::vector<char>> request, std::size_t size);
	void processRequest(std::shared_ptr<RequestHandler> handler, BillingData& requestData);
#ifdef OPEN_SERVER_DEBUG
#ifdef OPEN_PROXY_DEBUG
	void callProxyServer(std::shared_ptr<std::vector<char>> request, BillingData& requestData, proxyRespHandler respHandler);
#endif
#endif
};
