#pragma once
#include <string>
using std::string;
#include <vector>
#include "hex_tool.hpp"
using std::vector;
class BillingData
{
public:
	BillingData(const vector<char>& request);
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
	const vector<char>&  getPayloadData() {
		return this->payloadData;
	}

	void setPayloadData(const vector<char>& payloadData) {
		this->payloadData.resize(payloadData.size());
		this->payloadData.clear();
		this->appendPayloadData(payloadData);
	}

	void setPayloadData(const char* hexStr) {
		this->payloadData.clear();
		this->appendPayloadData(hexStr);
	}

	void appendPayloadData(const vector<char>& payloadData) {
		for (auto it = payloadData.begin(); it != payloadData.end(); it++) {
			this->payloadData.emplace_back(*it);
		}
		this->payloadLength = (unsigned short)this->payloadData.size() + 3;
	}

	void appendPayloadData(const char* hexStr) {
		vector<char> data;
		hexToBytes(hexStr, data);
		this->appendPayloadData(data);
	}

	void appendText(string text) {
		for (auto it = text.begin(); it != text.end(); it++) {
			this->payloadData.emplace_back(*it);
		}
		this->payloadLength = (unsigned short)this->payloadData.size() + 3;
	}

	void appendChar(unsigned char charItem) {
		this->payloadData.emplace_back(charItem);
		this->payloadLength = (unsigned short)this->payloadData.size() + 3;
	}

	const vector<char>& getId() {
		return this->id;
	}

	void setId(const vector<char>& id) {
		this->id.clear();
		this->id.emplace_back(id[0]);
		this->id.emplace_back(id[1]);
	}

	void packData(vector<char>& buff);
	void doDump(string& debug);
	const bool& isDataValid() {
		return this->isValid;
	}

private:
	bool isValid;
	unsigned short payloadLength;
	unsigned char payloadType;
	vector<char> id;
	vector<char> payloadData;
};