#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSiteTwitterControl.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Net::WebSite::WebSiteTwitterControl::WebSiteTwitterControl(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::String *userAgent)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = 0;
}

Net::WebSite::WebSiteTwitterControl::~WebSiteTwitterControl()
{
	SDEL_STRING(this->userAgent);
}

UOSInt Net::WebSite::WebSiteTwitterControl::GetChannelItems(Text::String *channelId, UOSInt pageNo, Data::ArrayList<Net::WebSite::WebSiteTwitterControl::ItemData*> *itemList, Net::WebSite::WebSiteTwitterControl::ChannelInfo *chInfo)
{
	Text::StringBuilderUTF8 sb;
	UOSInt retCnt = 0;
	UOSInt i;
	UOSInt j;
	sb.AppendC(UTF8STRC("https://twitter.com/"));
	sb.Append(channelId);
	ItemData *item;
	Text::XMLReader *reader;
	Text::XMLAttrib *attr;
	Int64 conversationId;
	Int64 recTime;
	Text::String *message;
	Text::String *imgURL;
	conversationId = 0;
	recTime = 0;
	imgURL = 0;
	message = 0;
#if defined(VERBOSE)
	printf("Requesting to URL %s\r\n", sb.ToString());
#endif
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, {STR_PTRC(this->userAgent)}, true, true);
	cli->Connect(sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, false);
	cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
	cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
	cli->SetTimeout(20000);
#if defined(VERBOSE)
	printf("HTTP Status code = %d\r\n", cli->GetRespStatus());
#endif
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
#if defined(VERBOSE)
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			printf("%d, <%s>\r\n", (UInt32)reader->GetPathLev(), reader->GetNodeText());
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			printf("%d, </%s>\r\n", (UInt32)reader->GetPathLev(), reader->GetNodeText());
		}
