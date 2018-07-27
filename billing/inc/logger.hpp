#pragma once
#include "common.hpp"
//日志记录工具
#include <iostream>
#include <fstream>
#include <string>

class Logger {
public:
	static void write(const char* str);
	static void write(const std::string& str);
	~Logger();
private:
	Logger();
	static Logger& getInstance();
	std::ofstream fs;
	void doWrite(const char* str);
	void doWrite(const std::string& str);
	void outputCurrentTime(std::ostream& out);
};