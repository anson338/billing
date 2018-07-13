#include "logger.hpp"
#include "billing_data.hpp"
#include "hex_tool.hpp"

BillingData::BillingData(std::shared_ptr<vector<char>> request) :isValid(true)
{
	unsigned short requestLength = (unsigned short)request->size();
	string tmp;
	vector<char> tmpBytes(2);
	tmpBytes.clear();
	tmpBytes.emplace_back(request->at(0));
	tmpBytes.emplace_back(request->at(1));
	bytesToHex(tmpBytes, tmp);
	//头部2字节检测
	if (tmp.compare("AA55") != 0) {
		this->isValid = false;
	}
	if (this->isValid) {
		//尾部2字节检测
		tmpBytes.clear();
		tmpBytes.emplace_back(request->at(requestLength - 2));
		tmpBytes.emplace_back(request->at(requestLength - 1));
		bytesToHex(tmpBytes, tmp);
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
		this->id.emplace_back(request->at(5));
		this->id.emplace_back(request->at(6));
		size_t dataCount = this->payloadLength - 3;
		this->payloadData.resize(dataCount);
		this->payloadData.clear();
		for (size_t i = 0; i < dataCount; i++) {
			this->payloadData.emplace_back(request->at(7+i));
		}
	}
}

BillingData::BillingData() :isValid(true)
{
}

BillingData::~BillingData()
{
}

void BillingData::packData(vector<char>& buff)
{
	if (!this->isValid) {
		return;
	}
	buff.resize(4 + 2 + this->payloadLength);
	buff.clear();
	vector<char> maskBuff(2);
	maskBuff.clear();
	hexToBytes("AA55", maskBuff);
	buff.emplace_back(maskBuff[0]);
	buff.emplace_back(maskBuff[1]);
	unsigned short len0, len1;
	len0 = this->payloadLength >> 2;
	len1 = this->payloadLength & 0xf;
	buff.emplace_back((unsigned char)len0);
	buff.emplace_back( (unsigned char)len1);
	buff.emplace_back( this->payloadType);
	buff.emplace_back(this->id[0]);
	buff.emplace_back(this->id[1]);
	for (auto it = this->payloadData.begin(); it != this->payloadData.end(); it++) {
		buff.emplace_back(*it);
	}
	buff.emplace_back(maskBuff[1]);
	buff.emplace_back(maskBuff[0]);
}

void BillingData::doDump(string& debug)
{
	debug.clear();
	debug.append("{\r\nisValid: ").append(this->isValid ? "true" : "false").append(",\r\n");
	debug.append("payloadLength: ").append(std::to_string(this->payloadLength)).append(",\r\n");
	string hexStr;
	bytesToHex(id, hexStr);
	debug.append("id: ").append(hexStr).append(",\r\n");
	debug.append(29, '=');
	bytesToHex(payloadData, hexStr);
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
