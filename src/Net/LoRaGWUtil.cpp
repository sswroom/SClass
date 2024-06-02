#include "Stdafx.h"
#include "Crypto/Encrypt/AES128.h"
#include "Crypto/Hash/AESCMAC.h"
#include "Data/Timestamp.h"
#include "Net/LoRaGWUtil.h"
#include "Text/TextBinEnc/Base64Enc.h"

void Net::LoRaGWUtil::ParseGWMPMessage(NN<Text::StringBuilderUTF8> sb, Bool toServer, UInt8 ver, UInt16 token, UInt8 msgType, const UInt8 *msg, UOSInt msgSize)
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
	sb->AppendUOSInt(msgSize);
	sb->AppendC(UTF8STRC(", type="));
	switch (msgType)
	{
	case 0:
		sb->AppendC(UTF8STRC("PUSH_DATA"));
		sb->AppendC(UTF8STRC(", GWEUI="));
		if (msgSize >= 8)
		{
			sb->AppendHexBuff(msg, 8, 0, Text::LineBreakType::None);
		}
		sb->AppendC(UTF8STRC(", Payload="));
		if (msgSize > 8)
		{
			sb->AppendC(msg + 8, msgSize - 8);
		}
		break;
	case 1:
		sb->AppendC(UTF8STRC("PUSH_ACK"));
		break;
	case 2:
		sb->AppendC(UTF8STRC("PULL_DATA"));
		sb->AppendC(UTF8STRC(", GWEUI="));
		if (msgSize >= 8)
		{
			sb->AppendHexBuff(msg, 8, 0, Text::LineBreakType::None);
		}
		break;
	case 3:
		sb->AppendC(UTF8STRC("PULL_RESP"));
		sb->AppendC(UTF8STRC(", Payload="));
		sb->AppendC(msg, msgSize);
		break;
	case 4:
		sb->AppendC(UTF8STRC("PULL_ACK"));
		if (msgSize >= 8)
		{
			sb->AppendC(UTF8STRC(", GWEUI="));
			sb->AppendHexBuff(msg, 8, 0, Text::LineBreakType::None);
		}
		break;
	case 5:
		sb->AppendC(UTF8STRC("TX_ACK"));
		sb->AppendC(UTF8STRC(", res="));
		if (msgSize == 1 && msg[0] == 0)
		{
			sb->AppendC(UTF8STRC("ok"));
		}
		else
		{
			sb->AppendHexBuff(msg, msgSize, ' ', Text::LineBreakType::None);
		}
		break;
	default:
		sb->AppendC(UTF8STRC("UNK("));
		sb->AppendU16(msgType);
		sb->AppendUTF8Char(')');
		break;
	}
}

void Net::LoRaGWUtil::ParseUDPMessage(NN<Text::StringBuilderUTF8> sb, Bool toServer, const UInt8 *msg, UOSInt msgSize)
{
	ParseGWMPMessage(sb, toServer, msg[0], ReadMUInt16(&msg[1]), msg[3], msg + 4, msgSize - 4);
}

UOSInt Net::LoRaGWUtil::GenUpPayload(UInt8 *buff, Bool needConfirm, UInt32 devAddr, UInt32 fCnt, UInt8 fPort, const UInt8 *nwkSKey, const UInt8 *appSKey, const UInt8 *payload, UOSInt payloadLen)
{
	UOSInt index;
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
	WriteUInt32(&buff[1], devAddr);
	buff[5] = 0x80; //FCtrl = ADR
	WriteUInt16(&buff[6], (UInt16)fCnt);
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
		WriteUInt32(&ablock[6], devAddr);
		WriteUInt32(&ablock[10], fCnt);
		ablock[14] = 0;
		ablock[15] = 0;
		while (payloadLen > 0)
		{
			ablock[15]++;
			aes.EncryptBlock(ablock, sblock);
			if (payloadLen >= 16)
			{
				WriteNUInt64(&buff[index], ReadNUInt64(sblock) ^ ReadNUInt64(payload));
				WriteNUInt64(&buff[index + 8], ReadNUInt64(&sblock[8]) ^ ReadNUInt64(&payload[8]));
				index += 16;
				payloadLen -= 16;
				payload += 16;
			}
			else
			{
				UOSInt i = 0;
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
	
	ablock[0] = 0x49;
	ablock[1] = 0;
	ablock[2] = 0;
	ablock[3] = 0;
	ablock[4] = 0;
	ablock[5] = 0; //dir
	WriteUInt32(&ablock[6], devAddr);
	WriteUInt32(&ablock[10], fCnt);
	ablock[14] = 0;
	ablock[15] = (UInt8)index;
	UInt8 cmac[16];
	Crypto::Hash::AESCMAC aescmac(nwkSKey);
	aescmac.Calc(ablock, 16);
	aescmac.Calc(buff, index);
	aescmac.GetValue(cmac);
	WriteNUInt32(&buff[index], ReadNUInt32(cmac));
	return index + 4;
}

void Net::LoRaGWUtil::GenRxpkJSON(NN<Text::StringBuilderUTF8> sb, UInt32 freq, UInt32 chan, UInt32 rfch, UInt32 codrk, Int32 rssi, Int32 lsnr, const UInt8 *data, UOSInt dataSize)
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
	sb->AppendUOSInt(dataSize);
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
