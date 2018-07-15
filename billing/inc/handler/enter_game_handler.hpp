#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>

class EnterGameHandler :public RequestHandler
{
public:
	EnterGameHandler(AccountModel& m) :RequestHandler(m) {
		this->payloadType = 0xa3;
#ifdef OPEN_SERVER_DEBUG
		Logger::write("EnterGameHandler construct");
#endif //OPEN_SERVER_DEBUG
	}
	~EnterGameHandler();
	void processRequest(BillingData& requestData, BillingData& responseData);
private:

};

EnterGameHandler::~EnterGameHandler()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("EnterGameHandler destrcut");
#endif //OPEN_SERVER_DEBUG
}
void EnterGameHandler::processRequest(BillingData& requestData, BillingData& responseData) {
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
	//获取登录角色名
	offset++;
	unsigned char charLength = (unsigned char)payloadData[offset];
	string charName;
	charName.resize(charLength);
	charName.clear();
	for (i = 0; i < charLength; i++) {
		offset++;
		charName.append(1, payloadData[offset]);
	}
	//更新状态为在线模式
	this->accountModel.updateOnlineStatus(username, true);
	unsigned char loginResult = 1;
#ifdef OPEN_SERVER_DEBUG
	Logger::write(string("user [") + username + "] " + charName + " entered game");
#endif
	//
	responseData.appendChar(usernameLength);
	responseData.appendText(username);
	responseData.appendChar(loginResult);
}