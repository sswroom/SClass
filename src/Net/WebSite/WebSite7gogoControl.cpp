#include "Stdafx.h"
#include "Data/ArrayListInt64.h"
#include "Math/Math.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSite7gogoControl.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"
#include <stdio.h>

Optional<Net::WebSite::WebSite7gogoControl::ItemData> Net::WebSite::WebSite7gogoControl::ParsePost(NN<Text::JSONObject> postObj)
{
	NN<Text::JSONBase> jsBase;
	NN<Text::JSONString> str1;
	NN<Text::JSONArray> arr1;
	NN<Text::JSONObject> obj1;
	Int32 bodyType;
	UOSInt i;
	UOSInt j;
	NN<Net::WebSite::WebSite7gogoControl::ItemData> item = MemAllocNN(Net::WebSite::WebSite7gogoControl::ItemData);
	item.ZeroContent();
	item->id = postObj->GetObjectInt64(CSTR("postId"));
	item->recTime = postObj->GetObjectInt64(CSTR("time")) * 1000;
	item->message = Text::String::NewEmpty();
	if (postObj->GetObjectValue(CSTR("body")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Array)
	{
		arr1 = NN<Text::JSONArray>::ConvertFrom(jsBase);
		i = 0;
		j = arr1->GetArrayLength();
		while (i < j)
		{
			if (arr1->GetArrayValue(i).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
			{
				obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
				bodyType = obj1->GetObjectInt32(CSTR("bodyType"));
				if (bodyType == 1)
				{
					if (obj1->GetObjectValue(CSTR("text")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
					{
						str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
						item->message->Release();
						item->message = str1->GetValue()->Clone();
					}
				}
				else if (bodyType == 3)
				{
					if (obj1->GetObjectValue(CSTR("image")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
					{
						str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
						if (item->imgURL)
						{
							Text::StringBuilderUTF8 sb;
							sb.Append(item->imgURL);
							sb.AppendUTF8Char(' ');
							sb.Append(str1->GetValue());
							item->imgURL->Release();
							item->imgURL = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
						}
						else
						{
							item->imgURL = str1->GetValue()->Clone().Ptr();
						}
					}
				}
				else if (bodyType == 8)
				{
					if (obj1->GetObjectValue(CSTR("movieUrlHq")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
					{
						str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
						if (item->imgURL)
						{
							Text::StringBuilderUTF8 sb;
							sb.Append(item->imgURL);
							sb.AppendUTF8Char(' ');
							sb.Append(str1->GetValue());
							item->imgURL->Release();
							item->imgURL = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
						}
						else
						{
							item->imgURL = str1->GetValue()->Clone().Ptr();
						}
					}
				}
			}
			i++;
		}
	}

	if (item->id != 0 && item->recTime != 0 && item->message->leng != 0)
	{
		return item;
	}
	else
	{
		SDEL_STRING(item->imgURL);
		item->message->Release();
		MemFreeNN(item);
		return 0;
	}
}

Net::WebSite::WebSite7gogoControl::WebSite7gogoControl(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent)
{
	this->sockf = sockf;
	this->encFact = encFact;
	this->userAgent = Text::String::CopyOrNull(userAgent);
}

Net::WebSite::WebSite7gogoControl::~WebSite7gogoControl()
{
	OPTSTR_DEL(this->userAgent);
}

OSInt Net::WebSite::WebSite7gogoControl::GetChannelItems(NN<Text::String> channelId, OSInt pageNo, NN<Data::ArrayListNN<Net::WebSite::WebSite7gogoControl::ItemData>> itemList, Optional<Net::WebSite::WebSite7gogoControl::ChannelInfo> chInfo)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	sb.AppendC(UTF8STRC("https://7gogo.jp/"));
	sb.Append(channelId);
	NN<Net::WebSite::WebSite7gogoControl::ItemData> item;
	Data::ArrayListInt64 idList;
//	printf("Requesting to URL %s\r\n", sb.ToString());
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, OPTSTR_CSTR(this->userAgent), true, true);
	cli->Connect(sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_HTML);
	while (reader.ReadNext())
	{
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Element && reader.GetNodeTextNN()->Equals(UTF8STRC("script")))
		{
			sb.ClearStr();
			reader.ReadNodeText(sb);
			if (sb.StartsWith(UTF8STRC("window.__DEHYDRATED_STATES__ = ")))
			{
				NN<Text::JSONBase> baseData;
				if (Text::JSONBase::ParseJSONStr(sb.ToCString().Substring(31)).SetTo(baseData))
				{
					if (baseData->GetType() == Text::JSONType::Object)
					{
						NN<Text::JSONObject> baseObj = NN<Text::JSONObject>::ConvertFrom(baseData);
						NN<Text::JSONBase> jsBase;
						NN<Text::JSONObject> obj1;
						NN<Text::JSONArray> arr1;
						NN<Text::JSONString> str1;
						UOSInt i;
						UOSInt j;
						UOSInt k;
						OSInt si;
						NN<ChannelInfo> nnchInfo;
						if (chInfo.SetTo(nnchInfo))
						{
							if (baseObj->GetObjectValue(CSTR("page:talk:service:entity:talk")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
							{
								obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
								if (obj1->GetObjectValue(CSTR("talkCode")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
								{
									str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
									SDEL_STRING(nnchInfo->talkCode);
									nnchInfo->talkCode = str1->GetValue()->Clone().Ptr();
								}
								if (obj1->GetObjectValue(CSTR("name")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
								{
									str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
									SDEL_STRING(nnchInfo->name);
									nnchInfo->name = str1->GetValue()->Clone().Ptr();
								}
								if (obj1->GetObjectValue(CSTR("detail")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
								{
									str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
									SDEL_STRING(nnchInfo->detail);
									nnchInfo->detail = str1->GetValue()->Clone().Ptr();
								}
								if (obj1->GetObjectValue(CSTR("imagePath")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
								{
									str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
									SDEL_STRING(nnchInfo->imagePath);
									nnchInfo->imagePath = str1->GetValue()->Clone().Ptr();
								}
								nnchInfo->editDate = obj1->GetObjectInt64(CSTR("editDate")) * 1000;
								if (obj1->GetObjectValue(CSTR("screenName")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::String)
								{
									str1 = NN<Text::JSONString>::ConvertFrom(jsBase);
									SDEL_STRING(nnchInfo->screenName);
									nnchInfo->screenName = str1->GetValue()->Clone().Ptr();
								}
							}
						}
						if (baseObj->GetObjectValue(CSTR("page:talk:service:entity:talkImages")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
						{
							obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
							if (obj1->GetObjectValue(CSTR("images")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Array)
							{
								arr1 = NN<Text::JSONArray>::ConvertFrom(jsBase);
								i = arr1->GetArrayLength();
								while (i-- > 0)
								{
									if (arr1->GetArrayValue(i).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
									{
										obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
										if (obj1->GetObjectValue(CSTR("post")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
										{
											if (ParsePost(NN<Text::JSONObject>::ConvertFrom(jsBase)).SetTo(item))
											{
												si = idList.SortedIndexOf(item->id);
												if (si >= 0)
												{
													item->message->Release();
													SDEL_STRING(item->imgURL);
													MemFreeNN(item);
												}
												else
												{
													k = idList.SortedInsert(item->id);
													itemList->Insert(k, item);
													retCnt++;
												}
											}
										}
									}
								}
							}
						}
						if (baseObj->GetObjectValue(CSTR("page:talk:service:entity:posts")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
						{
							obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
							if (obj1->GetObjectValue(CSTR("posts")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Array)
							{
								arr1 = NN<Text::JSONArray>::ConvertFrom(jsBase);
								i = 0;
								j = arr1->GetArrayLength();
								while (i < j)
								{
									if (arr1->GetArrayValue(i).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
									{
										obj1 = NN<Text::JSONObject>::ConvertFrom(jsBase);
										if (obj1->GetObjectValue(CSTR("post")).SetTo(jsBase) && jsBase->GetType() == Text::JSONType::Object)
										{
											if (ParsePost(NN<Text::JSONObject>::ConvertFrom(jsBase)).SetTo(item))
											{
												si = idList.SortedIndexOf(item->id);
												if (si >= 0)
												{
													item->message->Release();
													SDEL_STRING(item->imgURL);
													MemFreeNN(item);
												}
												else
												{
													k = idList.SortedInsert(item->id);
													itemList->Insert(k, item);
													retCnt++;
												}
											}
										}
									}
									i++;
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
				
			}
		}
	}
	if (!reader.IsComplete())
	{
		printf("Error found, errNo = %d\r\n", (Int32)reader.GetErrorCode());
	}
	cli.Delete();
	return retCnt;
}

void Net::WebSite::WebSite7gogoControl::FreeItems(NN<Data::ArrayListNN<Net::WebSite::WebSite7gogoControl::ItemData>> itemList)
{
	NN<Net::WebSite::WebSite7gogoControl::ItemData> item;
	UOSInt i = itemList->GetCount();
	while (i-- > 0)
	{
		item = itemList->GetItemNoCheck(i);
		item->message->Release();
		SDEL_STRING(item->imgURL);
		MemFreeNN(item);
	}
	itemList->Clear();
}

Optional<Text::String> Net::WebSite::WebSite7gogoControl::GetUserAgent()
{
	return this->userAgent;
}

void Net::WebSite::WebSite7gogoControl::FreeChannelInfo(NN<Net::WebSite::WebSite7gogoControl::ChannelInfo> chInfo)
{
	SDEL_STRING(chInfo->talkCode);
	SDEL_STRING(chInfo->name);
	SDEL_STRING(chInfo->detail);
	SDEL_STRING(chInfo->imagePath);
	SDEL_STRING(chInfo->screenName);
}
