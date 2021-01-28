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
		Int32 len = udh[0] + 1;
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
Int32 Text::SMSUserData::GetByteSize()
{
	Int32 udhSize = 0;
	if (this->udh)
	{
		udhSize = this->udh[0] + 1;
	}
	if (this->dcs == Text::SMSUtil::DCS_GSM7BIT)
	{
		Text::SMSUtil::DCS dcs;
		Int32 dataSize;
		Text::SMSUtil::GetTextInfo(this->msg, &dcs, &dataSize);
		return 1 + udhSize + Text::SMSUtil::GSMTextSize2DataSize(dataSize);
		
	}
	else if (this->dcs == Text::SMSUtil::DCS_UCS2)
	{
		return (Int32)(1 + udhSize + Text::StrCharCnt(this->msg) * 2);
	}
	else
	{
		return 0;
	}
} //Including UDL

Int32 Text::SMSUserData::GetBytes(UInt8 *bytes)
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
					*currPtr++ = (v >> 8);
					currPtr[-1] |= (UInt8)(v << 7);
					*currPtr++ = (v >> 1) & 0x3f;
					t = 2;
					break;
				case 1:
					currPtr[-1] |= (v >> 1) & 0x80;
					*currPtr++ = (v >> 9);
					currPtr[-1] |= (v << 6);
					*currPtr++ = (v >> 2) & 0x1f;
					t = 3;
					break;
				case 2:
					currPtr[-1] |= (v >> 2) & 0xc0;
					*currPtr++ = (v >> 10);
					currPtr[-1] |= (v << 5);
					*currPtr++ = (v >> 3) & 0x0f;
					t = 4;
					break;
				case 3:
					currPtr[-1] |= (v >> 3) & 0xe0;
					*currPtr++ = (v >> 11);
					currPtr[-1] |= (v << 4);
					*currPtr++ = (v >> 4) & 0x07;
					t = 5;
					break;
				case 4:
					currPtr[-1] |= (v >> 4) & 0xf0;
					*currPtr++ = (v >> 12);
					currPtr[-1] |= (v << 3);
					*currPtr++ = (v >> 5) & 0x03;
					t = 6;
					break;
				case 5:
					currPtr[-1] |= (v >> 5) & 0xf8;
					*currPtr++ = (v >> 13);
					currPtr[-1] |= (v << 2);
					*currPtr++ = (v >> 6) & 0x01;
					t = 7;
					break;
				case 6:
					currPtr[-1] |= (v >> 6) & 0xfc;
					*currPtr++ = (v >> 14);
					currPtr[-1] |= (v << 1);
					t = 0;
					break;
				case 7:
					currPtr[-1] |= (v >> 7) & 0xfe;
					*currPtr++ = v & 0x7f;
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
					*currPtr++ = v & 0xff;
					t = 1;
					break;
				case 1:
					currPtr[-1] |= (v << 7);
					*currPtr++ = (v >> 1);
					t = 2;
					break;
				case 2:
					currPtr[-1] |= (v << 6);
					*currPtr++ = (v >> 2);
					t = 3;
					break;
				case 3:
					currPtr[-1] |= (v << 5);
					*currPtr++ = (v >> 3);
					t = 4;
					break;
				case 4:
					currPtr[-1] |= (v << 4);
					*currPtr++ = (v >> 4);
					t = 5;
					break;
				case 5:
					currPtr[-1] |= (v << 3);
					*currPtr++ = (v >> 5);
					t = 6;
					break;
				case 6:
					currPtr[-1] |= (v << 2);
					*currPtr++ = (v >> 6);
					t = 7;
					break;
				case 7:
					currPtr[-1] |= (v << 1);
					t = 0;
					break;
				}
			}
		}
		bytes[0] = (UInt8)septetLeng;
		return (Int32)(currPtr - bytes);
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
			currPtr[0] = c >> 8;
			currPtr[1] = (UInt8)c;
			currPtr += 2;
		}
		bytes[0] = (UInt8)(currPtr - bytes - 1);
		return (Int32)(currPtr - bytes);
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

