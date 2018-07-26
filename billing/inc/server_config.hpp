#pragma once
#include <string>
#include <set>
#include <rapidjson/document.h>

/*服务器配置读取类*/
class ServerConfig
{
public:
	ServerConfig();
	~ServerConfig();
	const std::string& getIp() {
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
	const std::set<std::string>& getAllowIps() {
		return this->allowIps;
	}
	const bool& loadSuccess() {
		return this->status;
	}
	const char* getErrorMessage() {
		return this->errorMessage.c_str();
	}
private:
	std::string ip;
	unsigned short port;
	std::string dbHost;
	unsigned int dbPort;
	std::string dbUser;
	std::string dbPassword;
	std::string dbName;
	std::set<std::string> allowIps;
	//配置文件加载状态
	bool status;
	std::string errorMessage;
	//json相关
	void fetchJsonVal(const rapidjson::Document& d, const char * itemKey, std::string& output);
	void fetchJsonVal(const rapidjson::Document& d, const char * itemKey, unsigned short& output);
	void fetchJsonVal(const rapidjson::Document& d, const char * itemKey, unsigned int& output);
	void fetchJsonVal(const rapidjson::Document& d, const char * itemKey, std::set<std::string>& output);
};