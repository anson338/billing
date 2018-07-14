#include "inc/billing_server.hpp"
#include "inc/client_connection.hpp"
#include <iostream>
#include "inc/handler/connect_handler.hpp"
#include "inc/handler/login_handler.hpp"
#include "inc/handler/ping_handler.hpp"
#include "inc/handler/kick_handler.hpp"
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
#ifdef OPEN_PROXY_DEBUG
	Logger::write("prepare to connect to proxy server");
	auto proxyEndpoint = std::make_shared<tcp::endpoint>(
		asio::ip::address::from_string(PROXY_DEBUG_IP),
		PROXY_DEBUG_PORT
		);
	proxySocket = std::make_shared<tcp::socket>(ioService);
	proxySocket->async_connect(*proxyEndpoint, [this](const asio::error_code& error) {
		this->proxySocket->set_option(asio::socket_base::keep_alive(true));
		if (error) {
			this->proxySocket->close();
			this->proxySocket = nullptr;
			cout << "connect to proxy failed: " << error.message() << endl;
		}
		else {
			Logger::write("connect to proxy server success");
		}
	});
#endif //OPEN_PROXY_DEBUG
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
#ifdef OPEN_PROXY_DEBUG
	if (!this->proxySocket) {
		this->proxySocket->close();
	}
#endif
	Logger::write("billing server destrcut");
#endif //OPEN_SERVER_DEBUG
}

void BillingServer::run()
{
	cout << "billing server run at " << this->config.getIp() << ":" << this->config.getPort() << endl;
	if (this->testConnect()) {
		//加载handler
		this->handlers[0xa0] = std::make_shared<ConnectHandler>(*mysql);
		this->handlers[0xa1] = std::make_shared<PingHandler>(*mysql);
		this->handlers[0xa2] = std::make_shared<LoginHandler>(*mysql);
		this->handlers[0xa9] = std::make_shared<KickHandler>(*mysql);
		this->startAccept();
		ioService.run();
	}
}
void BillingServer::stop()
{
	tcp::socket clientSocket(ioService);
	vector<char> sendData(4, (char)0x0);
	clientSocket.async_connect(serverEndpoint, [this, &clientSocket, &sendData](const asio::error_code& error) {
		clientSocket.set_option(asio::socket_base::keep_alive(true));
		if (error) {
			clientSocket.close();
			cout << "connect failed: " << error.message() << endl;
			return;
		}
		this->sendClientRequest(clientSocket, sendData, [](tcp::socket& client, std::shared_ptr<std::vector<char>> response, const asio::error_code& ec) {
			if (ec) {
				cout << "send \"stop\" command failed: " << ec.message() << endl;
			}
			else {
				cout << "send \"stop\" command ok" << endl;
			}
			client.close();
		});
	});
	ioService.run();
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
	clientSocket.async_connect(serverEndpoint, [this, &clientSocket, &sendData](const asio::error_code& error) {
		clientSocket.set_option(asio::socket_base::keep_alive(true));
		if (error) {
			clientSocket.close();
			cout << "connect failed: " << error.message() << endl;
			return;
		}
		this->sendClientRequest(clientSocket, sendData, [](tcp::socket& client, std::shared_ptr<std::vector<char>> response, const asio::error_code& ec) {
			if (!ec) {
				Logger::write("get response");
				string debugStr;
				BillingData responseData(*response);
				responseData.doDump(debugStr);
				Logger::write(debugStr);
			}
			else if (ec == asio::error::eof) {
				//读取完毕
				Logger::write("read completely");
			}
			else {
				// Some other error.
				Logger::write(string("some eror: ") + ec.message());
			}
			client.close();
		});
	});
	ioService.run();
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

void BillingServer::sendClientRequest(tcp::socket& socket, std::vector<char>& dataBytes, reqHandler respHandler) {
	asio::async_write(socket, asio::buffer(dataBytes), [&socket, respHandler](const asio::error_code& ec, std::size_t bytes_transferred) {
		if (ec) {
			respHandler(socket, nullptr, ec);
			return;
		}
		auto response = std::make_shared<std::vector<char>>();
		response->resize(260);
		socket.async_receive(asio::buffer(*response), [response, &socket, respHandler](const asio::error_code& ec1, std::size_t responseSize) {
			if (ec1) {
				respHandler(socket, nullptr, ec1);
				return;
			}
			else {
				if (responseSize < response->capacity()) {
					//移除尾部多余空间
					response->resize(responseSize);
				}
				respHandler(socket, response, ec1);
			}
		});
	});
}