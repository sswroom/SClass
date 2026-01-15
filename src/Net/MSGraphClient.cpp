#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/MSGraphClient.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/URIEncoding.h"

#include "IO/FileStream.h"

#define GRAPHROOT CSTR("https://graph.microsoft.com/v1.0")

//#define VERBOSE
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

Bool Net::MSGraphAccessToken::IsExpired()
{
	Data::Timestamp t = Data::Timestamp::Now();
	return t > this->expiresIn;
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
	return nullptr;
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
			if (debugLog)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(CSTR("MSGraphClient: Error in getting access token: status = "))->AppendI32((Int32)status)->Append(CSTR(", content = "))->Append(content);
				log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
			}
		}
#if defined(VERBOSE)
		printf("MSGraphClient: Error in getting access token: status = %d, content = %s\r\n", (Int32)status, content.v.Ptr());
#endif
		return nullptr;
	}
	NN<Text::JSONBase> result;
	if (!Text::JSONBase::ParseJSONStr(content).SetTo(result))
	{
		if (this->log.SetTo(log))
		{
			log->LogMessage(CSTR("Error in parsing access token (Not JSON)"), IO::LogHandler::LogLevel::Error);
			if (debugLog)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(CSTR("MSGraphClient: Error in parsing access token (Not JSON): content = "))->Append(content);
				log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
			}
		}
#if defined(VERBOSE)
		printf("MSGraphClient: Error in parsing access token (Not JSON): content = %s\r\n", content.v.Ptr());
#endif
		return nullptr;
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
			if (debugLog)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(CSTR("MSGraphClient: Error in parsing access token (Fields not found): content = "))->Append(content);
				log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
			}
		}
#if defined(VERBOSE)
		printf("MSGraphClient: Error in parsing access token (Fields not found): content = %s\r\n", content.v.Ptr());
#endif
		result->EndUse();
		return nullptr;
	}
}

