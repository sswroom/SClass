#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSiteTwitterControl.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Net::WebSite::WebSiteTwitterControl::WebSiteTwitterControl(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent)
{
	this->clif = clif;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = nullptr;
}

Net::WebSite::WebSiteTwitterControl::~WebSiteTwitterControl()
{
	OPTSTR_DEL(this->userAgent);
}

UIntOS Net::WebSite::WebSiteTwitterControl::GetChannelItems(NN<Text::String> channelId, UIntOS pageNo, NN<Data::ArrayListNN<Net::WebSite::WebSiteTwitterControl::ItemData>> itemList, Optional<Net::WebSite::WebSiteTwitterControl::ChannelInfo> chInfo)
{
	Text::StringBuilderUTF8 sb;
	UIntOS retCnt = 0;
	UIntOS i;
	UIntOS j;
	sb.AppendC(UTF8STRC("https://twitter.com/"));
	sb.Append(channelId);
	NN<ItemData> item;
	NN<Text::XMLAttrib> attr;
	Int64 conversationId;
	Int64 recTime;
	Text::String *message;
	NN<Text::String> nnmessage;
	Text::String *imgURL;
	NN<Text::String> avalue;
	conversationId = 0;
	recTime = 0;
	imgURL = 0;
	message = 0;
#if defined(VERBOSE)
	printf("Requesting to URL %s\r\n", sb.ToString());
#endif
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, OPTSTR_CSTR(this->userAgent), true, true);
	cli->Connect(sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, false);
	cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
	cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
	cli->SetTimeout(20000);
#if defined(VERBOSE)
	printf("HTTP Status code = %d\r\n", cli->GetRespStatus());
#endif
	Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_HTML);
	while (reader.ReadNext())
	{
#if defined(VERBOSE)
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			printf("%d, <%s>\r\n", (UInt32)reader.GetPathLev(), reader.GetNodeText());
		}
		else if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			printf("%d, </%s>\r\n", (UInt32)reader.GetPathLev(), reader.GetNodeText());
		}
