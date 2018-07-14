#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>
#include <iostream>
using std::cout;
using std::endl;
//0xA4
class LogoutHandler :public RequestHandler
{
public:
	LogoutHandler(MYSQL& mysqlHandler) :RequestHandler(mysqlHandler) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("LogoutHandler construct");
#endif //OPEN_SERVER_DEBUG
	}
	~LogoutHandler();
	void processRequest(BillingData& requestData, BillingData& responseData);
private:

};

LogoutHandler::~LogoutHandler()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("LogoutHandler destrcut");
#endif //OPEN_SERVER_DEBUG
}
void LogoutHandler::processRequest(BillingData& requestData, BillingData& responseData) {
	responseData.setPayloadType(requestData.getPayloadType());
	responseData.setId(requestData.getId());
	auto payloadData = requestData.getPayloadData();
	size_t offset = 0, i;
	//获取登录用户名
	unsigned char usernameLength = (unsigned char)payloadData[offset];
	string username;
	username.resize(usernameLength);
	username.clear();
	for (i = 0; i < usernameLength; i++) {
		offset++;
		username.append(1, payloadData[offset]);
	}
	unsigned char logoutResult = 0;
	cout << "user [" << username << "] logout" << endl;
	//
	responseData.appendChar(usernameLength);
	responseData.appendText(username);
	responseData.appendChar(logoutResult);
}