#include "logger.hpp"
#include "billing_data.hpp"
#include "hex_tool.hpp"

BillingData::BillingData(const vector<char>& request) :isValid(true)
{
	unsigned short requestLength = (unsigned short)request.size();
	string tmp;
	vector<char> tmpBytes(2);
	tmpBytes.clear();
	tmpBytes.emplace_back(request.at(0));
	tmpBytes.emplace_back(request.at(1));
	bytesToHex(tmpBytes, tmp);
	//头部2字节检测
	if (tmp.compare("AA55") != 0) {
		this->isValid = false;
	}
	if (this->isValid) {
		//尾部2字节检测
		tmpBytes.clear();
		tmpBytes.emplace_back(request.at(requestLength - 2));
		tmpBytes.emplace_back(request.at(requestLength - 1));
		bytesToHex(tmpBytes, tmp);
		if (tmp.compare("55AA") != 0) {
			this->isValid = false;
		}
	}
	if (this->isValid) {
		unsigned short len0, len1;
		len0 = (unsigned char)request.at(2);
		len1 = (unsigned char)request.at(3);
		this->payloadLength = (len0 << 8) + len1;
		this->payloadType = (unsigned char)request.at(4);
		this->id.emplace_back(request.at(5));
		this->id.emplace_back(request.at(6));
		size_t dataCount = this->payloadLength - 3;
		this->payloadData.resize(dataCount);
		this->payloadData.clear();
		for (size_t i = 0; i < dataCount; i++) {
			this->payloadData.emplace_back(request.at(7 + i));
		}
	}
}

BillingData::BillingData() :isValid(true), payloadLength(3)
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
	unsigned char len0, len1;
	len0 = (unsigned char)(this->payloadLength >> 8);
	len1 = (unsigned char)(this->payloadLength & 0xff);
	buff.emplace_back(len0);
	buff.emplace_back(len1);
	buff.emplace_back(this->payloadType);
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
	if (!this->isValid) {
		debug.append("[not valid]");
		return;
	}
	debug.append("{\r\nisValid: ").append(this->isValid ? "true" : "false").append(",\r\n");
	debug.append("payloadLength: ").append(std::to_string(this->payloadLength)).append(",\r\n");
	vector<char> payloadTypeBytes(1, this->payloadType);
	string hexStr;
	bytesToHex(payloadTypeBytes, hexStr);
	debug.append("payloadType: 0x").append(hexStr).append(",\r\n");
	bytesToHex(id, hexStr);
	debug.append("id: 0x").append(hexStr).append(",\r\n");
	bytesToHexDebug(payloadData, hexStr);
	debug.append(hexStr).append("\r\n}");
}
