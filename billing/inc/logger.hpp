#pragma once
#include "common.hpp"
//日志记录工具
#ifdef OPEN_SERVER_DEBUG
#include <iostream>
#include <fstream>
#include <string>
using std::string;

class Logger {
public:
	static void write(const char* str);
	static void write(const string& str);
	~Logger();
private:
	Logger();
	static Logger* instance;
	static Logger* getInstance();
	std::ofstream fs;
	void doWrite(const char* str);
	void doWrite(const string& str);
	class GcObject
	{
	public:
		~GcObject() {
			if (instance) {
				delete instance;
			}
		}

	};
	static GcObject gc;
};

#endif // OPEN_SERVER_DEBUG