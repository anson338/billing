#pragma once
#include <vector>
#include <string>
using std::vector;
using std::string;
using std::size_t;

//char����ת��
void hexToBytes(const char * input, vector<char>& output);
void bytesToHex(const vector<char>& input, string & output, bool upperCase = true);
//c�ַ���ת��
void hexToBytes(const char * input, string& output);
void bytesToHex(const char * input, string & output, bool upperCase = true);
//debug����
void bytesToHexDebug(const vector<char>& rawBytes,string& debugStr,size_t rowBytesCount=16 , bool upperCase = true);
void bytesToHexDebug(const char * rawBytes, string& debugStr, size_t rowBytesCount = 16, bool upperCase = true);