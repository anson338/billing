#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>
#include "../hex_tool.hpp"

class ConnectHandler:public RequestHandler
{
public:
	ConnectHandler(MYSQL& mysqlHandler) :RequestHandler(mysqlHandler) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("ConnectHandler construct");
#endif //OPEN_SERVER_DEBUG
	}
	~ConnectHandler();
	void processRequest(BillingData& requestData, BillingData& responseData);
private:

};

ConnectHandler::~ConnectHandler()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("ConnectHandler destrcut");
#endif //OPEN_SERVER_DEBUG
}
void ConnectHandler::processRequest(BillingData& requestData, BillingData& responseData) {
	responseData.setPayloadType(requestData.getPayloadType());
	responseData.setId(requestData.getId());
	responseData.setPayloadData("2000");
}