#include "Stdafx.h"
#include "Net/SNS/SNSInstagram.h"
#include "Text/StringBuilderUTF8.h"

Net::SNS::SNSInstagram::SNSInstagram(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, const UTF8Char *userAgent, const UTF8Char *channelId)
{
	NEW_CLASS(this->ctrl, Net::WebSite::WebSiteInstagramControl(sockf, encFact, userAgent));
	this->channelId = Text::StrCopyNew(channelId);
	this->chName = 0;
	this->chDesc = 0;
	this->chError = false;
	NEW_CLASS(this->itemMap, Data::StringUTF8Map<SNSItem*>());

	SNSItem *snsItem;
	Net::WebSite::WebSiteInstagramControl::ItemData *item;
	Net::WebSite::WebSiteInstagramControl::ChannelInfo chInfo;
	Data::ArrayList<Net::WebSite::WebSiteInstagramControl::ItemData*> itemList;
	MemClear(&chInfo, sizeof(chInfo));
	this->ctrl->GetChannelItems(this->channelId, 0, &itemList, &chInfo);
	if (chInfo.full_name)
	{
		this->chName = Text::StrCopyNew(chInfo.full_name);
	}
	else
	{
		this->chName = Text::StrCopyNew(this->channelId);
		this->chError = true;
	}
	if (chInfo.biography)
	{
		this->chDesc = Text::StrCopyNew(chInfo.biography);
	}
	this->ctrl->FreeChannelInfo(&chInfo);
	UOSInt i = itemList.GetCount();
	Text::StringBuilderUTF8 sb;
	while (i-- > 0)
	{
		item = itemList.GetItem(i);
		if (item->moreImages)
		{
			Data::ArrayList<const UTF8Char *> imgList;
			Data::ArrayList<const UTF8Char *> videoList;
			UOSInt j;
			UOSInt k;
			const UTF8Char *csptr;
			if (this->ctrl->GetPageImages(item->shortCode, &imgList, &videoList))
			{
				SDEL_TEXT(item->imgURL);
				SDEL_TEXT(item->videoURL);
				if (imgList.GetCount() > 0)
				{
					sb.ClearStr();
					j = 0;
					k = imgList.GetCount();
					while (j < k)
					{
						csptr = imgList.GetItem(j);
						if (j > 0)
						{
							sb.AppendChar(' ', 1);
						}
						sb.Append(csptr);
						Text::StrDelNew(csptr);
						j++;
					}
					item->imgURL = Text::StrCopyNew(sb.ToString());
				}

				if (videoList.GetCount() > 0)
				{
					sb.ClearStr();
					j = 0;
					k = videoList.GetCount();
					while (j < k)
					{
						csptr = videoList.GetItem(j);
						if (j > 0)
						{
							sb.AppendChar(' ', 1);
						}
						sb.Append(csptr);
						Text::StrDelNew(csptr);
						j++;
					}
					item->videoURL = Text::StrCopyNew(sb.ToString());
				}
			}
		}

		sb.ClearStr();
		sb.Append((const UTF8Char*)"https://www.instagram.com/p/");
		sb.Append(item->shortCode);
		sb.Append((const UTF8Char*)"/");
		snsItem = CreateItem(item->shortCode, item->recTime, 0, item->message, sb.ToString(), item->imgURL, item->videoURL);
		this->itemMap->Put(item->shortCode, snsItem);
	}
	this->ctrl->FreeItems(&itemList);
}

Net::SNS::SNSInstagram::~SNSInstagram()
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

Bool Net::SNS::SNSInstagram::IsError()
{
	return this->chError;
}

Net::SNS::SNSControl::SNSType Net::SNS::SNSInstagram::GetSNSType()
{
	return Net::SNS::SNSControl::ST_INSTAGRAM;
}

const UTF8Char *Net::SNS::SNSInstagram::GetChannelId()
{
	return this->channelId;
}

const UTF8Char *Net::SNS::SNSInstagram::GetName()
{
	return this->chName;
}

UTF8Char *Net::SNS::SNSInstagram::GetDirName(UTF8Char *dirName)
{
	dirName = Text::StrConcat(dirName, (const UTF8Char*)"Instagram_");
	dirName = Text::StrConcat(dirName, this->channelId);
	return dirName;
}

UOSInt Net::SNS::SNSInstagram::GetCurrItems(Data::ArrayList<SNSItem*> *itemList)
{
	UOSInt initCnt = itemList->GetCount();
	itemList->AddRange(this->itemMap->GetValues());
	return itemList->GetCount() - initCnt;
}

UTF8Char *Net::SNS::SNSInstagram::GetItemShortId(UTF8Char *buff, SNSItem *item)
{
	return Text::StrConcat(buff, item->id);
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
	Data::ArrayListStrUTF8 idList;
	Bool changed = false;
	idList.AddRange(this->itemMap->GetKeys());

	this->ctrl->GetChannelItems(this->channelId, 0, &itemList, 0);
	UOSInt i = itemList.GetCount();
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
					Data::ArrayList<const UTF8Char *> imgList;
					Data::ArrayList<const UTF8Char *> videoList;
					UOSInt j;
					UOSInt k;
					const UTF8Char *csptr;
					if (this->ctrl->GetPageImages(item->shortCode, &imgList, &videoList))
					{
						SDEL_TEXT(item->imgURL);
						SDEL_TEXT(item->videoURL);
						if (imgList.GetCount() > 0)
						{
							sb.ClearStr();
							j = 0;
							k = imgList.GetCount();
							while (j < k)
							{
								csptr = imgList.GetItem(j);
								if (j > 0)
								{
									sb.AppendChar(' ', 1);
								}
								sb.Append(csptr);
								Text::StrDelNew(csptr);
								j++;
							}
							item->imgURL = Text::StrCopyNew(sb.ToString());
						}

						if (videoList.GetCount() > 0)
						{
							sb.ClearStr();
							j = 0;
							k = videoList.GetCount();
							while (j < k)
							{
								csptr = videoList.GetItem(j);
								if (j > 0)
								{
									sb.AppendChar(' ', 1);
								}
								sb.Append(csptr);
								Text::StrDelNew(csptr);
								j++;
							}
							item->videoURL = Text::StrCopyNew(sb.ToString());
						}
					}
				}

				sb.ClearStr();
				sb.Append((const UTF8Char*)"https://www.instagram.com/p/");
				sb.Append(item->shortCode);
				sb.Append((const UTF8Char*)"/");
				snsItem = CreateItem(item->shortCode, item->recTime, 0, item->message, sb.ToString(), item->imgURL, item->videoURL);
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
