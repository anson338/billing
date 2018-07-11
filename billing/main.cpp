#include "inc/billing_server.hpp"
#include <iostream>
#include <mysql.h>
int main()
{
	//BillingServer server;
	//server.run();
	MYSQL mysql;

	mysql_init(&mysql);
	//尝试连接最长时间
	const unsigned connectTimeOut = 3;
	mysql_options(&mysql,MYSQL_OPT_CONNECT_TIMEOUT ,&connectTimeOut);
	mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "your_prog_name");
	if (!mysql_real_connect(&mysql, "192.168.200.3", "tlbb", "tlbb1234", "web", 3306, NULL, 0))
	{
		std::cout << "Failed to connect to database : Error:" << mysql_error(&mysql) << std::endl;
	}
	else {
		std::cout << "connect ok !" << std::endl;
		mysql_close(&mysql);
	}
	std::cin.get();
	return 0;
}
