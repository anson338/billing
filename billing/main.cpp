#include "inc/common.hpp"
#include "inc/billing_server.hpp"
#include <iostream>

int main()
{
	BillingServer server;
	server.run();
	std::cin.get();
	return 0;
}
