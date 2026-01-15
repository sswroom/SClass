#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/SMSMessage.h"

UnsafeArray<UTF16Char> Text::SMSMessage::ParsePDUPhone(UnsafeArray<UTF16Char> buff, UnsafeArray<const UInt8> pduPhone, UInt8 phoneByteLen)
{
	UInt8 phoneSize = phoneByteLen;
	if (phoneSize > 0)
	{
		switch (pduPhone[0] & 0x70)
		{
		case 0x10:
			*buff++ = '+';
		case 0x20:
		case 0x30:
		case 0:
			pduPhone++;
			phoneSize--;
			while (phoneSize-- > 0)
			{
				*buff++ = (UTF16Char)((pduPhone[0] & 0xf) + 0x30);
				if ((pduPhone[0] & 0xf0) != 0xf0)
				{
					*buff++ = (UTF16Char)(((pduPhone[0] & 0xf0) >> 4) + 0x30);
				}
				else
				{
					break;
				}
				pduPhone++;
			}
			break;
		case 0x50:
			{
				Bool lastIs1B;
				UInt8 v = 0;
				Int32 i = 0;
				pduPhone++;
				phoneSize--;
				lastIs1B = false;
				while (phoneSize > 0)
				{
					switch (i)
					{
					case 0:
						v = pduPhone[0] & 0x7f;
						i = 1;
						break;
					case 1:
						v = (UInt8)(((pduPhone[0] & 0x80) >> 7) | ((pduPhone[1] << 1) & 0x7f));
						pduPhone++;
						i = 2;
						break;
					case 2:
						v = (UInt8)(((pduPhone[0] & 0xc0) >> 6) | ((pduPhone[1] << 2) & 0x7f));
						pduPhone++;
						i = 3;
						break;
					case 3:
						v = (UInt8)(((pduPhone[0] & 0xe0) >> 5) | ((pduPhone[1] << 3) & 0x7f));
						pduPhone++;
						i = 4;
						break;
					case 4:
						v = (UInt8)(((pduPhone[0] & 0xf0) >> 4) | ((pduPhone[1] << 4) & 0x7f));
						pduPhone++;
						i = 5;
						break;
					case 5:
						v = (UInt8)(((pduPhone[0] & 0xf8) >> 3) | ((pduPhone[1] << 5) & 0x7f));
						pduPhone++;
						i = 6;
						break;
					case 6:
						v = (UInt8)(((pduPhone[0] & 0xfc) >> 2) | ((pduPhone[1] << 6) & 0x7f));
						pduPhone++;
						i = 7;
						break;
					case 7:
						v = (UInt8)((pduPhone[0] & 0xfe) >> 1);
						pduPhone++;
						i = 0;
						break;
						
					}
					phoneSize--;
					if (lastIs1B)
					{
						*buff++ = Text::SMSUtil::gsm1b2text[v];
						lastIs1B = false;
					}
					else if (v == 0x1b)
					{
						lastIs1B = true;
					}
					else
					{
						*buff++ = Text::SMSUtil::gsm2text[v];
					}
				}
			}
			break;
		default:
			buff = Text::StrUTF8_UTF16(buff, (const UTF8Char*)"Unknown", 0);
			break;
		}
	}
	*buff = 0;
	return buff;
}

UInt8 Text::SMSMessage::ParseIBCD(UInt8 byte)
{
	return (UInt8)((byte & 0xf) * 10 + (byte >> 4));
}

void Text::SMSMessage::ParseTimestamp(UnsafeArray<const UInt8> buff, NN<Data::DateTime> time)
{
	if ((buff[6] & 0x8) != 0)
	{
		time->SetValue((UInt16)(ParseIBCD(buff[0]) + 2000), ParseIBCD(buff[1]), ParseIBCD(buff[2]), ParseIBCD(buff[3]), ParseIBCD(buff[4]), ParseIBCD(buff[5]), 0, (Int8)-ParseIBCD(buff[6] & 0xf7));
	}
	else
	{
		time->SetValue((UInt16)(ParseIBCD(buff[0]) + 2000), ParseIBCD(buff[1]), ParseIBCD(buff[2]), ParseIBCD(buff[3]), ParseIBCD(buff[4]), ParseIBCD(buff[5]), 0, (Int8)ParseIBCD(buff[6] & 0xf7));
	}
}

