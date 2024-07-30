#include "Stdafx.h"
#include "Data/ArrayListString.h"
#include "Net/SNS/SNSInstagram.h"
#include "Text/StringBuilderUTF8.h"

Net::SNS::SNSInstagram::SNSInstagram(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent, Text::CStringNN channelId)
{
	NEW_CLASS(this->ctrl, Net::WebSite::WebSiteInstagramControl(clif, ssl, encFact, userAgent));
	this->channelId = Text::String::New(channelId);
	this->chDesc = 0;
	this->chError = false;

	NN<SNSItem> snsItem;
	NN<Net::WebSite::WebSiteInstagramControl::ItemData> item;
	Net::WebSite::WebSiteInstagramControl::ChannelInfo chInfo;
	Data::ArrayListNN<Net::WebSite::WebSiteInstagramControl::ItemData> itemList;
	MemClear(&chInfo, sizeof(chInfo));
	this->ctrl->GetChannelItems(this->channelId, 0, itemList, &chInfo);
	if (chInfo.full_name)
	{
		this->chName = chInfo.full_name->Clone();
	}
	else
	{
		this->chName = this->channelId->Clone();
		this->chError = true;
	}
	if (chInfo.biography)
	{
		this->chDesc = chInfo.biography->Clone().Ptr();
	}
	this->ctrl->FreeChannelInfo(chInfo);
	UOSInt i = itemList.GetCount();
	Text::StringBuilderUTF8 sb;
	while (i-- > 0)
	{
		item = itemList.GetItemNoCheck(i);
		if (item->moreImages)
		{
			Data::ArrayListStringNN imgList;
			Data::ArrayListStringNN videoList;
			if (this->ctrl->GetPageImages(item->shortCode, &imgList, &videoList))
			{
				SDEL_STRING(item->imgURL);
				SDEL_STRING(item->videoURL);
				if (imgList.GetCount() > 0)
				{
					sb.ClearStr();
					sb.AppendJoin(imgList.Iterator(), ' ');
					imgList.FreeAll();
					item->imgURL = Text::String::New(sb.ToCString()).Ptr();
				}

				if (videoList.GetCount() > 0)
				{
					sb.ClearStr();
					sb.AppendJoin(videoList.Iterator(), ' ');
					videoList.FreeAll();
					item->videoURL = Text::String::New(sb.ToCString()).Ptr();
				}
			}
		}

		sb.ClearStr();
		sb.AppendC(UTF8STRC("https://www.instagram.com/p/"));
		sb.Append(item->shortCode);
		sb.AppendC(UTF8STRC("/"));
		NN<Text::String> s = Text::String::New(sb.ToString(), sb.GetLength());
		snsItem = CreateItem(item->shortCode, item->recTime, 0, item->message, s.Ptr(), item->imgURL, item->videoURL);
		s->Release();
		this->itemMap.PutNN(item->shortCode, snsItem);
	}
	this->ctrl->FreeItems(itemList);
}

Net::SNS::SNSInstagram::~SNSInstagram()
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

Bool Net::SNS::SNSInstagram::IsError()
{
	return this->chError;
}

Net::SNS::SNSControl::SNSType Net::SNS::SNSInstagram::GetSNSType()
{
	return Net::SNS::SNSControl::ST_INSTAGRAM;
}

NN<Text::String> Net::SNS::SNSInstagram::GetChannelId() const
{
	return this->channelId;
}

NN<Text::String> Net::SNS::SNSInstagram::GetName() const
{
	return this->chName;
}

UnsafeArray<UTF8Char> Net::SNS::SNSInstagram::GetDirName(UnsafeArray<UTF8Char> dirName)
{
	dirName = Text::StrConcatC(dirName, UTF8STRC("Instagram_"));
	dirName = this->channelId->ConcatTo(dirName);
	return dirName;
}

UOSInt Net::SNS::SNSInstagram::GetCurrItems(NN<Data::ArrayListNN<SNSItem>> itemList)
{
	UOSInt initCnt = itemList->GetCount();
	itemList->AddAll(this->itemMap);
	return itemList->GetCount() - initCnt;
}

UnsafeArray<UTF8Char> Net::SNS::SNSInstagram::GetItemShortId(UnsafeArray<UTF8Char> buff, NN<SNSItem> item)
{
	return item->id->ConcatTo(buff);
}

Int32 Net::SNS::SNSInstagram::GetMinIntevalMS()
{
	return 20 * 60000;
}

Bool Net::SNS::SNSInstagram::Reload()
{
	NN<SNSItem> snsItem;
	OSInt si;
	NN<Net::WebSite::WebSiteInstagramControl::ItemData> item;
	Data::ArrayListNN<Net::WebSite::WebSiteInstagramControl::ItemData> itemList;
	Data::ArrayListString idList;
	Bool changed = false;
	UOSInt i = 0;
	UOSInt j = this->itemMap.GetCount();
	idList.EnsureCapacity(j);
	while (i < j)
	{
		idList.Add(this->itemMap.GetKey(i).OrNull());
		i++;
	}

	this->ctrl->GetChannelItems(this->channelId, 0, itemList, 0);
	i = itemList.GetCount();
	if (i > 0)
	{
		Text::StringBuilderUTF8 sb;
		while (i-- > 0)
		{
			item = itemList.GetItemNoCheck(i);
			si = idList.SortedIndexOf(item->shortCode.Ptr());
			if (si >= 0)
			{
				idList.RemoveAt((UOSInt)si);
			}
			else
			{
				if (item->moreImages)
				{
					Data::ArrayListStringNN imgList;
					Data::ArrayListStringNN videoList;
					if (this->ctrl->GetPageImages(item->shortCode, &imgList, &videoList))
					{
						SDEL_STRING(item->imgURL);
						SDEL_STRING(item->videoURL);
						if (imgList.GetCount() > 0)
						{
							sb.ClearStr();
							sb.AppendJoin(imgList.Iterator(), ' ');
							imgList.FreeAll();
							item->imgURL = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
						}

						if (videoList.GetCount() > 0)
						{
							sb.ClearStr();
							sb.AppendJoin(videoList.Iterator(), ' ');
							videoList.FreeAll();
							item->videoURL = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
						}
					}
				}

				sb.ClearStr();
				sb.AppendC(UTF8STRC("https://www.instagram.com/p/"));
				sb.Append(item->shortCode);
				sb.AppendC(UTF8STRC("/"));
				NN<Text::String> s = Text::String::New(sb.ToString(), sb.GetLength());
				snsItem = CreateItem(item->shortCode, item->recTime, 0, item->message, s.Ptr(), item->imgURL, item->videoURL);
				s->Release();
				this->itemMap.PutNN(item->shortCode, snsItem);
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
