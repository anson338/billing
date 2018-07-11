#pragma once
#include <string>
using std::string;

/*服务器配置读取类*/
class ServerConfig
{
public:
	ServerConfig();
	~ServerConfig();
	const string& getIp();
	const unsigned short& getPort();
private:
	string ip;
	unsigned short port;
};