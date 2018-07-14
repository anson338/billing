#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>

//0xA9
class KickHandler:public RequestHandler
{
public:
	KickHandler(MYSQL& mysqlHandler) :RequestHandler(mysqlHandler) {
#ifdef OPEN_SERVER_DEBUG
		Logger::write("KickHandler construct");
#endif //OPEN_SERVER_DEBUG
	}
	~KickHandler();
	void processRequest(BillingData& requestData, BillingData& responseData);
private:

};

KickHandler::~KickHandler()
{
#ifdef OPEN_SERVER_DEBUG
	Logger::write("KickHandler destrcut");
#endif //OPEN_SERVER_DEBUG
}
void KickHandler::processRequest(BillingData& requestData, BillingData& responseData) {
#ifdef OPEN_SERVER_DEBUG
	Logger::write("KickHandler::processRequest=>StartUpKick");
#endif //OPEN_SERVER_DEBUG
	responseData.setPayloadType(requestData.getPayloadType());
	responseData.setId(requestData.getId());
	responseData.setPayloadData("01");
}