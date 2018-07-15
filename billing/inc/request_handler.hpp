#pragma once
#include "logger.hpp"
#include <mysql.h>
#include "billing_data.hpp"
#include "account_model.hpp"

class RequestHandler
{
public:
	RequestHandler(AccountModel& m) :accountModel(m) {};
	virtual void processRequest(BillingData& requestData, BillingData& responseData) = 0;
	virtual ~RequestHandler() {
	}
	const unsigned char& getPayloadType() {
		return this->payloadType;
	}
protected:
	unsigned char payloadType;
	AccountModel& accountModel;
};