#include "inc/common.hpp"
#include "inc/billing_server.hpp"
#include <iostream>


BillingServer::BillingServer()
{
	std::cout << "server constrcut" << std::endl;
}

BillingServer::~BillingServer()
{
	std::cout << "server destruct" << std::endl;
}

void BillingServer::run()
{
	std::cout << "server run at " << this->config.getIp() << ":" << this->config.getPort() << std::endl;
}
