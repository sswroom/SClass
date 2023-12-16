#include "Stdafx.h"
#include "Data/ArrayListString.h"
#include "Net/RSS.h"
#include "Net/SNS/SNSRSS.h"
#include "Sync/MutexUsage.h"
#include "Text/HTMLUtil.h"
#include "Text/StringBuilderUTF8.h"

void Net::SNS::SNSRSS::CalcCRC(const UInt8 *buff, UOSInt size, UInt8 *hashVal)
{
	Sync::MutexUsage mutUsage(this->crcMut);
	this->crc.Clear();
	this->crc.Calc(buff, size);
	this->crc.GetValue(hashVal);
}

Net::SNS::SNSRSS::SNSRSS(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, Text::String *userAgent, Text::CString channelId, NotNullPtr<IO::LogTool> log)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = SCOPY_STRING(userAgent);
	this->channelId = Text::String::New(channelId);
	this->log = log;
	this->chDesc = 0;
	this->timeout = 30000;

	Net::RSS *rss;
	SNSItem *snsItem;
	Net::RSSItem *item;
	NEW_CLASS(rss, Net::RSS(this->channelId->ToCString(), this->userAgent, this->sockf, this->ssl, this->timeout, this->log));
	if (rss->GetTitle())
	{
		this->chName = rss->GetTitle()->Clone();
	}
	else
	{
		this->chName = this->channelId->Clone();
	}
	if (rss->GetDescription())
	{
		this->chDesc = rss->GetDescription()->Clone().Ptr();
	}
	UOSInt i = rss->GetCount();
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	while (i-- > 0)
	{
		item = rss->GetItem(i);
		if (item->descHTML)
		{
			Data::ArrayListStringNN imgList;
			sb.ClearStr();
			Text::HTMLUtil::HTMLGetText(this->encFact, item->description->v, item->description->leng, false, sb, &imgList);
			sb2.ClearStr();
			if (item->imgURL)
			{
				sb2.Append(item->imgURL);
			}
			Data::ArrayIterator<NotNullPtr<Text::String>> it = imgList.Iterator();
			NotNullPtr<Text::String> s;
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
			NotNullPtr<Text::String> nns;
			NotNullPtr<Text::String> s2;
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
	SDEL_STRING(this->userAgent);
	this->chName->Release();
	SDEL_STRING(this->chDesc);
	i = this->itemMap.GetCount();
	while (i-- > 0)
	{
		FreeItem(this->itemMap.GetItem(i));
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

NotNullPtr<Text::String> Net::SNS::SNSRSS::GetChannelId() const
{
	return this->channelId;
}

NotNullPtr<Text::String> Net::SNS::SNSRSS::GetName() const
{
	return this->chName;
}

UTF8Char *Net::SNS::SNSRSS::GetDirName(UTF8Char *dirName)
{
	UInt8 crcVal[4];
	dirName = Text::StrConcatC(dirName, UTF8STRC("RSS_"));
	this->CalcCRC(this->channelId->v, this->channelId->leng, crcVal);
	dirName = Text::StrHexBytes(dirName, crcVal, 4, 0);
	return dirName;
}

UOSInt Net::SNS::SNSRSS::GetCurrItems(NotNullPtr<Data::ArrayList<SNSItem*>> itemList)
{
	UOSInt initCnt = itemList->GetCount();
	itemList->AddAll(this->itemMap);
	return itemList->GetCount() - initCnt;
}

UTF8Char *Net::SNS::SNSRSS::GetItemShortId(UTF8Char *buff, SNSItem *item)
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
	SNSItem *snsItem;
	OSInt si;
	Net::RSSItem *item;
	Data::ArrayListString idList;
	Bool changed = false;
	UOSInt i;
	UOSInt j = this->itemMap.GetCount();
	idList.EnsureCapacity(j);
	i = 0;
	while (i < j)
	{
		idList.Add(this->itemMap.GetKey(i));
		i++;
	}

	Net::RSS *rss;
	NEW_CLASS(rss, Net::RSS(this->channelId->ToCString(), this->userAgent, this->sockf, this->ssl, this->timeout, this->log));
	i = rss->GetCount();
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	if (i > 0)
	{
		while (i-- > 0)
		{
			item = rss->GetItem(i);
			si = idList.SortedIndexOf(item->guid);
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
					Text::HTMLUtil::HTMLGetText(this->encFact, item->description->v, item->description->leng, false, sb, &imgList);
					sb2.ClearStr();
					if (item->imgURL)
					{
						sb2.Append(item->imgURL);
					}
					Data::ArrayIterator<NotNullPtr<Text::String>> it = imgList.Iterator();
					NotNullPtr<Text::String> s;
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
					NotNullPtr<Text::String> nns;
					NotNullPtr<Text::String> s2;
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
			snsItem = this->itemMap.Remove(idList.GetItem(i));
			FreeItem(snsItem);
			changed = true;
		}
	}
	else
	{
		DEL_CLASS(rss);
	}
	
	return changed;
}
