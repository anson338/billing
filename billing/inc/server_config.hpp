#pragma once
#include <string>
using std::string;

/*���������ö�ȡ��*/
class ServerConfig
{
public:
	ServerConfig();
	~ServerConfig();
	const string& getIp();
	const int& getPort();
private:
	string ip;
	int port;
};