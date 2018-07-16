#pragma once
#include <string>
using std::string;
#include <set>

/*服务器配置读取类*/
class ServerConfig
{
public:
	ServerConfig();
	~ServerConfig();
	const string& getIp() {
		return this->ip;
	}
	const unsigned short& getPort() {
		return this->port;
	}
	const char* getDbHost() {
		return this->dbHost.c_str();
	}
	unsigned int& getDbPort() {
		return this->dbPort;
	}
	const char* getDbUser() {
		return this->dbUser.c_str();
	}
	const char* getDbPassword() {
		return this->dbPassword.c_str();
	}
	const char* getDbName() {
		return this->dbName.c_str();
	}
	const std::set<string>& getAllowIps() {
		return this->allowIps;
	}
private:
	string ip;
	unsigned short port;
	string dbHost;
	unsigned int dbPort;
	string dbUser;
	string dbPassword;
	string dbName;
	std::set<string> allowIps;
};