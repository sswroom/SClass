#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSiteTwitterControl.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Net::WebSite::WebSiteTwitterControl::WebSiteTwitterControl(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, const UTF8Char *userAgent)
{
	this->sockf = sockf;
	this->encFact = encFact;
	this->userAgent = 0;
}

Net::WebSite::WebSiteTwitterControl::~WebSiteTwitterControl()
{
	SDEL_TEXT(this->userAgent);
}

OSInt Net::WebSite::WebSiteTwitterControl::GetChannelItems(const UTF8Char *channelId, OSInt pageNo, Data::ArrayList<Net::WebSite::WebSiteTwitterControl::ItemData*> *itemList, Net::WebSite::WebSiteTwitterControl::ChannelInfo *chInfo)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	OSInt i;
	OSInt j;
	sb.Append((const UTF8Char*)"https://twitter.com/");
	sb.Append(channelId);
	ItemData *item;
	Text::XMLReader *reader;
	Text::XMLAttrib *attr;
	Int64 conversationId;
	Int64 recTime;
	const UTF8Char *message;
	const UTF8Char *imgURL;
	conversationId = 0;
	recTime = 0;
	imgURL = 0;
	message = 0;
#if defined(VERBOSE)
	printf("Requesting to URL %s\r\n", sb.ToString());
