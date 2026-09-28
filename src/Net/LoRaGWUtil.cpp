#include "Stdafx.h"
#include "Crypto/Encrypt/AES128.h"
#include "Crypto/Hash/AESCMAC.h"
#include "Data/Timestamp.h"
#include "DB/CSVFile.h"
#include "Net/LoRaGWUtil.h"
#include "Text/TextBinEnc/Base64Enc.h"

void Net::LoRaGWUtil::ParseGWMPMessage(NN<Text::StringBuilderUTF8> sb, Bool toServer, UInt8 ver, UInt16 token, UInt8 msgType, Data::ByteArrayR msg)
{
	if (toServer)
	{
		sb->AppendC(UTF8STRC("To Server"));
	}
	else
	{
		sb->AppendC(UTF8STRC("Fr Server"));
	}
	sb->AppendC(UTF8STRC(", ver="));
	sb->AppendU16(ver);
	sb->AppendC(UTF8STRC(", token="));
	sb->AppendU16(token);
	sb->AppendC(UTF8STRC(", leng="));
	sb->AppendUIntOS(msg.GetSize());
	sb->AppendC(UTF8STRC(", type="));
	switch (msgType)
	{
	case 0:
		sb->AppendC(UTF8STRC("PUSH_DATA"));
		sb->AppendC(UTF8STRC(", GWEUI="));
		if (msg.GetSize() >= 8)
		{
			sb->AppendHexBuff(msg.Arr(), 8, 0, Text::LineBreakType::None);
		}
		if (msg.GetSize() > 8)
		{
			sb->AppendC(UTF8STRC(", Payload="));
			sb->AppendC(msg.Arr() + 8, msg.GetSize() - 8);
		}
		break;
	case 1:
		sb->AppendC(UTF8STRC("PUSH_ACK"));
		break;
	case 2:
		sb->AppendC(UTF8STRC("PULL_DATA"));
		sb->AppendC(UTF8STRC(", GWEUI="));
		if (msg.GetSize() >= 8)
		{
			sb->AppendHexBuff(msg.Arr(), 8, 0, Text::LineBreakType::None);
		}
		if (msg.GetSize() > 8)
		{
			sb->AppendC(UTF8STRC(", Payload="));
			sb->AppendC(msg.Arr() + 8, msg.GetSize() - 8);
		}
		break;
	case 3:
		sb->AppendC(UTF8STRC("PULL_RESP"));
		sb->AppendC(UTF8STRC(", Payload="));
		sb->AppendC(msg.Arr(), msg.GetSize());
		break;
	case 4:
		sb->AppendC(UTF8STRC("PULL_ACK"));
		if (msg.GetSize() >= 8)
		{
			sb->AppendC(UTF8STRC(", GWEUI="));
			sb->AppendHexBuff(msg.Arr(), 8, 0, Text::LineBreakType::None);
		}
		if (msg.GetSize() > 8)
		{
			sb->AppendC(UTF8STRC(", Payload="));
			sb->AppendC(msg.Arr() + 8, msg.GetSize() - 8);
		}
		break;
	case 5:
		sb->AppendC(UTF8STRC("TX_ACK"));
		sb->AppendC(UTF8STRC(", res="));
		if (msg.GetSize() == 1 && msg[0] == 0)
		{
			sb->AppendC(UTF8STRC("ok"));
		}
		else
		{
			sb->AppendHexBuff(msg.Arr(), msg.GetSize(), ' ', Text::LineBreakType::None);
		}
		break;
	default:
		sb->AppendC(UTF8STRC("UNK("));
		sb->AppendU16(msgType);
		sb->AppendUTF8Char(')');
		break;
	}
}

void Net::LoRaGWUtil::ParseUDPMessage(NN<Text::StringBuilderUTF8> sb, Bool toServer, Data::ByteArrayR msg)
{
	ParseGWMPMessage(sb, toServer, msg[0], ReadMUInt16(&msg[1]), msg[3], msg.SubArray(4));
}

