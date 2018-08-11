#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>

class RegHandler :public RequestHandler
{
public:
	RegHandler(AccountModel& m) :RequestHandler(m) {
		this->payloadType = 0xf1;
#ifdef OPEN_SERVER_DEBUG
		Logger::write("RegHandler construct");
#endif //OPEN_SERVER_DEBUG
	}
	~RegHandler();
	void processRequest(BillingData& requestData, BillingData& responseData);
private:
};

RegHandler::~RegHandler()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("RegHandler destrcut");
#endif //OPEN_SERVER_DEBUG
}
void RegHandler::processRequest(BillingData& requestData, BillingData& responseData) {
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
	//超级密码
	offset++;
	unsigned char superPassLength = (unsigned char)payloadData[offset];
	std::string superPassword;
	superPassword.resize(superPassLength);
	superPassword.clear();
	for (i = 0; i < superPassLength; i++) {
		offset++;
		superPassword.append(1, payloadData[offset]);
	}
	//密码
	offset++;
	unsigned char passLength = (unsigned char)payloadData[offset];
	std::string password;
	password.resize(passLength);
	password.clear();
	for (i = 0; i < passLength; i++) {
		offset++;
		password.append(1, payloadData[offset]);
	}
	//登录IP
	offset++;
	unsigned char ipLength = (unsigned char)payloadData[offset];
	std::string loginIp;
	loginIp.resize(ipLength);
	loginIp.clear();
	for (i = 0; i < ipLength; i++) {
		offset++;
		loginIp.append(1, payloadData[offset]);
	}
	//邮箱
	offset++;
	unsigned char emailLength = (unsigned char)payloadData[offset];
	std::string email;
	email.resize(emailLength);
	email.clear();
	for (i = 0; i < emailLength; i++) {
		offset++;
		email.append(1, payloadData[offset]);
	}
	unsigned char regResult = this->accountModel.getRegResult(username, password, superPassword, email);
	Logger::write(std::string("user [") + username + "] (" + email + ")try to reg from " + loginIp + " : " + (regResult==1?"ok":"error"));
	//
	responseData.appendChar(usernameLength);
	responseData.appendText(username);
	responseData.appendChar(regResult);
}