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

Net::MSGraphAccessToken::MSGraphAccessToken(NN<Text::String> type, Int32 expiresIn, Int32 extExpiresIn, NN<Text::String> accessToken)
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

void Net::MSGraphAccessToken::InitClient(NN<Net::HTTPClient> cli)
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

Net::MSGraphEventMessageRequest::MSGraphEventMessageRequest(NN<Text::JSONObject> obj)
{
	this->obj = obj;
	this->obj->BeginUse();
}

Net::MSGraphEventMessageRequest::~MSGraphEventMessageRequest()
{
	this->obj->EndUse();
}

Optional<Net::MSGraphEventMessageRequest> Net::MSGraphEventMessageRequest::Parse(NN<Text::JSONObject> obj)
{
	NN<Text::String> type;
	if (obj->GetValueString(CSTR("@odata.type")).SetTo(type) && type->Equals(CSTR("#microsoft.graph.eventMessageRequest")))
	{
		MSGraphClient::HasUnknownTypes(obj, IsKnownType, CSTR("MSGraphEventMessageRequest"));
		NN<MSGraphEventMessageRequest> msg;
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
	NN<IO::LogTool> log;
	if (status != Net::WebStatus::SC_OK)
	{
		if (this->log.SetTo(log))
		{
			log->LogMessage(CSTR("Error in getting access token"), IO::LogHandler::LogLevel::Error);
		}
#if defined(VERBOSE)
		printf("MSGraphClient: Error in getting access token: status = %d, content = %s\r\n", (Int32)status, content.v.Ptr());
#endif
		return 0;
	}
	NN<Text::JSONBase> result;
	if (!Text::JSONBase::ParseJSONStr(content).SetTo(result))
	{
		if (this->log.SetTo(log))
		{
			log->LogMessage(CSTR("Error in parsing access token (Not JSON)"), IO::LogHandler::LogLevel::Error);
		}
#if defined(VERBOSE)
		printf("MSGraphClient: Error in parsing access token (Not JSON): content = %s\r\n", content.v.Ptr());
#endif
		return 0;
	}
	NN<Text::String> t;
	NN<Text::String> at;
	Int32 expiresIn;
	Int32 extExpiresIn;
	if (result->GetValueString(CSTR("token_type")).SetTo(t) &&
		result->GetValueString(CSTR("access_token")).SetTo(at) &&
		result->GetValueAsInt32(CSTR("expires_in"), expiresIn) &&
		result->GetValueAsInt32(CSTR("ext_expires_in"), extExpiresIn))
	{
		NN<MSGraphAccessToken> token;
		NEW_CLASSNN(token, MSGraphAccessToken(t, expiresIn, extExpiresIn, at));
		result->EndUse();
		if (this->log.SetTo(log))
		{
			Text::StringBuilderUTF8 sbLog;
			sbLog.Append(CSTR("token_type = "));
			sbLog.Append(t);
			sbLog.Append(CSTR(", access_token = "));
			sbLog.Append(at);
			sbLog.Append(CSTR(", expires_in = "));
			sbLog.AppendI32(expiresIn);
			sbLog.Append(CSTR(", ext_expires_in = "));
			sbLog.AppendI32(extExpiresIn);
			log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Command);
		}
		return token;
	}
	else
	{
		if (this->log.SetTo(log))
		{
			log->LogMessage(CSTR("Error in parsing access token (Fields not found)"), IO::LogHandler::LogLevel::Error);
		}
#if defined(VERBOSE)
		printf("MSGraphClient: Error in parsing access token (Fields not found): content = %s\r\n", content.v.Ptr());
#endif
		result->EndUse();
		return 0;
	}
}

