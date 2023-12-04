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
	if (t.Set(result->GetValueString(CSTR("token_type"))) &&
		at.Set(result->GetValueString(CSTR("access_token"))) &&
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
		printf("MSGraphClient: Error in getting access token: status = %d\r\n", (Int32)status);
#endif
	}
	return 0;
}

Bool Net::MSGraphClient::EntityGetMe(NotNullPtr<MSGraphAccessToken> token)
{
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, CSTR("https://graph.microsoft.com/v1.0/me"), Net::WebUtil::RequestMethod::HTTP_GET, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	Text::StringBuilderUTF8 sb;
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
#if defined(VERBOSE)
		printf("MSGraphClient: %d, %s\r\n", (Int32)status, sb.ToString());
#endif
		return false;
	}
	else
	{
		cli.Delete();
#if defined(VERBOSE)
		printf("MSGraphClient: Error in getting mail messages: status = %d\r\n", (Int32)status);
#endif
	}
	return 0;
}

Bool Net::MSGraphClient::MailMessagesGet(NotNullPtr<MSGraphAccessToken> token, Text::CStringNN userName, UOSInt top, UOSInt skip)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (userName.leng > 200)
		return false;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://graph.microsoft.com/v1.0/users/"));
	sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, userName.v);
	sb.AppendP(sbuff, sptr);
	sb.Append(CSTR("/messages"));
	Bool found = false;
	if (top != 0)
	{
		sb.Append(CSTR("?top="));
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
		sb.Append(CSTR("?skip="));
		sb.AppendUOSInt(top);
	}
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	token->InitClient(cli);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	sb.ClearStr();
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{
		cli.Delete();
		IO::FileStream fs(CSTR("mailmessages.json"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.WriteCont(sb.v, sb.leng);

#if defined(VERBOSE)
		printf("MSGraphClient: %d, %s\r\n", (Int32)status, sb.ToString());
#endif
		return false;
	}
	else
	{
		cli.Delete();
#if defined(VERBOSE)
		printf("MSGraphClient: Error in getting mail messages: status = %d\r\n", (Int32)status);
#endif
	}
	return 0;
}
