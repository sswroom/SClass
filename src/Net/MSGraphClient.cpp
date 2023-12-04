#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/MSGraphClient.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/URIEncoding.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Net::MSGraphClient::MSGraphClient(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl)
{
	this->sockf = sockf;
	this->ssl = ssl;
}

Net::MSGraphClient::~MSGraphClient()
{
}

Bool Net::MSGraphClient::GetAccessToken(Text::CStringNN tenantId, Text::CStringNN clientId, Text::CStringNN clientSecret, NotNullPtr<MSGraphAccessToken> tokenOut)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	if (tenantId.leng > 40 || clientId.leng > 40 || clientSecret.leng > 64)
	{
		return false;
	}
	sb.Append(CSTR("https://login.microsoftonline.com/"));
	sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, tenantId.v);
	sb.AppendP(sbuff, sptr);
	sb.Append(CSTR("/oauth2/v2.0/token"));
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	cli->FormAdd(CSTR("client_id"), clientId);
	cli->FormAdd(CSTR("client_secret"), clientSecret);
	cli->FormAdd(CSTR("grant_type"), CSTR("client_credentials"));
	sb.ClearStr();
	sb.Append(CSTR("api://"));
	sb.Append(clientId);
	sb.Append(CSTR("/.default"));
	cli->FormAdd(CSTR("scope"), sb.ToCString());
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	sb.ClearStr();
	if (cli->ReadAllContent(sb, 4096, 1048576))
	{

	}
        String s = HTTPMyClient.formPostAsString(url, Map.of("client_id", clientId, "scope", "api://"+clientId+"/.default", "client_secret", clientSecret, "grant_type", "client_credentials"), statusCode, 5000);
        if (s != null && statusCode.value == 200)
        {
            AccessTokenResult token = new AccessTokenResult();
            JSONBase json = JSONBase.parseJSONStr(s);
            token.type = json.getValueString("token_type");
            long t = System.currentTimeMillis();
            token.expiresIn = new Timestamp(t + json.getValueAsInt32("expires_in") * 1000);
            token.extExpiresIn = new Timestamp(t + json.getValueAsInt32("ext_expires_in") * 1000);
            token.accessToken = json.getValueString("access_token");
            System.out.println(s);
            System.out.println(DataTools.toJSONString(token));
            return token;
        }
        if (log != null)
        {
            log.logMessage("Error in getting access token: status = "+statusCode.value+", content = "+s, LogLevel.ERROR);
        }
        return null;

}
