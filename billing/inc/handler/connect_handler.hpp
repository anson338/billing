#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>

//0xA0
class ConnectHandler :public RequestHandler
{
public:
	ConnectHandler(AccountModel& m) :RequestHandler(m) {
		this->payloadType = 0xa0;
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
#ifdef OPEN_SERVER_DEBUG
	Logger::write("ConnectHandler::processRequest");
#endif //OPEN_SERVER_DEBUG
	responseData.setPayloadType(requestData.getPayloadType());
	responseData.setId(requestData.getId());
	responseData.setPayloadData("2000");
}