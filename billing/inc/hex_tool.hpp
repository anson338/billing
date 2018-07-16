#pragma once
#include <vector>
#include <string>

//char数组转换
void hexToBytes(const char * input, std::vector<char>& output);
void bytesToHex(const std::vector<char>& input, std::string & output, bool upperCase = true);
//c字符串转换
void hexToBytes(const char * input, std::string& output);
void bytesToHex(const char * input, std::string & output, bool upperCase = true);
//debug工具
void bytesToHexDebug(const std::vector<char>& rawBytes,std::string& debugStr,std::size_t rowBytesCount=16 , bool upperCase = true);
void bytesToHexDebug(const char * rawBytes, std::string& debugStr, std::size_t rowBytesCount = 16, bool upperCase = true);