#include "Stdafx.h"
#include "Data/ArrayListInt64.h"
#include "Math/Math.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSiteInstagramControl.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"
#include <stdio.h>

Text::JSONBase *Net::WebSite::WebSiteInstagramControl::ParsePageJSON(const UTF8Char *url)
{
	Text::StringBuilderUTF8 sb;
	Text::JSONBase *baseData = 0;
	Text::XMLReader *reader;
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->userAgent, true, true);
	cli->Connect(url, "GET", 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(reader->GetNodeText(), (const UTF8Char*)"script"))
		{
			sb.ClearStr();
			reader->ReadNodeText(&sb);
			if (sb.StartsWith((const UTF8Char*)"window._sharedData = "))
			{
				if (sb.EndsWith(';'))
				{
					baseData = Text::JSONBase::ParseJSONStrLen(sb.ToString() + 21, sb.GetLength() - 22);
				}
				else
				{
					baseData = Text::JSONBase::ParseJSONStr(sb.ToString() + 21);
				}
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return baseData;
}

Net::WebSite::WebSiteInstagramControl::WebSiteInstagramControl(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, const UTF8Char *userAgent)
{
	this->sockf = sockf;
	this->encFact = encFact;
	this->userAgent = userAgent?Text::StrCopyNew(userAgent):0;
}

Net::WebSite::WebSiteInstagramControl::~WebSiteInstagramControl()
{
	SDEL_TEXT(this->userAgent);
}

OSInt Net::WebSite::WebSiteInstagramControl::GetChannelItems(const UTF8Char *channelId, OSInt pageNo, Data::ArrayList<Net::WebSite::WebSiteInstagramControl::ItemData*> *itemList, Net::WebSite::WebSiteInstagramControl::ChannelInfo *chInfo)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	sb.Append((const UTF8Char*)"https://www.instagram.com/");
	sb.Append(channelId);
	sb.AppendChar('/', 1);
	Net::WebSite::WebSiteInstagramControl::ItemData *item;
	Text::JSONBase *baseData = this->ParsePageJSON(sb.ToString());
	if (baseData)
	{
		Text::JSONBase *jsBase;
		Text::JSONObject *obj1;
		Text::JSONArray *arr1;
		Text::JSONStringUTF8 *str1;
		UOSInt i;
		UOSInt j;
		if (baseData->GetJSType() == Text::JSONBase::JST_OBJECT)
		{
			obj1 = (Text::JSONObject*)baseData;
			if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"entry_data")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
			{
				obj1 = (Text::JSONObject*)jsBase;
				if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"ProfilePage")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_ARRAY)
				{
					arr1 = (Text::JSONArray*)jsBase;
					jsBase = arr1->GetArrayValue(0);
					if (jsBase && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
					{
						obj1 = (Text::JSONObject*)jsBase;
						if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"graphql")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
						{
							obj1 = (Text::JSONObject*)jsBase;
							if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"user")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
							{
								obj1 = (Text::JSONObject*)jsBase;
								if (chInfo)
								{
									if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"full_name")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
									{
										str1 = (Text::JSONStringUTF8*)jsBase;
										SDEL_TEXT(chInfo->full_name);
										chInfo->full_name = Text::StrCopyNew(str1->GetValue());
									}
									if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"biography")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
									{
										str1 = (Text::JSONStringUTF8*)jsBase;
										SDEL_TEXT(chInfo->biography);
										chInfo->biography = Text::StrCopyNew(str1->GetValue());
									}
									if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"profile_pic_url_hd")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
									{
										str1 = (Text::JSONStringUTF8*)jsBase;
										SDEL_TEXT(chInfo->profile_pic_url_hd);
										chInfo->profile_pic_url_hd = Text::StrCopyNew(str1->GetValue());
									}
									if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"username")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
									{
										str1 = (Text::JSONStringUTF8*)jsBase;
										SDEL_TEXT(chInfo->username);
										chInfo->username = Text::StrCopyNew(str1->GetValue());
									}
								}
								if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"edge_owner_to_timeline_media")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
								{
									obj1 = (Text::JSONObject*)jsBase;
									if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"edges")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_ARRAY)
									{
										arr1 = (Text::JSONArray*)jsBase;
										i = 0;
										j = arr1->GetArrayLength();
										while (i < j)
										{
											jsBase = arr1->GetArrayValue(i);
											if (jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
											{
												obj1 = (Text::JSONObject*)jsBase;
												if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"node")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
												{
													obj1 = (Text::JSONObject*)jsBase;
													item = MemAlloc(Net::WebSite::WebSiteInstagramControl::ItemData, 1);
													MemClear(item, sizeof(Net::WebSite::WebSiteInstagramControl::ItemData));
													if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"id")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
													{
														item->id = Text::StrToInt64(((Text::JSONStringUTF8*)jsBase)->GetValue());
													}
													if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"shortcode")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
													{
														item->shortCode = Text::StrCopyNew(((Text::JSONStringUTF8*)jsBase)->GetValue());
													}
													if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"taken_at_timestamp")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_NUMBER)
													{
														item->recTime = Math::Double2Int64(((Text::JSONNumber*)jsBase)->GetValue()) * 1000;
													}
													if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"display_url")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
													{
														item->imgURL = Text::StrCopyNew(((Text::JSONStringUTF8*)jsBase)->GetValue());
													}
													if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"video_url")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
													{
														item->videoURL = Text::StrCopyNew(((Text::JSONStringUTF8*)jsBase)->GetValue());
													}
													if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"__typename")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
													{
														item->moreImages = Text::StrEquals(((Text::JSONStringUTF8*)jsBase)->GetValue(), (const UTF8Char*)"GraphSidecar");
													}

													if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"edge_media_to_caption")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
													{
														obj1 = (Text::JSONObject*)jsBase;
														if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"edges")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_ARRAY)
														{
															jsBase = ((Text::JSONArray*)jsBase)->GetArrayValue(0);
															if (jsBase != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
															{
																obj1 = (Text::JSONObject*)jsBase;
																if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"node")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
																{
																	obj1 = (Text::JSONObject*)jsBase;
																	if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"text")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
																	{
																		item->message = Text::StrCopyNew(((Text::JSONStringUTF8*)jsBase)->GetValue());
																	}
																}
															}
														}
													}
													if (item->id != 0 && item->shortCode != 0 && item->recTime != 0 && item->message != 0)
													{
														itemList->Add(item);
														retCnt++;
													}
													else
													{
														SDEL_TEXT(item->shortCode);
														SDEL_TEXT(item->imgURL);
														SDEL_TEXT(item->videoURL);
														SDEL_TEXT(item->message);
														MemFree(item);
													}
												}												
											}
											i++;
										}
									}
								}
							}

						}
					}
				}
			}
		}
		baseData->EndUse();
	}
	else
	{
		printf("Error in parsing JSON data\r\n");
	}
	return retCnt;
}

