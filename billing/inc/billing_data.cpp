#include "logger.hpp"
#include "billing_data.hpp"
#include "hex_tool.hpp"

BillingData::BillingData(std::shared_ptr<string> request) :isValid(true)
{
	unsigned short requestLength = (unsigned short)request->length();
	string tmp;
	charsToHex(request->substr(0, 2).c_str(), tmp);
	if (tmp.compare("AA55") != 0) {
		this->isValid = false;
	}
	if (this->isValid) {
		charsToHex(request->substr(requestLength - 2, 2).c_str(), tmp);
		if (tmp.compare("55AA") != 0) {
			this->isValid = false;
		}
	}
	if (this->isValid) {
		unsigned short len0, len1;
		len0 = (unsigned short)request->at(2);
		len1 = (unsigned short)request->at(3);
		this->payloadLength = (len0 << 8) + len1;
		this->payloadType = (unsigned char)request->at(4);
		this->id = request->substr(5, 2);
		this->payloadData = request->substr(7, this->payloadLength - 3);
	}
}

BillingData::BillingData() :isValid(true)
{
}

BillingData::~BillingData()
{
}

void BillingData::packData(string& buff)
{
	if (!this->isValid) {
		return;
	}
	buff.clear();
	string tmpStr;
	hexToChars("AA55", tmpStr);
	buff.append(tmpStr);
	unsigned short len0, len1;
	len0 = this->payloadLength >> 2;
	len1 = this->payloadLength & 0xf;
	buff.append(1, (unsigned char)len0);
	buff.append(1, (unsigned char)len1);
	buff.append(1, this->payloadType);
	buff.append(this->id);
	buff.append(this->payloadData);
	hexToChars("55AA", tmpStr);
	buff.append(tmpStr);
}

void BillingData::doDump(string& debug)
{
	debug.clear();
	debug.append("{\r\nisValid: ").append(this->isValid ? "true" : "false").append(",\r\n");
	debug.append("payloadLength: ").append(std::to_string(this->payloadLength)).append(",\r\n");
	debug.append("id: ").append(id).append(",\r\n");
	debug.append(29, '=');
	string hexStr;
	charsToHex(payloadData.c_str(), hexStr);
	for (std::size_t i = 0; i < hexStr.length(); i++) {
		if (i % 2 == 0) {
			if (i % 20 == 0) {
				debug.append("\r\n");
			}
			else {
				debug.append(" ");
			}
		}
		debug.append(1, hexStr[i]);
	}
	debug.append("\r\n");
	debug.append(29, '=').append("\r\n}");
}
