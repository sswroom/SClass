#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/SMSUserData.h"

Text::SMSUserData::SMSUserData(const UTF16Char *msg, Text::SMSUtil::DCS dcs, const UInt8 *udh)
{
	this->msg = Text::StrCopyNew(msg);
	this->dcs = dcs;
	if (udh)
	{
		UInt32 len = (UInt32)udh[0] + 1;
		this->udh = MemAlloc(UInt8, len);
		MemCopyNO(this->udh, udh, len);
	}
	else
	{
		this->udh = 0;
	}
}

Text::SMSUserData::~SMSUserData()
{
	Text::StrDelNew(this->msg);
	if (this->udh)
	{
		MemFree(this->udh);
	}
}

Bool Text::SMSUserData::HasUDH()
{
	return this->udh != 0;
}

Text::SMSUtil::DCS Text::SMSUserData::GetDCS()
{
	return this->dcs;
}

UInt32 Text::SMSUserData::GetByteSize()
{
	UInt32 udhSize = 0;
	if (this->udh)
	{
		udhSize = (UInt32)this->udh[0] + 1;
	}
	if (this->dcs == Text::SMSUtil::DCS_GSM7BIT)
	{
		Text::SMSUtil::DCS dcs;
		UInt32 dataSize;
		Text::SMSUtil::GetTextInfo(this->msg, &dcs, &dataSize);
		return 1 + udhSize + Text::SMSUtil::GSMTextSize2DataSize(dataSize);
		
	}
	else if (this->dcs == Text::SMSUtil::DCS_UCS2)
	{
		return (UInt32)(1 + udhSize + Text::StrCharCnt(this->msg) * 2);
	}
	else
	{
		return 0;
	}
} //Including UDL

