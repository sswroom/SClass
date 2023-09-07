#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/SocketFactory.h"
#include "Net/WhoisRecord.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Net::WhoisRecord::WhoisRecord(UInt32 recordIP)
{
	this->recordIP = recordIP;
	this->startIP = 0;
	this->endIP = 0;
}

Net::WhoisRecord::~WhoisRecord()
{
	LIST_FREE_STRING(&this->items);
}

void Net::WhoisRecord::AddItem(const UTF8Char *item, UOSInt itemLen)
{
	if (item[0] == 0 && this->items.GetCount() == 0)
	{
		return;
	}
	if (startIP == 0)
	{
		UOSInt i;
		UOSInt j;
		UTF8Char *sarr[4];
		Int32 bitCnt;
		UInt8 ip[4];
		if (Text::StrStartsWithICaseC(item, itemLen, UTF8STRC("inetnum:")))
		{
			Text::StringBuilderUTF8 sb;
			UTF8Char *sptr;
			sb.AppendC(&item[8], itemLen - 8);
			sb.Trim();
			sptr = sb.v;
			i = Text::StrIndexOfC(sptr, sb.GetLength(), UTF8STRC(" - "));
			if (i == INVALID_INDEX)
			{
				i = Text::StrIndexOfChar(sptr, '/');
				if (i != INVALID_INDEX)
				{
					*(Int32*)ip = 0;
					sptr[i] = 0;
					bitCnt = Text::StrToInt32(&sptr[i]);
					j = Text::StrSplit(sarr, 4, sptr, '.');
					while (j-- > 0)
					{
						ip[j] = Text::StrToUInt8(sarr[j]);
					}
					this->startIP = ReadUInt32(ip);
					this->endIP = BSWAPU32(ReadMUInt32(ip) + (UInt32)(1 << (32 - bitCnt)) - 1);
				}
				else
				{
					i = INVALID_INDEX;
				}
			}
			else
			{
				sptr[i] = 0;
				this->startIP = Net::SocketUtil::GetIPAddr(Text::CStringNN(sptr, i));
				this->endIP = Net::SocketUtil::GetIPAddr(Text::CStringNN(&sptr[i + 3], sb.GetLength() - i - 3));
			}
		}
		else if (Text::StrStartsWithICaseC(item, itemLen, UTF8STRC("NetRange:")))
		{
			Text::StringBuilderUTF8 sb;
			UTF8Char *sptr;
			sb.AppendC(&item[9], itemLen - 9);
			sb.Trim();
			sptr = sb.v;
			i = Text::StrIndexOfC(sptr, sb.GetLength(), UTF8STRC(" - "));
			if (i == INVALID_INDEX)
			{
				if (i != INVALID_INDEX)
				{
					*(Int32*)ip = 0;
					sptr[i] = 0;
					bitCnt = Text::StrToInt32(&sptr[i]);
					j = Text::StrSplit(sarr, 4, sptr, '.');
					while (j-- > 0)
					{
						ip[j] = Text::StrToUInt8(sarr[j]);
					}
					this->startIP = ReadUInt32(ip);
					this->endIP = BSWAPU32((UInt32)ReadMUInt32(ip) + (UInt32)(1 << (32 - bitCnt)) - 1);
				}
				else
				{
					i = INVALID_INDEX;
				}
			}
			else
			{
				sptr[i] = 0;
				this->startIP = Net::SocketUtil::GetIPAddr(Text::CStringNN(sptr, i));
				this->endIP = Net::SocketUtil::GetIPAddr(Text::CStringNN(&sptr[i + 3], sb.GetLength() - i - 3));
			}
		}
	}
	this->items.Add(Text::String::New(item, itemLen));
}

UOSInt Net::WhoisRecord::GetCount()
{
	return this->items.GetCount();
}

Text::String *Net::WhoisRecord::GetItem(UOSInt index)
{
	return this->items.GetItem(index);
}


UTF8Char *Net::WhoisRecord::GetNetworkName(UTF8Char *buff)
{
	UOSInt i;
	UOSInt j;
	Text::String *s;
	UTF8Char *sptr;
	i = 0;
	j = this->items.GetCount();
	while (i < j)
	{
		s = this->items.GetItem(i);
		if (Text::StrStartsWithICaseC(s->v, s->leng, UTF8STRC("netname:")))
		{
			sptr = Text::StrConcatC(buff, &s->v[8], s->leng - 8);
			return Text::StrTrimC(buff, (UOSInt)(sptr - buff));
		}
		i++;
	}
	return 0;
}

UTF8Char *Net::WhoisRecord::GetCountryCode(UTF8Char *buff)
{
	UOSInt i;
	UOSInt j;
	Text::String *s;
	UTF8Char *sptr;
	i = 0;
	j = this->items.GetCount();
	while (i < j)
	{
		s = this->items.GetItem(i);
		if (Text::StrStartsWithICaseC(s->v, s->leng, UTF8STRC("country:")))
		{
			sptr = Text::StrConcatC(buff, &s->v[8], s->leng - 8);
			return Text::StrTrimC(buff, (UOSInt)(sptr - buff));
		}
		i++;
	}
	return 0;
}

UInt32 Net::WhoisRecord::GetStartIP()
{
	if (this->startIP == 0)
		return this->recordIP;
	return this->startIP;
}

UInt32 Net::WhoisRecord::GetEndIP()
{
	if (this->endIP == 0)
		return this->recordIP;
	return this->endIP;
}
