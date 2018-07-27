#include "logger.hpp"
#include "billing_data.hpp"
#include "hex_tool.hpp"
using std::vector;
using std::string;

BillingData::BillingData(vector<char>& request) :isValid(false)
{
	if (!request.empty()) {
		this->parseData(request);
	}
}

void BillingData::parseData(vector<char>& request)
{
	auto it = request.begin();
	auto offset = it;
	unsigned char mask0 = 0xAA;
	unsigned char mask1 = 0x55;
	unsigned char tmpChar = 0;
	bool matchMask = false;
	while (it != request.end()) {
		tmpChar = static_cast<unsigned char>(*it);
		if (tmpChar == mask0) {
			if (it + 1 == request.end()) {
				return;
			}
			tmpChar = static_cast<unsigned char>(*(it + 1));
			if (tmpChar == mask1) {
				matchMask = true;
				it++;
				offset = it + 1;
				break;
			}
		}
		it++;
	}
	if (!matchMask) {
		return;
	}
	//两字节的长度
	if (it + 2 >= request.end()) {
		return;
	}
	it++;
	this->payloadLength = static_cast<unsigned short>(static_cast<unsigned char>(*it)) << 8;
	it++;
	this->payloadLength += static_cast<unsigned short>(static_cast<unsigned char>(*it));
	//判断剩余字节数量
	if (it + static_cast<int>(this->payloadLength) + 2 >= request.end()) {
		//剩余字节不足
		return;
	}
	//一字节类型标识
	it++;
	this->payloadType = static_cast<unsigned char>(*it);
	//两字节的id
	it++;
	this->id.insert(this->id.end(), it, it + 2);
	//负载数据
	it++;
	this->payloadData.insert(this->payloadData.end(), it, it + static_cast<int>(this->payloadLength) - 3);
	//尾部字节判断
	it++;
	if ((static_cast<unsigned char>(*it) == mask1) && (static_cast<unsigned char>(*(it + 1)) == mask0)) {
		this->isValid = true;
		//从缓冲区移除这段字节序列
		request.erase(request.begin(),it+2);
	}
	else {
		//移除头部序列AA55
		request.erase(request.begin(), offset);
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
