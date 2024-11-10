#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/Google/GoogleOAuth2.h"
#include "Text/JSON.h"

//https://developers.google.com/identity/protocols/oauth2/service-account#httprest

Optional<Net::Google::AccessTokenResponse> Net::Google::GoogleOAuth2::GetServiceToken(NN<GoogleServiceAccount> serviceAccount, Text::CStringNN scope)
{
	NN<Crypto::Token::JWToken> token;
	if (!serviceAccount->ToJWT(this->ssl, scope).SetTo(token))
	{
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion="));
	token->ToString(sb);
	token.Delete();
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, CSTR("https://oauth2.googleapis.com/token"), Net::WebUtil::RequestMethod::HTTP_POST, false);
	cli->AddContentType(CSTR("application/x-www-form-urlencoded"));
	cli->AddContentLength(sb.leng);
	cli->WriteCont(sb.v, sb.leng);
	sb.ClearStr();
	cli->ReadAllContent(sb, 8192, 1048576);
	Optional<AccessTokenResponse> ret = 0;
	NN<Text::String> accessToken;
	NN<Text::String> tokenType;
	Int32 expiresIn;
	NN<Text::JSONBase> json;
	if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(json))
	{
		if (json->GetValueString(CSTR("access_token")).SetTo(accessToken) &&
			json->GetValueString(CSTR("token_type")).SetTo(tokenType) &&
			json->GetValueAsInt32(CSTR("expires_in"), expiresIn))
		{
			NEW_CLASSOPT(ret, AccessTokenResponse(accessToken, tokenType, expiresIn));
		}
		json->EndUse();
	}
	cli.Delete();
	return ret;
}
