#include "inc/common.hpp"
#include "inc/billing_server.hpp"
#include <cstring>
int main(int argc, char** argv)
{
	std::shared_ptr<BillingServer> server;
	if (argc >= 2) {
		const char* command = argv[1];
		if (strcmp(command, "stop") == 0) {
			server = std::make_shared<BillingServer>(false);
			server->stop();
			return 0;
		}
#ifdef OPEN_SERVER_DEBUG
		else if (strcmp(command, "test") == 0) {
			server = std::make_shared<BillingServer>(false);
			server->sendTestData();
			return 0;
		}
#endif
	}
	try
	{
		server = std::make_shared<BillingServer>();
		server->run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
