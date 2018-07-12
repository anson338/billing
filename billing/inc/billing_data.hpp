#pragma once
#include <string>
using std::string;
#include <memory>
#include <iostream>
using std::endl;
class BillingData
{
public:
	BillingData(std::shared_ptr<string> request);
	BillingData();
	~BillingData();
	const unsigned char& getPayloadType() {
		return this->payloadType;
	}
	void setPayloadType(const unsigned char payloadType) {
		this->payloadType = payloadType;
	}
	const unsigned short& getPayloadLength() {
		return this->payloadLength;
	}
	const char*  getPayloadData() {
		return this->payloadData.c_str();
	}
	void setPayloadData(const char* payloadData) {
		this->payloadData.clear();
		this->payloadData+= payloadData;
		this->payloadLength =(unsigned short) this->payloadData.length() + 3;
	}
	const char* getId() {
		return this->id.c_str();
	}

	void setId(const char* id) {
		this->id.clear();
		this->id.append(id);
	}

	void packData(string& buff);
	void doDump(string& debug);
	const bool& isDataValid() {
		return this->isValid;
	}

private:
	bool isValid;
	unsigned short payloadLength;
	unsigned char payloadType;
	string id;
	string payloadData;
};