UnsafeArray<UInt8> Text::SMSMessage::ToPDUPhone(UnsafeArray<UInt8> buff, UnsafeArray<const UTF16Char> phoneNum, OptOut<UInt8> byteSize, OptOut<UInt8> phoneSize)
{
	UTF16Char c;
	UInt8 oByteSize = 1;
	UInt8 oPhoneSize = 0;

	if (*phoneNum == '+')
	{
		phoneNum++;
		*buff++ = 0x91;
	}
	else
	{
		*buff++ = 0x81;
	}
	while ((c = *phoneNum++) != 0)
	{
		oByteSize++;
		*buff = c & 0xf;
		if ((c = *phoneNum++) != 0)
		{
			*buff++ |= (UInt8)((c << 4) & 0xff);
			oPhoneSize = (UInt8)(oPhoneSize + 2);
		}
		else
		{
			*buff++ |= 0xf0;
			oPhoneSize = (UInt8)(oPhoneSize + 1);
			break;
		}
	}
	byteSize.Set(oByteSize);
	phoneSize.Set(oPhoneSize);
	return buff;
}

Text::SMSMessage::SMSMessage(UnsafeArray<const UTF16Char> address, UnsafeArrayOpt<const UTF16Char> smsc, Optional<SMSUserData> ud)
{
	UnsafeArray<const UTF16Char> nnsmsc;
	if (Text::SMSUtil::IsPhone(address) && (!smsc.SetTo(nnsmsc) || Text::SMSUtil::IsPhone(nnsmsc)))
	{
		this->address = Text::StrCopyNew(address);
		if (smsc.SetTo(nnsmsc))
		{
			this->smsc = Text::StrCopyNew(nnsmsc);
		}
		else
		{
			this->smsc = nullptr;
		}
	}
	else
	{
		this->address = nullptr;
		this->smsc = nullptr;
	}
	this->ud = ud;
	this->mms = false;
	this->replyPath = false;
	this->statusReport = false;
	this->rejectDup = false;
	this->msgRef = 0;
}

Text::SMSMessage::~SMSMessage()
{
	this->ud.Delete();
	UnsafeArray<const UTF16Char> s;
	if (this->address.SetTo(s))
	{
		Text::StrDelNew(s);
	}
	if (this->smsc.SetTo(s))
	{
		Text::StrDelNew(s);
	}
}

void Text::SMSMessage::SetMessageTime(NN<Data::DateTime> msgTime)
{
	this->msgTime.SetValue(msgTime);
}

void Text::SMSMessage::SetMoreMsgToSend(Bool mms)
{
	this->mms = mms;
}

void Text::SMSMessage::SetReplyPath(Bool replyPath)
{
	this->replyPath = replyPath;
}

void Text::SMSMessage::SetStatusReport(Bool statusReport)
{
	this->statusReport = statusReport;
}

void Text::SMSMessage::SetRejectDuplicates(Bool rejectDup)
{
	this->rejectDup = rejectDup;
}

void Text::SMSMessage::SetMessageRef(UInt8 msgRef)
{
	this->msgRef = msgRef;
}

void Text::SMSMessage::GetMessageTime(NN<Data::DateTime> msgTime)
{
	msgTime->SetValue(this->msgTime);
}

UnsafeArrayOpt<const UTF16Char> Text::SMSMessage::GetAddress()
{
	return this->address;
}

UnsafeArrayOpt<const UTF16Char> Text::SMSMessage::GetSMSC()
{
	return this->smsc;
}

UnsafeArrayOpt<const UTF16Char> Text::SMSMessage::GetContent()
{
	NN<SMSUserData> ud;
	if (this->ud.SetTo(ud))
		return ud->GetMessage();
	return nullptr;
}

UIntOS Text::SMSMessage::ToSubmitPDU(UnsafeArray<UInt8> buff)
{
	NN<SMSUserData> ud;
	UnsafeArray<const UTF16Char> address;
	if (!this->ud.SetTo(ud) || !this->address.SetTo(address))
		return 0;
	UnsafeArray<UInt8> currPtr = buff;
	*currPtr++ = 0;
	currPtr[0] = 1;
	if (this->rejectDup)
		currPtr[0] |= 4;
	if (this->replyPath)
		currPtr[0] |= 128;
	if (ud->HasUDH())
		currPtr[0] |= 64;
	if (this->statusReport)
		currPtr[0] |= 32;
	currPtr[1] = this->msgRef;
	currPtr += 2;
	currPtr = ToPDUPhone(currPtr + 1, address, 0, currPtr[0]);
	*currPtr++ = 0;
	*currPtr++ = (UInt8)ud->GetDCS();
	currPtr += ud->GetBytes(currPtr);
	return (UIntOS)(currPtr - buff);
}

