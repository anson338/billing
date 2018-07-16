#include "inc/logger.hpp"
using std::string;

Logger::Logger() {
#ifdef OPEN_SERVER_DEBUG
	std::cout << "Logger system construct" << std::endl;
#endif
	fs.open("log.log", std::ios_base::app);
}

Logger::~Logger() {
	if (fs.is_open()) {
		fs.close();
	}
#ifdef OPEN_SERVER_DEBUG
	std::cout << "Logger system destruct" << std::endl;
#endif
}

Logger& Logger::getInstance() {
	static Logger instance;
	return instance;
}

void Logger::write(const char* str) {
	getInstance().doWrite(str);
}

void Logger::write(const string& str) {
	getInstance().doWrite(str);
}

void Logger::doWrite(const char* str) {
	std::cout << str << std::endl;
	this->fs << str << std::endl;
}

void Logger::doWrite(const string& str) {
	std::cout << str << std::endl;
	this->fs << str << std::endl;
}