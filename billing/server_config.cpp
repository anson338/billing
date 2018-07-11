#include "inc/common.hpp"
#include "inc/server_config.hpp"
#include <fstream>
#include <exception>
#include <json.hpp>
using json = nlohmann::json;

ServerConfig::ServerConfig()
{
	const string defaultIp = "";
	const int defaultPort = 12680;
	try {
		std::ifstream inFile(SERVER_CONFIG_FILE);
		json config;
		inFile >> config;
		inFile.close();
		this->ip = config.value("ip", defaultIp);
		this->port = config.value("port", defaultPort);
	}
	catch (const std::exception&) {
		this->ip = defaultIp;
		this->port = defaultPort;
	}
}

ServerConfig::~ServerConfig()
{
}

const string & ServerConfig::getIp()
{
	return this->ip;
}

const unsigned short & ServerConfig::getPort()
{
	return this->port;
}
