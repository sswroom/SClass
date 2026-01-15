#include "Stdafx.h"
#include "Data/ArrayListInt64.h"
#include "Net/SNS/SNS7gogo.h"
#include "Text/StringBuilderUTF8.h"

Net::SNS::SNS7gogo::SNS7gogo(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent, Text::CStringNN channelId)
{
	NEW_CLASSNN(this->ctrl, Net::WebSite::WebSite7gogoControl(clif, ssl, encFact, userAgent));
	this->channelId = Text::String::New(channelId);
	this->chDesc = nullptr;
	this->chError = false;

	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SNSItem> snsItem;
	NN<Net::WebSite::WebSite7gogoControl::ItemData> item;
	Net::WebSite::WebSite7gogoControl::ChannelInfo chInfo;
	Data::ArrayListNN<Net::WebSite::WebSite7gogoControl::ItemData> itemList;
	NN<Text::String> s;
	MemClear(&chInfo, sizeof(chInfo));
	this->ctrl->GetChannelItems(this->channelId, 0, itemList, chInfo);
	if (chInfo.name.SetTo(s))
	{
		this->chName = s->Clone();
	}
	else
	{
		this->chName = this->channelId->Clone();
		this->chError = true;
	}
	if (chInfo.detail.SetTo(s))
	{
		this->chDesc = s->Clone();
	}
	this->ctrl->FreeChannelInfo(chInfo);
	UOSInt i = itemList.GetCount();
	Text::StringBuilderUTF8 sb;
	while (i-- > 0)
	{
		item = itemList.GetItemNoCheck(i);
		sptr = Text::StrInt64(sbuff, item->id);
		sb.ClearStr();
		sb.AppendC(UTF8STRC("https://7gogo.jp/"));
		sb.Append(this->channelId);
		sb.AppendC(UTF8STRC("/"));
		sb.AppendI64(item->id);
		NN<Text::String> s = Text::String::NewP(sbuff, sptr);
		NN<Text::String> s2 = Text::String::New(sb.ToString(), sb.GetLength());
		snsItem = CreateItem(s, item->recTime, nullptr, item->message, s2.Ptr(), item->imgURL, nullptr);
		s->Release();
		s2->Release();
		this->itemMap.Put(item->id, snsItem);
	}
	this->ctrl->FreeItems(itemList);
}

Net::SNS::SNS7gogo::~SNS7gogo()
{
	UOSInt i;
	this->ctrl.Delete();
	this->chName->Release();
	OPTSTR_DEL(this->chDesc);
	i = this->itemMap.GetCount();
	while (i-- > 0)
	{
		FreeItem(this->itemMap.GetItemNoCheck(i));
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

NN<Text::String> Net::SNS::SNS7gogo::GetChannelId() const
{
	return this->channelId;
}

NN<Text::String> Net::SNS::SNS7gogo::GetName() const
{
	return this->chName;
}

UnsafeArray<UTF8Char> Net::SNS::SNS7gogo::GetDirName(UnsafeArray<UTF8Char> dirName)
{
	dirName = Text::StrConcatC(dirName, UTF8STRC("7gogo_"));
	dirName = this->channelId->ConcatTo(dirName);
	return dirName;
}

UOSInt Net::SNS::SNS7gogo::GetCurrItems(NN<Data::ArrayListNN<SNSItem>> itemList)
{
	UOSInt initCnt = itemList->GetCount();
	itemList->AddAll(this->itemMap);
	return itemList->GetCount() - initCnt;
}

UnsafeArray<UTF8Char> Net::SNS::SNS7gogo::GetItemShortId(UnsafeArray<UTF8Char> buff, NN<SNSItem> item)
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
	UnsafeArray<UTF8Char> sptr;
	NN<SNSItem> snsItem;
	OSInt si;
	NN<Net::WebSite::WebSite7gogoControl::ItemData> item;
	Data::ArrayListNN<Net::WebSite::WebSite7gogoControl::ItemData> itemList;
	Data::ArrayListInt64 idList;
	Bool changed = false;
	this->itemMap.AddKeysTo(idList);
	
	this->ctrl->GetChannelItems(this->channelId, 0, itemList, nullptr);
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
				sb.AppendC(UTF8STRC("https://7gogo.jp/"));
				sb.Append(this->channelId);
				sb.AppendC(UTF8STRC("/"));
				sb.AppendI64(item->id);
				NN<Text::String> s = Text::String::NewP(sbuff, sptr);
				NN<Text::String> s2 = Text::String::New(sb.ToString(), sb.GetLength());
				snsItem = CreateItem(s, item->recTime, nullptr, item->message, s2.Ptr(), item->imgURL, nullptr);
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
