#include "Stdafx.h"
#include "Net/SNS/SNSTwitter.h"
#include "Text/StringBuilderUTF8.h"

Net::SNS::SNSTwitter::SNSTwitter(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, const UTF8Char *userAgent, const UTF8Char *channelId)
{
	NEW_CLASS(this->ctrl, Net::WebSite::WebSiteTwitterControl(sockf, encFact, userAgent));
	this->channelId = Text::StrCopyNew(channelId);
	this->chName = 0;
	this->chDesc = 0;
	this->chError = false;
	NEW_CLASS(this->itemMap, Data::Integer64Map<SNSItem*>());

	UTF8Char sbuff[32];
	SNSItem *snsItem;
	Net::WebSite::WebSiteTwitterControl::ItemData *item;
	Net::WebSite::WebSiteTwitterControl::ChannelInfo chInfo;
	Data::ArrayList<Net::WebSite::WebSiteTwitterControl::ItemData*> itemList;
	MemClear(&chInfo, sizeof(chInfo));
	this->ctrl->GetChannelItems(this->channelId, 0, &itemList, &chInfo);
	if (chInfo.name)
	{
		this->chName = chInfo.name;
	}
	else
	{
		this->chName = Text::StrCopyNew(this->channelId);
		this->chError = true;
	}
	if (chInfo.bio)
	{
		this->chDesc = chInfo.bio;
	}
	OSInt i = itemList.GetCount();
	Text::StringBuilderUTF8 sb;
	while (i-- > 0)
	{
		item = itemList.GetItem(i);
		Text::StrInt64(sbuff, item->id);
		sb.ClearStr();
		sb.Append((const UTF8Char*)"https://twitter.com/");
		sb.Append(this->channelId);
		sb.Append((const UTF8Char*)"/status/");
		sb.AppendI64(item->id);
		snsItem = CreateItem(sbuff, item->recTime, 0, item->message, sb.ToString(), item->imgURL, 0);
		this->itemMap->Put(item->id, snsItem);
	}
	this->ctrl->FreeItems(&itemList);
}

Net::SNS::SNSTwitter::~SNSTwitter()
{
	OSInt i;
	DEL_CLASS(this->ctrl);
	SDEL_TEXT(this->chName);
	SDEL_TEXT(this->chDesc);
	Data::ArrayList<SNSItem*> *itemList = this->itemMap->GetValues();
	i = itemList->GetCount();
	while (i-- > 0)
	{
		FreeItem(itemList->GetItem(i));
	}
	DEL_CLASS(this->itemMap);
}

Bool Net::SNS::SNSTwitter::IsError()
{
	return this->chError;
}

Net::SNS::SNSControl::SNSType Net::SNS::SNSTwitter::GetSNSType()
{
	return Net::SNS::SNSControl::ST_TWITTER;
}

const UTF8Char *Net::SNS::SNSTwitter::GetChannelId()
{
	return this->channelId;
}

const UTF8Char *Net::SNS::SNSTwitter::GetName()
{
	return this->chName;
}

UTF8Char *Net::SNS::SNSTwitter::GetDirName(UTF8Char *dirName)
{
	dirName = Text::StrConcat(dirName, (const UTF8Char*)"Twitter_");
	dirName = Text::StrConcat(dirName, this->channelId);
	return dirName;
}

OSInt Net::SNS::SNSTwitter::GetCurrItems(Data::ArrayList<SNSItem*> *itemList)
{
	OSInt initCnt = itemList->GetCount();
	itemList->AddRange(this->itemMap->GetValues());
	return itemList->GetCount() - initCnt;
}

UTF8Char *Net::SNS::SNSTwitter::GetItemShortId(UTF8Char *buff, SNSItem *item)
{
	return Text::StrConcat(buff, item->id);
}

Int32 Net::SNS::SNSTwitter::GetMinIntevalMS()
{
	return 15 * 60000;
}

Bool Net::SNS::SNSTwitter::Reload()
{
	UTF8Char sbuff[32];
	SNSItem *snsItem;
	OSInt j;
	Net::WebSite::WebSiteTwitterControl::ItemData *item;
	Data::ArrayList<Net::WebSite::WebSiteTwitterControl::ItemData*> itemList;
	Data::ArrayListInt64 idList;
	Bool changed = false;
	idList.AddRange(this->itemMap->GetKeys());

	this->ctrl->GetChannelItems(this->channelId, 0, &itemList, 0);
	OSInt i = itemList.GetCount();
	if (i > 0)
	{
		Text::StringBuilderUTF8 sb;
		while (i-- > 0)
		{
			item = itemList.GetItem(i);
			j = idList.SortedIndexOf(item->id);
			if (j >= 0)
			{
				idList.RemoveAt(j);
			}
			else
			{
				Text::StrInt64(sbuff, item->id);
				sb.ClearStr();
				sb.Append((const UTF8Char*)"https://twitter.com/");
				sb.Append(this->channelId);
				sb.Append((const UTF8Char*)"/status/");
				sb.AppendI64(item->id);
				snsItem = CreateItem(sbuff, item->recTime, 0, item->message, sb.ToString(), item->imgURL, 0);
				this->itemMap->Put(item->id, snsItem);
				changed = true;
			}
		}
		this->ctrl->FreeItems(&itemList);

		i = idList.GetCount();
		while (i-- > 0)
		{
			snsItem = this->itemMap->Remove(idList.GetItem(i));
			FreeItem(snsItem);
			changed = true;
		}
	}
	return changed;
}
