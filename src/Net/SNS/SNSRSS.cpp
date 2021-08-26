#include "Stdafx.h"
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
	this->channelId = Text::StrCopyNew(channelId);
	this->chName = 0;
	this->chDesc = 0;
	NEW_CLASS(this->itemMap, Data::StringUTF8Map<SNSItem*>());
	NEW_CLASS(this->crc, Crypto::Hash::CRC32R());
	NEW_CLASS(this->crcMut, Sync::Mutex());

	Net::RSS *rss;
	SNSItem *snsItem;
	Net::RSSItem *item;
	NEW_CLASS(rss, Net::RSS(this->channelId, this->userAgent, this->sockf, this->ssl));
	if (rss->GetTitle())
	{
		this->chName = Text::StrCopyNew(rss->GetTitle());
	}
	else
	{
		this->chName = Text::StrCopyNew(this->channelId);
	}
	if (rss->GetDescription())
	{
		this->chDesc = Text::StrCopyNew(rss->GetDescription());
	}
	UOSInt i = rss->GetCount();
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	while (i-- > 0)
	{
		item = rss->GetItem(i);
		if (item->descHTML)
		{
			Data::ArrayList<const UTF8Char *> imgList;
			sb.ClearStr();
			Text::HTMLUtil::HTMLGetText(this->encFact, item->description, Text::StrCharCnt(item->description), false, &sb, &imgList);
			sb2.ClearStr();
			if (item->imgURL)
			{
				sb2.Append(item->imgURL);
			}
			UOSInt j = 0;
			UOSInt k = imgList.GetCount();
			const UTF8Char *csptr;
			while (j < k)
			{
				csptr = imgList.GetItem(j);
				if (sb2.GetLength() > 0)
				{
					sb2.AppendChar(' ', 1);
				}
				sb2.Append(csptr);
				Text::StrDelNew(csptr);
				j++;
			}
			snsItem = CreateItem(item->guid, item->pubDate->ToTicks(), item->title, sb.ToString(), item->link, sb2.ToString(), 0);
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
	SDEL_TEXT(this->chName);
	SDEL_TEXT(this->chDesc);
	Data::ArrayList<SNSItem*> *itemList = this->itemMap->GetValues();
	i = itemList->GetCount();
	while (i-- > 0)
	{
		FreeItem(itemList->GetItem(i));
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

const UTF8Char *Net::SNS::SNSRSS::GetChannelId()
{
	return this->channelId;
}

const UTF8Char *Net::SNS::SNSRSS::GetName()
{
	return this->chName;
}

UTF8Char *Net::SNS::SNSRSS::GetDirName(UTF8Char *dirName)
{
	UInt8 crcVal[4];
	dirName = Text::StrConcat(dirName, (const UTF8Char*)"RSS_");
	this->CalcCRC(this->channelId, Text::StrCharCnt(this->channelId), crcVal);
	dirName = Text::StrHexBytes(dirName, crcVal, 4, 0);
	return dirName;
}

UOSInt Net::SNS::SNSRSS::GetCurrItems(Data::ArrayList<SNSItem*> *itemList)
{
	UOSInt initCnt = itemList->GetCount();
	itemList->AddAll(this->itemMap->GetValues());
	return itemList->GetCount() - initCnt;
}

UTF8Char *Net::SNS::SNSRSS::GetItemShortId(UTF8Char *buff, SNSItem *item)
{
	UInt8 crcVal[4];
	this->CalcCRC(item->id, Text::StrCharCnt(item->id), crcVal);
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
	Data::ArrayListStrUTF8 idList;
	Bool changed = false;
	idList.AddAll(this->itemMap->GetKeys());

	Net::RSS *rss;
	NEW_CLASS(rss, Net::RSS(this->channelId, this->userAgent, this->sockf, this->ssl));
	UOSInt i = rss->GetCount();
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
					Data::ArrayList<const UTF8Char *> imgList;
					sb.ClearStr();
					Text::HTMLUtil::HTMLGetText(this->encFact, item->description, Text::StrCharCnt(item->description), false, &sb, &imgList);
					sb2.ClearStr();
					if (item->imgURL)
					{
						sb2.Append(item->imgURL);
					}
					UOSInt j = 0;
					UOSInt k = imgList.GetCount();
					const UTF8Char *csptr;
					while (j < k)
					{
						csptr = imgList.GetItem(j);
						if (sb2.GetLength() > 0)
						{
							sb2.AppendChar(' ', 1);
						}
						sb2.Append(csptr);
						Text::StrDelNew(csptr);
						j++;
					}
					snsItem = CreateItem(item->guid, item->pubDate->ToTicks(), item->title, sb.ToString(), item->link, sb2.ToString(), 0);
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