UInt32 Text::SMSUserData::GetBytes(UInt8 *bytes)
{
	Int32 i;
	UInt8 *currPtr;
	const UTF16Char *src = this->msg;
	UTF16Char c;
	if (this->dcs == Text::SMSUtil::DCS_GSM7BIT)
	{
		Int32 septetLeng = 0;
		Int32 t = 0;
		UInt16 v;
		currPtr = bytes + 1;
		if (udh)
		{
			t = i = udh[0] + 1;
			while (i-- > 0)
			{
				currPtr[i] = udh[i];
			}
			currPtr += t;
			septetLeng = (t / 7) << 3;
			if (t % 7)
			{
				t = (t % 7) + 1;
			}
			else
			{
				t = 0;
			}
			septetLeng += t;
			*currPtr = 0;
		}
		while ((c = *src++) != 0)
		{
			v = Text::SMSUtil::text2gsm[c];
			if (v >= 0x100)
			{
				septetLeng += 2;
				switch (t)
				{
				case 0:
					*currPtr++ = (UInt8)(v >> 8);
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 7));
					*currPtr++ = (UInt8)((v >> 1) & 0x3f);
					t = 2;
					break;
				case 1:
					currPtr[-1] = (UInt8)(currPtr[-1] | ((v >> 1) & 0x80));
					*currPtr++ = (UInt8)(v >> 9);
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 6));
					*currPtr++ = (UInt8)((v >> 2) & 0x1f);
					t = 3;
					break;
				case 2:
					currPtr[-1] = (UInt8)(currPtr[-1] | ((v >> 2) & 0xc0));
					*currPtr++ = (UInt8)(v >> 10);
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 5));
					*currPtr++ = (UInt8)((v >> 3) & 0x0f);
					t = 4;
					break;
				case 3:
					currPtr[-1] = (UInt8)(currPtr[-1] | ((v >> 3) & 0xe0));
					*currPtr++ = (UInt8)(v >> 11);
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 4));
					*currPtr++ = (UInt8)((v >> 4) & 0x07);
					t = 5;
					break;
				case 4:
					currPtr[-1] = (UInt8)(currPtr[-1] | ((v >> 4) & 0xf0));
					*currPtr++ = (UInt8)(v >> 12);
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 3));
					*currPtr++ = (UInt8)((v >> 5) & 0x03);
					t = 6;
					break;
				case 5:
					currPtr[-1] = (UInt8)(currPtr[-1] | ((v >> 5) & 0xf8));
					*currPtr++ = (UInt8)(v >> 13);
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 2));
					*currPtr++ = (UInt8)((v >> 6) & 0x01);
					t = 7;
					break;
				case 6:
					currPtr[-1] = (UInt8)(currPtr[-1] | ((v >> 6) & 0xfc));
					*currPtr++ = (UInt8)(v >> 14);
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 1));
					t = 0;
					break;
				case 7:
					currPtr[-1] = (UInt8)(currPtr[-1] | ((v >> 7) & 0xfe));
					*currPtr++ = (UInt8)(v & 0x7f);
					t = 1;
					break;
				}
			}
			else
			{
				septetLeng++;
				switch (t)
				{
				case 0:
					*currPtr++ = (UInt8)(v & 0xff);
					t = 1;
					break;
				case 1:
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 7));
					*currPtr++ = (UInt8)(v >> 1);
					t = 2;
					break;
				case 2:
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 6));
					*currPtr++ = (UInt8)(v >> 2);
					t = 3;
					break;
				case 3:
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 5));
					*currPtr++ = (UInt8)(v >> 3);
					t = 4;
					break;
				case 4:
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 4));
					*currPtr++ = (UInt8)(v >> 4);
					t = 5;
					break;
				case 5:
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 3));
					*currPtr++ = (UInt8)(v >> 5);
					t = 6;
					break;
				case 6:
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 2));
					*currPtr++ = (UInt8)(v >> 6);
					t = 7;
					break;
				case 7:
					currPtr[-1] = (UInt8)(currPtr[-1] | (v << 1));
					t = 0;
					break;
				}
			}
		}
		bytes[0] = (UInt8)septetLeng;
		return (UInt32)(currPtr - bytes);
	}
	else if (this->dcs == Text::SMSUtil::DCS_UCS2)
	{
		currPtr = bytes + 1;
		if (this->udh)
		{
			i = udh[0] + 1;
			while (i-- > 0)
			{
				currPtr[i] = udh[i];
			}
			currPtr += udh[0] + 1;
		}
		while ((c = *src++) != 0)
		{
			currPtr[0] = (UInt8)(c >> 8);
			currPtr[1] = (UInt8)c;
			currPtr += 2;
		}
		bytes[0] = (UInt8)(currPtr - bytes - 1);
		return (UInt32)(currPtr - bytes);
	}
	return 0;
}

const UInt8 *Text::SMSUserData::GetUDH()
{
	return this->udh;
}
const UTF16Char *Text::SMSUserData::GetMessage()
{
	return this->msg;
}

