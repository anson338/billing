#pragma once
#include "logger.hpp"
#include <mysql.h>
#include <memory>
#include <string>

class AccountInfo {
public:
	AccountInfo() :id(0), point(0), is_online(0), is_lock(0) {};
	int id;
	char name[33];
	char password[33];
	char question[65];
	char answer[65];
	char email[65];
	char qq[17];
	int point;
	unsigned short is_online, is_lock;
};
class AccountModel
{
public:
	AccountModel(std::shared_ptr<MYSQL> mysql);
	~AccountModel();
	unsigned char getLoginResult(std::string& username, std::string& password);
	const unsigned int getUserPoint(std::string& username);
	void updateOnlineStatus(std::string& username, bool isOnline);
private:
	std::shared_ptr<MYSQL> mysqlHandler;
	MYSQL * getMysql() {
		return this->mysqlHandler.get();
	};
	void updateLockStatus(std::string& username, bool isLock);
	void getAccountInfo(std::string& username, AccountInfo& accountInfo);
};