#include "inc/logger.hpp"
#include "inc/server_config.hpp"
#include <fstream>
#include <exception>
#include <json.hpp>
using json = nlohmann::json;

ServerConfig::ServerConfig()
{
	const string defaultIp = "127.0.0.1";
	const unsigned short defaultPort = 12680;
	const string defaultDbHost = "localhost";
	const unsigned int defaultDbPort = 3306;
	const  string defaultDbUser = "root";
	const  string defaultDbPassword = "root";
	const  string defaultDbName = "web";
	try {
		std::ifstream inFile(SERVER_CONFIG_FILE);
		json config;
		inFile >> config;
		inFile.close();
		this->ip = config.value("ip", defaultIp);
		this->port = config.value("port", defaultPort);
		this->dbHost = config.value("db_host", defaultDbHost);
		this->dbPort = config.value("db_host", defaultDbPort);
		this->dbUser = config.value("db_host", defaultDbUser);
		this->dbPassword = config.value("db_host", defaultDbPassword);
		this->dbName = config.value("db_host", defaultDbName);
#ifdef OPEN_SERVER_DEBUG
		Logger::write(string("load config file ") + SERVER_CONFIG_FILE + " ok !");
#endif //OPEN_SERVER_DEBUG
	}
	catch (const std::exception&) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write(string("load config file ") + SERVER_CONFIG_FILE + " failed !");
#endif //OPEN_SERVER_DEBUG
		this->ip = defaultIp;
		this->port = defaultPort;
		this->dbHost = defaultDbHost;
		this->dbPort = defaultDbPort;
		this->dbUser = defaultDbUser;
		this->dbPassword = defaultDbPassword;
		this->dbName = defaultDbName;
	}
}

ServerConfig::~ServerConfig()
{
}