#endif
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("div")))
		{
			if (reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("class")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("stream-item-header")))
			{
#if defined(VERBOSE)
				printf("stream-item-header found\r\n");
#endif
				UIntOS pathLev = reader.GetPathLev();
				conversationId = 0;
				recTime = 0;
				SDEL_STRING(imgURL);
				SDEL_STRING(message);
				while (reader.ReadNext() && reader.GetPathLev() > pathLev)
				{
					if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("small")) && reader.GetAttribCount() > 0)
					{
						if (reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("class")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("time")))
						{
							UIntOS smallPathLev = reader.GetPathLev();
							while (reader.ReadNext() && reader.GetPathLev() > smallPathLev)
							{
								if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("a")) && reader.GetAttribCount() > 0)
								{
									i = 0;
									j = reader.GetAttribCount();
									while (i < j)
									{
										attr = reader.GetAttribNoCheck(i);
										if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("data-conversation-id")) && attr->value.SetTo(avalue))
										{
											conversationId = avalue->ToInt64();
#if defined(VERBOSE)
											printf("conversation-id = %lld\r\n", conversationId);
#endif
										}
										i++;
									}
								}
								else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("span")) && reader.GetAttribCount() > 0)
								{
									i = 0;
									j = reader.GetAttribCount();
									while (i < j)
									{
										attr = reader.GetAttribNoCheck(i);
										if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("data-time-ms")) && attr->value.SetTo(avalue))
										{
											recTime = avalue->ToInt64();
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
			else if (reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("class")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("js-tweet-text-container")))
			{
#if defined(VERBOSE)
				printf("js-tweet-text-container found\r\n");
#endif				
				UIntOS pathLev = reader.GetPathLev();
				while (reader.ReadNext() && reader.GetPathLev() > pathLev)
				{
					if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("p")))
					{
						UIntOS smallPathLev = reader.GetPathLev();
						sb.ClearStr();
						while (reader.ReadNext() && reader.GetPathLev() > smallPathLev)
						{
							if (reader.GetNodeType() == Text::XMLNode::NodeType::Text)
							{
								sb.AppendOpt(reader.GetNodeText());
							}
							else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
							{
								if (reader.GetNodeTextNN()->Equals(UTF8STRC("img")))
								{
									i = 0;
									j = reader.GetAttribCount();
									while (i < j)
									{
										attr = reader.GetAttribNoCheck(i);
										if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("alt")) && attr->value.SetTo(avalue))
										{
											sb.Append(avalue);
										}
										i++;
									}
								}
								else if (reader.GetNodeTextNN()->Equals(UTF8STRC("a")) && reader.GetAttribCount() > 0)
								{
									if (reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("href")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("https://")))
									{
										reader.SkipElement();
									}
								}
							}
						}
						SDEL_STRING(message);
						message = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
					}
				}
			}
			else if (reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("class")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("AdaptiveMediaOuterContainer")))
			{
#if defined(VERBOSE)
				printf("AdaptiveMediaOuterContainer found\r\n");
#endif				
				UIntOS pathLev = reader.GetPathLev();
				while (reader.ReadNext() && reader.GetPathLev() > pathLev)
				{
					if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("div")))
					{
						i = 0;
						j = reader.GetAttribCount();
						while (i < j)
						{
							attr = reader.GetAttribNoCheck(i);
							if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("data-image-url")) && attr->value.SetTo(avalue))
							{
								if (imgURL == 0)
								{
									imgURL = avalue->Clone().Ptr();
								}
								else
								{
									sb.ClearStr();
									sb.Append(imgURL);
									sb.AppendUTF8Char(' ');
									imgURL->Release();
									sb.Append(avalue);
									imgURL = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
								}
							}
							i++;
						}
					}
				}
			}
			else if (reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("class")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("stream-item-footer")))
			{
#if defined(VERBOSE)
				printf("stream-item-footer found\r\n");
#endif				
				if (conversationId != 0 && recTime != 0 && nnmessage.Set(message))
				{
					item = MemAllocNN(ItemData);
					item->id = conversationId;
					item->recTime = recTime;
					item->message = nnmessage;
					item->imgURL = imgURL;
					message = 0;
					imgURL = 0;
					itemList->Add(item);
					retCnt++;
				}
			}
			else if (reader.GetAttrib((UIntOS)0).SetTo(attr) && Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("class")) && Text::String::OrEmpty(attr->value)->Equals(UTF8STRC("ProfileHeaderCard")))
			{
				NN<ChannelInfo> nnchInfo;
				UIntOS pathLev = reader.GetPathLev();
				while (reader.ReadNext() && reader.GetPathLev() > pathLev)
				{
					if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						if (reader.GetNodeTextNN()->Equals(UTF8STRC("h1")))
						{
							sb.ClearStr();
							reader.ReadNodeText(sb);
							sb.TrimWSCRLF();
							if (chInfo.SetTo(nnchInfo))
							{
								OPTSTR_DEL(nnchInfo->name);
								nnchInfo->name = Text::String::New(sb.ToString(), sb.GetLength());
							}
						}
						else if (reader.GetNodeTextNN()->Equals(UTF8STRC("p")))
						{
							i = 0;
							j = reader.GetAttribCount();
							while (i < j)
							{
								attr = reader.GetAttribNoCheck(i);
								if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("class")) && Text::String::OrEmpty(attr->value)->StartsWith(UTF8STRC("ProfileHeaderCard-bio")))
								{
									sb.ClearStr();
									reader.ReadNodeText(sb);
									sb.TrimWSCRLF();
									if (chInfo.SetTo(nnchInfo))
									{
										OPTSTR_DEL(nnchInfo->bio);
										nnchInfo->bio = Text::String::New(sb.ToString(), sb.GetLength());
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
	if (!reader.IsComplete())
	{
#if defined(VERBOSE)
		printf("Error found, errNo = %d\r\n", (Int32)reader.GetErrorCode());
#endif
	}
	SDEL_STRING(message);
	SDEL_STRING(imgURL);
	cli.Delete();
	return retCnt;
}

void Net::WebSite::WebSiteTwitterControl::FreeItems(NN<Data::ArrayListNN<Net::WebSite::WebSiteTwitterControl::ItemData>> itemList)
{
	NN<ItemData> item;
	UIntOS i = itemList->GetCount();
	while (i-- > 0)
	{
		item = itemList->GetItemNoCheck(i);
		item->message->Release();
		OPTSTR_DEL(item->imgURL);
		MemFreeNN(item);
	}
	itemList->Clear();
}

Optional<Text::String> Net::WebSite::WebSiteTwitterControl::GetUserAgent()
{
	return this->userAgent;
}
