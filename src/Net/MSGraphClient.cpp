#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/MSGraphClient.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/URIEncoding.h"

#include "IO/FileStream.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Net::MSGraphAccessToken::MSGraphAccessToken(NotNullPtr<Text::String> type, Int32 expiresIn, Int32 extExpiresIn, NotNullPtr<Text::String> accessToken)
{
	Data::Timestamp t = Data::Timestamp::Now();
	this->type = type->Clone();
	this->accessToken = accessToken->Clone();
	this->extExpiresIn = t.AddSecond(extExpiresIn);
	this->expiresIn = t.AddSecond(expiresIn);
}

Net::MSGraphAccessToken::~MSGraphAccessToken()
{
	this->type->Release();
	this->accessToken->Release();
}

void Net::MSGraphAccessToken::InitClient(NotNullPtr<Net::HTTPClient> cli)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->type);
	sb.AppendUTF8Char(' ');
	sb.Append(this->accessToken);
	cli->AddHeaderC(CSTR("Authorization"), sb.ToCString());
}

/*
Text::CStringNN Net::MSGraphEventMessageRequest::knownTypes[] = {
	CSTR("@odata.etag"),
	CSTR("@odata.type"),
	CSTR("allowNewTimeProposals"),
	CSTR("bccRecipients"),
	CSTR("body"),
	CSTR("bodyPreview"),
	CSTR("categories"),
	CSTR("ccRecipients"),
	CSTR("changeKey"),
	CSTR("conversationId"),
	CSTR("conversationIndex"),
	CSTR("createdDateTime"),
	CSTR("endDateTime"),
	CSTR("flag"),
	CSTR("from"),
	CSTR("id"),
	CSTR("importance"),
	CSTR("inferenceClassification"),
	CSTR("internetMessageId"),
	CSTR("isAllDay"),
	CSTR("isDelegated"),
	CSTR("isDeliveryReceiptRequested"),
	CSTR("isDraft"),
	CSTR("isOutOfDate"),
	CSTR("isRead"),
	CSTR("isReadReceiptRequested"),
	CSTR("lastModifiedDateTime"),
	CSTR("location"),
	CSTR("meetingMessageType"),
	CSTR("meetingRequestType"),
	CSTR("parentFolderId"),
	CSTR("previousEndDateTime"),
	CSTR("previousLocation"),
	CSTR("previousStartDateTime"),
	CSTR("receivedDateTime"),
	CSTR("recurrence"),
	CSTR("replyTo"),
	CSTR("responseRequested"),
	CSTR("sender"),
	CSTR("sentDateTime"),
	CSTR("startDateTime"),
	CSTR("subject"),
	CSTR("toRecipients"),
	CSTR("type"),
	CSTR("webLink")
};

Net::MSGraphEventMessageRequest::MSGraphEventMessageRequest(NotNullPtr<Text::JSONObject> obj)
{
	this->obj = obj;
	this->obj->BeginUse();
}

Net::MSGraphEventMessageRequest::~MSGraphEventMessageRequest()
{
	this->obj->EndUse();
}

Optional<Net::MSGraphEventMessageRequest> Net::MSGraphEventMessageRequest::Parse(NotNullPtr<Text::JSONObject> obj)
{
	NotNullPtr<Text::String> type;
	if (obj->GetValueString(CSTR("@odata.type")).SetTo(type) && type->Equals(CSTR("#microsoft.graph.eventMessageRequest")))
	{
		MSGraphClient::HasUnknownTypes(obj, IsKnownType, CSTR("MSGraphEventMessageRequest"));
		NotNullPtr<MSGraphEventMessageRequest> msg;
		NEW_CLASSNN(msg, MSGraphEventMessageRequest(obj));
		return msg;
	}
	return 0;
}

Bool Net::MSGraphEventMessageRequest::IsKnownType(Text::CStringNN type)
{
	OSInt i = 0;
	OSInt j = (sizeof(knownTypes) / sizeof(knownTypes[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = knownTypes[k].CompareToFast(type);
		if (l > 0)
			j = k - 1;
		else if (l < 0)
			i = k + 1;
		else
			return true;
	}
	return false;
}*/

Optional<Net::MSGraphAccessToken> Net::MSGraphClient::AccessTokenParse(Net::WebStatus::StatusCode status, Text::CStringNN content)
{
	if (status != Net::WebStatus::SC_OK)
	{
#if defined(VERBOSE)
		printf("MSGraphClient: Error in getting access token: status = %d, content = %s\r\n", (Int32)status, content.v);
#endif
		return 0;
	}
	Text::JSONBase *result = Text::JSONBase::ParseJSONStr(content);
	if (result == 0)
	{
#if defined(VERBOSE)
		printf("MSGraphClient: Error in parsing access token (Not JSON): content = %s\r\n", content.v);
#endif
		return 0;
	}
	NotNullPtr<Text::String> t;
	NotNullPtr<Text::String> at;
	Int32 expiresIn;
	Int32 extExpiresIn;
	if (result->GetValueString(CSTR("token_type")).SetTo(t) &&
		result->GetValueString(CSTR("access_token")).SetTo(at) &&
		result->GetValueAsInt32(CSTR("expires_in"), expiresIn) &&
		result->GetValueAsInt32(CSTR("ext_expires_in"), extExpiresIn))
	{
		NotNullPtr<MSGraphAccessToken> token;
		NEW_CLASSNN(token, MSGraphAccessToken(t, expiresIn, extExpiresIn, at));
		result->EndUse();
		return token;
	}
	else
	{
#if defined(VERBOSE)
		printf("MSGraphClient: Error in parsing access token (Fields not found): content = %s\r\n", content.v);
#endif
		result->EndUse();
		return 0;
	}
}

