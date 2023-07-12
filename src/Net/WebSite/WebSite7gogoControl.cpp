#include "Stdafx.h"
#include "Data/ArrayListInt64.h"
#include "Math/Math.h"
#include "Net/HTTPClient.h"
#include "Net/WebSite/WebSite7gogoControl.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"
#include <stdio.h>

Net::WebSite::WebSite7gogoControl::ItemData *Net::WebSite::WebSite7gogoControl::ParsePost(Text::JSONObject *postObj)
{
	Text::JSONBase *jsBase;
	Text::JSONString *str1;
	Text::JSONArray *arr1;
	Text::JSONObject *obj1;
	Int32 bodyType;
	UOSInt i;
	UOSInt j;
	Net::WebSite::WebSite7gogoControl::ItemData *item = MemAlloc(Net::WebSite::WebSite7gogoControl::ItemData, 1);
	MemClear(item, sizeof(Net::WebSite::WebSite7gogoControl::ItemData));
	item->id = postObj->GetObjectInt64(CSTR("postId"));
	item->recTime = postObj->GetObjectInt64(CSTR("time")) * 1000;
	item->message = Text::String::NewEmpty();
	if ((jsBase = postObj->GetObjectValue(CSTR("body"))) != 0 && jsBase->GetType() == Text::JSONType::Array)
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
				bodyType = obj1->GetObjectInt32(CSTR("bodyType"));
				if (bodyType == 1)
				{
					if ((jsBase = obj1->GetObjectValue(CSTR("text"))) != 0 && jsBase->GetType() == Text::JSONType::String)
					{
						str1 = (Text::JSONString*)jsBase;
						item->message->Release();
						item->message = str1->GetValue()->Clone();
					}
				}
				else if (bodyType == 3)
				{
					if ((jsBase = obj1->GetObjectValue(CSTR("image"))) != 0 && jsBase->GetType() == Text::JSONType::String)
					{
						str1 = (Text::JSONString*)jsBase;
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
					if ((jsBase = obj1->GetObjectValue(CSTR("movieUrlHq"))) != 0 && jsBase->GetType() == Text::JSONType::String)
					{
						str1 = (Text::JSONString*)jsBase;
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
		MemFree(item);
		return 0;
	}
}

Net::WebSite::WebSite7gogoControl::WebSite7gogoControl(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::String *userAgent)
{
	this->sockf = sockf;
	this->encFact = encFact;
	this->userAgent = SCOPY_STRING(userAgent);
}

Net::WebSite::WebSite7gogoControl::~WebSite7gogoControl()
{
	SDEL_STRING(this->userAgent);
}

OSInt Net::WebSite::WebSite7gogoControl::GetChannelItems(NotNullPtr<Text::String> channelId, OSInt pageNo, Data::ArrayList<Net::WebSite::WebSite7gogoControl::ItemData*> *itemList, Net::WebSite::WebSite7gogoControl::ChannelInfo *chInfo)
{
	Text::StringBuilderUTF8 sb;
	OSInt retCnt = 0;
	sb.AppendC(UTF8STRC("https://7gogo.jp/"));
	sb.Append(channelId);
	Net::WebSite::WebSite7gogoControl::ItemData *item;
	Data::ArrayListInt64 idList;
	Text::XMLReader *reader;
//	printf("Requesting to URL %s\r\n", sb.ToString());
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, {STR_PTRC(this->userAgent)}, true, true);
	cli->Connect(sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	NEW_CLASS(reader, Text::XMLReader(this->encFact, cli, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NodeType::Element && reader->GetNodeText()->Equals(UTF8STRC("script")))
		{
			sb.ClearStr();
			reader->ReadNodeText(&sb);
			if (sb.StartsWith(UTF8STRC("window.__DEHYDRATED_STATES__ = ")))
			{
				Text::JSONBase *baseData = Text::JSONBase::ParseJSONStr(sb.ToCString().Substring(31));
				if (baseData)
				{
					if (baseData->GetType() == Text::JSONType::Object)
					{
						Text::JSONObject *baseObj = (Text::JSONObject*)baseData;
						Text::JSONBase *jsBase;
						Text::JSONObject *obj1;
						Text::JSONArray *arr1;
						Text::JSONString *str1;
						UOSInt i;
						UOSInt j;
						UOSInt k;
						OSInt si;
						if (chInfo)
						{
							jsBase = baseObj->GetObjectValue(CSTR("page:talk:service:entity:talk"));
							if (jsBase && jsBase->GetType() == Text::JSONType::Object)
							{
								obj1 = (Text::JSONObject*)jsBase;
								if ((jsBase = obj1->GetObjectValue(CSTR("talkCode"))) != 0 && jsBase->GetType() == Text::JSONType::String)
								{
									str1 = (Text::JSONString*)jsBase;
									SDEL_STRING(chInfo->talkCode);
									chInfo->talkCode = str1->GetValue()->Clone().Ptr();
								}
								if ((jsBase = obj1->GetObjectValue(CSTR("name"))) != 0 && jsBase->GetType() == Text::JSONType::String)
								{
									str1 = (Text::JSONString*)jsBase;
									SDEL_STRING(chInfo->name);
									chInfo->name = str1->GetValue()->Clone().Ptr();
								}
								if ((jsBase = obj1->GetObjectValue(CSTR("detail"))) != 0 && jsBase->GetType() == Text::JSONType::String)
								{
									str1 = (Text::JSONString*)jsBase;
									SDEL_STRING(chInfo->detail);
									chInfo->detail = str1->GetValue()->Clone().Ptr();
								}
								if ((jsBase = obj1->GetObjectValue(CSTR("imagePath"))) != 0 && jsBase->GetType() == Text::JSONType::String)
								{
									str1 = (Text::JSONString*)jsBase;
									SDEL_STRING(chInfo->imagePath);
									chInfo->imagePath = str1->GetValue()->Clone().Ptr();
								}
								chInfo->editDate = obj1->GetObjectInt64(CSTR("editDate")) * 1000;
								if ((jsBase = obj1->GetObjectValue(CSTR("screenName"))) != 0 && jsBase->GetType() == Text::JSONType::String)
								{
									str1 = (Text::JSONString*)jsBase;
									SDEL_STRING(chInfo->screenName);
									chInfo->screenName = str1->GetValue()->Clone().Ptr();
								}
							}
						}
						jsBase = baseObj->GetObjectValue(CSTR("page:talk:service:entity:talkImages"));
						if (jsBase && jsBase->GetType() == Text::JSONType::Object)
						{
							obj1 = (Text::JSONObject*)jsBase;
							if ((jsBase = obj1->GetObjectValue(CSTR("images"))) != 0 && jsBase->GetType() == Text::JSONType::Array)
							{
								arr1 = (Text::JSONArray*)jsBase;
								i = arr1->GetArrayLength();
								while (i-- > 0)
								{
									jsBase = arr1->GetArrayValue(i);
									if (jsBase->GetType() == Text::JSONType::Object)
									{
										obj1 = (Text::JSONObject*)jsBase;
										if ((jsBase = obj1->GetObjectValue(CSTR("post"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
										{
											item = ParsePost((Text::JSONObject*)jsBase);
											if (item)
											{
												si = idList.SortedIndexOf(item->id);
												if (si >= 0)
												{
													item->message->Release();
													SDEL_STRING(item->imgURL);
													MemFree(item);
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
						jsBase = baseObj->GetObjectValue(CSTR("page:talk:service:entity:posts"));
						if (jsBase && jsBase->GetType() == Text::JSONType::Object)
						{
							obj1 = (Text::JSONObject*)jsBase;
							if ((jsBase = obj1->GetObjectValue(CSTR("posts"))) != 0 && jsBase->GetType() == Text::JSONType::Array)
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
										if ((jsBase = obj1->GetObjectValue(CSTR("post"))) != 0 && jsBase->GetType() == Text::JSONType::Object)
										{
											item = ParsePost((Text::JSONObject*)jsBase);
											if (item)
											{
												si = idList.SortedIndexOf(item->id);
												if (si >= 0)
												{
													item->message->Release();
													SDEL_STRING(item->imgURL);
													MemFree(item);
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
	if (!reader->IsComplete())
	{
		printf("Error found, errNo = %d\r\n", (Int32)reader->GetErrorCode());
	}
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	return retCnt;
}

void Net::WebSite::WebSite7gogoControl::FreeItems(Data::ArrayList<Net::WebSite::WebSite7gogoControl::ItemData*> *itemList)
{
	Net::WebSite::WebSite7gogoControl::ItemData *item;
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

Text::String *Net::WebSite::WebSite7gogoControl::GetUserAgent()
{
	return this->userAgent;
}

void Net::WebSite::WebSite7gogoControl::FreeChannelInfo(Net::WebSite::WebSite7gogoControl::ChannelInfo *chInfo)
{
	SDEL_STRING(chInfo->talkCode);
	SDEL_STRING(chInfo->name);
	SDEL_STRING(chInfo->detail);
	SDEL_STRING(chInfo->imagePath);
	SDEL_STRING(chInfo->screenName);
}