UIntOS Net::LoRaGWUtil::GenUpPayload(UnsafeArray<UInt8> buff, Bool needConfirm, UInt32 devAddr, UInt32 fCnt, UInt8 fPort, UnsafeArray<const UInt8> nwkSKey, UnsafeArray<const UInt8> appSKey, UnsafeArray<const UInt8> payload, UIntOS payloadLen)
{
	UIntOS index;
	// MHDR
	if (needConfirm)
	{
		buff[0] = 0x80;
	}
	else
	{
		buff[0] = 0x40;
	}
	UInt8 ablock[16];
	UInt8 sblock[16];
	// FHDR
	WriteLUInt32(&buff[1], devAddr);
	buff[5] = 0x80; //FCtrl = ADR
	WriteLUInt16(&buff[6], (UInt16)fCnt);
	buff[8] = fPort;
	index = 9;
	if (payloadLen > 0)
	{
		Crypto::Encrypt::AES128 aes((fPort == 0)?nwkSKey:appSKey);
		ablock[0] = 1;
		ablock[1] = 0;
		ablock[2] = 0;
		ablock[3] = 0;
		ablock[4] = 0;
		ablock[5] = 0; //dir
		WriteLUInt32(&ablock[6], devAddr);
		WriteLUInt32(&ablock[10], fCnt);
		ablock[14] = 0;
		ablock[15] = 0;
		while (payloadLen > 0)
		{
			ablock[15]++;
			aes.EncryptBlock(ablock, sblock);
			if (payloadLen >= 16)
			{
				WriteNUInt64(&buff[index], ReadNUInt64(&sblock[0]) ^ ReadNUInt64(&payload[0]));
				WriteNUInt64(&buff[index + 8], ReadNUInt64(&sblock[8]) ^ ReadNUInt64(&payload[8]));
				index += 16;
				payloadLen -= 16;
				payload += 16;
			}
			else
			{
				UIntOS i = 0;
				while (i < payloadLen)
				{
					buff[index + i] = sblock[i] ^ payload[i];
					i++;
				}
				index += payloadLen;
				payloadLen = 0;
				break;
			}
		}
	}
	
	CalcMIC(&buff[index], devAddr, fCnt, false, nwkSKey, buff, index);
	return index + 4;
}

void Net::LoRaGWUtil::CalcMIC(UnsafeArray<UInt8> micBuff, UInt32 devAddr, UInt32 fCnt, Bool downLink, UnsafeArray<const UInt8> nwkSKey, UnsafeArray<const UInt8> msg, UIntOS msgLen)
{
	UInt8 ablock[16];
	ablock[0] = 0x49;
	ablock[1] = 0;
	ablock[2] = 0;
	ablock[3] = 0;
	ablock[4] = 0;
	ablock[5] = downLink ? 1 : 0;
	WriteLUInt32(&ablock[6], devAddr);
	WriteLUInt32(&ablock[10], fCnt);
	ablock[14] = 0;
	ablock[15] = (UInt8)msgLen;
	UInt8 cmac[16];
	Crypto::Hash::AESCMAC aescmac(nwkSKey);
	aescmac.Calc(ablock, 16);
	aescmac.Calc(msg, msgLen);
	aescmac.GetValue(cmac);
	WriteNUInt32(&micBuff[0], ReadNUInt32(cmac));
}

void Net::LoRaGWUtil::GenRxpkJSON(NN<Text::StringBuilderUTF8> sb, UInt32 freq, UInt32 chan, UInt32 rfch, UInt32 codrk, Int32 rssi, Int32 lsnr, UnsafeArray<const UInt8> data, UIntOS dataSize)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	sb->AppendC(UTF8STRC("{\"rxpk\":[{\"tmst\":"));
	sb->AppendU32((UInt32)(ts.inst.sec * 1000000) + ts.inst.nanosec / 1000);
	sb->AppendC(UTF8STRC(",\"time\":\""));
	sptr = ts.ToString(sbuff, "yyyy-MM-dd\\THH:mm:ss.ffffff\\Z");
	sb->AppendP(sbuff, sptr);
	sb->AppendC(UTF8STRC("\",\"chan\":"));
	sb->AppendU32(chan);
	sb->AppendC(UTF8STRC(",\"rfch\":"));
	sb->AppendU32(rfch);
	sb->AppendC(UTF8STRC(",\"freq\":"));
	sptr = Text::StrUInt32(sbuff, freq);
	sb->AppendP(sbuff, sptr - 6);
	sb->AppendUTF8Char('.');
	sb->AppendC(sptr - 6, 6);
	sb->AppendC(UTF8STRC(",\"stat\":1,\"modu\":\"LORA\",\"datr\":\"SF7BW125\",\"codr\":\""));
	sb->AppendU32(codrk);
	sb->AppendC(UTF8STRC("/5\",\"lsnr\":"));
	sptr = Text::StrInt32(sbuff, lsnr);
	sb->AppendP(sbuff, sptr - 1);
	sb->AppendUTF8Char('.');
	sb->AppendUTF8Char(sptr[-1]);
	sb->AppendC(UTF8STRC(",\"rssi\":"));
	sb->AppendI32(rssi);
	sb->AppendC(UTF8STRC(",\"size\":"));
	sb->AppendUIntOS(dataSize);
	sb->AppendC(UTF8STRC(",\"data\":\""));
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::Normal, false);
	b64.EncodeBin(sb, data, dataSize);
	sb->AppendC(UTF8STRC("\"}]}"));
}