template<class T> Bool Net::MSGraphClient::GetList(NN<MSGraphAccessToken> token, Text::CStringNN url, Text::CStringNN funcName, NN<Data::ArrayListNN<T>> dataList)
{
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	Text::StringBuilderUTF8 sb;
	if (cli->ReadAllContent(sb, 4096, 1048576 * 4))
	{
		cli.Delete();
		NN<Text::JSONBase> json;
		if (!Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(json))
		{
#if defined(VERBOSE)
			printf("MSGraphClient: %s cannot parse result: %d, %s\r\n", funcName.v.Ptr(), (Int32)status, sb.ToPtr());
			printf("MSGraphClient: %s url: %s\r\n", funcName.v.Ptr(), url.v.Ptr());
#endif
			return false;
		}
		if (json->GetType() != Text::JSONType::Object)
		{
			json->EndUse();
#if defined(VERBOSE)
			printf("MSGraphClient: %s not json object: %d, %s\r\n", funcName.v.Ptr(), (Int32)status, sb.ToPtr());
#endif
			return false;
		}
		NN<Text::JSONObject> obj = NN<Text::JSONObject>::ConvertFrom(json);
		NN<Text::JSONArray> arr;
		if (!obj->GetObjectArray(CSTR("value")).SetTo(arr))
		{
			json->EndUse();
#if defined(VERBOSE)
			printf("MSGraphClient: %s value array not found: %d, %s\r\n", funcName.v.Ptr(), (Int32)status, sb.ToPtr());
#endif
			return false;
		}
		NN<Text::JSONObject> dataObj;
		NN<T> data;
		UOSInt i = 0;
		UOSInt j = arr->GetArrayLength();
		while (i < j)
		{
			if (arr->GetArrayObject(i).SetTo(dataObj))
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
		printf("MSGraphClient: %s request error: status = %d\r\n", funcName.v.Ptr(), (Int32)status);
#endif
	}
	return false;
}

Net::MSGraphClient::MSGraphClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl)
{
	this->clif = clif;
	this->ssl = ssl;
	this->log = 0;
}

Net::MSGraphClient::~MSGraphClient()
{
}

void Net::MSGraphClient::SetLog(NN<IO::LogTool> log)
{
	this->log = log;
}

