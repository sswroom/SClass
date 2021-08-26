#include "Stdafx.h"
#include "Net/SNS/SNS7gogo.h"
#include "Text/StringBuilderUTF8.h"

Net::SNS::SNS7gogo::SNS7gogo(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, const UTF8Char *userAgent, const UTF8Char *channelId)
{
	NEW_CLASS(this->ctrl, Net::WebSite::WebSite7gogoControl(sockf, ssl, encFact, userAgent));
	this->channelId = Text::StrCopyNew(channelId);
	this->chName = 0;
	this->chDesc = 0;
	this->chError = false;
	NEW_CLASS(this->itemMap, Data::Int64Map<SNSItem*>());

	UTF8Char sbuff[32];
	SNSItem *snsItem;
	Net::WebSite::WebSite7gogoControl::ItemData *item;
	Net::WebSite::WebSite7gogoControl::ChannelInfo chInfo;
	Data::ArrayList<Net::WebSite::WebSite7gogoControl::ItemData*> itemList;
	MemClear(&chInfo, sizeof(chInfo));
	this->ctrl->GetChannelItems(this->channelId, 0, &itemList, &chInfo);
	if (chInfo.name)
	{
		this->chName = Text::StrCopyNew(chInfo.name);
	}
	else
	{
		this->chName = Text::StrCopyNew(this->channelId);
		this->chError = true;
	}
	if (chInfo.detail)
	{
		this->chDesc = Text::StrCopyNew(chInfo.detail);
	}
	this->ctrl->FreeChannelInfo(&chInfo);
	UOSInt i = itemList.GetCount();
	Text::StringBuilderUTF8 sb;
	while (i-- > 0)
	{
		item = itemList.GetItem(i);
		Text::StrInt64(sbuff, item->id);
		sb.ClearStr();
		sb.Append((const UTF8Char*)"https://7gogo.jp/");
		sb.Append(this->channelId);
		sb.Append((const UTF8Char*)"/");
		sb.AppendI64(item->id);
		snsItem = CreateItem(sbuff, item->recTime, 0, item->message, sb.ToString(), item->imgURL, 0);
		this->itemMap->Put(item->id, snsItem);
	}
	this->ctrl->FreeItems(&itemList);
}

Net::SNS::SNS7gogo::~SNS7gogo()
{
	UOSInt i;
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

Bool Net::SNS::SNS7gogo::IsError()
{
	return this->chError;
}

Net::SNS::SNSControl::SNSType Net::SNS::SNS7gogo::GetSNSType()
{
	return Net::SNS::SNSControl::ST_7GOGO;
}

const UTF8Char *Net::SNS::SNS7gogo::GetChannelId()
{
	return this->channelId;
}

const UTF8Char *Net::SNS::SNS7gogo::GetName()
{
	return this->chName;
}

UTF8Char *Net::SNS::SNS7gogo::GetDirName(UTF8Char *dirName)
{
	dirName = Text::StrConcat(dirName, (const UTF8Char*)"7gogo_");
	dirName = Text::StrConcat(dirName, this->channelId);
	return dirName;
}

UOSInt Net::SNS::SNS7gogo::GetCurrItems(Data::ArrayList<SNSItem*> *itemList)
{
	UOSInt initCnt = itemList->GetCount();
	itemList->AddAll(this->itemMap->GetValues());
	return itemList->GetCount() - initCnt;
}

UTF8Char *Net::SNS::SNS7gogo::GetItemShortId(UTF8Char *buff, SNSItem *item)
{
	return Text::StrConcat(buff, item->id);
}

Int32 Net::SNS::SNS7gogo::GetMinIntevalMS()
{
	return 10 * 60000;
}

Bool Net::SNS::SNS7gogo::Reload()
{
	UTF8Char sbuff[32];
	SNSItem *snsItem;
	OSInt si;
	Net::WebSite::WebSite7gogoControl::ItemData *item;
	Data::ArrayList<Net::WebSite::WebSite7gogoControl::ItemData*> itemList;
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
				sb.Append((const UTF8Char*)"https://7gogo.jp/");
				sb.Append(this->channelId);
				sb.Append((const UTF8Char*)"/");
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