void Net::LoRaGWUtil::GenStatJSON(NN<Text::StringBuilderUTF8> sb, const Data::Timestamp &ts, UInt32 rxnb, UInt32 rxok, UInt32 rwfw, Double ackr, UInt32 dwnb, UInt32 txnb)
{
	UTF8Char sbuff[40];
	UnsafeArray<UTF8Char> sptr;
	sb->AppendC(UTF8STRC("{\"stat\":{\"time\":\""));
	sptr = ts.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
	sb->AppendP(sbuff, sptr);
	sb->AppendC(UTF8STRC(" UTC\",\"rxnb\":"));
	sb->AppendU32(rxnb);
	sb->AppendC(UTF8STRC(",\"rxok\":"));
	sb->AppendU32(rxok);
	sb->AppendC(UTF8STRC(",\"rwfw\":"));
	sb->AppendU32(rwfw);
	sb->AppendC(UTF8STRC(",\"ackr\":"));
	sb->AppendDouble(ackr);
	sb->AppendC(UTF8STRC(",\"dwnb\":"));
	sb->AppendU32(dwnb);
	sb->AppendC(UTF8STRC(",\"txnb\":"));
	sb->AppendU32(txnb);
	sb->AppendC(UTF8STRC("}}"));
}

void Net::LoRaGWUtil::GenStatJSON(NN<Text::StringBuilderUTF8> sb, const Data::Timestamp &ts, UInt32 rxnb, UInt32 rxok, UInt32 rwfw, Double ackr, UInt32 dwnb, UInt32 txnb, Double lat, Double lon, Int32 altitude)
{
	UTF8Char sbuff[40];
	UnsafeArray<UTF8Char> sptr;
	sb->AppendC(UTF8STRC("{\"stat\":{\"time\":\""));
	sptr = ts.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
	sb->AppendP(sbuff, sptr);
	sb->AppendC(UTF8STRC(" UTC\",\"rxnb\":"));
	sb->AppendU32(rxnb);
	sb->AppendC(UTF8STRC(",\"rxok\":"));
	sb->AppendU32(rxok);
	sb->AppendC(UTF8STRC(",\"rwfw\":"));
	sb->AppendU32(rwfw);
	sb->AppendC(UTF8STRC(",\"ackr\":"));
	sb->AppendDouble(ackr);
	sb->AppendC(UTF8STRC(",\"dwnb\":"));
	sb->AppendU32(dwnb);
	sb->AppendC(UTF8STRC(",\"txnb\":"));
	sb->AppendU32(txnb);
	sb->AppendC(UTF8STRC(",\"lati\":"));
	sb->AppendDouble(lat);
	sb->AppendC(UTF8STRC(",\"long\":"));
	sb->AppendDouble(lon);
	sb->AppendC(UTF8STRC(",\"alti\":"));
	sb->AppendI32(altitude);
	sb->AppendC(UTF8STRC("}}"));
}