void Net::WebSite::WebSiteInstagramControl::FreeItems(Data::ArrayList<Net::WebSite::WebSiteInstagramControl::ItemData*> *itemList)
{
	Net::WebSite::WebSiteInstagramControl::ItemData *item;
	UOSInt i = itemList->GetCount();
	while (i-- > 0)
	{
		item = itemList->GetItem(i);
		Text::StrDelNew(item->message);
		SDEL_TEXT(item->imgURL);
		SDEL_TEXT(item->videoURL);
		MemFree(item);
	}
	itemList->Clear();
}

void Net::WebSite::WebSiteInstagramControl::FreeChannelInfo(Net::WebSite::WebSiteInstagramControl::ChannelInfo *chInfo)
{
	SDEL_TEXT(chInfo->full_name);
	SDEL_TEXT(chInfo->biography);
	SDEL_TEXT(chInfo->profile_pic_url_hd);
	SDEL_TEXT(chInfo->username);
}

OSInt Net::WebSite::WebSiteInstagramControl::GetPageImages(const UTF8Char *shortCode, Data::ArrayList<const UTF8Char*> *imageList, Data::ArrayList<const UTF8Char*> *videoList)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	sb.Append((const UTF8Char*)"https://www.instagram.com/p/");
	sb.Append(shortCode);
	sb.AppendChar('/', 1);
	Text::JSONBase *baseData = this->ParsePageJSON(sb.ToString());
	if (baseData)
	{
		Text::JSONBase *jsBase;
		Text::JSONObject *obj1;
		Text::JSONArray *arr1;
		UOSInt i;
		UOSInt j;
		if (baseData->GetJSType() == Text::JSONBase::JST_OBJECT)
		{
			obj1 = (Text::JSONObject*)baseData;
			if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"entry_data")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
			{
				obj1 = (Text::JSONObject*)jsBase;
				if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"PostPage")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_ARRAY)
				{
					arr1 = (Text::JSONArray*)jsBase;
					jsBase = arr1->GetArrayValue(0);
					if (jsBase && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
					{
						obj1 = (Text::JSONObject*)jsBase;
						if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"graphql")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
						{
							obj1 = (Text::JSONObject*)jsBase;
							if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"shortcode_media")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
							{
								obj1 = (Text::JSONObject*)jsBase;
								if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"edge_sidecar_to_children")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
								{
									obj1 = (Text::JSONObject*)jsBase;
									if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"edges")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_ARRAY)
									{
										arr1 = (Text::JSONArray*)jsBase;
										i = 0;
										j = arr1->GetArrayLength();
										while (i < j)
										{
											jsBase = arr1->GetArrayValue(i);
											if (jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
											{
												obj1 = (Text::JSONObject*)jsBase;
												if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"node")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
												{
													obj1 = (Text::JSONObject*)jsBase;
													if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"video_url")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
													{
														videoList->Add(Text::StrCopyNew(((Text::JSONStringUTF8*)jsBase)->GetValue()));
														retCnt++;
													}
													else if ((jsBase = obj1->GetObjectValue((const UTF8Char*)"display_url")) != 0 && jsBase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
													{
														imageList->Add(Text::StrCopyNew(((Text::JSONStringUTF8*)jsBase)->GetValue()));
														retCnt++;
													}
												}												
											}
											i++;
										}
									}
								}
							}

						}
					}
				}
			}
		}
		baseData->EndUse();
	}
	else
	{
		printf("Error in parsing JSON data\r\n");
	}
	return retCnt;
}

const UTF8Char *Net::WebSite::WebSiteInstagramControl::GetUserAgent()
{
	return this->userAgent;
}
