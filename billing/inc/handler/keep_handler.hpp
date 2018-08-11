#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>

class KeepHandler :public RequestHandler
{
public:
	KeepHandler(AccountModel& m) :RequestHandler(m) {
		this->payloadType = 0xa6;
#ifdef OPEN_SERVER_DEBUG
		Logger::write("KeepHandler construct");
#endif //OPEN_SERVER_DEBUG
	}
	~KeepHandler();
	void processRequest(BillingData& requestData, BillingData& responseData);
private:

};

KeepHandler::~KeepHandler()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("KeepHandler destrcut");
#endif //OPEN_SERVER_DEBUG
}
void KeepHandler::processRequest(BillingData& requestData, BillingData& responseData) {
	responseData.setPayloadType(requestData.getPayloadType());
	responseData.setId(requestData.getId());
	auto payloadData = requestData.getPayloadData();
	size_t offset = 0, i;
	//用户名
	unsigned char usernameLength = (unsigned char)payloadData[offset];
	std::string username;
	username.resize(usernameLength);
	username.clear();
	for (i = 0; i < usernameLength; i++) {
		offset++;
		username.append(1, payloadData[offset]);
	}
#ifdef OPEN_SERVER_DEBUG
	//u2 等级
	//u8 其它数据
	unsigned short userLevel;
	offset++;
	userLevel = payloadData[offset] << 8;
	offset++;
	userLevel += payloadData[offset];
	Logger::write(std::string("user [") + username + "] level: " + std::to_string(userLevel));
#endif
	//
	responseData.appendChar(usernameLength);
	responseData.appendText(username);
	responseData.appendChar(1);
}