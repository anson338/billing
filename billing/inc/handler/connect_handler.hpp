#pragma once
#include "../logger.hpp"
#include "../request_handler.hpp"
#include "../billing_data.hpp"
#include <mysql.h>
#include "../hex_tool.hpp"

class ConnectHandler:public RequestHandler
{
public:
	ConnectHandler(MYSQL& mysqlHandler) :RequestHandler(mysqlHandler) {}
	~ConnectHandler();
	void processRequest(BillingData& requestData, BillingData& responseData);
private:

};

ConnectHandler::~ConnectHandler()
{
}
void ConnectHandler::processRequest(BillingData& requestData, BillingData& responseData) {
	responseData.setPayloadType(requestData.getPayloadType());
	responseData.setId(requestData.getId());
	vector<char> payloadData;
	hexToBytes("2000", payloadData);
	requestData.setPayloadData(payloadData);
}