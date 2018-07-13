#include "inc/billing_server.hpp"
#include "inc/client_connection.hpp"
#include <iostream>
#include "inc/handler/connect_handler.hpp"
using std::cout;
using std::endl;

BillingServer::BillingServer() :serverEndpoint(
	tcp::endpoint(
		asio::ip::address::from_string(config.getIp()),
		config.getPort()
	)
), acceptor(
	tcp::acceptor(
		ioService,
		serverEndpoint
	)
), stopMask(false)
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("billing server constrcut");
#endif //OPEN_SERVER_DEBUG
}

BillingServer::~BillingServer()
{
	mysql_close(&mysql);
#ifdef OPEN_SERVER_DEBUG
	Logger::write("billing server destrcut");
#endif //OPEN_SERVER_DEBUG
}

void BillingServer::run()
{
	cout << "billing server run at " << this->config.getIp() << ":" << this->config.getPort() << endl;
	if (this->testConnect()) {
		this->startAccept();
		ioService.run();
	}
	this->handlers[0xa0] = std::make_shared<ConnectHandler>(mysql);
}
void BillingServer::stop()
{
	//使用同步接口
	try
	{
		tcp::socket socket(ioService);
		asio::error_code ec;
		socket.connect(serverEndpoint, ec);
		if (ec) {
			throw std::exception(ec.message().c_str());
		}
		vector<char> command(4,(char)0x0);
		socket.write_some(asio::buffer(command), ec); 
		if (ec) {
			throw std::exception(ec.message().c_str());
		}
		socket.close();
		cout << "send \"stop\" command success" << endl;
	}
	catch (const std::exception& e)
	{
		cout << "send \"stop\" command failed: " << e.what() << endl;
	}

}
void BillingServer::startAccept() {
	if (this->stopMask) {
		cout << "stop server accept" << endl;
		return;
	}
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

bool BillingServer::testConnect()
{
	mysql_init(&mysql);
	//尝试连接最长时间
	const unsigned connectTimeOut = 3;
	mysql_options(&mysql, MYSQL_OPT_CONNECT_TIMEOUT, &connectTimeOut);
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf8");
	//开启断线自动重连
	my_bool reconnect = 1;
	mysql_options(&mysql, MYSQL_OPT_RECONNECT, &reconnect);
	if (!mysql_real_connect(&mysql, config.getDbHost(), config.getDbUser(), config.getDbPassword(), config.getDbName(), config.getDbPort(), NULL, 0))
	{
		cout << "Connect to database Error: " << mysql_error(&mysql) << endl;
		return false;
	}
	else {
		cout << "connect to mysql server ok !" << endl;
		cout << "mysql version: " << mysql_get_server_info(&mysql) << endl;
		return true;
	}
}
