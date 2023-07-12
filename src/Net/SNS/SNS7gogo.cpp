#include "Stdafx.h"
#include "Data/ArrayListInt64.h"
#include "Net/SNS/SNS7gogo.h"
#include "Text/StringBuilderUTF8.h"

Net::SNS::SNS7gogo::SNS7gogo(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::String *userAgent, Text::CString channelId)
{
	NEW_CLASS(this->ctrl, Net::WebSite::WebSite7gogoControl(sockf, ssl, encFact, userAgent));
	this->channelId = Text::String::New(channelId);
	this->chName = 0;
	this->chDesc = 0;
	this->chError = false;

	UTF8Char sbuff[32];
	UTF8Char *sptr;
	SNSItem *snsItem;
	Net::WebSite::WebSite7gogoControl::ItemData *item;
	Net::WebSite::WebSite7gogoControl::ChannelInfo chInfo;
	Data::ArrayList<Net::WebSite::WebSite7gogoControl::ItemData*> itemList;
	MemClear(&chInfo, sizeof(chInfo));
	this->ctrl->GetChannelItems(this->channelId, 0, &itemList, &chInfo);
	if (chInfo.name)
	{
		this->chName = chInfo.name->Clone();
	}
	else
	{
		this->chName = this->channelId->Clone();
		this->chError = true;
	}
	if (chInfo.detail)
	{
		this->chDesc = chInfo.detail->Clone();
	}
	this->ctrl->FreeChannelInfo(&chInfo);
	UOSInt i = itemList.GetCount();
	Text::StringBuilderUTF8 sb;
	while (i-- > 0)
	{
		item = itemList.GetItem(i);
		sptr = Text::StrInt64(sbuff, item->id);
		sb.ClearStr();
		sb.AppendC(UTF8STRC("https://7gogo.jp/"));
		sb.Append(this->channelId);
		sb.AppendC(UTF8STRC("/"));
		sb.AppendI64(item->id);
		Text::String *s = Text::String::NewP(sbuff, sptr);
		Text::String *s2 = Text::String::New(sb.ToString(), sb.GetLength());
		snsItem = CreateItem(s, item->recTime, 0, item->message, s2, item->imgURL, 0);
		s->Release();
		s2->Release();
		this->itemMap.Put(item->id, snsItem);
	}
	this->ctrl->FreeItems(&itemList);
}

Net::SNS::SNS7gogo::~SNS7gogo()
{
	UOSInt i;
	DEL_CLASS(this->ctrl);
	SDEL_STRING(this->chName);
	SDEL_STRING(this->chDesc);
	i = this->itemMap.GetCount();
	while (i-- > 0)
	{
		FreeItem(this->itemMap.GetItem(i));
	}
}

Bool Net::SNS::SNS7gogo::IsError()
{
	return this->chError;
}

Net::SNS::SNSControl::SNSType Net::SNS::SNS7gogo::GetSNSType()
{
	return Net::SNS::SNSControl::ST_7GOGO;
}

Text::String *Net::SNS::SNS7gogo::GetChannelId()
{
	return this->channelId;
}

Text::String *Net::SNS::SNS7gogo::GetName()
{
	return this->chName;
}

UTF8Char *Net::SNS::SNS7gogo::GetDirName(UTF8Char *dirName)
{
	dirName = Text::StrConcatC(dirName, UTF8STRC("7gogo_"));
	dirName = this->channelId->ConcatTo(dirName);
	return dirName;
}

UOSInt Net::SNS::SNS7gogo::GetCurrItems(Data::ArrayList<SNSItem*> *itemList)
{
	UOSInt initCnt = itemList->GetCount();
	itemList->AddAll(&this->itemMap);
	return itemList->GetCount() - initCnt;
}

UTF8Char *Net::SNS::SNS7gogo::GetItemShortId(UTF8Char *buff, SNSItem *item)
{
	return item->id->ConcatTo(buff);
}

Int32 Net::SNS::SNS7gogo::GetMinIntevalMS()
{
	return 10 * 60000;
}

Bool Net::SNS::SNS7gogo::Reload()
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	SNSItem *snsItem;
	OSInt si;
	Net::WebSite::WebSite7gogoControl::ItemData *item;
	Data::ArrayList<Net::WebSite::WebSite7gogoControl::ItemData*> itemList;
	Data::ArrayListInt64 idList;
	Bool changed = false;
	this->itemMap.AddKeysTo(&idList);
	
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
				sptr = Text::StrInt64(sbuff, item->id);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("https://7gogo.jp/"));
				sb.Append(this->channelId);
				sb.AppendC(UTF8STRC("/"));
				sb.AppendI64(item->id);
				Text::String *s = Text::String::NewP(sbuff, sptr);
				Text::String *s2 = Text::String::New(sb.ToString(), sb.GetLength());
				snsItem = CreateItem(s, item->recTime, 0, item->message, s2, item->imgURL, 0);
				s->Release();
				s2->Release();
				this->itemMap.Put(item->id, snsItem);
				changed = true;
			}
		}
		this->ctrl->FreeItems(&itemList);

		i = idList.GetCount();
		while (i-- > 0)
		{
			snsItem = this->itemMap.Remove(idList.GetItem(i));
			FreeItem(snsItem);
			changed = true;
		}
	}
	return changed;
}
