#include "Stdafx.h"
#include "Data/ArrayListInt64.h"
#include "Math/Math_C.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSiteInstagramControl.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"
#include <stdio.h>

Optional<Text::JSONBase> Net::WebSite::WebSiteInstagramControl::ParsePageJSON(Text::CStringNN url)
{
	Text::StringBuilderUTF8 sb;
	Optional<Text::JSONBase> baseData = nullptr;
	NN<Text::XMLReader> reader;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, OPTSTR_CSTR(this->userAgent), true, true);
	cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	NEW_CLASSNN(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeTextNN()->Equals(UTF8STRC("script")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
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
	reader.Delete();
	cli.Delete();
	return baseData;
}

Net::WebSite::WebSiteInstagramControl::WebSiteInstagramControl(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent)
{
	this->clif = clif;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = Text::String::CopyOrNull(userAgent);
}

Net::WebSite::WebSiteInstagramControl::~WebSiteInstagramControl()
{
	OPTSTR_DEL(this->userAgent);
}

OSInt Net::WebSite::WebSiteInstagramControl::GetChannelItems(NN<Text::String> channelId, OSInt pageNo, NN<Data::ArrayListNN<Net::WebSite::WebSiteInstagramControl::ItemData>> itemList, Optional<Net::WebSite::WebSiteInstagramControl::ChannelInfo> chInfo)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	sb.AppendC(UTF8STRC("https://www.instagram.com/"));
	sb.Append(channelId);
	sb.AppendUTF8Char('/');
	NN<Net::WebSite::WebSiteInstagramControl::ItemData> item;
	NN<Text::JSONBase> baseData;
	if (this->ParsePageJSON(sb.ToCString()).SetTo(baseData))
	{
		NN<Text::JSONBase> jsBase;
		NN<Text::JSONObject> obj1;
		NN<Text::JSONArray> arr1;
		NN<Text::JSONString> str1;
		UOSInt i;
		UOSInt j;
		if (baseData->GetType() == Text::JSONType::Object)
		{
			obj1 = NN<Text::JSONObject>::ConvertFrom(baseData);
			if (obj1->GetObjectValue(CSTR("entry_data")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
			{
				obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
				if (obj1->GetObjectValue(CSTR("ProfilePage")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Array)
				{
					arr1 = NN<Text::JSONArray>::ConvertFrom(jsBase);
					if (arr1->GetArrayValue(0).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
					{
						obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
						if (obj1->GetObjectValue(CSTR("graphql")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
						{
							obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
							if (obj1->GetObjectValue(CSTR("user")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
							{
								obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
								NN<ChannelInfo> nnchInfo;
								if (chInfo.SetTo(nnchInfo))
								{
									if (obj1->GetObjectValue(CSTR("full_name")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
									{
										str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
										OPTSTR_DEL(nnchInfo->full_name);
										nnchInfo->full_name = str1->GetValue()->Clone();
									}
									if (obj1->GetObjectValue(CSTR("biography")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
									{
										str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
										OPTSTR_DEL(nnchInfo->biography);
										nnchInfo->biography = str1->GetValue()->Clone();
									}
									if (obj1->GetObjectValue(CSTR("profile_pic_url_hd")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
									{
										str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
										OPTSTR_DEL(nnchInfo->profile_pic_url_hd);
										nnchInfo->profile_pic_url_hd = str1->GetValue()->Clone();
									}
									if (obj1->GetObjectValue(CSTR("username")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
									{
										str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
										OPTSTR_DEL(nnchInfo->username);
										nnchInfo->username = str1->GetValue()->Clone();
									}
								}
								if (obj1->GetObjectValue(CSTR("edge_owner_to_timeline_media")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
								{
									obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
									if (obj1->GetObjectValue(CSTR("edges")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Array)
									{
										arr1 = NN<Text::JSONArray>::ConvertFrom(jsBase);
										i = 0;
										j = arr1->GetArrayLength();
										while (i < j)
										{
											if (arr1->GetArrayValue(i).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
											{
												obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
												if (obj1->GetObjectValue(CSTR("node")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
												{
													obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
													item = MemAllocNN(Net::WebSite::WebSiteInstagramControl::ItemData);
													item.ZeroContent();
													if (obj1->GetObjectValue(CSTR("id")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
													{
														item->id = NN<Text::JSONString>::ConvertFrom(jsBase)->GetValue()->ToInt64();
													}
													if (obj1->GetObjectValue(CSTR("shortcode")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
													{
														item->shortCode = NN<Text::JSONString>::ConvertFrom(jsBase)->GetValue()->Clone();
													}
													else
													{
														item->shortCode = Text::String::NewEmpty();
													}
													item->message = Text::String::NewEmpty();
													item->recTime = obj1->GetObjectInt64(CSTR("taken_at_timestamp")) * 1000;
													if (obj1->GetObjectValue(CSTR("display_url")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
													{
														item->imgURL = NN<Text::JSONString>::ConvertFrom(jsBase)->GetValue()->Clone();
													}
													if (obj1->GetObjectValue(CSTR("video_url")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
													{
														item->videoURL = NN<Text::JSONString>::ConvertFrom(jsBase)->GetValue()->Clone();
													}
													if (obj1->GetObjectValue(CSTR("__typename")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
													{
														item->moreImages = NN<Text::JSONString>::ConvertFrom(jsBase)->GetValue()->Equals(UTF8STRC("GraphSidecar"));
													}

													if (obj1->GetObjectValue(CSTR("edge_media_to_caption")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
													{
														obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
														if (obj1->GetObjectValue(CSTR("edges")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Array)
														{
															if (NN<Text::JSONArray>::ConvertFrom(jsBase)->GetArrayValue(0).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
															{
																obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
																if (obj1->GetObjectValue(CSTR("node")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
																{
																	obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
																	if (obj1->GetObjectValue(CSTR("text")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
																	{
																		item->message->Release();
																		item->message = NN<Text::JSONString>::ConvertFrom(jsBase)->GetValue()->Clone();
																	}
																}
															}
														}
													}
													if (item->id != 0 && item->recTime != 0)
													{
														itemList->Add(item);
														retCnt++;
													}
													else
													{
														item->shortCode->Release();
														OPTSTR_DEL(item->imgURL);
														OPTSTR_DEL(item->videoURL);
														item->message->Release();
														MemFreeNN(item);
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

void Net::WebSite::WebSiteInstagramControl::FreeItems(NN<Data::ArrayListNN<Net::WebSite::WebSiteInstagramControl::ItemData>> itemList)
{
	NN<Net::WebSite::WebSiteInstagramControl::ItemData> item;
	UOSInt i = itemList->GetCount();
	while (i-- > 0)
	{
		item = itemList->GetItemNoCheck(i);
		item->message->Release();
		OPTSTR_DEL(item->imgURL);
		OPTSTR_DEL(item->videoURL);
		MemFreeNN(item);
	}
	itemList->Clear();
}

void Net::WebSite::WebSiteInstagramControl::FreeChannelInfo(NN<Net::WebSite::WebSiteInstagramControl::ChannelInfo> chInfo)
{
	OPTSTR_DEL(chInfo->full_name);
	OPTSTR_DEL(chInfo->biography);
	OPTSTR_DEL(chInfo->profile_pic_url_hd);
	OPTSTR_DEL(chInfo->username);
}

OSInt Net::WebSite::WebSiteInstagramControl::GetPageImages(NN<Text::String> shortCode, NN<Data::ArrayListStringNN> imageList, NN<Data::ArrayListStringNN> videoList)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	sb.AppendC(UTF8STRC("https://www.instagram.com/p/"));
	sb.Append(shortCode);
	sb.AppendUTF8Char('/');
	NN<Text::JSONBase> baseData;
	if (this->ParsePageJSON(sb.ToCString()).SetTo(baseData))
	{
		NN<Text::JSONBase> jsBase;
		NN<Text::JSONObject> obj1;
		NN<Text::JSONArray> arr1;
		UOSInt i;
		UOSInt j;
		if (baseData->GetType() == Text::JSONType::Object)
		{
			obj1 = NN<Text::JSONObject>::ConvertFrom(baseData);
			if (obj1->GetObjectValue(CSTR("entry_data")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
			{
				obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
				if (obj1->GetObjectValue(CSTR("PostPage")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Array)
				{
					arr1 = NN<Text::JSONArray>::ConvertFrom(jsBase);
					if (arr1->GetArrayValue(0).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
					{
						obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
						if (obj1->GetObjectValue(CSTR("graphql")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
						{
							obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
							if (obj1->GetObjectValue(CSTR("shortcode_media")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
							{
								obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
								if (obj1->GetObjectValue(CSTR("edge_sidecar_to_children")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
								{
									obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
									if (obj1->GetObjectValue(CSTR("edges")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Array)
									{
										arr1 = NN<Text::JSONArray>::ConvertFrom(jsBase);
										i = 0;
										j = arr1->GetArrayLength();
										while (i < j)
										{
											if (arr1->GetArrayValue(i).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
											{
												obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
												if (obj1->GetObjectValue(CSTR("node")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
												{
													obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
													if (obj1->GetObjectValue(CSTR("video_url")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
													{
														videoList->Add(NN<Text::JSONString>::ConvertFrom(jsBase)->GetValue()->Clone());
														retCnt++;
													}
													else if (obj1->GetObjectValue(CSTR("display_url")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
													{
														imageList->Add(NN<Text::JSONString>::ConvertFrom(jsBase)->GetValue()->Clone());
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

Optional<Text::String> Net::WebSite::WebSiteInstagramControl::GetUserAgent()
{
	return this->userAgent;
}
