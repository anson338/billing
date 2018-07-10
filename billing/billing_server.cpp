#include "inc/billing_server.hpp"
#include "inc/client_connection.hpp"


BillingServer::BillingServer() :acceptor(

	tcp::acceptor(
		ioService,
		tcp::endpoint(
			config.getIp().empty() ? asio::ip::address_v4::any() : asio::ip::address::from_string(config.getIp()),
			config.getPort()
		)
	)

)
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("server constrcut");
#endif //OPEN_SERVER_DEBUG
}

BillingServer::~BillingServer()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("server destrcut");
#endif //OPEN_SERVER_DEBUG
}

void BillingServer::run()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write(string("server run at ") + this->config.getIp() + ":" + std::to_string(this->config.getPort()));
#endif //OPEN_SERVER_DEBUG
	this->startAccept();
	ioService.run();
}
void BillingServer::startAccept() {
	auto client = std::make_shared<ClientConnection>(this, ioService);
	this->acceptor.async_accept(
		client->getSocket(),
		[client](const asio::error_code& error) {
		client->acceptHandler(error);
	}
	);
#ifdef OPEN_SERVER_DEBUG
	Logger::write("start accept");
#endif //OPEN_SERVER_DEBUG
}