#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>

class LogoutHandler :public RequestHandler
{
public:
	LogoutHandler(AccountModel& m) :RequestHandler(m) {
		this->payloadType = 0xa4;
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
	std::string username;
	username.resize(usernameLength);
	username.clear();
	for (i = 0; i < usernameLength; i++) {
		offset++;
		username.append(1, payloadData[offset]);
	}
	this->accountModel.updateOnlineStatus(username, false);
	unsigned char logoutResult = 0;
	Logger::write(std::string("user [") + username + "] logout");
	//
	responseData.appendChar(usernameLength);
	responseData.appendText(username);
	responseData.appendChar(logoutResult);
}