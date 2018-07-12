#pragma once
#include "logger.hpp"
#include <mysql.h>
#include "billing_data.hpp"

class RequestHandler
{
public:
	RequestHandler(MYSQL& mysqlHandler):mysql(mysqlHandler) {}
	virtual void processRequest(BillingData& requestData, BillingData& responseData)=0;
	virtual ~RequestHandler() {
	}

protected:
	MYSQL & mysql;
};