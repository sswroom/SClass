#include "Stdafx.h"
#include "Data/ArrayListString.h"
#include "Net/RSS.h"
#include "Net/SNS/SNSRSS.h"
#include "Sync/MutexUsage.h"
#include "Text/HTMLUtil.h"
#include "Text/StringBuilderUTF8.h"

void Net::SNS::SNSRSS::CalcCRC(UnsafeArray<const UInt8> buff, UOSInt size, UnsafeArray<UInt8> hashVal)
{
	Sync::MutexUsage mutUsage(this->crcMut);
	this->crc.Clear();
	this->crc.Calc(buff, size);
	this->crc.GetValue(hashVal);
}

Net::SNS::SNSRSS::SNSRSS(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent, Text::CStringNN channelId, NN<IO::LogTool> log)
{
	this->clif = clif;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = Text::String::CopyOrNull(userAgent);
	this->channelId = Text::String::New(channelId);
	this->log = log;
	this->chDesc = 0;
	this->timeout = 30000;

	Net::RSS *rss;
	NN<Text::String> s;
	NN<SNSItem> snsItem;
	NN<Net::RSSItem> item;
	NEW_CLASS(rss, Net::RSS(this->channelId->ToCString(), this->userAgent, this->clif, this->ssl, this->timeout, this->log));
	if (rss->GetTitle().SetTo(s))
	{
		this->chName = s->Clone();
	}
	else
	{
		this->chName = this->channelId->Clone();
	}
	if (rss->GetDescription().SetTo(s))
	{
		this->chDesc = s->Clone();
	}
	UOSInt i = rss->GetCount();
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	while (i-- > 0)
	{
		item = rss->GetItemNoCheck(i);
		if (item->descHTML)
		{
			Data::ArrayListStringNN imgList;
			sb.ClearStr();
			s = Text::String::OrEmpty(item->description);
			Text::HTMLUtil::HTMLGetText(this->encFact, s->v, s->leng, false, sb, &imgList);
			sb2.ClearStr();
			if (item->imgURL.SetTo(s))
			{
				sb2.Append(s);
			}
			Data::ArrayIterator<NN<Text::String>> it = imgList.Iterator();
			NN<Text::String> s;
			while (it.HasNext())
			{
				s = it.Next();
				if (sb2.GetLength() > 0)
				{
					sb2.AppendUTF8Char(' ');
				}
				sb2.Append(s);
				s->Release();
			}
			NN<Text::String> nns;
			NN<Text::String> s2;
			nns = Text::String::New(sb.ToString(), sb.GetLength());
			s2 = Text::String::New(sb2.ToString(), sb2.GetLength());
			snsItem = CreateItem(Text::String::OrEmpty(item->guid), item->pubDate.ToTicks(), item->title, nns, item->link, s2.Ptr(), 0);
			nns->Release();
			s2->Release();
		}
		else
		{
			snsItem = CreateItem(Text::String::OrEmpty(item->guid), item->pubDate.ToTicks(), item->title, Text::String::OrEmpty(item->description), item->link, item->imgURL, 0);
		}
		this->itemMap.Put(item->guid, snsItem);
	}
	DEL_CLASS(rss);
}

Net::SNS::SNSRSS::~SNSRSS()
{
	UOSInt i;
	OPTSTR_DEL(this->userAgent);
	this->chName->Release();
	OPTSTR_DEL(this->chDesc);
	i = this->itemMap.GetCount();
	while (i-- > 0)
	{
		FreeItem(this->itemMap.GetItemNoCheck(i));
	}
}

Bool Net::SNS::SNSRSS::IsError()
{
	return this->itemMap.GetCount() <= 0;
}

Net::SNS::SNSControl::SNSType Net::SNS::SNSRSS::GetSNSType()
{
	return Net::SNS::SNSControl::ST_RSS;
}

NN<Text::String> Net::SNS::SNSRSS::GetChannelId() const
{
	return this->channelId;
}

NN<Text::String> Net::SNS::SNSRSS::GetName() const
{
	return this->chName;
}

