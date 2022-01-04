#include "Stdafx.h"
#include "Data/ArrayListString.h"
#include "Net/SNS/SNSInstagram.h"
#include "Text/StringBuilderUTF8.h"

Net::SNS::SNSInstagram::SNSInstagram(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, const UTF8Char *userAgent, const UTF8Char *channelId)
{
	NEW_CLASS(this->ctrl, Net::WebSite::WebSiteInstagramControl(sockf, ssl, encFact, userAgent));
	this->channelId = Text::String::NewNotNull(channelId);
	this->chName = 0;
	this->chDesc = 0;
	this->chError = false;
	NEW_CLASS(this->itemMap, Data::FastStringMap<SNSItem*>());

	SNSItem *snsItem;
	Net::WebSite::WebSiteInstagramControl::ItemData *item;
	Net::WebSite::WebSiteInstagramControl::ChannelInfo chInfo;
	Data::ArrayList<Net::WebSite::WebSiteInstagramControl::ItemData*> itemList;
	MemClear(&chInfo, sizeof(chInfo));
	this->ctrl->GetChannelItems(this->channelId, 0, &itemList, &chInfo);
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
		this->chDesc = chInfo.biography->Clone();
	}
	this->ctrl->FreeChannelInfo(&chInfo);
	UOSInt i = itemList.GetCount();
	Text::StringBuilderUTF8 sb;
	while (i-- > 0)
	{
		item = itemList.GetItem(i);
		if (item->moreImages)
		{
			Data::ArrayList<Text::String *> imgList;
			Data::ArrayList<Text::String *> videoList;
			UOSInt j;
			UOSInt k;
			Text::String *s;
			if (this->ctrl->GetPageImages(item->shortCode, &imgList, &videoList))
			{
				SDEL_STRING(item->imgURL);
				SDEL_STRING(item->videoURL);
				if (imgList.GetCount() > 0)
				{
					sb.ClearStr();
					j = 0;
					k = imgList.GetCount();
					while (j < k)
					{
						s = imgList.GetItem(j);
						if (j > 0)
						{
							sb.AppendChar(' ', 1);
						}
						sb.Append(s);
						s->Release();
						j++;
					}
					item->imgURL = Text::String::New(sb.ToString(), sb.GetLength());
				}

				if (videoList.GetCount() > 0)
				{
					sb.ClearStr();
					j = 0;
					k = videoList.GetCount();
					while (j < k)
					{
						s = videoList.GetItem(j);
						if (j > 0)
						{
							sb.AppendChar(' ', 1);
						}
						sb.Append(s);
						s->Release();
						j++;
					}
					item->videoURL = Text::String::New(sb.ToString(), sb.GetLength());
				}
			}
		}

		sb.ClearStr();
		sb.AppendC(UTF8STRC("https://www.instagram.com/p/"));
		sb.Append(item->shortCode);
		sb.AppendC(UTF8STRC("/"));
		Text::String *s = Text::String::New(sb.ToString(), sb.GetLength());
		snsItem = CreateItem(item->shortCode, item->recTime, 0, item->message, s, item->imgURL, item->videoURL);
		s->Release();
		this->itemMap->Put(item->shortCode, snsItem);
	}
	this->ctrl->FreeItems(&itemList);
}

Net::SNS::SNSInstagram::~SNSInstagram()
{
	UOSInt i;
	DEL_CLASS(this->ctrl);
	SDEL_STRING(this->chName);
	SDEL_STRING(this->chDesc);
	i = this->itemMap->GetCount();
	while (i-- > 0)
	{
		FreeItem(this->itemMap->GetItem(i));
	}
	DEL_CLASS(this->itemMap);
}

Bool Net::SNS::SNSInstagram::IsError()
{
	return this->chError;
}

Net::SNS::SNSControl::SNSType Net::SNS::SNSInstagram::GetSNSType()
{
	return Net::SNS::SNSControl::ST_INSTAGRAM;
}

Text::String *Net::SNS::SNSInstagram::GetChannelId()
{
	return this->channelId;
}

Text::String *Net::SNS::SNSInstagram::GetName()
{
	return this->chName;
}

UTF8Char *Net::SNS::SNSInstagram::GetDirName(UTF8Char *dirName)
{
	dirName = Text::StrConcat(dirName, (const UTF8Char*)"Instagram_");
	dirName = this->channelId->ConcatTo(dirName);
	return dirName;
}

UOSInt Net::SNS::SNSInstagram::GetCurrItems(Data::ArrayList<SNSItem*> *itemList)
{
	UOSInt initCnt = itemList->GetCount();
	itemList->AddAll(this->itemMap);
	return itemList->GetCount() - initCnt;
}

UTF8Char *Net::SNS::SNSInstagram::GetItemShortId(UTF8Char *buff, SNSItem *item)
{
	return item->id->ConcatTo(buff);
}

Int32 Net::SNS::SNSInstagram::GetMinIntevalMS()
{
	return 20 * 60000;
}

Bool Net::SNS::SNSInstagram::Reload()
{
	SNSItem *snsItem;
	OSInt si;
	Net::WebSite::WebSiteInstagramControl::ItemData *item;
	Data::ArrayList<Net::WebSite::WebSiteInstagramControl::ItemData*> itemList;
	Data::ArrayListString idList;
	Bool changed = false;
	UOSInt i = 0;
	UOSInt j = this->itemMap->GetCount();
	idList.EnsureCapacity(j);
	while (i < j)
	{
		idList.Add(this->itemMap->GetKey(i));
		i++;
	}

	this->ctrl->GetChannelItems(this->channelId, 0, &itemList, 0);
	i = itemList.GetCount();
	if (i > 0)
	{
		Text::StringBuilderUTF8 sb;
		while (i-- > 0)
		{
			item = itemList.GetItem(i);
			si = idList.SortedIndexOf(item->shortCode);
			if (si >= 0)
			{
				idList.RemoveAt((UOSInt)si);
			}
			else
			{
				if (item->moreImages)
				{
					Data::ArrayList<Text::String *> imgList;
					Data::ArrayList<Text::String *> videoList;
					UOSInt j;
					UOSInt k;
					Text::String *s;
					if (this->ctrl->GetPageImages(item->shortCode, &imgList, &videoList))
					{
						SDEL_STRING(item->imgURL);
						SDEL_STRING(item->videoURL);
						if (imgList.GetCount() > 0)
						{
							sb.ClearStr();
							j = 0;
							k = imgList.GetCount();
							while (j < k)
							{
								s = imgList.GetItem(j);
								if (j > 0)
								{
									sb.AppendChar(' ', 1);
								}
								sb.Append(s);
								s->Release();
								j++;
							}
							item->imgURL = Text::String::New(sb.ToString(), sb.GetLength());
						}

						if (videoList.GetCount() > 0)
						{
							sb.ClearStr();
							j = 0;
							k = videoList.GetCount();
							while (j < k)
							{
								s = videoList.GetItem(j);
								if (j > 0)
								{
									sb.AppendChar(' ', 1);
								}
								sb.Append(s);
								s->Release();
								j++;
							}
							item->videoURL = Text::String::New(sb.ToString(), sb.GetLength());
						}
					}
				}

				sb.ClearStr();
				sb.AppendC(UTF8STRC("https://www.instagram.com/p/"));
				sb.Append(item->shortCode);
				sb.AppendC(UTF8STRC("/"));
				Text::String *s = Text::String::New(sb.ToString(), sb.GetLength());
				snsItem = CreateItem(item->shortCode, item->recTime, 0, item->message, s, item->imgURL, item->videoURL);
				s->Release();
				this->itemMap->Put(item->shortCode, snsItem);
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