template<class T> Bool Net::MSGraphClient::GetList(NotNullPtr<MSGraphAccessToken> token, Text::CStringNN url, Text::CStringNN funcName, NotNullPtr<Data::ArrayListNN<T>> dataList)
{
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	Text::StringBuilderUTF8 sb;
	if (cli->ReadAllContent(sb, 4096, 1048576 * 4))
	{
		cli.Delete();
		Text::JSONBase *json = Text::JSONBase::ParseJSONStr(sb.ToCString());
		if (json == 0)
		{
#if defined(VERBOSE)
			printf("MSGraphClient: %s cannot parse result: %d, %s\r\n", funcName.v, (Int32)status, sb.ToString());
			printf("MSGraphClient: %s url: %s\r\n", funcName.v, url.v);
#endif
			return false;
		}
		if (json->GetType() != Text::JSONType::Object)
		{
			json->EndUse();
#if defined(VERBOSE)
			printf("MSGraphClient: %s not json object: %d, %s\r\n", funcName.v, (Int32)status, sb.ToString());
#endif
			return false;
		}
		Text::JSONObject *obj = (Text::JSONObject*)json;
		Text::JSONArray *arr = obj->GetObjectArray(CSTR("value"));
		if (arr == 0)
		{
			json->EndUse();
#if defined(VERBOSE)
			printf("MSGraphClient: %s value array not found: %d, %s\r\n", funcName.v, (Int32)status, sb.ToString());
#endif
			return false;
		}
		NotNullPtr<Text::JSONObject> dataObj;
		NotNullPtr<T> data;
		UOSInt i = 0;
		UOSInt j = arr->GetArrayLength();
		while (i < j)
		{
			if (dataObj.Set(arr->GetArrayObject(i)))
			{
				NEW_CLASSNN(data, T(dataObj));
				if (data->IsValid())
				{
					dataList->Add(data);
				}
				else
				{
					data.Delete();
				}
			}
			i++;
		}
		json->EndUse();
//		IO::FileStream fs(CSTR("mailfolders.json"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
//		fs.WriteCont(sb.v, sb.leng);
		return true;
	}
	else
	{
		cli.Delete();
#if defined(VERBOSE)
		printf("MSGraphClient: %s request error: status = %d\r\n", funcName.v, (Int32)status);
#endif
	}
	return false;
}

Net::MSGraphClient::MSGraphClient(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl)
{
	this->sockf = sockf;
	this->ssl = ssl;
}

Net::MSGraphClient::~MSGraphClient()
{
}

Optional<Net::MSGraphAccessToken> Net::MSGraphClient::AccessTokenGet(Text::CStringNN tenantId, Text::CStringNN clientId, Text::CStringNN clientSecret, Text::CString scope)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	if (tenantId.leng > 40 || clientId.leng > 40 || clientSecret.leng > 64)
	{
		return 0;
	}
	sb.Append(CSTR("https://login.microsoftonline.com/"));
	sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, tenantId.v);
	sb.AppendP(sbuff, sptr);
	sb.Append(CSTR("/oauth2/v2.0/token"));
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	cli->FormBegin();
	cli->FormAdd(CSTR("client_id"), clientId);
	cli->FormAdd(CSTR("client_secret"), clientSecret);
	cli->FormAdd(CSTR("grant_type"), CSTR("client_credentials"));
	if (scope.v)
		cli->FormAdd(CSTR("scope"), scope);
	else
		cli->FormAdd(CSTR("scope"), CSTR("https://graph.microsoft.com/.default"));

	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	sb.ClearStr();
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
		return AccessTokenParse(status, sb.ToCString());
	}
	else
	{
		cli.Delete();
#if defined(VERBOSE)
		printf("MSGraphClient: AccessTokenGet request error: status = %d\r\n", (Int32)status);
#endif
	}
	return 0;
}