UOSInt Text::SMSUserData::CreateSMSs(NN<Data::ArrayListNN<Text::SMSUserData>> smsList, const UTF8Char *osmsMessage)
{
	UInt8 udh[6];
	UTF16Char sbuff[161];
	Text::SMSUtil::DCS dcs;
	UInt32 msgLeng;
	UTF16Char *u16MsgPtr;
	UTF16Char *u16Msg;
	UOSInt u16Len = Text::StrUTF8_UTF16Cnt(osmsMessage);
	u16MsgPtr = MemAlloc(UTF16Char, u16Len + 1);
	u16Msg = u16MsgPtr;
	Text::StrUTF8_UTF16(u16MsgPtr, osmsMessage, 0);
	Text::SMSUtil::GetTextInfo(u16Msg, &dcs, &msgLeng);
	UOSInt cnt = 0;
	UInt8 refId = (UInt8)(u16Msg[0] & 0xff);
	UInt8 totalCnt;
	NN<Text::SMSUserData> ud;
	const UTF16Char *sptr;

	if (dcs == Text::SMSUtil::DCS_UCS2)
	{
		if (msgLeng > 140)
		{
			UOSInt charCnt;
			charCnt = Text::StrCharCnt(u16Msg);
			if (charCnt % 67)
			{
				totalCnt = (UInt8)((charCnt / 67) + 1);
			}
			else
			{
				totalCnt = (UInt8)(charCnt / 67);
			}
			while (*u16Msg)
			{
				charCnt = Text::StrCharCnt(u16Msg);
				if (charCnt > 67)
				{
					charCnt = 67;
				}
				MemCopyNO(sbuff, u16Msg, sizeof(WChar) * charCnt);
				sbuff[charCnt] = 0;

				udh[0] = 5;
				udh[1] = 0;
				udh[2] = 3;
				udh[3] = refId;
				udh[4] = totalCnt;
				udh[5] = (UInt8)(cnt + 1);
				NEW_CLASSNN(ud, Text::SMSUserData(sbuff, dcs, udh));
				smsList->Add(ud);
				cnt++;
				u16Msg = &u16Msg[charCnt];
			}
			MemFree(u16MsgPtr);
			return cnt;
		}
		else
		{
			NEW_CLASSNN(ud, Text::SMSUserData(u16Msg, dcs, 0));
			smsList->Add(ud);
			MemFree(u16MsgPtr);
			return 1;
		}
	}
	else if (dcs == Text::SMSUtil::DCS_GSM7BIT)
	{
		if (msgLeng > 160)
		{
			UOSInt charCnt;
			charCnt = Text::StrCharCnt(u16Msg);
			/////////////////////////////////
			if (charCnt % 153)
			{
				totalCnt = (UInt8)((charCnt / 153) + 1);
			}
			else
			{
				totalCnt = (UInt8)(charCnt / 153);
			}
			while (*u16Msg)
			{
				sptr = Text::SMSUtil::TrimGSMText(sbuff, u16Msg, 134);

				udh[0] = 5;
				udh[1] = 0;
				udh[2] = 3;
				udh[3] = refId;
				udh[4] = totalCnt;
				udh[5] = (UInt8)(cnt + 1);
				NEW_CLASSNN(ud, Text::SMSUserData(sbuff, dcs, udh));
				smsList->Add(ud);
				cnt++;
				u16Msg = &u16Msg[sptr - sbuff];
			}
			MemFree(u16MsgPtr);
			return cnt;
		}
		else
		{
			NEW_CLASSNN(ud, Text::SMSUserData(u16Msg, dcs, 0));
			smsList->Add(ud);
			MemFree(u16MsgPtr);
			return 1;
		}
	}
	else
	{
		MemFree(u16MsgPtr);
		return 0;
	}
}

Optional<Text::SMSUserData> Text::SMSUserData::CreateSMSTrim(const UTF16Char *smsMessage, UInt8 *udh)
{
	UTF16Char sbuff[161];
	UInt32 udhSize;
	Text::SMSUtil::DCS dcs;
	UInt32 msgLeng;
	if (udh == 0)
	{
		udhSize = 0;
	}
	else
	{
		udhSize = (UInt32)udh[0] + 1;
	}
	Text::SMSUserData *ud;
	Text::SMSUtil::GetTextInfo(smsMessage, &dcs, &msgLeng);
	if (dcs == Text::SMSUtil::DCS_UCS2)
	{
		if (msgLeng > 140 - udhSize)
		{
			MemCopyNO(sbuff, smsMessage, sizeof(WChar) * ((140 - udhSize) >> 1));
			sbuff[(140 - udhSize) >> 1] = 0;
			NEW_CLASS(ud, Text::SMSUserData(sbuff, dcs, udh));
			return ud;
		}
		else
		{
			NEW_CLASS(ud, Text::SMSUserData(smsMessage, dcs, udh));
			return ud;
		}
	}
	else if (dcs == Text::SMSUtil::DCS_GSM7BIT)
	{
		UInt32 dataLeng = Text::SMSUtil::GSMTextSize2DataSize(msgLeng);
		if (dataLeng > 140 - udhSize)
		{
			Text::SMSUtil::TrimGSMText(sbuff, smsMessage, 140 - udhSize);
			NEW_CLASS(ud, Text::SMSUserData(sbuff, dcs, udh));
			return ud;
		}
		else
		{
			NEW_CLASS(ud, Text::SMSUserData(smsMessage, dcs, udh));
			return ud;
		}
	}
	else
	{
		return 0;
	}
}

