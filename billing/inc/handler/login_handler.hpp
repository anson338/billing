#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>
#include <iostream>
using std::cout;
using std::endl;
//0xA2
class LoginHandler :public RequestHandler
{
public:
	LoginHandler(MYSQL& mysqlHandler) :RequestHandler(mysqlHandler) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("LoginHandler construct");
#endif //OPEN_SERVER_DEBUG
	}
	~LoginHandler();
	void processRequest(BillingData& requestData, BillingData& responseData);
private:

};

LoginHandler::~LoginHandler()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("LoginHandler destrcut");
#endif //OPEN_SERVER_DEBUG
}
void LoginHandler::processRequest(BillingData& requestData, BillingData& responseData) {
	responseData.setPayloadType(requestData.getPayloadType());
	responseData.setId(requestData.getId());
	auto payloadData = requestData.getPayloadData();
	size_t offset = 0, i;
	//��ȡ��¼�û���
	unsigned char usernameLength = (unsigned char)payloadData[offset];
	string username;
	username.resize(usernameLength);
	username.clear();
	for (i = 0; i < usernameLength; i++) {
		offset++;
		username.append(1, payloadData[offset]);
	}
	//��ȡ��¼����
	offset++;
	unsigned char passLength = (unsigned char)payloadData[offset];
	string password;
	password.resize(passLength);
	password.clear();
	for (i = 0; i < passLength; i++) {
		offset++;
		password.append(1, payloadData[offset]);
	}
	//��¼IP
	offset++;
	unsigned char ipLength = (unsigned char)payloadData[offset];
	string loginIp;
	loginIp.resize(ipLength);
	loginIp.clear();
	for (i = 0; i < ipLength; i++) {
		offset++;
		loginIp.append(1, payloadData[offset]);
	}
	unsigned char loginResult = 1;
	//@todo ��¼��֤
	const char* loginResultStr[] = {
		"-",//0 ��Чֵ
		"login success",//1 ��¼�ɹ�
		"account not exists",//2 �û�������
		"password not correct" ,//3 �������
		"account is allready online",//4 ��ɫ����
		"account is allready online on other server",//5 �н�ɫ��������������������¼
		"connect failed",//6 ����ʧ��,���Ժ�����
		"account baned",//7 �˺ű�ͣȨ
		"point is not available",//8 ��������?(�շ���Ϸʹ��?)
		"regisiter"//9 �˺�ע�ᵯ��
	};
	cout << "user [" << username << "] try to login from " << loginIp << " : " << loginResultStr[loginResult] << endl;
	//
	responseData.appendChar(usernameLength);
	responseData.appendText(username);
	responseData.appendChar(loginResult);
}