template<class T> Bool Net::MSGraphClient::GetList(NN<MSGraphAccessToken> token, Text::CStringNN url, Text::CStringNN funcName, NN<Data::ArrayListNN<T>> dataList)
{
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	Text::StringBuilderUTF8 sb;
	NN<IO::LogTool> log;
	if (cli->ReadAllContent(sb, 4096, 1048576 * 4))
	{
		cli.Delete();
		NN<Text::JSONBase> json;
		if (!Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(json))
		{
			if (this->debugLog && this->log.SetTo(log))
			{
				Text::StringBuilderUTF8 sbLog;
				sbLog.Append(CSTR("MSGraphClient: "))->Append(funcName)->Append(CSTR(" cannot parse result: "))->AppendI32((Int32)status)->Append(CSTR(", "))->Append(sb);
				log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
				sbLog.ClearStr()->Append(CSTR("MSGraphClient: "))->Append(funcName)->Append(CSTR(" url: "))->Append(url);
				log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
			}
#if defined(VERBOSE)
			printf("MSGraphClient: %s cannot parse result: %d, %s\r\n", funcName.v.Ptr(), (Int32)status, sb.ToPtr());
			printf("MSGraphClient: %s url: %s\r\n", funcName.v.Ptr(), url.v.Ptr());
#endif
			return false;
		}
		if (json->GetType() != Text::JSONType::Object)
		{
			json->EndUse();
			if (this->debugLog && this->log.SetTo(log))
			{
				Text::StringBuilderUTF8 sbLog;
				sbLog.Append(CSTR("MSGraphClient: "))->Append(funcName)->Append(CSTR(" not json object: "))->AppendI32((Int32)status)->Append(CSTR(", "))->Append(sb);
				log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
			}
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
			if (this->debugLog && this->log.SetTo(log))
			{
				Text::StringBuilderUTF8 sbLog;
				sbLog.Append(CSTR("MSGraphClient: "))->Append(funcName)->Append(CSTR(" value array not found: "))->AppendI32((Int32)status)->Append(CSTR(", "))->Append(sb);
				log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
			}
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
		if (this->debugLog && this->log.SetTo(log))
		{
			Text::StringBuilderUTF8 sbLog;
			sbLog.Append(CSTR("MSGraphClient: "))->Append(funcName)->Append(CSTR(" request error: status = "))->AppendI32((Int32)status);
			log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
#if defined(VERBOSE)
		printf("MSGraphClient: %s request error: status = %d\r\n", funcName.v.Ptr(), (Int32)status);
#endif
	}
	return false;
}

void Net::MSGraphClient::BuildRcpt(NN<Text::JSONBuilder> builder, NN<const Data::ArrayListNN<Net::Email::EmailMessage::EmailAddress>> recpList, Net::Email::EmailMessage::RecipientType type, Text::CStringNN name)
{
	UOSInt i;
	UOSInt j;
	NN<Net::Email::EmailMessage::EmailAddress> addr;
	NN<Text::String> s;
	Bool found = false;
	i = 0;
	j = recpList->GetCount();
	while (i < j)
	{
		addr = recpList->GetItemNoCheck(i);
		if (addr->type == type)
		{
			if (!found)
			{
				found = true;
				builder->ObjectBeginArray(name);
			}
			builder->ArrayBeginObject();
			builder->ObjectBeginObject(CSTR("emailAddress"));
			if (addr->name.SetTo(s))
			{
				builder->ObjectAddStr(CSTR("name"), s);
			}
			builder->ObjectAddStr(CSTR("address"), addr->addr);
			builder->ObjectEnd();
			builder->ObjectEnd();
		}
		i++;
	}
	if (found)
	{
		builder->ArrayEnd();
	}
}

Net::MSGraphClient::MSGraphClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl)
{
	this->clif = clif;
	this->ssl = ssl;
	this->log = nullptr;
	this->debugLog = false;
	this->attSplitSize = 1048576 * 4;
}

Net::MSGraphClient::~MSGraphClient()
{
}

void Net::MSGraphClient::SetLog(NN<IO::LogTool> log, Bool debugLog)
{
	this->debugLog = debugLog;
	this->log = log;
}

void Net::MSGraphClient::SetAttSplitSize(UOSInt attSplitSize)
{
	this->attSplitSize = attSplitSize;
}

Optional<Net::MSGraphAccessToken> Net::MSGraphClient::AccessTokenGet(Text::CStringNN tenantId, Text::CStringNN clientId, Text::CStringNN clientSecret, Text::CString scope)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	NN<IO::LogTool> log;
	if (tenantId.leng > 40 || clientId.leng > 40 || clientSecret.leng > 64)
	{
		return nullptr;
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
	return nullptr;
}

Optional<Net::MSGraphEntity> Net::MSGraphClient::EntityGet(NN<MSGraphAccessToken> token, Text::CString userName)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::LogTool> log;
	Text::StringBuilderUTF8 sb;
	sb.Append(GRAPHROOT);
	Text::CStringNN nns;
	if (userName.SetTo(nns))
	{
		sb.Append(CSTR("/users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.Append(CSTR("/me"));
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	sb.ClearStr();
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
		if (this->debugLog && this->log.SetTo(log))
		{
			Text::StringBuilderUTF8 sbLog;
			sbLog.Append(CSTR("MSGraphClient: EntityGetMe "))->AppendI32((Int32)status)->Append(CSTR(", "))->Append(sb);
			log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
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
			return nullptr;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		cli.Delete();
		if (this->debugLog && this->log.SetTo(log))
		{
			Text::StringBuilderUTF8 sbLog;
			sbLog.Append(CSTR("MSGraphClient: EntityGetMe request error: status = "))->AppendI32((Int32)status);
			log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
#if defined(VERBOSE)
		printf("MSGraphClient: EntityGetMe request error: status = %d\r\n", (Int32)status);
#endif
	}
	return nullptr;
}

Bool Net::MSGraphClient::MailMessagesGet(NN<MSGraphAccessToken> token, Text::CString userName, UOSInt top, UOSInt skip, NN<Data::ArrayListNN<MSGraphEventMessageRequest>> msgList, OutParam<Bool> hasNext)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (userName.leng > 200)
		return false;
	NN<IO::LogTool> log;
	Text::StringBuilderUTF8 sb;
	sb.Append(GRAPHROOT);
	Text::CStringNN nns;
	if (userName.SetTo(nns))
	{
		sb.Append(CSTR("/users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.Append(CSTR("/me"));
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
				if (this->debugLog)
				{
					Text::StringBuilderUTF8 sbLog;
					sbLog.Append(CSTR("MSGraphClient: "))->AppendI32((Int32)status)->Append(CSTR(", "))->Append(sb);
					log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
				}
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
				if (this->debugLog)
				{
					Text::StringBuilderUTF8 sbLog;
					sbLog.Append(CSTR("MSGraphClient: "))->AppendI32((Int32)status)->Append(CSTR(", "))->Append(sb);
					log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
				}
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
				if (this->debugLog)
				{
					Text::StringBuilderUTF8 sbLog;
					sbLog.Append(CSTR("MSGraphClient: "))->AppendI32((Int32)status)->Append(CSTR(", "))->Append(sb);
					log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
				}
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
	sb.Append(GRAPHROOT);
	Text::CStringNN nns;
	if (userName.SetTo(nns))
	{
		sb.Append(CSTR("/users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.Append(CSTR("/me"));
	}
	sb.Append(CSTR("/mailFolders"));
	if (includeHidden)
		sb.Append(CSTR("?includeHiddenFolders=true"));
	return this->GetList(token, sb.ToCString(), CSTR("MailFoldersGet"), folderList);
}

Optional<Net::MSGraphEventMessageRequest> Net::MSGraphClient::MailMessageCreate(NN<MSGraphAccessToken> token, Text::CString userName, NN<Net::Email::EmailMessage> message)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (userName.leng > 200)
		return nullptr;
	NN<IO::LogTool> log;
	Text::StringBuilderUTF8 sb;
	sb.Append(GRAPHROOT);
	Text::CStringNN nns;
	NN<Text::String> s;
	if (userName.SetTo(nns))
	{
		sb.Append(CSTR("/users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.Append(CSTR("/me"));
	}
	sb.Append(CSTR("/messages"));
	Text::JSONBuilder builder(Text::JSONBuilder::OT_OBJECT);
	builder.ObjectAddStr(CSTR("subject"), message->GetSubject());
	UInt64 contentLen;
	UnsafeArray<UInt8> content;
	Text::CStringNN cstr;
	if (message->GetContent(contentLen).SetTo(content))
	{
		builder.ObjectBeginObject(CSTR("body"));
		if (message->GetContentType().SetTo(s) && s->Equals(CSTR("text/html")))
		{
			builder.ObjectAddStr(CSTR("contentType"), CSTR("html"));
		}
		else
		{
			builder.ObjectAddStr(CSTR("contentType"), CSTR("text"));
		}
		builder.ObjectAddStr(CSTR("content"), Text::CStringNN(content, (UOSInt)contentLen));
		builder.ObjectEnd();
	}
	NN<const Data::ArrayListNN<Net::Email::EmailMessage::EmailAddress>> recpList = message->GetRecpList();
	BuildRcpt(builder, recpList, Net::Email::EmailMessage::RecipientType::To, CSTR("toRecipients"));
	BuildRcpt(builder, recpList, Net::Email::EmailMessage::RecipientType::Cc, CSTR("ccRecipients"));
	BuildRcpt(builder, recpList, Net::Email::EmailMessage::RecipientType::Bcc, CSTR("bccRecipients"));

	cstr = builder.Build();
	if (this->log.SetTo(log))
	{
		Text::StringBuilderUTF8 sbLog;
		sbLog.Append(CSTR("MailMessagesCreate: POST "));
		sbLog.Append(sb);
		log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Action);
		sbLog.ClearStr();
		sbLog.Append(CSTR("MailMessagesCreate: "));
		sbLog.Append(cstr);
		log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	token->InitClient(cli);
	cli->AddContentType(CSTR("application/json"));
	cli->AddContentLength(cstr.leng);
	cli->WriteCont(cstr.v, cstr.leng);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	sb.ClearStr();
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
		if (this->log.SetTo(log))
		{
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
		if (status != Net::WebStatus::SC_CREATED)
		{
			if (this->log.SetTo(log))
			{
				sb.ClearStr();
				sb.Append(CSTR("MailMessagesCreate: Response status is not 201 (CREATED): "));
				sb.Append(Net::WebStatus::GetCodeName(status).OrEmpty());
				log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
			}
			return nullptr;
		}
		NN<Text::JSONBase> json;
		if (!Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(json))
		{
			if (this->log.SetTo(log))
			{
				log->LogMessage(CSTR("MailMessagesCreate cannot parse result"), IO::LogHandler::LogLevel::Error);
				if (this->debugLog)
				{
					Text::StringBuilderUTF8 sbLog;
					sbLog.Append(CSTR("MSGraphClient: "))->AppendI32((Int32)status)->Append(CSTR(", "))->Append(sb);
					log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
				}
			}
#if defined(VERBOSE)
			printf("MSGraphClient: MailMessagesCreate cannot parse result: %d, %s\r\n", (Int32)status, sb.ToPtr());
#endif
			return nullptr;
		}
		if (json->GetType() != Text::JSONType::Object)
		{
			json->EndUse();
			if (this->log.SetTo(log))
			{
				log->LogMessage(CSTR("MailMessagesCreate not json object"), IO::LogHandler::LogLevel::Error);
				if (this->debugLog)
				{
					Text::StringBuilderUTF8 sbLog;
					sbLog.Append(CSTR("MSGraphClient: "))->AppendI32((Int32)status)->Append(CSTR(", "))->Append(sb);
					log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
				}
			}
#if defined(VERBOSE)
			printf("MSGraphClient: MailMessagesCreate not json object: %d, %s\r\n", (Int32)status, sb.ToPtr());
#endif
			return nullptr;
		}
		NN<MSGraphEventMessageRequest> msg;
		NEW_CLASSNN(msg, MSGraphEventMessageRequest(json));
		json->EndUse();
		if (msg->IsValid())
		{
		}
		else
		{
			if (this->log.SetTo(log))
			{
				log->LogMessage(CSTR("MailMessagesCreate message format not valid"), IO::LogHandler::LogLevel::Error);
			}
			msg.Delete();
			return nullptr;
		}
		UOSInt i = 0;
		UOSInt j = message->AttachmentGetCount();
		NN<Net::Email::EmailMessage::Attachment> att;
		Bool succ = true;
		while (i < j)
		{
			if (message->AttachmentGetItem(i).SetTo(att))
			{
				sb.ClearStr();
				sb.Append(GRAPHROOT);
				if (userName.SetTo(nns))
				{
					sb.Append(CSTR("/users/"));
					sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
					sb.AppendP(sbuff, sptr);
				}
				else
				{
					sb.Append(CSTR("/me"));
				}
				sb.Append(CSTR("/messages"));
				sb.AppendUTF8Char('/');
				sb.Append(msg->GetId());
				sb.Append(CSTR("/attachments"));
				sb.Append(CSTR("/createUploadSession"));
				Text::JSONBuilder builder2(Text::JSONBuilder::OT_OBJECT);
				builder2.ObjectBeginObject(CSTR("AttachmentItem"));
				builder2.ObjectAddStr(CSTR("attachmentType"), CSTR("file"));
				builder2.ObjectAddStr(CSTR("name"), att->fileName);
				builder2.ObjectAddUInt64(CSTR("size"), att->contentLen);
				if (att->isInline)
				{
					builder2.ObjectAddBool(CSTR("isInline"), att->isInline);
					builder2.ObjectAddStr(CSTR("contentId"), att->contentId);
				}
				builder2.ObjectEnd();
				cstr = builder2.Build();
				if (this->log.SetTo(log))
				{
					Text::StringBuilderUTF8 sbLog;
					sbLog.Append(CSTR("MailMessagesCreate: POST "));
					sbLog.Append(sb);
					log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Action);
					sbLog.ClearStr();
					sbLog.Append(CSTR("MailMessagesCreate: "));
					sbLog.Append(cstr);
					log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
				}
				cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
				token->InitClient(cli);
				cli->AddContentType(CSTR("application/json"));
				cli->AddContentLength(cstr.leng);
				cli->WriteCont(cstr.v, cstr.leng);
				status = cli->GetRespStatus();
				sb.ClearStr();
				if (cli->ReadAllContent(sb, 4096, 1048576))
				{
					cli.Delete();
					NN<Text::JSONBase> json;
					if (status != Net::WebStatus::SC_CREATED)
					{
						if (this->log.SetTo(log))
						{
							sb.ClearStr();
							sb.Append(CSTR("MailMessagesCreate: Response status is not 201 (CREATED): "));
							sb.Append(Net::WebStatus::GetCodeName(status).OrEmpty());
							log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
						}
					}
					else if (!Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(json))
					{
						if (this->log.SetTo(log))
						{
							log->LogMessage(CSTR("MailMessagesCreate cannot parse uploadSession result"), IO::LogHandler::LogLevel::Error);
							if (this->debugLog)
							{
								Text::StringBuilderUTF8 sbLog;
								sbLog.Append(CSTR("MSGraphClient: "))->AppendI32((Int32)status)->Append(CSTR(", "))->Append(sb);
								log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
							}
						}
#if defined(VERBOSE)
						printf("MSGraphClient: MailMessagesCreate cannot parse result: %d, %s\r\n", (Int32)status, sb.ToPtr());
#endif
					}
					else
					{
						NN<MSGraphUploadSession> uplSess;
						NEW_CLASSNN(uplSess, MSGraphUploadSession(json))
						if (uplSess->IsValid())
						{
							UOSInt currOfst = 0;
							UOSInt endOfst;
							while (currOfst < att->contentLen)
							{
								endOfst = currOfst + this->attSplitSize;
								if (endOfst > att->contentLen)
									endOfst = att->contentLen;

								cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, uplSess->GetUploadUrl()->ToCString(), Net::WebUtil::RequestMethod::HTTP_PUT, false);
								cli->SetTimeout(5000);
								cli->AddContentType(CSTR("application/octet-stream"));
								cli->AddContentLength(endOfst - currOfst);
								sb.ClearStr();
								sb.Append(CSTR("bytes "));
								sb.AppendUOSInt(currOfst);
								sb.AppendUTF8Char('-');
								sb.AppendUOSInt(endOfst - 1);
								sb.AppendUTF8Char('/');
								sb.AppendUOSInt(att->contentLen);
								cli->AddHeaderC(CSTR("Content-Range"), sb.ToCString());
								if (this->log.SetTo(log))
								{
									Text::StringBuilderUTF8 sbLog;
									sbLog.Append(CSTR("MailMessagesCreate: PUT "));
									sbLog.Append(uplSess->GetUploadUrl());
									log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Action);
									sbLog.ClearStr();
									sbLog.Append(CSTR("MailMessagesCreate: Content-Range: "));
									sbLog.Append(sb);
									log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Raw);
								}
								cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
								cli->WriteCont(&att->content[currOfst], endOfst - currOfst);
								status = cli->GetRespStatus();
								if (status == 200)
								{
									cli.Delete();
									currOfst = endOfst;
									if (endOfst >= att->contentLen)
									{
										if (this->log.SetTo(log))
										{
											sb.ClearStr();
											sb.Append(CSTR("MailMessagesCreate: File upload pass end of file: "));
											sb.AppendUOSInt(att->contentLen);
											log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
										}
										succ = false;
										break;
									}
								}
								else if (status == 201)
								{
									cli.Delete();
									if (endOfst != att->contentLen)
									{
										if (this->log.SetTo(log))
										{
											sb.ClearStr();
											sb.Append(CSTR("MailMessagesCreate: File upload missing data: "));
											sb.AppendUOSInt(endOfst);
											sb.Append(CSTR(" !="));
											sb.AppendUOSInt(att->contentLen);
											log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
										}
										succ = false;
										break;
									}
									break;
								}
								else
								{
									if (this->log.SetTo(log))
									{
										sb.ClearStr()->Append(CSTR("MailMessagesCreate: File upload unknown response: "))->Append(Net::WebStatus::GetCodeName(status).OrEmpty());
										log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
										sb.ClearStr();
										if (cli->ReadAllContent(sb, 4096, 1048576))
										{
											log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
										}
									}
									cli.Delete();
									succ = false;
									break;
								}
							}							
						}
						else
						{
							if (this->log.SetTo(log))
							{
								log->LogMessage(CSTR("MailMessagesCreate uploadSession is not valid"), IO::LogHandler::LogLevel::Error);
							}
							succ = false;
						}
						uplSess.Delete();
					}
				}
				else
				{
					cli.Delete();
					if (this->log.SetTo(log))
					{
						Text::StringBuilderUTF8 sbLog;
						sbLog.Append(CSTR("MailMessagesCreate request error: status = "));
						sbLog.AppendI32((Int32)status);
						log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Error);
					}
					succ = false;
#if defined(VERBOSE)
					printf("MSGraphClient: MailMessagesCreate request error: status = %d\r\n", (Int32)status);
#endif
				}
			}
			if (!succ)
			{
				break;
			}
			i++;
		}
		if (!succ)
		{
			this->MailMessageDelete(token, userName, msg->GetId()->ToCString());
			msg.Delete();
			return nullptr;
		}
		return msg;
	}
	else
	{
		cli.Delete();
		if (this->log.SetTo(log))
		{
			Text::StringBuilderUTF8 sbLog;
			sbLog.Append(CSTR("MailMessagesCreate request error: status = "));
			sbLog.AppendI32((Int32)status);
			log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Error);
		}
#if defined(VERBOSE)
		printf("MSGraphClient: MailMessagesCreate request error: status = %d\r\n", (Int32)status);
#endif
	}
	return nullptr;
}

Bool Net::MSGraphClient::MailMessageSend(NN<MSGraphAccessToken> token, Text::CString userName, NN<MSGraphEventMessageRequest> message)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (userName.leng > 200)
		return false;
	NN<IO::LogTool> log;
	Text::StringBuilderUTF8 sb;
	sb.Append(GRAPHROOT);
	Text::CStringNN nns;
	if (userName.SetTo(nns))
	{
		sb.Append(CSTR("/users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.Append(CSTR("/me"));
	}
	sb.Append(CSTR("/messages/"));
	sb.Append(message->GetId());
	sb.Append(CSTR("/send"));
	if (this->log.SetTo(log))
	{
		Text::StringBuilderUTF8 sbLog;
		sbLog.Append(CSTR("MailMessageSend: POST "));
		sbLog.Append(sb);
		log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Action);
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	token->InitClient(cli);
	cli->AddContentLength(0);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	if (status == Net::WebStatus::SC_ACCEPTED)
	{
		cli.Delete();
		return true;
	}
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
		if (this->log.SetTo(log))
		{
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
		if (this->log.SetTo(log))
		{
			sb.ClearStr();
			sb.Append(CSTR("MailMessagesCreate: Response status is not 202 (ACCEPTED): "));
			sb.Append(Net::WebStatus::GetCodeName(status).OrEmpty());
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		return false;
	}
	return false;
}

Bool Net::MSGraphClient::MailMessageDelete(NN<MSGraphAccessToken> token, Text::CString userName, Text::CStringNN msgId)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (userName.leng > 200)
		return false;
	NN<IO::LogTool> log;
	Text::StringBuilderUTF8 sb;
	sb.Append(GRAPHROOT);
	Text::CStringNN nns;
	if (userName.SetTo(nns))
	{
		sb.Append(CSTR("/users/"));
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
		sb.AppendP(sbuff, sptr);
	}
	else
	{
		sb.Append(CSTR("/me"));
	}
	sb.Append(CSTR("/messages/"));
	sb.Append(msgId);
	if (this->log.SetTo(log))
	{
		Text::StringBuilderUTF8 sbLog;
		sbLog.Append(CSTR("MailMessageDelete: DELETE "));
		sbLog.Append(sb);
		log->LogMessage(sbLog.ToCString(), IO::LogHandler::LogLevel::Action);
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_DELETE, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	if (status == Net::WebStatus::SC_NO_CONTENT)
	{
		cli.Delete();
		return true;
	}
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
		if (this->log.SetTo(log))
		{
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
		if (this->log.SetTo(log))
		{
			sb.ClearStr();
			sb.Append(CSTR("MailMessagesCreate: Response status is not 204 (NO CONTENT): "));
			sb.Append(Net::WebStatus::GetCodeName(status).OrEmpty());
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		return false;
	}
	return false;
}

Bool Net::MSGraphClient::SendEmail(NN<MSGraphAccessToken> token, Text::CString userName, NN<Net::Email::EmailMessage> message)
{
	NN<MSGraphEventMessageRequest> msg;
	if (this->MailMessageCreate(token, userName, message).SetTo(msg))
	{
		Bool succ = this->MailMessageSend(token, userName, msg);
		if (!succ)
		{
			this->MailMessageDelete(token, userName, msg->GetId()->ToCString());
		}
		msg.Delete();
		return succ;
	}
	return false;
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
