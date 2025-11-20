#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
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
	this->items.FreeAll();
}

void Net::WhoisRecord::AddItem(UnsafeArray<const UTF8Char> item, UOSInt itemLen)
{
	if (item[0] == 0 && this->items.GetCount() == 0)
	{
		return;
	}
	if (startIP == 0)
	{
		UOSInt i;
		UOSInt j;
		UnsafeArray<UTF8Char> sarr[4];
		Int32 bitCnt;
		UInt8 ip[4];
		if (Text::StrStartsWithICaseC(item, itemLen, UTF8STRC("inetnum:")))
		{
			Text::StringBuilderUTF8 sb;
			UnsafeArray<UTF8Char> sptr;
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
			UnsafeArray<UTF8Char> sptr;
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

UOSInt Net::WhoisRecord::GetCount() const
{
	return this->items.GetCount();
}

Optional<Text::String> Net::WhoisRecord::GetItem(UOSInt index) const
{
	return this->items.GetItem(index);
}

Data::ArrayIterator<NN<Text::String>> Net::WhoisRecord::Iterator() const
{
	return this->items.Iterator();
}

UnsafeArrayOpt<UTF8Char> Net::WhoisRecord::GetNetworkName(UnsafeArray<UTF8Char> buff)
{
	NN<Text::String> s;
	UnsafeArray<UTF8Char> sptr;
	Data::ArrayIterator<NN<Text::String>> it = this->items.Iterator();
	while (it.HasNext())
	{
		s = it.Next();
		if (Text::StrStartsWithICaseC(s->v, s->leng, UTF8STRC("netname:")))
		{
			sptr = Text::StrConcatC(buff, &s->v[8], s->leng - 8);
			return Text::StrTrimC(buff, (UOSInt)(sptr - buff));
		}
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> Net::WhoisRecord::GetCountryCode(UnsafeArray<UTF8Char> buff)
{
	NN<Text::String> s;
	UnsafeArray<UTF8Char> sptr;
	Data::ArrayIterator<NN<Text::String>> it = this->items.Iterator();
	while (it.HasNext())
	{
		s = it.Next();
		if (Text::StrStartsWithICaseC(s->v, s->leng, UTF8STRC("country:")))
		{
			sptr = Text::StrConcatC(buff, &s->v[8], s->leng - 8);
			return Text::StrTrimC(buff, (UOSInt)(sptr - buff));
		}
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
