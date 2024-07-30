#include "Stdafx.h"
#include "Data/ArrayListInt64.h"
#include "Net/SNS/SNSTwitter.h"
#include "Text/StringBuilderUTF8.h"

Net::SNS::SNSTwitter::SNSTwitter(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent, Text::CStringNN channelId)
{
	NEW_CLASS(this->ctrl, Net::WebSite::WebSiteTwitterControl(clif, ssl, encFact, userAgent));
	this->channelId = Text::String::New(channelId);
	this->chDesc = 0;
	this->chError = false;

	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SNSItem> snsItem;
	NN<Net::WebSite::WebSiteTwitterControl::ItemData> item;
	Net::WebSite::WebSiteTwitterControl::ChannelInfo chInfo;
	Data::ArrayListNN<Net::WebSite::WebSiteTwitterControl::ItemData> itemList;
	MemClear(&chInfo, sizeof(chInfo));
	this->ctrl->GetChannelItems(this->channelId, 0, itemList, chInfo);
	if (!this->chName.Set(chInfo.name))
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
		item = itemList.GetItemNoCheck(i);
		sptr = Text::StrInt64(sbuff, item->id);
		sb.ClearStr();
		sb.AppendC(UTF8STRC("https://twitter.com/"));
		sb.Append(this->channelId);
		sb.AppendC(UTF8STRC("/status/"));
		sb.AppendI64(item->id);
		NN<Text::String> s = Text::String::NewP(sbuff, sptr);
		NN<Text::String> s2 = Text::String::New(sb.ToString(), sb.GetLength());
		snsItem = CreateItem(s, item->recTime, 0, Text::String::OrEmpty(item->message), s2.Ptr(), item->imgURL, 0);
		s->Release();
		s2->Release();
		this->itemMap.Put(item->id, snsItem);
	}
	this->ctrl->FreeItems(itemList);
}

Net::SNS::SNSTwitter::~SNSTwitter()
{
	UOSInt i;
	DEL_CLASS(this->ctrl);
	this->chName->Release();
	SDEL_STRING(this->chDesc);
	i = this->itemMap.GetCount();
	while (i-- > 0)
	{
		FreeItem(this->itemMap.GetItemNoCheck(i));
	}
}

Bool Net::SNS::SNSTwitter::IsError()
{
	return this->chError;
}

Net::SNS::SNSControl::SNSType Net::SNS::SNSTwitter::GetSNSType()
{
	return Net::SNS::SNSControl::ST_TWITTER;
}

NN<Text::String> Net::SNS::SNSTwitter::GetChannelId() const
{
	return this->channelId;
}

NN<Text::String> Net::SNS::SNSTwitter::GetName() const
{
	return this->chName;
}

UnsafeArray<UTF8Char> Net::SNS::SNSTwitter::GetDirName(UnsafeArray<UTF8Char> dirName)
{
	dirName = Text::StrConcatC(dirName, UTF8STRC("Twitter_"));
	dirName = this->channelId->ConcatTo(dirName);
	return dirName;
}

UOSInt Net::SNS::SNSTwitter::GetCurrItems(NN<Data::ArrayListNN<SNSItem>> itemList)
{
	return itemList->AddAll(this->itemMap);
}

UnsafeArray<UTF8Char> Net::SNS::SNSTwitter::GetItemShortId(UnsafeArray<UTF8Char> buff, NN<SNSItem> item)
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
	UnsafeArray<UTF8Char> sptr;
	NN<SNSItem> snsItem;
	OSInt si;
	NN<Net::WebSite::WebSiteTwitterControl::ItemData> item;
	Data::ArrayListNN<Net::WebSite::WebSiteTwitterControl::ItemData> itemList;
	Data::ArrayListInt64 idList;
	Bool changed = false;
	this->itemMap.AddKeysTo(idList);

	this->ctrl->GetChannelItems(this->channelId, 0, itemList, 0);
	UOSInt i = itemList.GetCount();
	if (i > 0)
	{
		Text::StringBuilderUTF8 sb;
		while (i-- > 0)
		{
			item = itemList.GetItemNoCheck(i);
			si = idList.SortedIndexOf(item->id);
			if (si >= 0)
			{
				idList.RemoveAt((UOSInt)si);
			}
			else
			{
				sptr = Text::StrInt64(sbuff, item->id);
				sb.ClearStr();
				sb.AppendC(UTF8STRC("https://twitter.com/"));
				sb.Append(this->channelId);
				sb.AppendC(UTF8STRC("/status/"));
				sb.AppendI64(item->id);
				NN<Text::String> s = Text::String::NewP(sbuff, sptr);
				NN<Text::String> s2 = Text::String::New(sb.ToString(), sb.GetLength());
				snsItem = CreateItem(s, item->recTime, 0, Text::String::OrEmpty(item->message), s2.Ptr(), item->imgURL, 0);
				s->Release();
				s2->Release();
				this->itemMap.Put(item->id, snsItem);
				changed = true;
			}
		}
		this->ctrl->FreeItems(itemList);

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
	return changed;
}