Optional<Net::MSGraphEntity> Net::MSGraphClient::EntityGet(NotNullPtr<MSGraphAccessToken> token, Text::CString userName)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://graph.microsoft.com/v1.0/"));
	if (userName.v)
	{
		sb.Append(CSTR("users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, userName.v);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.Append(CSTR("me"));
	}
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	sb.ClearStr();
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
#if defined(VERBOSE)
		printf("MSGraphClient: EntityGetMe %d, %s\r\n", (Int32)status, sb.ToString());
#endif
		NotNullPtr<Text::JSONBase> json;
		if (json.Set(Text::JSONBase::ParseJSONStr(sb.ToCString())))
		{
			NotNullPtr<MSGraphEntity> entity;
			NEW_CLASSNN(entity, MSGraphEntity(json));
			json->EndUse();
			if (entity->IsValid())
				return entity;
			entity.Delete();
			return 0;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		cli.Delete();
#if defined(VERBOSE)
		printf("MSGraphClient: EntityGetMe request error: status = %d\r\n", (Int32)status);
#endif
	}
	return 0;
}

Bool Net::MSGraphClient::MailMessagesGet(NotNullPtr<MSGraphAccessToken> token, Text::CString userName, UOSInt top, UOSInt skip, NotNullPtr<Data::ArrayListNN<MSGraphEventMessageRequest>> msgList, OutParam<Bool> hasNext)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (userName.leng > 200)
		return false;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://graph.microsoft.com/v1.0/"));
	if (userName.v)
	{
		sb.Append(CSTR("users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, userName.v);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.Append(CSTR("me"));
	}
	sb.Append(CSTR("/messages"));
	Bool found = false;
	if (top != 0)
	{
		sb.Append(CSTR("?%24top="));
		sb.AppendUOSInt(top);
		found = true;
	}
	if (skip != 0)
	{
		if (found)
			sb.AppendUTF8Char('&');
		else
			sb.AppendUTF8Char('?');
		found = true;
		sb.Append(CSTR("?%24skip="));
		sb.AppendUOSInt(top);
	}
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	sb.ClearStr();
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
		Text::JSONBase *json = Text::JSONBase::ParseJSONStr(sb.ToCString());
		if (json == 0)
		{
#if defined(VERBOSE)
			printf("MSGraphClient: MailMessagesGet cannot parse result: %d, %s\r\n", (Int32)status, sb.ToString());
#endif
			return false;
		}
		if (json->GetType() != Text::JSONType::Object)
		{
			json->EndUse();
#if defined(VERBOSE)
			printf("MSGraphClient: MailMessagesGet not json object: %d, %s\r\n", (Int32)status, sb.ToString());
#endif
			return false;
		}
		Text::JSONObject *obj = (Text::JSONObject*)json;
		hasNext.Set(obj->GetValue(CSTR("@odata.nextLink")) != 0);
		Text::JSONArray *arr = obj->GetObjectArray(CSTR("value"));
		if (arr == 0)
		{
			json->EndUse();
#if defined(VERBOSE)
			printf("MSGraphClient: MailMessagesGet value array not found: %d, %s\r\n", (Int32)status, sb.ToString());
#endif
			return false;
		}
		NotNullPtr<Text::JSONObject> msgObj;
		NotNullPtr<MSGraphEventMessageRequest> msg;
		UOSInt i = 0;
		UOSInt j = arr->GetArrayLength();
		while (i < j)
		{
			if (msgObj.Set(arr->GetArrayObject(i)))
			{
				NEW_CLASSNN(msg, MSGraphEventMessageRequest(msgObj));
				if (msg->IsValid())
				{
					msgList->Add(msg);
				}
				else
				{
					msg.Delete();
				}
			}
			i++;
		}
		json->EndUse();
//		IO::FileStream fs(CSTR("mailmessages.json"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
//		fs.WriteCont(sb.v, sb.leng);
		return true;
	}
	else
	{
		cli.Delete();
#if defined(VERBOSE)
		printf("MSGraphClient: MailFoldersGet request error: status = %d\r\n", (Int32)status);
#endif
	}
	return 0;
}

Bool Net::MSGraphClient::MailFoldersGet(NotNullPtr<MSGraphAccessToken> token, Text::CString userName, Bool includeHidden, NotNullPtr<Data::ArrayListNN<MSGraphMailFolder>> folderList)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (userName.leng > 200)
		return false;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://graph.microsoft.com/v1.0/"));
	if (userName.v)
	{
		sb.Append(CSTR("users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, userName.v);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.Append(CSTR("me"));
	}
	sb.Append(CSTR("/mailFolders"));
	if (includeHidden)
		sb.Append(CSTR("?includeHiddenFolders=true"));
	return this->GetList(token, sb.ToCString(), CSTR("MailFoldersGet"), folderList);
}

Bool Net::MSGraphClient::HasUnknownTypes(NotNullPtr<Text::JSONObject> obj, IsKnownTypeFunc isKnownType, Text::CStringNN typeName)
{
	Bool ret = false;
	Data::ArrayListNN<Text::String> names;
	obj->GetObjectNames(names);
	Data::ArrayIterator<NotNullPtr<Text::String>> it = names.Iterator();
	NotNullPtr<Text::String> name;
	while (it.HasNext())
	{
		name = it.Next();
		if (!isKnownType(name->ToCString()))
		{
#if defined(VERBOSE)
			printf("%s: type unknown: %s\r\n", typeName.v, name->v);
#endif
			ret = true;
		}
	}
	return ret;
}