void Net::LoRaGWUtil::PHYPayloadDetail(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> buff, UIntOS buffSize, NN<Data::UInt32FastMapNN<LoRaDevInfo>> devMap)
{
	UInt32 devAddr = 0;
	UInt32 fCnt = 0;
	Bool downLink = false;
	switch (buff[0] >> 5)
	{
	case 0:
		sb->AppendC(UTF8STRC("Message Type = Join Request\r\n"));
		break;
	case 1:
		sb->AppendC(UTF8STRC("Message Type = Join Accept\r\n"));
		break;
	case 2:
		sb->AppendC(UTF8STRC("Message Type = Unconfirmed Data Up\r\n"));
		devAddr = MACPayloadDetail(sb, devMap, downLink = false, buff + 1, buffSize - 5, fCnt);
		break;
	case 3:
		sb->AppendC(UTF8STRC("Message Type = Unconfirmed Data Down\r\n"));
		devAddr = MACPayloadDetail(sb, devMap, downLink = true, buff + 1, buffSize - 5, fCnt);
		break;
	case 4:
		sb->AppendC(UTF8STRC("Message Type = Confirmed Data Up\r\n"));
		devAddr = MACPayloadDetail(sb, devMap, downLink = false, buff + 1, buffSize - 5, fCnt);
		break;
	case 5:
		sb->AppendC(UTF8STRC("Message Type = Confirmed Data Down\r\n"));
		devAddr = MACPayloadDetail(sb, devMap, downLink = true, buff + 1, buffSize - 5, fCnt);
		break;
	case 6:
		sb->AppendC(UTF8STRC("Message Type = RFU\r\n"));
		break;
	case 7:
		sb->AppendC(UTF8STRC("Message Type = Propriety\r\n"));
		break;
	}
	UInt32 mic = ReadMUInt32(&buff[buffSize - 4]);
	sb->AppendC(UTF8STRC("MIC = 0x"));
	sb->AppendHex32(mic);
	sb->AppendC(UTF8STRC("\r\n"));
	UInt8 calcMIC[4];
	NN<Net::LoRaGWUtil::LoRaDevInfo> dev;
	if (devMap->Get(devAddr).SetTo(dev))
	{
		UInt32 defMIC;
		UInt32 actualFCnt = fCnt;
		Net::LoRaGWUtil::CalcMIC(calcMIC, devAddr, fCnt, downLink, dev->nwkSKey, buff, buffSize - 4);
		defMIC = ReadMUInt32(calcMIC);
		UIntOS i = 8;
		if (defMIC != mic)
		{
			while (i-- > 0)
			{
				fCnt += 65536;
				Net::LoRaGWUtil::CalcMIC(calcMIC, devAddr, fCnt, downLink, dev->nwkSKey, buff, buffSize - 4);
				if (ReadMUInt32(calcMIC) == mic)
				{
					defMIC = ReadMUInt32(calcMIC);
					actualFCnt = fCnt;
				}
			}

		}
		sb->AppendC(UTF8STRC("Calculated MIC = 0x"));
		sb->AppendHex32(defMIC);
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendC(UTF8STRC("Actual FCnt = "));
		sb->AppendU32(actualFCnt);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

UInt32 Net::LoRaGWUtil::MACPayloadDetail(NN<Text::StringBuilderUTF8> sb, NN<Data::UInt32FastMapNN<LoRaDevInfo>> devMap, Bool downLink, UnsafeArray<const UInt8> buff, UIntOS buffSize, OutParam<UInt32> fCnt)
{
	if (buffSize < 7)
	{
		return 0;
	}
	UInt32 devAddr = ReadLUInt32(&buff[0]);
	sb->AppendC(UTF8STRC("DevAddr = 0x"));
	sb->AppendHex32(devAddr);
	sb->AppendC(UTF8STRC("\r\nADR = "));
	sb->AppendUIntOS(((UIntOS)buff[4] & 0x80) >> 7);
	sb->AppendC(UTF8STRC("\r\nACK = "));
	sb->AppendUIntOS(((UIntOS)buff[4] & 0x20) >> 5);
	if (downLink)
	{
		sb->AppendC(UTF8STRC("\r\nRFU = "));
		sb->AppendUIntOS(((UIntOS)buff[4] & 0x40) >> 6);
		sb->AppendC(UTF8STRC("\r\nFPending = "));
		sb->AppendUIntOS(((UIntOS)buff[4] & 0x10) >> 4);
	}
	else
	{
		sb->AppendC(UTF8STRC("\r\nADRACKReq = "));
		sb->AppendUIntOS(((UIntOS)buff[4] & 0x40) >> 6);
		sb->AppendC(UTF8STRC("\r\nClassB = "));
		sb->AppendUIntOS(((UIntOS)buff[4] & 0x10) >> 4);
	}
	UIntOS fOptsLen = (UIntOS)buff[4] & 0xF;
	sb->AppendC(UTF8STRC("\r\nFOptsLen = "));
	sb->AppendUIntOS(fOptsLen);
	sb->AppendC(UTF8STRC("\r\nFCnt = "));
	sb->AppendU16(ReadLUInt16(&buff[5]));
	fCnt.Set(ReadLUInt16(&buff[5]));
	if (fOptsLen + 7 > buffSize)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		return devAddr;
	}
	if (fOptsLen > 0)
	{
		sb->AppendC(UTF8STRC("\r\nFOpts = "));
		sb->AppendHexBuff(buff + 7, fOptsLen, ' ', Text::LineBreakType::None);
	}
	sb->AppendC(UTF8STRC("\r\n"));
	buff += fOptsLen + 7;
	buffSize -= fOptsLen + 7;
	if (buffSize == 0)
	{
		return devAddr;
	}
	sb->AppendC(UTF8STRC("FPort = "));
	sb->AppendU16(buff[0]);
	if (buffSize > 1)
	{
		sb->AppendC(UTF8STRC("\r\nFRMPayload = "));
		sb->AppendHexBuff(buff + 1, buffSize - 1, ' ', Text::LineBreakType::None);
	}
	sb->AppendC(UTF8STRC("\r\n"));
	NN<Net::LoRaGWUtil::LoRaDevInfo> dev;
	if (devMap->Get(devAddr).SetTo(dev))
	{
		sb->AppendC(UTF8STRC("DevEUI = 0x"));
		sb->AppendHexBuff(dev->devEUI, 8, 0, Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendC(UTF8STRC("NwkSKey = 0x"));
		sb->AppendHexBuff(dev->nwkSKey, 16, 0, Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendC(UTF8STRC("AppSKey = 0x"));
		sb->AppendHexBuff(dev->appSKey, 16, 0, Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	return devAddr;
}

Bool Net::LoRaGWUtil::LoadCSV(Text::CStringNN fileName, NN<Data::UInt32FastMapNN<LoRaDevInfo>> devMap)
{
	UIntOS devEUICol = INVALID_INDEX;
	UIntOS nwkSKeyCol = INVALID_INDEX;
	UIntOS appSKeyCol = INVALID_INDEX;
	DB::CSVFile csv(fileName, 65001);
	NN<DB::DBReader> r;
	if (!csv.QueryTableData(nullptr, CSTR(""), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		return false;
	}
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UIntOS j = r->ColCount();
	while (j-- > 0)
	{
		if (r->GetName(j, sbuff).SetTo(sptr))
		{
			Text::CStringNN colName = CSTRP(sbuff, sptr);
			if (colName.EqualsICase(UTF8STRC("DevEUI")))
			{
				devEUICol = j;
			}
			else if (colName.EqualsICase(UTF8STRC("NwkSKey")))
			{
				nwkSKeyCol = j;
			}
			else if (colName.EqualsICase(UTF8STRC("AppSKey")))
			{
				appSKeyCol = j;
			}
		}
	}

	if (devEUICol == INVALID_INDEX || nwkSKeyCol == INVALID_INDEX || appSKeyCol == INVALID_INDEX)
	{
		csv.CloseReader(r);
		return false;
	}
	devMap->MemFreeAll();
	while (r->ReadNext())
	{
		NN<LoRaDevInfo> dev;
		NN<Text::String> devEUI = r->GetNewStrNN(devEUICol);
		NN<Text::String> nwkSKey = r->GetNewStrNN(nwkSKeyCol);
		NN<Text::String> appSKey = r->GetNewStrNN(appSKeyCol);
		if (devEUI->leng == 16 && nwkSKey->leng == 32 && appSKey->leng == 32)
		{
			dev = MemAllocNN(LoRaDevInfo);
			if (devEUI->Hex2Bytes(dev->devEUI) == 8 && nwkSKey->Hex2Bytes(dev->nwkSKey) == 16 && appSKey->Hex2Bytes(dev->appSKey) == 16)
			{
				devMap->Put(ReadMUInt32(&dev->devEUI[4]), dev);
			}
			else
			{
				MemFreeNN(dev);
			}
		}
		devEUI->Release();
		nwkSKey->Release();
		appSKey->Release();
	}
	csv.CloseReader(r);
	return true;
}

Text::CStringNN Net::LoRaGWUtil::MessageTypeGetName(UInt8 msgType)
{
	switch (msgType)
	{
	case 0:
		return CSTR("Join Request");
	case 1:
		return CSTR("Join Accept");
	case 2:
		return CSTR("Unconfirmed Data Up");
	case 3:
		return CSTR("Unconfirmed Data Down");
	case 4:
		return CSTR("Confirmed Data Up");
	case 5:
		return CSTR("Confirmed Data Down");
	case 6:
		return CSTR("RFU");
	case 7:
		return CSTR("Proprietary");
	default:
		return CSTR("Unknown");
	}
}