Optional<Text::SMSMessage> Text::SMSMessage::CreateFromPDU(UnsafeArray<const UInt8> pduBytes)
{
	UTF16Char sbuff[256];
	UnsafeArray<UTF16Char> sptr = sbuff;
	UnsafeArrayOpt<UTF16Char> smsc;
	UnsafeArray<UTF16Char> address;
	Data::DateTime msgTime;
	Optional<Text::SMSUserData> ud;
	Text::SMSMessage *msg;

	if (pduBytes[0] == 0)
	{
		smsc = nullptr;
		pduBytes++;
	}
	else
	{
		smsc = sptr;
		sptr = ParsePDUPhone(sptr, &pduBytes[1], pduBytes[0]) + 1;
		pduBytes += pduBytes[0] + 1;
	}
	if ((pduBytes[0] & 3) == 0) //SMS DELIVER
	{
		Bool mms = (pduBytes[0] & 4) != 0;
		Bool sri = (pduBytes[0] & 32) != 0;
		Bool udhi = (pduBytes[0] & 64) != 0;
		Bool rp = (pduBytes[0] & 128) != 0;
//		UInt8 pid;
		UInt8 dcs;
		UInt8 addrByteLen;

		pduBytes++;
		address = sptr;
		addrByteLen = *pduBytes++;
		addrByteLen = (UInt8)(1 + (addrByteLen >> 1) + (addrByteLen & 1));
		sptr = ParsePDUPhone(sptr, pduBytes, addrByteLen) + 1;
		pduBytes += addrByteLen;
		/*pid = **/pduBytes++;	//protocol (TP-PID)
		dcs = *pduBytes++;
		ParseTimestamp(pduBytes, msgTime);
		pduBytes += 7;
		ud = Text::SMSUserData::CreateSMSFromBytes(pduBytes, udhi, (Text::SMSUtil::DCS)(dcs & 0xc));
		NEW_CLASS(msg, Text::SMSMessage(address, smsc, ud));
		msg->SetMessageTime(msgTime);
		msg->SetMoreMsgToSend(mms);
		msg->SetReplyPath(rp);
		msg->SetStatusReport(sri);
		return msg;
	}
	else if ((pduBytes[0] & 3) == 1) //SMS SUBMIT
	{
		Bool rd = (pduBytes[0] & 4) != 0;
		UInt8 vpf = (pduBytes[0] & 24) >> 3;
		Bool srr = (pduBytes[0] & 32) != 0;
		Bool udhi = (pduBytes[0] & 64) != 0;
		Bool rp = (pduBytes[0] & 128) != 0;
		UInt8 mr;
//		UInt8 pid;
		UInt8 dcs;
		UInt8 addrByteLen;

		pduBytes++;
		mr = *pduBytes++;
		address = sptr;
		addrByteLen = *pduBytes++;
		addrByteLen = (UInt8)(1 + (addrByteLen >> 1) + (addrByteLen & 1));
		sptr = ParsePDUPhone(sptr, pduBytes, addrByteLen) + 1;
		pduBytes += addrByteLen;
		/*pid = **/pduBytes++;	//protocol (TP-PID)
		dcs = *pduBytes++;
		if (vpf == 2)
		{
			pduBytes++;
		}
		else if (vpf == 1 || vpf == 3)
		{
			pduBytes += 7;
		}
		ud = Text::SMSUserData::CreateSMSFromBytes(pduBytes, udhi, (Text::SMSUtil::DCS)(dcs & 0xc));
		NEW_CLASS(msg, Text::SMSMessage(address, smsc, ud));
		msg->SetRejectDuplicates(rd);
		msg->SetReplyPath(rp);
		msg->SetStatusReport(srr);
		msg->SetMessageRef(mr);
		return msg;
	}
	else if ((pduBytes[0] & 3) == 2) //SMS COMMAND
	{
		return nullptr;
	}
	else
	{
		return nullptr;
	}
	return nullptr;
}