OSInt Text::SMSUserData::CreateSMSs(Data::ArrayList<Text::SMSUserData*> *smsList, const UTF8Char *osmsMessage)
{
	UInt8 udh[6];
	UTF16Char sbuff[161];
	Text::SMSUtil::DCS dcs;
	Int32 msgLeng;
	UTF16Char *u16MsgPtr;
	UTF16Char *u16Msg;
	OSInt u16Len = Text::StrUTF8_UTF16Cnt(osmsMessage, -1);
	u16MsgPtr = MemAlloc(UTF16Char, u16Len + 1);
	u16Msg = u16MsgPtr;
	Text::StrUTF8_UTF16(u16MsgPtr, osmsMessage, -1, 0);
	Text::SMSUtil::GetTextInfo(u16Msg, &dcs, &msgLeng);
	OSInt cnt = 0;
	UInt8 refId = (UInt8)(u16Msg[0] & 0xff);
	UInt8 totalCnt;
	Text::SMSUserData *ud;
	const UTF16Char *sptr;

	if (dcs == Text::SMSUtil::DCS_UCS2)
	{
		if (msgLeng > 140)
		{
			OSInt charCnt;
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
				NEW_CLASS(ud, Text::SMSUserData(sbuff, dcs, udh));
				smsList->Add(ud);
				cnt++;
				u16Msg = &u16Msg[charCnt];
			}
			MemFree(u16MsgPtr);
			return cnt;
		}
		else
		{
			NEW_CLASS(ud, Text::SMSUserData(u16Msg, dcs, 0));
			smsList->Add(ud);
			MemFree(u16MsgPtr);
			return 1;
		}
	}
	else if (dcs == Text::SMSUtil::DCS_GSM7BIT)
	{
		if (msgLeng > 160)
		{
			OSInt charCnt;
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
				NEW_CLASS(ud, Text::SMSUserData(sbuff, dcs, udh));
				smsList->Add(ud);
				cnt++;
				u16Msg = &u16Msg[sptr - sbuff];
			}
			MemFree(u16MsgPtr);
			return cnt;
		}
		else
		{
			NEW_CLASS(ud, Text::SMSUserData(u16Msg, dcs, 0));
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

Text::SMSUserData *Text::SMSUserData::CreateSMSTrim(const UTF16Char *smsMessage, UInt8 *udh)
{
	UTF16Char sbuff[161];
	Int32 udhSize;
	Text::SMSUtil::DCS dcs;
	Int32 msgLeng;
	if (udh == 0)
	{
		udhSize = 0;
	}
	else
	{
		udhSize = udh[0] + 1;
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
		Int32 dataLeng = Text::SMSUtil::GSMTextSize2DataSize(msgLeng);
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

Text::SMSUserData *Text::SMSUserData::CreateSMSFromBytes(const UInt8 *bytes, Bool hasUDH, Text::SMSUtil::DCS dcs)
{
	UTF16Char sbuff[161];
	UTF16Char *sptr = sbuff;
	Int32 i;
	const UInt8 *srcPtr = &bytes[1];
	UInt8 size = bytes[0];
	const UInt8 *udh = 0;
	Text::SMSUserData *ud;
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
				size -= i / 7 + 1;
				i = (i % 7) + 1;
			}
			else
			{
				size -= i / 7;
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
		NEW_CLASS(ud, Text::SMSUserData(sbuff, Text::SMSUtil::DCS_GSM7BIT, udh));
		return ud;
	}
	else if (dcs == Text::SMSUtil::DCS_UCS2)
	{
		if (hasUDH)
		{
			udh = srcPtr;
			srcPtr += udh[0] + 1;
			size -= udh[0] + 1;
		}
		while (size >= 2)
		{
			*sptr++ = (((WChar)srcPtr[0]) << 8) | srcPtr[1];
			size -= 2;
			srcPtr += 2;
		}
		*sptr = 0;
		NEW_CLASS(ud, Text::SMSUserData(sbuff, Text::SMSUtil::DCS_UCS2, udh));
		return ud;
	}
	else
	{
		return 0;
	}
}
