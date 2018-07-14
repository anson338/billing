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
	std::make_shared<tcp::acceptor>(
		ioService,
		serverEndpoint
		)
), mysql(
	std::make_shared<MYSQL>()
), stopMask(false)
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("billing server constrcut");
#endif //OPEN_SERVER_DEBUG
}

BillingServer::BillingServer(bool mask) :serverEndpoint(
	tcp::endpoint(
		asio::ip::address::from_string(config.getIp()),
		config.getPort()
	)
), stopMask(false)
{
}

BillingServer::~BillingServer()
{
	if (mysql) {
		mysql_close(mysql.get());
	}
#ifdef OPEN_SERVER_DEBUG
	Logger::write("billing server destrcut");
#endif //OPEN_SERVER_DEBUG
}

void BillingServer::run()
{
	cout << "billing server run at " << this->config.getIp() << ":" << this->config.getPort() << endl;
	if (this->testConnect()) {
		//加载handler
		this->handlers[0xa0] = std::make_shared<ConnectHandler>(*mysql);
		this->startAccept();
		ioService.run();
	}
}
void BillingServer::stop()
{
	tcp::socket clientSocket(ioService);
	asio::error_code ec;
	this->syncConnect(serverEndpoint, clientSocket, ec);
	if (ec) {
		cout << "send \"stop\" command failed: " << ec.message() << endl;
		return;
	}
	vector<char> command(4, (char)0x0);
	this->sendClientRequest(clientSocket, ec, command, [](std::shared_ptr<vector<char>> response) {
	});
	if (ec) {
		cout << "send \"stop\" command failed: " << ec.message() << endl;
	}
	else {
		cout << "send \"stop\" command ok" << endl;
	}

}

#ifdef OPEN_SERVER_DEBUG
void BillingServer::sendTestData() {
	BillingData testData;
	vector<char> idArr;
	idArr.emplace_back('a');
	idArr.emplace_back('b');
	testData.setId(idArr);
	testData.setPayloadType(0xa0);
	testData.setPayloadData("000000"
		"650d3139322e3136382e3230302e33");
	vector<char> sendData;
	testData.packData(sendData);
	string debugStr;
	testData.doDump(debugStr);
	cout << debugStr << endl;
	tcp::socket clientSocket(ioService);
	asio::error_code ec;
	this->syncConnect(serverEndpoint, clientSocket, ec);
	if (ec) {
		cout << "sync connect failed: " << ec.message() << endl;
		return;
	}
	this->sendClientRequest(clientSocket, ec, sendData, [](std::shared_ptr<vector<char>> response) {
		Logger::write("get response");
		BillingData responseData(response);
		string debugStr;
		responseData.doDump(debugStr);
		Logger::write(debugStr);
	});
	if (ec) {
		if (ec == asio::error::eof) {
			//读取完毕
			Logger::write("read completely");
		}
		else {
			// Some other error.
			Logger::write(string("some eror: ") + ec.message());
		}
	}
}
#endif

void BillingServer::startAccept() {
	if (this->stopMask) {
		cout << "stop server accept" << endl;
		return;
	}
	auto client = std::make_shared<ClientConnection>(this, ioService);
	this->acceptor->async_accept(
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
	MYSQL *mysqlPointer = mysql.get();
	mysql_init(mysqlPointer);
	//尝试连接最长时间
	const unsigned connectTimeOut = 3;
	mysql_options(mysqlPointer, MYSQL_OPT_CONNECT_TIMEOUT, &connectTimeOut);
	mysql_options(mysqlPointer, MYSQL_SET_CHARSET_NAME, "utf8");
	//开启断线自动重连
	my_bool reconnect = 1;
	mysql_options(mysqlPointer, MYSQL_OPT_RECONNECT, &reconnect);
	if (!mysql_real_connect(mysqlPointer, config.getDbHost(), config.getDbUser(), config.getDbPassword(), config.getDbName(), config.getDbPort(), NULL, 0))
	{
		cout << "Connect to database Error: " << mysql_error(mysqlPointer) << endl;
		return false;
	}
	else {
		cout << "connect to mysql server ok !" << endl;
		cout << "mysql version: " << mysql_get_server_info(mysqlPointer) << endl;
		return true;
	}
}

void BillingServer::syncConnect(tcp::endpoint& remotePoint, tcp::socket& socket, asio::error_code& ec) {
	socket.connect(serverEndpoint, ec);
	if (ec) {
		return;
	}
	socket.set_option(asio::socket_base::keep_alive(true), ec);
}

void BillingServer::sendClientRequest(tcp::socket& socket, asio::error_code& ec, std::vector<char>& dataBytes, reqHandler respHandler) {
	asio::write(socket, asio::buffer(dataBytes), ec);
	if (ec) {
		return;
	}
	if (respHandler) {
		size_t responseSize = 0;
		auto response = std::make_shared<std::vector<char>>(260);
		while (responseSize == 0)
		{
			responseSize = socket.read_some(asio::buffer(*response), ec);
			if (ec) {
				return;
			}
		}
		if (responseSize < response->capacity()) {
			//移除尾部多余空间
			response->resize(responseSize);
		}
		respHandler(response);
		socket.close();
	}
}