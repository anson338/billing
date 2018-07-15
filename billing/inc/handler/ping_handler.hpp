#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>

class PingHandler:public RequestHandler
{
public:
	PingHandler(AccountModel& m) :RequestHandler(m) {
		this->payloadType = 0xa1;
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
	//requestData
	// ZoneId: 2u
	// WorldId: 2u
	// PlayerCount: 2u 
	//
	responseData.setPayloadType(requestData.getPayloadType());
	responseData.setId(requestData.getId());
	responseData.setPayloadData("0100");
}