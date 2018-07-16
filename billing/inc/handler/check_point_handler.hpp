#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>

class CheckPointHandler :public RequestHandler
{
public:
	CheckPointHandler(AccountModel& m) :RequestHandler(m) {
		this->payloadType = 0xe2;
#ifdef OPEN_SERVER_DEBUG
		Logger::write("CheckPointHandler construct");
#endif //OPEN_SERVER_DEBUG
	}
	~CheckPointHandler();
	void processRequest(BillingData& requestData, BillingData& responseData);
private:

};

CheckPointHandler::~CheckPointHandler()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("CheckPointHandler destrcut");
#endif //OPEN_SERVER_DEBUG
}
void CheckPointHandler::processRequest(BillingData& requestData, BillingData& responseData) {
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
	//当前IP
	offset++;
	unsigned char ipLength = (unsigned char)payloadData[offset];
	std::string loginIp;
	loginIp.resize(ipLength);
	loginIp.clear();
	for (i = 0; i < ipLength; i++) {
		offset++;
		loginIp.append(1, payloadData[offset]);
	}
	//获取登录角色名
	offset++;
	unsigned char charLength = (unsigned char)payloadData[offset];
	std::string charName;
	charName.resize(charLength);
	charName.clear();
	for (i = 0; i < charLength; i++) {
		offset++;
		charName.append(1, payloadData[offset]);
	}
	//
	responseData.appendChar(usernameLength);
	responseData.appendText(username);
	Logger::write(std::string("user [") + username + "] " + charName + " check point at " + loginIp);
	unsigned int pointResult = this->accountModel.getUserPoint(username);
	pointResult = (pointResult + 1) * 1000;
	unsigned char tmpChar;
	tmpChar = (unsigned char)(pointResult >> 24);
	responseData.appendChar(tmpChar);
	tmpChar = (unsigned char)((pointResult >> 16) & 0xff);
	responseData.appendChar(tmpChar);
	tmpChar = (unsigned char)((pointResult >> 8) & 0xff);
	responseData.appendChar(tmpChar);
	tmpChar = (unsigned char)(pointResult & 0xff);
	responseData.appendChar(tmpChar);
}