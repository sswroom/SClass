#include "Stdafx.h"
#include "Data/ArrayListInt64.h"
#include "Math/Math.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSiteInstagramControl.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"
#include <stdio.h>

Text::JSONBase *Net::WebSite::WebSiteInstagramControl::ParsePageJSON(Text::CString url)
{
	Text::StringBuilderUTF8 sb;
	Text::JSONBase *baseData = 0;
	Text::XMLReader *reader;
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, {STR_PTRC(this->userAgent)}, true, true);
	cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT && reader->GetNodeText()->Equals(UTF8STRC("script")))
		{
			sb.ClearStr();
			reader->ReadNodeText(&sb);
			if (sb.StartsWith(UTF8STRC("window._sharedData = ")))
			{
				if (sb.EndsWith(';'))
				{
					sb.RemoveChars(1);
				}
				baseData = Text::JSONBase::ParseJSONStr(sb.ToCString().Substring(21));
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return baseData;
}

Net::WebSite::WebSiteInstagramControl::WebSiteInstagramControl(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::String *userAgent)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = SCOPY_STRING(userAgent);
}

Net::WebSite::WebSiteInstagramControl::~WebSiteInstagramControl()
{
	SDEL_STRING(this->userAgent);
}

OSInt Net::WebSite::WebSiteInstagramControl::GetChannelItems(Text::String *channelId, OSInt pageNo, Data::ArrayList<Net::WebSite::WebSiteInstagramControl::ItemData*> *itemList, Net::WebSite::WebSiteInstagramControl::ChannelInfo *chInfo)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	sb.AppendC(UTF8STRC("https://www.instagram.com/"));
	sb.Append(channelId);
	sb.AppendUTF8Char('/');
	Net::WebSite::WebSiteInstagramControl::ItemData *item;
	Text::JSONBase *baseData = this->ParsePageJSON(sb.ToCString());
	if (baseData)
	{
		Text::JSONBase *jsBase;
		Text::JSONObject *obj1;
		Text::JSONArray *arr1;
		Text::JSONString *str1;
		UOSInt i;
		UOSInt j;
		if (baseData->GetType() == Text::JSONType::Object)
		{
			obj1 = (Text::JSONObject*)baseData;
			if ((jsBase = obj1->GetObjectValue(CSTR("entry_data"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
			{
				obj1 = (Text::JSONObject*)jsBase;
				if ((jsBase = obj1->GetObjectValue(CSTR("ProfilePage"))) != 0 && jsBase->GetType() == Text::JSONType::Array)
				{
					arr1 = (Text::JSONArray*)jsBase;
					jsBase = arr1->GetArrayValue(0);
					if (jsBase && jsBase->GetType() == Text::JSONType::Object)
					{
						obj1 = (Text::JSONObject*)jsBase;
						if ((jsBase = obj1->GetObjectValue(CSTR("graphql"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
						{
							obj1 = (Text::JSONObject*)jsBase;
							if ((jsBase = obj1->GetObjectValue(CSTR("user"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
							{
								obj1 = (Text::JSONObject*)jsBase;
								if (chInfo)
								{
									if ((jsBase = obj1->GetObjectValue(CSTR("full_name"))) != 0 && jsBase->GetType() == Text::JSONType::String)
									{
										str1 = (Text::JSONString*)jsBase;
										SDEL_STRING(chInfo->full_name);
										chInfo->full_name = str1->GetValue()->Clone();
									}
									if ((jsBase = obj1->GetObjectValue(CSTR("biography"))) != 0 && jsBase->GetType() == Text::JSONType::String)
									{
										str1 = (Text::JSONString*)jsBase;
										SDEL_STRING(chInfo->biography);
										chInfo->biography = str1->GetValue()->Clone();
									}
									if ((jsBase = obj1->GetObjectValue(CSTR("profile_pic_url_hd"))) != 0 && jsBase->GetType() == Text::JSONType::String)
									{
										str1 = (Text::JSONString*)jsBase;
										SDEL_STRING(chInfo->profile_pic_url_hd);
										chInfo->profile_pic_url_hd = str1->GetValue()->Clone();
									}
									if ((jsBase = obj1->GetObjectValue(CSTR("username"))) != 0 && jsBase->GetType() == Text::JSONType::String)
									{
										str1 = (Text::JSONString*)jsBase;
										SDEL_STRING(chInfo->username);
										chInfo->username = str1->GetValue()->Clone();
									}
								}
								if ((jsBase = obj1->GetObjectValue(CSTR("edge_owner_to_timeline_media"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
								{
									obj1 = (Text::JSONObject*)jsBase;
									if ((jsBase = obj1->GetObjectValue(CSTR("edges"))) != 0 && jsBase->GetType() == Text::JSONType::Array)
									{
										arr1 = (Text::JSONArray*)jsBase;
										i = 0;
										j = arr1->GetArrayLength();
										while (i < j)
										{
											jsBase = arr1->GetArrayValue(i);
											if (jsBase->GetType() == Text::JSONType::Object)
											{
												obj1 = (Text::JSONObject*)jsBase;
												if ((jsBase = obj1->GetObjectValue(CSTR("node"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
												{
													obj1 = (Text::JSONObject*)jsBase;
													item = MemAlloc(Net::WebSite::WebSiteInstagramControl::ItemData, 1);
													MemClear(item, sizeof(Net::WebSite::WebSiteInstagramControl::ItemData));
													if ((jsBase = obj1->GetObjectValue(CSTR("id"))) != 0 && jsBase->GetType() == Text::JSONType::String)
													{
														item->id = ((Text::JSONString*)jsBase)->GetValue()->ToInt64();
													}
													if ((jsBase = obj1->GetObjectValue(CSTR("shortcode"))) != 0 && jsBase->GetType() == Text::JSONType::String)
													{
														item->shortCode = ((Text::JSONString*)jsBase)->GetValue()->Clone();
													}
													item->recTime = obj1->GetObjectInt64(CSTR("taken_at_timestamp")) * 1000;
													if ((jsBase = obj1->GetObjectValue(CSTR("display_url"))) != 0 && jsBase->GetType() == Text::JSONType::String)
													{
														item->imgURL = ((Text::JSONString*)jsBase)->GetValue()->Clone();
													}
													if ((jsBase = obj1->GetObjectValue(CSTR("video_url"))) != 0 && jsBase->GetType() == Text::JSONType::String)
													{
														item->videoURL = ((Text::JSONString*)jsBase)->GetValue()->Clone();
													}
													if ((jsBase = obj1->GetObjectValue(CSTR("__typename"))) != 0 && jsBase->GetType() == Text::JSONType::String)
													{
														item->moreImages = ((Text::JSONString*)jsBase)->GetValue()->Equals(UTF8STRC("GraphSidecar"));
													}

													if ((jsBase = obj1->GetObjectValue(CSTR("edge_media_to_caption"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
													{
														obj1 = (Text::JSONObject*)jsBase;
														if ((jsBase = obj1->GetObjectValue(CSTR("edges"))) != 0 && jsBase->GetType() == Text::JSONType::Array)
														{
															jsBase = ((Text::JSONArray*)jsBase)->GetArrayValue(0);
															if (jsBase != 0 && jsBase->GetType() == Text::JSONType::Object)
															{
																obj1 = (Text::JSONObject*)jsBase;
																if ((jsBase = obj1->GetObjectValue(CSTR("node"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
																{
																	obj1 = (Text::JSONObject*)jsBase;
																	if ((jsBase = obj1->GetObjectValue(CSTR("text"))) != 0 && jsBase->GetType() == Text::JSONType::String)
																	{
																		item->message = ((Text::JSONString*)jsBase)->GetValue()->Clone();
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
														SDEL_STRING(item->shortCode);
														SDEL_STRING(item->imgURL);
														SDEL_STRING(item->videoURL);
														SDEL_STRING(item->message);
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
		item->message->Release();
		SDEL_STRING(item->imgURL);
		SDEL_STRING(item->videoURL);
		MemFree(item);
	}
	itemList->Clear();
}

void Net::WebSite::WebSiteInstagramControl::FreeChannelInfo(Net::WebSite::WebSiteInstagramControl::ChannelInfo *chInfo)
{
	SDEL_STRING(chInfo->full_name);
	SDEL_STRING(chInfo->biography);
	SDEL_STRING(chInfo->profile_pic_url_hd);
	SDEL_STRING(chInfo->username);
}

OSInt Net::WebSite::WebSiteInstagramControl::GetPageImages(Text::String *shortCode, Data::ArrayList<Text::String*> *imageList, Data::ArrayList<Text::String*> *videoList)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	sb.AppendC(UTF8STRC("https://www.instagram.com/p/"));
	sb.Append(shortCode);
	sb.AppendUTF8Char('/');
	Text::JSONBase *baseData = this->ParsePageJSON(sb.ToCString());
	if (baseData)
	{
		Text::JSONBase *jsBase;
		Text::JSONObject *obj1;
		Text::JSONArray *arr1;
		UOSInt i;
		UOSInt j;
		if (baseData->GetType() == Text::JSONType::Object)
		{
			obj1 = (Text::JSONObject*)baseData;
			if ((jsBase = obj1->GetObjectValue(CSTR("entry_data"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
			{
				obj1 = (Text::JSONObject*)jsBase;
				if ((jsBase = obj1->GetObjectValue(CSTR("PostPage"))) != 0 && jsBase->GetType() == Text::JSONType::Array)
				{
					arr1 = (Text::JSONArray*)jsBase;
					jsBase = arr1->GetArrayValue(0);
					if (jsBase && jsBase->GetType() == Text::JSONType::Object)
					{
						obj1 = (Text::JSONObject*)jsBase;
						if ((jsBase = obj1->GetObjectValue(CSTR("graphql"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
						{
							obj1 = (Text::JSONObject*)jsBase;
							if ((jsBase = obj1->GetObjectValue(CSTR("shortcode_media"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
							{
								obj1 = (Text::JSONObject*)jsBase;
								if ((jsBase = obj1->GetObjectValue(CSTR("edge_sidecar_to_children"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
								{
									obj1 = (Text::JSONObject*)jsBase;
									if ((jsBase = obj1->GetObjectValue(CSTR("edges"))) != 0 && jsBase->GetType() == Text::JSONType::Array)
									{
										arr1 = (Text::JSONArray*)jsBase;
										i = 0;
										j = arr1->GetArrayLength();
										while (i < j)
										{
											jsBase = arr1->GetArrayValue(i);
											if (jsBase->GetType() == Text::JSONType::Object)
											{
												obj1 = (Text::JSONObject*)jsBase;
												if ((jsBase = obj1->GetObjectValue(CSTR("node"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
												{
													obj1 = (Text::JSONObject*)jsBase;
													if ((jsBase = obj1->GetObjectValue(CSTR("video_url"))) != 0 && jsBase->GetType() == Text::JSONType::String)
													{
														videoList->Add(((Text::JSONString*)jsBase)->GetValue()->Clone());
														retCnt++;
													}
													else if ((jsBase = obj1->GetObjectValue(CSTR("display_url"))) != 0 && jsBase->GetType() == Text::JSONType::String)
													{
														imageList->Add(((Text::JSONString*)jsBase)->GetValue()->Clone());
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

Text::String *Net::WebSite::WebSiteInstagramControl::GetUserAgent()
{
	return this->userAgent;
}
