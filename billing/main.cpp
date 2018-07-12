#include "inc/billing_server.hpp"
#include <cstring>
int main(int argc, char** argv)
{
	auto server = std::make_shared<BillingServer>();
	if (argc >= 2) {
		const char* command = argv[1];
		if (strcmp(command, "stop") == 0) {
			server->stop();
			return 0;
		}
	}
	server->run();
	return 0;
}