#endif
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("div")))
		{
			attr = reader->GetAttrib((UOSInt)0);
			if (attr && attr->name->Equals(UTF8STRC("class")) && attr->value && attr->value->Equals(UTF8STRC("stream-item-header")))
			{
#if defined(VERBOSE)
				printf("stream-item-header found\r\n");
#endif
				UOSInt pathLev = reader->GetPathLev();
				conversationId = 0;
				recTime = 0;
				SDEL_STRING(imgURL);
				SDEL_STRING(message);
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("small")) && reader->GetAttribCount() > 0)
					{
						attr = reader->GetAttrib((UOSInt)0);
						if (attr && attr->name->Equals(UTF8STRC("class")) && attr->value && attr->value->Equals(UTF8STRC("time")))
						{
							UOSInt smallPathLev = reader->GetPathLev();
							while (reader->ReadNext() && reader->GetPathLev() > smallPathLev)
							{
								if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("a")) && reader->GetAttribCount() > 0)
								{
									i = 0;
									j = reader->GetAttribCount();
									while (i < j)
									{
										attr = reader->GetAttrib(i);
										if (attr->name->Equals(UTF8STRC("data-conversation-id")) && attr->value)
										{
											conversationId = attr->value->ToInt64();
#if defined(VERBOSE)
											printf("conversation-id = %lld\r\n", conversationId);
#endif
										}
										i++;
									}
								}
								else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("span")) && reader->GetAttribCount() > 0)
								{
									i = 0;
									j = reader->GetAttribCount();
									while (i < j)
									{
										attr = reader->GetAttrib(i);
										if (attr->name->Equals(UTF8STRC("data-time-ms")) && attr->value)
										{
											recTime = attr->value->ToInt64();
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
			else if (attr && attr->name->Equals(UTF8STRC("class")) && attr->value && attr->value->Equals(UTF8STRC("js-tweet-text-container")))
			{
#if defined(VERBOSE)
				printf("js-tweet-text-container found\r\n");
#endif				
				UOSInt pathLev = reader->GetPathLev();
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("p")))
					{
						UOSInt smallPathLev = reader->GetPathLev();
						sb.ClearStr();
						while (reader->ReadNext() && reader->GetPathLev() > smallPathLev)
						{
							if (reader->GetNodeType() == Text::XMLNode::NT_TEXT)
							{
								sb.Append(reader->GetNodeText());
							}
							else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
							{
								if (reader->GetNodeText()->Equals(UTF8STRC("img")))
								{
									i = 0;
									j = reader->GetAttribCount();
									while (i < j)
									{
										attr = reader->GetAttrib(i);
										if (attr->name->Equals(UTF8STRC("alt")) && attr->value)
										{
											sb.Append(attr->value);
										}
										i++;
									}
								}
								else if (reader->GetNodeText()->Equals(UTF8STRC("a")) && reader->GetAttribCount() > 0)
								{
									attr = reader->GetAttrib((UOSInt)0);
									if (attr && attr->name->Equals(UTF8STRC("href")) && attr->value && attr->value->Equals(UTF8STRC("https://")))
									{
										reader->SkipElement();
									}
								}
							}
						}
						SDEL_STRING(message);
						message = Text::String::New(sb.ToString(), sb.GetLength());
					}
				}
			}
			else if (attr && attr->name->Equals(UTF8STRC("class")) && attr->value && attr->value->Equals(UTF8STRC("AdaptiveMediaOuterContainer")))
			{
#if defined(VERBOSE)
				printf("AdaptiveMediaOuterContainer found\r\n");
#endif				
				UOSInt pathLev = reader->GetPathLev();
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("div")))
					{
						i = 0;
						j = reader->GetAttribCount();
						while (i < j)
						{
							attr = reader->GetAttrib(i);
							if (attr->name->Equals(UTF8STRC("data-image-url")) && attr->value)
							{
								if (imgURL == 0)
								{
									imgURL = attr->value->Clone();
								}
								else
								{
									sb.ClearStr();
									sb.Append(imgURL);
									sb.AppendUTF8Char(' ');
									imgURL->Release();
									sb.Append(attr->value);
									imgURL = Text::String::New(sb.ToString(), sb.GetLength());
								}
							}
							i++;
						}
					}
				}
			}
			else if (attr && attr->name->Equals(UTF8STRC("class")) && attr->value && attr->value->Equals(UTF8STRC("stream-item-footer")))
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
			else if (attr && attr->name->Equals(UTF8STRC("class")) && attr->value && attr->value->Equals(UTF8STRC("ProfileHeaderCard")))
			{
				UOSInt pathLev = reader->GetPathLev();
				while (reader->ReadNext() && reader->GetPathLev() > pathLev)
				{
					if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (reader->GetNodeText()->Equals(UTF8STRC("h1")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							sb.TrimWSCRLF();
							if (chInfo)
							{
								SDEL_STRING(chInfo->name);
								chInfo->name = Text::String::New(sb.ToString(), sb.GetLength());
							}
						}
						else if (reader->GetNodeText()->Equals(UTF8STRC("p")))
						{
							i = 0;
							j = reader->GetAttribCount();
							while (i < j)
							{
								attr = reader->GetAttrib(i);
								if (attr->name->Equals(UTF8STRC("class")) && attr->value && attr->value->StartsWith(UTF8STRC("ProfileHeaderCard-bio")))
								{
									sb.ClearStr();
									reader->ReadNodeText(&sb);
									sb.TrimWSCRLF();
									if (chInfo)
									{
										SDEL_STRING(chInfo->bio);
										chInfo->bio = Text::String::New(sb.ToString(), sb.GetLength());
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
	SDEL_STRING(message);
	SDEL_STRING(imgURL);
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return retCnt;
}

void Net::WebSite::WebSiteTwitterControl::FreeItems(Data::ArrayList<Net::WebSite::WebSiteTwitterControl::ItemData*> *itemList)
{
	ItemData *item;
	UOSInt i = itemList->GetCount();
	while (i-- > 0)
	{
		item = itemList->GetItem(i);
		item->message->Release();
		SDEL_STRING(item->imgURL);
		MemFree(item);
	}
	itemList->Clear();
}

Text::String *Net::WebSite::WebSiteTwitterControl::GetUserAgent()
{
	return this->userAgent;
}
