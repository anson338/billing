#include "inc/logger.hpp"
#include "inc/server_config.hpp"
#include <fstream>
#include <exception>
#include <stdexcept>
#include <json.hpp>
using std::string;
using json = nlohmann::json;
using std::set;
using std::ifstream;

ServerConfig::ServerConfig() :status(false)
{
	const string defaultIp = "127.0.0.1";
	const unsigned short defaultPort = 12680;
	const string defaultDbHost = "127.0.0.1";
	const unsigned int defaultDbPort = 3306;
	const  string defaultDbUser = "root";
	const  string defaultDbPassword = "root";
	const  string defaultDbName = "web";
	const set<string> defaultAllowIps;
	try {
		ifstream inFile(SERVER_CONFIG_FILE);
		if (!inFile) {
			throw std::runtime_error("read file failed");
		}
		json config;
		inFile >> config;
		inFile.close();
		this->ip = config.value("ip", defaultIp);
		this->port = config.value("port", defaultPort);
		this->dbHost = config.value("db_host", defaultDbHost);
		this->dbPort = config.value("db_port", defaultDbPort);
		this->dbUser = config.value("db_user", defaultDbUser);
		this->dbPassword = config.value("db_password", defaultDbPassword);
		this->dbName = config.value("db_name", defaultDbName);
		this->allowIps = config.value("allow_ips", defaultAllowIps);
		this->status = true;
	}
	catch (const std::exception& e) {
		//赋予默认值
		this->ip = defaultIp;
		this->port = defaultPort;
		this->dbHost = defaultDbHost;
		this->dbPort = defaultDbPort;
		this->dbUser = defaultDbUser;
		this->dbPassword = defaultDbPassword;
		this->dbName = defaultDbName;
		this->allowIps = defaultAllowIps;
		this->errorMessage.append(e.what());
	}
}

ServerConfig::~ServerConfig()
{
}