#endif
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->userAgent, true, true);
	cli->Connect(sb.ToString(), "GET", 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"div"))
		{
			attr = reader->GetAttrib((OSInt)0);
			if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"class") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"stream-item-header"))
			{
#if defined(VERBOSE)
				printf("stream-item-header found\r\n");
#endif
				OSInt pathLev = reader->GetPathLev();
				conversationId = 0;
				recTime = 0;
				SDEL_TEXT(imgURL);
				SDEL_TEXT(message);
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"small") && reader->GetAttribCount() > 0)
					{
						attr = reader->GetAttrib((OSInt)0);
						if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"class") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"time"))
						{
							OSInt smallPathLev = reader->GetPathLev();
							while (reader->ReadNext() && reader->GetPathLev() > smallPathLev)
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"a") && reader->GetAttribCount() > 0)
								{
									i = 0;
									j = reader->GetAttribCount();
									while (i < j)
									{
										attr = reader->GetAttrib(i);
										if (Text::StrEquals(attr->name, (const UTF8Char*)"data-conversation-id") && attr->value)
										{
											conversationId = Text::StrToInt64(attr->value);
#if defined(VERBOSE)
											printf("conversation-id = %lld\r\n", conversationId);
#endif
										}
										i++;
									}
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"span") && reader->GetAttribCount() > 0)
								{
									i = 0;
									j = reader->GetAttribCount();
									while (i < j)
									{
										attr = reader->GetAttrib(i);
										if (Text::StrEquals(attr->name, (const UTF8Char*)"data-time-ms") && attr->value)
										{
											recTime = Text::StrToInt64(attr->value);
#if defined(VERBOSE)
											printf("data-time = %lld\r\n", recTime);
#endif											
										}
										i++;
									}
								}
							}
						}
					}
				}
			}
			else if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"class") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"js-tweet-text-container"))
			{
#if defined(VERBOSE)
				printf("js-tweet-text-container found\r\n");
#endif				
				OSInt pathLev = reader->GetPathLev();
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"p"))
					{
						OSInt smallPathLev = reader->GetPathLev();
						sb.ClearStr();
						while (reader->ReadNext() && reader->GetPathLev() > smallPathLev)
						{
							if (reader->GetNodeType() == Text::XMLNode::NT_TEXT)
							{
								sb.Append(reader->GetNodeText());
							}
							else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
							{
								if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"img"))
								{
									i = 0;
									j = reader->GetAttribCount();
									while (i < j)
									{
										attr = reader->GetAttrib(i);
										if (Text::StrEquals(attr->name, (const UTF8Char*)"alt") && attr->value)
										{
											sb.Append(attr->value);
										}
										i++;
									}
								}
								else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"a") && reader->GetAttribCount() > 0)
								{
									attr = reader->GetAttrib((OSInt)0);
									if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"href") && attr->value && Text::StrStartsWith(attr->value, (const UTF8Char*)"https://"))
									{
										reader->SkipElement();
									}
								}
							}
						}
						SDEL_TEXT(message);
						message = Text::StrCopyNew(sb.ToString());
					}
				}
			}
			else if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"class") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"AdaptiveMediaOuterContainer"))
			{
#if defined(VERBOSE)
				printf("AdaptiveMediaOuterContainer found\r\n");
#endif				
				OSInt pathLev = reader->GetPathLev();
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"div"))
					{
						i = 0;
						j = reader->GetAttribCount();
						while (i < j)
						{
							attr = reader->GetAttrib(i);
							if (Text::StrEquals(attr->name, (const UTF8Char*)"data-image-url") && attr->value)
							{
								if (imgURL == 0)
								{
									imgURL = Text::StrCopyNew(attr->value);
								}
								else
								{
									sb.ClearStr();
									sb.Append(imgURL);
									sb.AppendChar(' ', 1);
									Text::StrDelNew(imgURL);
									sb.Append(attr->value);
									imgURL = Text::StrCopyNew(sb.ToString());
								}
							}
							i++;
						}
					}
				}
			}
			else if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"class") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"stream-item-footer"))
			{
#if defined(VERBOSE)
				printf("stream-item-footer found\r\n");
#endif				
				if (conversationId != 0 && recTime != 0 && message != 0)
				{
					item = MemAlloc(ItemData, 1);
					item->id = conversationId;
					item->recTime = recTime;
					item->message = message;
					item->imgURL = imgURL;
					message = 0;
					imgURL = 0;
					itemList->Add(item);
					retCnt++;
				}
			}
			else if (attr && Text::StrEquals(attr->name, (const UTF8Char*)"class") && attr->value && Text::StrEquals(attr->value, (const UTF8Char*)"ProfileHeaderCard"))
			{
				OSInt pathLev = reader->GetPathLev();
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"h1"))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							sb.TrimWSCRLF();
							if (chInfo)
							{
								SDEL_TEXT(chInfo->name);
								chInfo->name = Text::StrCopyNew(sb.ToString());
							}
						}
						else if (Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"p"))
						{
							i = 0;
							j = reader->GetAttribCount();
							while (i < j)
							{
								attr = reader->GetAttrib(i);
								if (Text::StrEquals(attr->name, (const UTF8Char*)"class") && attr->value && Text::StrStartsWith(attr->value, (const UTF8Char*)"ProfileHeaderCard-bio"))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									sb.TrimWSCRLF();
									if (chInfo)
									{
										SDEL_TEXT(chInfo->bio);
										chInfo->bio = Text::StrCopyNew(sb.ToString());
									}
									break;
								}
								i++;
							}
						}
					}
				}

			}
		}
	}
	if (!reader->IsComplete())
	{
#if defined(VERBOSE)
		printf("Error found, errNo = %d\r\n", (Int32)reader->GetErrorCode());
#endif
	}
	SDEL_TEXT(message);
	SDEL_TEXT(imgURL);
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return retCnt;
}

void Net::WebSite::WebSiteTwitterControl::FreeItems(Data::ArrayList<Net::WebSite::WebSiteTwitterControl::ItemData*> *itemList)
{
	ItemData *item;
	OSInt i = itemList->GetCount();
	while (i-- > 0)
	{
		item = itemList->GetItem(i);
		Text::StrDelNew(item->message);
		SDEL_TEXT(item->imgURL);
		MemFree(item);
	}
	itemList->Clear();
}

const UTF8Char *Net::WebSite::WebSiteTwitterControl::GetUserAgent()
{
	return this->userAgent;
}
