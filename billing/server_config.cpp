#include "inc/logger.hpp"
#include "inc/server_config.hpp"
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <exception>
#include <stdexcept>
using std::string;
using std::set;
using std::ifstream;
using rapidjson::IStreamWrapper;
using rapidjson::Document;
using rapidjson::Value;

ServerConfig::ServerConfig() :status(false)
{
	try {
		ifstream inFile(SERVER_CONFIG_FILE);
		if (!inFile) {
			throw std::runtime_error("read file failed");
		}
		IStreamWrapper isw(inFile);
		Document d;
		d.ParseStream(isw);
		inFile.close();
		fetchJsonVal(d, "ip", this->ip);
		fetchJsonVal(d, "port", this->port);
		fetchJsonVal(d, "db_host", this->dbHost);
		fetchJsonVal(d, "db_port", this->dbPort);
		fetchJsonVal(d, "db_user", this->dbUser);
		fetchJsonVal(d, "db_password", this->dbPassword);
		fetchJsonVal(d, "db_name", this->dbName);
		fetchJsonVal(d, "allow_ips", this->allowIps);
		this->status = true;
	}
	catch (const std::exception& e) {
		this->errorMessage.append(e.what());
	}
}

ServerConfig::~ServerConfig()
{
}
//json相关
void ServerConfig::fetchJsonVal(const rapidjson::Document& d, const char * itemKey, std::string& output)
{
	string itemField = string("[") + itemKey + "]";
	if (!d.HasMember(itemKey)) {
		throw std::runtime_error(itemField + " not found !");
	}
	const Value& itemVal = d[itemKey];
	if (!itemVal.IsString()) {
		throw std::runtime_error(itemField + " type error: not string !");
	}
	output = itemVal.GetString();
}
void ServerConfig::fetchJsonVal(const rapidjson::Document& d, const char * itemKey, unsigned short& output)
{
	string itemField = string("[") + itemKey + "]";
	if (!d.HasMember(itemKey)) {
		throw std::runtime_error(itemField + " not found !");
	}
	const Value& itemVal = d[itemKey];
	if (!itemVal.IsInt()) {
		throw std::runtime_error(itemField + " type error: not int !");
	}
	output = static_cast<unsigned short>(itemVal.GetInt());
}
void ServerConfig::fetchJsonVal(const rapidjson::Document& d, const char * itemKey, unsigned int& output)
{
	string itemField = string("[") + itemKey + "]";
	if (!d.HasMember(itemKey)) {
		throw std::runtime_error(itemField + " not found !");
	}
	const Value& itemVal = d[itemKey];
	if (!itemVal.IsInt()) {
		throw std::runtime_error(itemField + " type error: not int !");
	}
	output = static_cast<unsigned int>(itemVal.GetInt());
}
void ServerConfig::fetchJsonVal(const rapidjson::Document& d, const char * itemKey, std::set<std::string>& output)
{
	string itemField = string("[") + itemKey + "]";
	if (!d.HasMember(itemKey)) {
		throw std::runtime_error(itemField + " not found !");
	}
	const Value& itemVal = d[itemKey];
	if (!itemVal.IsArray()) {
		throw std::runtime_error(itemField + " type error: not array !");
	}
	if (!output.empty()) {
		output.clear();
	}
	for (auto& v : itemVal.GetArray()) {
		if (!v.IsString()) {
			throw std::runtime_error(itemField + " type error: not string array !");
		}
		output.emplace(v.GetString());
	}
}