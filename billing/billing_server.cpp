#include "inc/billing_server.hpp"
#include "inc/client_connection.hpp"
#include "inc/handler/connect_handler.hpp"
#include "inc/handler/login_handler.hpp"
#include "inc/handler/reg_handler.hpp"
#include "inc/handler/logout_handler.hpp"
#include "inc/handler/ping_handler.hpp"
#include "inc/handler/kick_handler.hpp"
#include "inc/handler/enter_game_handler.hpp"
#include "inc/handler/check_point_handler.hpp"
#include "inc/account_model.hpp"
#include <cstring>
using asio::ip::tcp;
using std::string;
using std::to_string;
using std::vector;

#ifdef OPEN_SERVER_DEBUG
#include "inc/billing_data.hpp"
#endif
#define T_SERVER_ENDPOINT() tcp::endpoint( \
	(config.getIp() == "0.0.0.0") ? asio::ip::address_v4::any() : asio::ip::address::from_string(config.getIp()),\
	config.getPort()\
	)

#define T_CLIENT_ENDPOINT() (config.getIp() == "0.0.0.0")?tcp::endpoint( \
	asio::ip::address_v4::loopback() ,\
	config.getPort()\
	) : serverEndpoint

BillingServer::BillingServer() : serverEndpoint(T_SERVER_ENDPOINT()), acceptor(
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
			Logger::write(string("connect to proxy failed: ") + error.message());
		}
		else {
			Logger::write("connect to proxy server success");
		}
	});
#endif //OPEN_PROXY_DEBUG
#endif //OPEN_SERVER_DEBUG
}