UnsafeArray<UTF8Char> Net::SNS::SNSRSS::GetDirName(UnsafeArray<UTF8Char> dirName)
{
	UInt8 crcVal[4];
	dirName = Text::StrConcatC(dirName, UTF8STRC("RSS_"));
	this->CalcCRC(this->channelId->v, this->channelId->leng, crcVal);
	dirName = Text::StrHexBytes(dirName, crcVal, 4, 0);
	return dirName;
}

UOSInt Net::SNS::SNSRSS::GetCurrItems(NN<Data::ArrayListNN<SNSItem>> itemList)
{
	UOSInt initCnt = itemList->GetCount();
	itemList->AddAll(this->itemMap);
	return itemList->GetCount() - initCnt;
}

UnsafeArray<UTF8Char> Net::SNS::SNSRSS::GetItemShortId(UnsafeArray<UTF8Char> buff, NN<SNSItem> item)
{
	UInt8 crcVal[4];
	this->CalcCRC(item->id->v, item->id->leng, crcVal);
	return Text::StrHexBytes(buff, crcVal, 4, 0);
}

Int32 Net::SNS::SNSRSS::GetMinIntevalMS()
{
	return 15 * 60000;
}

Bool Net::SNS::SNSRSS::Reload()
{
	NN<SNSItem> snsItem;
	OSInt si;
	NN<Net::RSSItem> item;
	Data::ArrayListStringNN idList;
	Bool changed = false;
	UOSInt i;
	UOSInt j = this->itemMap.GetCount();
	idList.EnsureCapacity(j);
	i = 0;
	while (i < j)
	{
		idList.Add(Text::String::OrEmpty(this->itemMap.GetKey(i)));
		i++;
	}

	Net::RSS *rss;
	NN<Text::String> nns;
	NEW_CLASS(rss, Net::RSS(this->channelId->ToCString(), this->userAgent, this->clif, this->ssl, this->timeout, this->log));
	i = rss->GetCount();
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	if (i > 0)
	{
		while (i-- > 0)
		{
			item = rss->GetItemNoCheck(i);
			si = idList.SortedIndexOf(Text::String::OrEmpty(item->guid));
			if (si >= 0)
			{
				idList.RemoveAt((UOSInt)si);
			}
			else
			{
				if (item->descHTML)
				{
					Data::ArrayListStringNN imgList;
					sb.ClearStr();
					nns = Text::String::OrEmpty(item->description);
					Text::HTMLUtil::HTMLGetText(this->encFact, nns->v, nns->leng, false, sb, &imgList);
					sb2.ClearStr();
					if (item->imgURL.SetTo(nns))
					{
						sb2.Append(nns);
					}
					Data::ArrayIterator<NN<Text::String>> it = imgList.Iterator();
					NN<Text::String> s;
					while (it.HasNext())
					{
						s = it.Next();
						if (sb2.GetLength() > 0)
						{
							sb2.AppendUTF8Char(' ');
						}
						sb2.Append(s);
						s->Release();
					}
					NN<Text::String> s2;
					nns = Text::String::New(sb.ToString(), sb.GetLength());
					s2 = Text::String::New(sb2.ToString(), sb2.GetLength());
					snsItem = CreateItem(Text::String::OrEmpty(item->guid), item->pubDate.ToTicks(), item->title, nns, item->link, s2.Ptr(), 0);
					nns->Release();
					s2->Release();
				}
				else
				{
					snsItem = CreateItem(Text::String::OrEmpty(item->guid), item->pubDate.ToTicks(), item->title, Text::String::OrEmpty(item->description), item->link, item->imgURL, 0);
				}
				this->itemMap.Put(item->guid, snsItem);
				changed = true;
			}
		}
		DEL_CLASS(rss);

		i = idList.GetCount();
		while (i-- > 0)
		{
			if (this->itemMap.Remove(idList.GetItem(i)).SetTo(snsItem))
			{
				FreeItem(snsItem);
				changed = true;
			}
		}
	}
	else
	{
		DEL_CLASS(rss);
	}
	
	return changed;
}