Optional<Text::SMSUserData> Text::SMSUserData::CreateSMSFromBytes(const UInt8 *bytes, Bool hasUDH, Text::SMSUtil::DCS dcs)
{
	UTF16Char sbuff[161];
	UTF16Char *sptr = sbuff;
	Int32 i;
	const UInt8 *srcPtr = &bytes[1];
	UInt8 size = bytes[0];
	const UInt8 *udh = 0;
	NN<Text::SMSUserData> ud;
	Bool lastIs1B;
	Int32 v = 0;

	if (dcs == Text::SMSUtil::DCS_GSM7BIT)
	{
		i = 0;
		if (hasUDH)
		{
			udh = srcPtr;
			srcPtr += udh[0] + 1;
			i = (udh[0] + 1) * 8;
			if (i % 7)
			{
				size = (UInt8)(size - i / 7 + 1);
				i = (i % 7) + 1;
			}
			else
			{
				size = (UInt8)(size - i / 7);
			}
		}
		lastIs1B = false;
		while (size > 0)
		{
			switch (i)
			{
			case 0:
				v = srcPtr[0] & 0x7f;
				i = 1;
				break;
			case 1:
				v = ((srcPtr[0] & 0x80) >> 7) | ((srcPtr[1] << 1) & 0x7f);
				srcPtr++;
				i = 2;
				break;
			case 2:
				v = ((srcPtr[0] & 0xc0) >> 6) | ((srcPtr[1] << 2) & 0x7f);
				srcPtr++;
				i = 3;
				break;
			case 3:
				v = ((srcPtr[0] & 0xe0) >> 5) | ((srcPtr[1] << 3) & 0x7f);
				srcPtr++;
				i = 4;
				break;
			case 4:
				v = ((srcPtr[0] & 0xf0) >> 4) | ((srcPtr[1] << 4) & 0x7f);
				srcPtr++;
				i = 5;
				break;
			case 5:
				v = ((srcPtr[0] & 0xf8) >> 3) | ((srcPtr[1] << 5) & 0x7f);
				srcPtr++;
				i = 6;
				break;
			case 6:
				v = ((srcPtr[0] & 0xfc) >> 2) | ((srcPtr[1] << 6) & 0x7f);
				srcPtr++;
				i = 7;
				break;
			case 7:
				v = (srcPtr[0] & 0xfe) >> 1;
				srcPtr++;
				i = 0;
				break;
				
			}
			size--;
			if (lastIs1B)
			{
				*sptr++ = Text::SMSUtil::gsm1b2text[v];
				lastIs1B = false;
			}
			else if (v == 0x1b)
			{
				lastIs1B = true;
			}
			else
			{
				*sptr++ = Text::SMSUtil::gsm2text[v];
			}
		}
		*sptr = 0;
		NEW_CLASSNN(ud, Text::SMSUserData(sbuff, Text::SMSUtil::DCS_GSM7BIT, udh));
		return ud;
	}
	else if (dcs == Text::SMSUtil::DCS_UCS2)
	{
		if (hasUDH)
		{
			udh = srcPtr;
			srcPtr += udh[0] + 1;
			size = (UInt8)(size - udh[0] + 1);
		}
		while (size >= 2)
		{
			*sptr++ = (UTF16Char)((((UTF16Char)srcPtr[0]) << 8) | srcPtr[1]);
			size = (UInt8)(size - 2);
			srcPtr += 2;
		}
		*sptr = 0;
		NEW_CLASSNN(ud, Text::SMSUserData(sbuff, Text::SMSUtil::DCS_UCS2, udh));
		return ud;
	}
	else
	{
		return 0;
	}
}