BillingServer::BillingServer(bool mask) :serverEndpoint(T_SERVER_ENDPOINT()), stopMask(false)
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("billing server constrcut");
#endif
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
	if (!this->config.loadSuccess()) {
		Logger::write(string("load config file \"") + SERVER_CONFIG_FILE + "\" failed : " + config.getErrorMessage());
#ifdef IN_WIN32_SYSTEM
		std::cin.get();
#endif // IN_WIN32_SYSTEM
		return;
	}
	Logger::write(string("billing server run at ") + this->config.getIp() + ":" + to_string(this->config.getPort()));
	if (this->testConnect()) {
		this->accountModel = std::make_shared<AccountModel>(this->mysql);
		//加载handler
		this->loadHandler(std::make_shared<ConnectHandler>(*accountModel));
		this->loadHandler(std::make_shared<PingHandler>(*accountModel));
		this->loadHandler(std::make_shared<LoginHandler>(*accountModel, this->config.isAutoRegOpen()));
		if (this->config.isAutoRegOpen()) {
			this->loadHandler(std::make_shared<RegHandler>(*accountModel));
		}
		this->loadHandler(std::make_shared<EnterGameHandler>(*accountModel));
		this->loadHandler(std::make_shared<LogoutHandler>(*accountModel));
		this->loadHandler(std::make_shared<KickHandler>(*accountModel));
		this->loadHandler(std::make_shared<CheckPointHandler>(*accountModel));
		this->startAccept();
		Logger::write("billing server start success.");
		ioService.run();
	}
	else {
#ifdef IN_WIN32_SYSTEM
		std::cin.get();
#endif // IN_WIN32_SYSTEM
	}
}
void BillingServer::stop()
{
	tcp::socket clientSocket(ioService);
	vector<char> sendData(4, (char)0x0);
	auto clientEndPoint = T_CLIENT_ENDPOINT();
	clientSocket.async_connect(clientEndPoint, [this, &clientSocket, &sendData](const asio::error_code& error) {
		clientSocket.set_option(asio::socket_base::keep_alive(true));
		if (error) {
			clientSocket.close();
			Logger::write(string("connect failed: ") + error.message());
			return;
		}
		this->sendClientRequest(clientSocket, sendData, [&clientSocket](std::shared_ptr<vector<char>> response, const asio::error_code& ec)mutable {
			if (ec) {
				Logger::write(string("send \"stop\" command failed: ") + ec.message());
			}
			else {
				Logger::write("send \"stop\" command ok");
			}
			clientSocket.close();
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
	testData.setPayloadType(0xf1);
	testData.setPayloadData("146161626263634067616D652E736F68"
		"752E636F6D2039636266386134646362"
		"38653330363832623932376633353264"
		"36353539613020653130616463333934"
		"39626135396162626535366530353766"
		"323066383833650D3139322E3136382E"
		"3230302E310F36373537393732324071"
		"712E636F6D000000000000");
	vector<char> sendData;
	testData.packData(sendData);
	tcp::socket clientSocket(ioService);
	auto clientEndPoint = T_CLIENT_ENDPOINT();
	clientSocket.async_connect(clientEndPoint, [this, &clientSocket, &sendData](const asio::error_code& error) {
		clientSocket.set_option(asio::socket_base::keep_alive(true));
		if (error) {
			clientSocket.close();
			Logger::write(string("connect failed: ") + error.message());
			return;
		}
		this->sendClientRequest(clientSocket, sendData, [&clientSocket](std::shared_ptr<std::vector<char>> response, const asio::error_code& ec) {
			if (!ec) {
				Logger::write("get response");
				string debugStr;
				BillingData responseData(*response);
				responseData.doDump(debugStr);
				Logger::write(debugStr);
				bytesToHexDebug(*response, debugStr);
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
			clientSocket.close();
		});
	});
	ioService.run();
}
#endif

void BillingServer::startAccept() {
	if (this->stopMask) {
		Logger::write("stop server accept");
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
		Logger::write(string("Connect to database Error: ") + mysql_error(mysqlPointer));
		return false;
	}
	Logger::write("connect to mysql server ok !");
	Logger::write(string("mysql version: ") + mysql_get_server_info(mysqlPointer));
	//获取account表字段信息
	if (mysql_query(mysqlPointer, "SHOW COLUMNS FROM account") != 0)
	{
		Logger::write(string("Get account table info Error: ") + mysql_error(mysqlPointer));
		return false;
	}
	MYSQL_RES *res = mysql_store_result(mysqlPointer);
	if (!res) {
		Logger::write(string("Get account table info Error: ") + mysql_error(mysqlPointer));
		return false;
	}
	//获取字段信息
	MYSQL_FIELD *fields = mysql_fetch_fields(res);
	unsigned int fieldsCount = mysql_num_fields(res), fieldIndex = 0, i;
	for (i = 0; i < fieldsCount; i++) {
		if (strcmp("Field", fields[i].name) == 0) {
			fieldIndex = i;
			break;
		}
	}
	//需要两个字段:is_online,is_lock
	bool hasExtraFields[] = { false,false };
	const char* extraFields[] = { "is_online","is_lock" };
	auto row = mysql_fetch_row(res);
	while (row) {
		//判断是否存在附加字段
		for (i = 0; i < 2; i++) {
			if (strcmp(row[fieldIndex], extraFields[i]) == 0) {
				hasExtraFields[i] = true;
			}
		}
		row = mysql_fetch_row(res);
	}
	mysql_free_result(res);
	//添加附加字段
	for (i = 0; i < 2; i++) {
		if (hasExtraFields[i]) {
			continue;
		}
		string sql = "ALTER TABLE `account` ADD COLUMN `";
		sql += extraFields[i];
		sql += "`  smallint(1) UNSIGNED NOT NULL DEFAULT 0";
		if (mysql_real_query(mysqlPointer, sql.c_str(), sql.length()) != 0) {
			Logger::write(string("add extra column ") + extraFields[i] + " failed: " + mysql_error(mysqlPointer));
			return false;
		}
	}
	return true;
}

void BillingServer::sendClientRequest(tcp::socket& socket, vector<char>& dataBytes, std::size_t size, reqHandler respHandler)
{
	asio::async_write(socket, (size == 0) ? asio::buffer(dataBytes) : asio::buffer(dataBytes, size), [&socket, respHandler](const asio::error_code& ec, std::size_t bytes_transferred) {
		if (ec) {
			respHandler(nullptr, ec);
			return;
		}
		if (respHandler) {
			//
			auto response = std::make_shared<vector<char>>(260);
			socket.async_receive(asio::buffer(*response), [response, &socket, respHandler](const asio::error_code& ec1, std::size_t responseSize) {
				if (ec1) {
					respHandler(nullptr, ec1);
				}
				else {
					if (responseSize < response->capacity()) {
						//移除尾部多余空间
						response->resize(responseSize);
					}
					respHandler(response, ec1);
				}
			});
			//
		}
	});
}

void BillingServer::sendClientRequest(tcp::socket& socket, vector<char>& dataBytes, reqHandler respHandler) {
	this->sendClientRequest(socket, dataBytes, 0, respHandler);
}

void BillingServer::loadHandler(std::shared_ptr<RequestHandler> handler)
{
	this->handlers[handler->getPayloadType()] = handler;
}
