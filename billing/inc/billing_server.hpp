#pragma once
#include "server_config.hpp"

class BillingServer
{
public:
	BillingServer();
	~BillingServer();
	void run();

private:
	ServerConfig config;
};