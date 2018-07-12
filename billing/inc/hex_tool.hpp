#pragma once

#include <string>
#include <cstring>
#include <exception>
void hexToChars(const char * input, std::string & output)
{
	size_t leng = strlen(input);
	output.resize(2 * leng);
	output.clear();
	size_t outputIndex;
	for (size_t i = 0; i < leng; i++) {
		if (i % 2 == 0) {
			outputIndex = i / 2;
			output.append(1, input[i]);
		}
		else {
			outputIndex = (i - 1) / 2;
			output[outputIndex] += input[i];
		}
		if ((input[i] >= '0') && (input[i] <= '9')) {
			output[outputIndex] -= '0';
		}
		else if ((input[i] >= 'A') && (input[i] <= 'F')) {
			output[outputIndex] -= '7';
		}
		else if ((input[i] >= 'a') && (input[i] <= 'f')) {
			output[outputIndex] -= 'W';
		}
		else {
			//非16进制格式
			throw std::exception("bad hex string format");
		}
		if (i % 2 == 0) {
			output[outputIndex] = output[outputIndex] << 4;
		}
	}
}

void charsToHex(const char * input, std::string & output, bool upperCase = true)
{
	size_t leng = strlen(input) * 2;
	output.resize(leng);
	output.clear();
	size_t inputIndex;
	unsigned char tmpChar;
	for (size_t i = 0; i < leng; i++) {
		if (i % 2 == 0) {
			inputIndex = i / 2;
			tmpChar = (unsigned char)input[inputIndex];
			output.append(1, tmpChar >> 4);
			//std::cout << "[" << inputIndex << "]" << 0 + tmpChar << std::endl;
		}
		else {
			inputIndex = (i - 1) / 2;
			tmpChar = (unsigned char)input[inputIndex];
			output.append(1, tmpChar & 0xf);
		}
		if ((output[i] >= 0) && (output[i] <= 9)) {
			output[i] += '0';
		}
		else {
			if (upperCase) {
				output[i] += '7';
			}
			else {
				output[i] += 'W';
			}
		}
	}
}