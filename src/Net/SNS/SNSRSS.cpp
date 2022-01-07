#include "Stdafx.h"
#include "Data/ArrayListString.h"
#include "Net/RSS.h"
#include "Net/SNS/SNSRSS.h"
#include "Sync/MutexUsage.h"
#include "Text/HTMLUtil.h"
#include "Text/StringBuilderUTF8.h"

void Net::SNS::SNSRSS::CalcCRC(const UInt8 *buff, UOSInt size, UInt8 *hashVal)
{
	Sync::MutexUsage(this->crcMut);
	this->crc->Clear();
	this->crc->Calc(buff, size);
	this->crc->GetValue(hashVal);
}

Net::SNS::SNSRSS::SNSRSS(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, const UTF8Char *userAgent, const UTF8Char *channelId)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = userAgent?Text::StrCopyNew(userAgent):0;
	this->channelId = Text::String::NewNotNull(channelId);
	this->chName = 0;
	this->chDesc = 0;
	NEW_CLASS(this->itemMap, Data::FastStringMap<SNSItem*>());
	NEW_CLASS(this->crc, Crypto::Hash::CRC32R());
	NEW_CLASS(this->crcMut, Sync::Mutex());

	Net::RSS *rss;
	SNSItem *snsItem;
	Net::RSSItem *item;
	NEW_CLASS(rss, Net::RSS(this->channelId->v, this->userAgent, this->sockf, this->ssl));
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
		this->chDesc = rss->GetDescription()->Clone();
	}
	UOSInt i = rss->GetCount();
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	while (i-- > 0)
	{
		item = rss->GetItem(i);
		if (item->descHTML)
		{
			Data::ArrayList<Text::String *> imgList;
			sb.ClearStr();
			Text::HTMLUtil::HTMLGetText(this->encFact, item->description->v, item->description->leng, false, &sb, &imgList);
			sb2.ClearStr();
			if (item->imgURL)
			{
				sb2.Append(item->imgURL);
			}
			UOSInt j = 0;
			UOSInt k = imgList.GetCount();
			Text::String *s;
			while (j < k)
			{
				s = imgList.GetItem(j);
				if (sb2.GetLength() > 0)
				{
					sb2.AppendChar(' ', 1);
				}
				sb2.Append(s);
				s->Release();
				j++;
			}
			Text::String *s2;
			s = Text::String::New(sb.ToString(), sb.GetLength());
			s2 = Text::String::New(sb2.ToString(), sb2.GetLength());
			snsItem = CreateItem(item->guid, item->pubDate->ToTicks(), item->title, s, item->link, s2, 0);
			s->Release();
			s2->Release();
		}
		else
		{
			snsItem = CreateItem(item->guid, item->pubDate->ToTicks(), item->title, item->description, item->link, item->imgURL, 0);
		}
		this->itemMap->Put(item->guid, snsItem);
	}
	DEL_CLASS(rss);
}

Net::SNS::SNSRSS::~SNSRSS()
{
	UOSInt i;
	SDEL_TEXT(this->userAgent);
	SDEL_STRING(this->chName);
	SDEL_STRING(this->chDesc);
	i = this->itemMap->GetCount();
	while (i-- > 0)
	{
		FreeItem(this->itemMap->GetItem(i));
	}
	DEL_CLASS(this->itemMap);
	DEL_CLASS(this->crc);
	DEL_CLASS(this->crcMut);
}

Bool Net::SNS::SNSRSS::IsError()
{
	return this->itemMap->GetCount() <= 0;
}

Net::SNS::SNSControl::SNSType Net::SNS::SNSRSS::GetSNSType()
{
	return Net::SNS::SNSControl::ST_RSS;
}

Text::String *Net::SNS::SNSRSS::GetChannelId()
{
	return this->channelId;
}

Text::String *Net::SNS::SNSRSS::GetName()
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

UOSInt Net::SNS::SNSRSS::GetCurrItems(Data::ArrayList<SNSItem*> *itemList)
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
	UOSInt j = this->itemMap->GetCount();
	idList.EnsureCapacity(j);
	i = 0;
	while (i < j)
	{
		idList.Add(this->itemMap->GetKey(i));
		i++;
	}

	Net::RSS *rss;
	NEW_CLASS(rss, Net::RSS(this->channelId->v, this->userAgent, this->sockf, this->ssl));
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
					Data::ArrayList<Text::String *> imgList;
					sb.ClearStr();
					Text::HTMLUtil::HTMLGetText(this->encFact, item->description->v, item->description->leng, false, &sb, &imgList);
					sb2.ClearStr();
					if (item->imgURL)
					{
						sb2.Append(item->imgURL);
					}
					UOSInt j = 0;
					UOSInt k = imgList.GetCount();
					Text::String *s;
					while (j < k)
					{
						s = imgList.GetItem(j);
						if (sb2.GetLength() > 0)
						{
							sb2.AppendChar(' ', 1);
						}
						sb2.Append(s);
						s->Release();
						j++;
					}
					Text::String *s2;
					s = Text::String::New(sb.ToString(), sb.GetLength());
					s2 = Text::String::New(sb2.ToString(), sb2.GetLength());
					snsItem = CreateItem(item->guid, item->pubDate->ToTicks(), item->title, s, item->link, s2, 0);
					s->Release();
					s2->Release();
				}
				else
				{
					snsItem = CreateItem(item->guid, item->pubDate->ToTicks(), item->title, item->description, item->link, item->imgURL, 0);
				}
				this->itemMap->Put(item->guid, snsItem);
				changed = true;
			}
		}
		DEL_CLASS(rss);

		i = idList.GetCount();
		while (i-- > 0)
		{
			snsItem = this->itemMap->Remove(idList.GetItem(i));
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
