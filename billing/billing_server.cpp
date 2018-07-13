#include "inc/billing_server.hpp"
#include "inc/client_connection.hpp"
#include <iostream>
#include "inc/handler/connect_handler.hpp"
using std::cout;
using std::endl;
#ifdef OPEN_SERVER_DEBUG
#include "inc/billing_data.hpp"
#endif

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
		//加载handler
		this->handlers[0xa0] = std::make_shared<ConnectHandler>(mysql);
		this->startAccept();
		ioService.run();
	}
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
		vector<char> command(4, (char)0x0);
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

#ifdef OPEN_SERVER_DEBUG
void BillingServer::sendTestData() {
	try
	{
		tcp::socket socket(ioService);
		asio::error_code ec;
		socket.connect(serverEndpoint, ec);
		if (ec) {
			throw std::runtime_error(ec.message().c_str());
		}
		BillingData testData;
		vector<char> idArr;
		idArr.emplace_back('a');
		idArr.emplace_back('b');
		testData.setId(idArr);
		testData.setPayloadType(0xa0);
		testData.setPayloadData("01020304");
		vector<char> sendData;
		testData.packData(sendData);
		string debugStr;
		testData.doDump(debugStr);
		cout << debugStr << endl;
		bytesToHexDebug(sendData, debugStr);
		cout << "full binary data" << endl;
		cout << debugStr << endl;
		//绑定读取
		this->readTestData(socket);
		socket.async_send(asio::buffer(sendData), [this, &socket](const asio::error_code& error, std::size_t bytes_transferred) {
			Logger::write("send ok");
		});
		ioService.run();
	}
	catch (const std::exception& e)
	{
		cout << "some error: " << e.what() << endl;
	}
}
void BillingServer::readTestData(tcp::socket& socket) {
	auto response = std::make_shared<vector<char>>();
	response->resize(260);
	response->clear();
	socket.async_receive(asio::buffer(*response), [response, this, &socket](const asio::error_code& error, std::size_t bytes_transferred) {
		if (!error) {
			if (bytes_transferred > 0) {
				string debugStr;
				bytesToHexDebug(*response, debugStr);
				cout << "get response data" << endl;
				cout << debugStr << endl;
				socket.close();
			}
			else {
				this->readTestData(socket);
			}
			Logger::write("read ok");
			Logger::write(std::to_string(bytes_transferred));
		}
		else if (error == asio::error::eof) {
			//读取完毕
			socket.close();
		}
		else {
			// Some other error.
			throw asio::system_error(error);
		}
	});
}
#endif

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
