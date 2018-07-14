#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>

//0xA1
class PingHandler:public RequestHandler
{
public:
	PingHandler(MYSQL& mysqlHandler) :RequestHandler(mysqlHandler) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("PingHandler construct");
#endif //OPEN_SERVER_DEBUG
	}
	~PingHandler();
	void processRequest(BillingData& requestData, BillingData& responseData);
private:

};

PingHandler::~PingHandler()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("PingHandler destrcut");
#endif //OPEN_SERVER_DEBUG
}
void PingHandler::processRequest(BillingData& requestData, BillingData& responseData) {
	responseData.setPayloadType(requestData.getPayloadType());
	responseData.setId(requestData.getId());
	responseData.setPayloadData("0100");
}