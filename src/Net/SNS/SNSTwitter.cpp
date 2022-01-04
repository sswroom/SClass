#include "Stdafx.h"
#include "Net/SNS/SNSTwitter.h"
#include "Text/StringBuilderUTF8.h"

Net::SNS::SNSTwitter::SNSTwitter(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, const UTF8Char *userAgent, const UTF8Char *channelId)
{
	NEW_CLASS(this->ctrl, Net::WebSite::WebSiteTwitterControl(sockf, ssl, encFact, userAgent));
	this->channelId = Text::String::NewNotNull(channelId);
	this->chName = 0;
	this->chDesc = 0;
	this->chError = false;
	NEW_CLASS(this->itemMap, Data::Int64Map<SNSItem*>());

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
		this->chName = this->channelId->Clone();
		this->chError = true;
	}
	if (chInfo.bio)
	{
		this->chDesc = chInfo.bio;
	}
	UOSInt i = itemList.GetCount();
	Text::StringBuilderUTF8 sb;
	while (i-- > 0)
	{
		item = itemList.GetItem(i);
		Text::StrInt64(sbuff, item->id);
		sb.ClearStr();
		sb.AppendC(UTF8STRC("https://twitter.com/"));
		sb.Append(this->channelId);
		sb.AppendC(UTF8STRC("/status/"));
		sb.AppendI64(item->id);
		Text::String *s = Text::String::NewNotNull(sbuff);
		Text::String *s2 = Text::String::New(sb.ToString(), sb.GetLength());
		snsItem = CreateItem(s, item->recTime, 0, item->message, s2, item->imgURL, 0);
		s->Release();
		s2->Release();
		this->itemMap->Put(item->id, snsItem);
	}
	this->ctrl->FreeItems(&itemList);
}

Net::SNS::SNSTwitter::~SNSTwitter()
{
	UOSInt i;
	DEL_CLASS(this->ctrl);
	SDEL_STRING(this->chName);
	SDEL_STRING(this->chDesc);
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

Text::String *Net::SNS::SNSTwitter::GetChannelId()
{
	return this->channelId;
}

Text::String *Net::SNS::SNSTwitter::GetName()
{
	return this->chName;
}

UTF8Char *Net::SNS::SNSTwitter::GetDirName(UTF8Char *dirName)
{
	dirName = Text::StrConcat(dirName, (const UTF8Char*)"Twitter_");
	dirName = this->channelId->ConcatTo(dirName);
	return dirName;
}

UOSInt Net::SNS::SNSTwitter::GetCurrItems(Data::ArrayList<SNSItem*> *itemList)
{
	UOSInt initCnt = itemList->GetCount();
	itemList->AddAll(this->itemMap->GetValues());
	return itemList->GetCount() - initCnt;
}

UTF8Char *Net::SNS::SNSTwitter::GetItemShortId(UTF8Char *buff, SNSItem *item)
{
	return item->id->ConcatTo(buff);
}

Int32 Net::SNS::SNSTwitter::GetMinIntevalMS()
{
	return 15 * 60000;
}

Bool Net::SNS::SNSTwitter::Reload()
{
	UTF8Char sbuff[32];
	SNSItem *snsItem;
	OSInt si;
	Net::WebSite::WebSiteTwitterControl::ItemData *item;
	Data::ArrayList<Net::WebSite::WebSiteTwitterControl::ItemData*> itemList;
	Data::ArrayListInt64 idList;
	Bool changed = false;
	idList.AddAll(this->itemMap->GetKeys());

	this->ctrl->GetChannelItems(this->channelId, 0, &itemList, 0);
	UOSInt i = itemList.GetCount();
	if (i > 0)
	{
		Text::StringBuilderUTF8 sb;
		while (i-- > 0)
		{
			item = itemList.GetItem(i);
			si = idList.SortedIndexOf(item->id);
			if (si >= 0)
			{
				idList.RemoveAt((UOSInt)si);
			}
			else
			{
				Text::StrInt64(sbuff, item->id);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("https://twitter.com/"));
				sb.Append(this->channelId);
				sb.AppendC(UTF8STRC("/status/"));
				sb.AppendI64(item->id);
				Text::String *s = Text::String::NewNotNull(sbuff);
				Text::String *s2 = Text::String::New(sb.ToString(), sb.GetLength());
				snsItem = CreateItem(s, item->recTime, 0, item->message, s2, item->imgURL, 0);
				s->Release();
				s2->Release();
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