Optional<Net::MSGraphAccessToken> Net::MSGraphClient::AccessTokenGet(Text::CStringNN tenantId, Text::CStringNN clientId, Text::CStringNN clientSecret, Text::CString scope)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	NN<IO::LogTool> log;
	if (tenantId.leng > 40 || clientId.leng > 40 || clientSecret.leng > 64)
	{
		return 0;
	}
	sb.Append(CSTR("https://login.microsoftonline.com/"));
	sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, tenantId.v);
	sb.AppendP(sbuff, sptr);
	sb.Append(CSTR("/oauth2/v2.0/token"));
	if (this->log.SetTo(log))
	{
		Text::StringBuilderUTF8 sbLog;
		sbLog.Append(CSTR("AccessTokenGet: POST "));
		sbLog.Append(sb);
		log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	cli->FormBegin();
	cli->FormAdd(CSTR("client_id"), clientId);
	cli->FormAdd(CSTR("client_secret"), clientSecret);
	cli->FormAdd(CSTR("grant_type"), CSTR("client_credentials"));
	Text::CStringNN nns;
	if (scope.SetTo(nns))
		cli->FormAdd(CSTR("scope"), nns);
	else
		cli->FormAdd(CSTR("scope"), CSTR("https://graph.microsoft.com/.default"));

	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	sb.ClearStr();
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
		if (this->log.SetTo(log))
		{
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
		return AccessTokenParse(status, sb.ToCString());
	}
	else
	{
		cli.Delete();
		if (this->log.SetTo(log))
		{
			Text::StringBuilderUTF8 sbLog;
			sbLog.Append(CSTR("AccessTokenGet request error: status = "));
			sbLog.AppendI32(status);
			log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
#if defined(VERBOSE)
		printf("MSGraphClient: AccessTokenGet request error: status = %d\r\n", (Int32)status);
#endif
	}
	return 0;
}

Optional<Net::MSGraphEntity> Net::MSGraphClient::EntityGet(NN<MSGraphAccessToken> token, Text::CString userName)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://graph.microsoft.com/v1.0/"));
	Text::CStringNN nns;
	if (userName.SetTo(nns))
	{
		sb.Append(CSTR("users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.Append(CSTR("me"));
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	sb.ClearStr();
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
#if defined(VERBOSE)
		printf("MSGraphClient: EntityGetMe %d, %s\r\n", (Int32)status, sb.ToPtr());
#endif
		NN<Text::JSONBase> json;
		if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(json))
		{
			NN<MSGraphEntity> entity;
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

Bool Net::MSGraphClient::MailMessagesGet(NN<MSGraphAccessToken> token, Text::CString userName, UOSInt top, UOSInt skip, NN<Data::ArrayListNN<MSGraphEventMessageRequest>> msgList, OutParam<Bool> hasNext)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (userName.leng > 200)
		return false;
	NN<IO::LogTool> log;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://graph.microsoft.com/v1.0/"));
	Text::CStringNN nns;
	if (userName.SetTo(nns))
	{
		sb.Append(CSTR("users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
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
	if (this->log.SetTo(log))
	{
		Text::StringBuilderUTF8 sbLog;
		sbLog.Append(CSTR("MailMessagesGet: GET "));
		sbLog.Append(sb);
		log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Action);
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	sb.ClearStr();
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
		if (this->log.SetTo(log))
		{
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
		NN<Text::JSONBase> json;
		if (!Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(json))
		{
			if (this->log.SetTo(log))
			{
				log->LogMessage(CSTR("MailMessagesGet cannot parse result"), IO::LogHandler::LogLevel::Error);
			}
#if defined(VERBOSE)
			printf("MSGraphClient: MailMessagesGet cannot parse result: %d, %s\r\n", (Int32)status, sb.ToPtr());
#endif
			return false;
		}
		if (json->GetType() != Text::JSONType::Object)
		{
			json->EndUse();
			if (this->log.SetTo(log))
			{
				log->LogMessage(CSTR("MailMessagesGet not json object"), IO::LogHandler::LogLevel::Error);
			}
#if defined(VERBOSE)
			printf("MSGraphClient: MailMessagesGet not json object: %d, %s\r\n", (Int32)status, sb.ToPtr());
#endif
			return false;
		}
		NN<Text::JSONObject> obj = NN<Text::JSONObject>::ConvertFrom(json);
		hasNext.Set(obj->GetValue(CSTR("@odata.nextLink")).NotNull());
		NN<Text::JSONArray> arr;
		if (!obj->GetObjectArray(CSTR("value")).SetTo(arr))
		{
			json->EndUse();
			if (this->log.SetTo(log))
			{
				log->LogMessage(CSTR("MailMessagesGet value array not found"), IO::LogHandler::LogLevel::Error);
			}
#if defined(VERBOSE)
			printf("MSGraphClient: MailMessagesGet value array not found: %d, %s\r\n", (Int32)status, sb.ToPtr());
#endif
			return false;
		}
		NN<Text::JSONObject> msgObj;
		NN<MSGraphEventMessageRequest> msg;
		UOSInt i = 0;
		UOSInt j = arr->GetArrayLength();
		while (i < j)
		{
			if (arr->GetArrayObject(i).SetTo(msgObj))
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
		if (this->log.SetTo(log))
		{
			Text::StringBuilderUTF8 sbLog;
			sbLog.Append(CSTR("MailFoldersGet request error: status = "));
			sbLog.AppendI32((Int32)status);
			log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Error);
		}
#if defined(VERBOSE)
		printf("MSGraphClient: MailFoldersGet request error: status = %d\r\n", (Int32)status);
#endif
	}
	return 0;
}

Bool Net::MSGraphClient::MailFoldersGet(NN<MSGraphAccessToken> token, Text::CString userName, Bool includeHidden, NN<Data::ArrayListNN<MSGraphMailFolder>> folderList)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (userName.leng > 200)
		return false;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://graph.microsoft.com/v1.0/"));
	Text::CStringNN nns;
	if (userName.SetTo(nns))
	{
		sb.Append(CSTR("users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
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

Bool Net::MSGraphClient::HasUnknownTypes(NN<Text::JSONObject> obj, IsKnownTypeFunc isKnownType, Text::CStringNN typeName)
{
	Bool ret = false;
	Data::ArrayListNN<Text::String> names;
	obj->GetObjectNames(names);
	Data::ArrayIterator<NN<Text::String>> it = names.Iterator();
	NN<Text::String> name;
	while (it.HasNext())
	{
		name = it.Next();
		if (!isKnownType(name->ToCString()))
		{
#if defined(VERBOSE)
			printf("%s: type unknown: %s\r\n", typeName.v.Ptr(), name->v.Ptr());
#endif
			ret = true;
		}
	}
	return ret;
}
