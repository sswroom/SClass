#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/Google/GoogleIdentityToolkit.h"
#include "Text/JSONBuilder.h"

// https://firebase.google.com/docs/reference/rest/auth

Optional<Net::Google::TokenResponse> Net::Google::GoogleIdentityToolkit::SignInWithCustomToken(Text::CStringNN token)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://identitytoolkit.googleapis.com/v1/accounts:signInWithCustomToken?key="));
	sb.Append(this->apiKey);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddStr(CSTR("token"), token);
	json.ObjectAddBool(CSTR("returnSecureToken"), true);
	cli->AddContentType(CSTR("application/json"));
	Text::CStringNN req = json.Build();
	cli->AddContentLength(req.leng);
	cli->WriteCont(req.v, req.leng);
	sb.ClearStr();
	cli->ReadAllContent(sb, 65536, 1048576);
	Optional<TokenResponse> ret = nullptr;
	NN<TokenResponse> res;
	NN<Text::JSONBase> jret;
	printf("%s\r\n", sb.v.Ptr());
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK && Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jret))
	{
		NN<Text::String> idToken;
		NN<Text::String> refreshToken;
		Int32 expiresIn;
		if (jret->GetValueString(CSTR("idToken")).SetTo(idToken) &&
			jret->GetValueString(CSTR("refreshToken")).SetTo(refreshToken) &&
			jret->GetValueAsInt32(CSTR("expiresIn"), expiresIn))
		{
			NEW_CLASSNN(res, TokenResponse(idToken, refreshToken, expiresIn));
			ret = res;
		}
		jret->EndUse();
	}
	cli.Delete();
	return ret;
}

Optional<Net::Google::RefreshTokenResponse> Net::Google::GoogleIdentityToolkit::RefreshToken(Text::CStringNN refreshToken)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://securetoken.googleapis.com/v1/token?key="));
	sb.Append(this->apiKey);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	sb.ClearStr();
	sb.Append(CSTR("grant_type=refresh_token"));
	sb.Append(CSTR("&refresh_token="));
	sb.Append(refreshToken);
	cli->AddContentType(CSTR("application/x-www-form-urlencoded"));
	cli->AddContentLength(sb.leng);
	cli->WriteCont(sb.v, sb.leng);
	sb.ClearStr();
	cli->ReadAllContent(sb, 65536, 1048576);
	Optional<RefreshTokenResponse> ret = nullptr;
	NN<RefreshTokenResponse> res;
	NN<Text::JSONBase> jret;
	printf("%s\r\n", sb.v.Ptr());
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK && Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jret))
	{
		NN<Text::String> tokenType;
		NN<Text::String> userId;
		NN<Text::String> projectId;
		NN<Text::String> idToken;
		NN<Text::String> refreshToken;
		Int32 expiresIn;
		if (jret->GetValueString(CSTR("token_type")).SetTo(tokenType) &&
			jret->GetValueString(CSTR("user_id")).SetTo(userId) &&
			jret->GetValueString(CSTR("project_id")).SetTo(projectId) &&
			jret->GetValueString(CSTR("id_token")).SetTo(idToken) &&
			jret->GetValueString(CSTR("refresh_token")).SetTo(refreshToken) &&
			jret->GetValueAsInt32(CSTR("expires_in"), expiresIn))
		{
			NEW_CLASSNN(res, RefreshTokenResponse(idToken, refreshToken, expiresIn, tokenType, userId, projectId));
			ret = res;
		}
		jret->EndUse();
	}
	cli.Delete();
	return ret;
}

Optional<Net::Google::SignUpResponse> Net::Google::GoogleIdentityToolkit::SignUp(Text::CStringNN email, Text::CStringNN password)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://identitytoolkit.googleapis.com/v1/accounts:signUp?key="));
	sb.Append(this->apiKey);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddStr(CSTR("email"), email);
	json.ObjectAddStr(CSTR("password"), password);
	json.ObjectAddBool(CSTR("returnSecureToken"), true);
	cli->AddContentType(CSTR("application/json"));
	Text::CStringNN req = json.Build();
	cli->AddContentLength(req.leng);
	cli->WriteCont(req.v, req.leng);
	sb.ClearStr();
	cli->ReadAllContent(sb, 65536, 1048576);
	Optional<SignUpResponse> ret = nullptr;
	NN<SignUpResponse> res;
	NN<Text::JSONBase> jret;
	printf("%s\r\n", sb.v.Ptr());
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK && Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jret))
	{
		NN<Text::String> localId;
		NN<Text::String> email;
		NN<Text::String> idToken;
		NN<Text::String> refreshToken;
		Int32 expiresIn;
		if (jret->GetValueString(CSTR("localId")).SetTo(localId) &&
			jret->GetValueString(CSTR("email")).SetTo(email) &&
			jret->GetValueString(CSTR("idToken")).SetTo(idToken) &&
			jret->GetValueString(CSTR("refreshToken")).SetTo(refreshToken) &&
			jret->GetValueAsInt32(CSTR("expiresIn"), expiresIn))
		{
			NEW_CLASSNN(res, SignUpResponse(idToken, refreshToken, expiresIn, localId, email));
			ret = res;
		}
		jret->EndUse();
	}
	cli.Delete();
	return ret;
}
			
Optional<Net::Google::VerifyPasswordResponse> Net::Google::GoogleIdentityToolkit::SignInWithPassword(Text::CStringNN email, Text::CStringNN password)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key="));
	sb.Append(this->apiKey);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddStr(CSTR("email"), email);
	json.ObjectAddStr(CSTR("password"), password);
	json.ObjectAddBool(CSTR("returnSecureToken"), true);
	cli->AddContentType(CSTR("application/json"));
	Text::CStringNN req = json.Build();
	cli->AddContentLength(req.leng);
	cli->WriteCont(req.v, req.leng);
	sb.ClearStr();
	cli->ReadAllContent(sb, 65536, 1048576);
	Optional<VerifyPasswordResponse> ret = nullptr;
	NN<VerifyPasswordResponse> res;
	NN<Text::JSONBase> jret;
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK && Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jret))
	{
		NN<Text::String> localId;
		NN<Text::String> email;
		NN<Text::String> displayName;
		NN<Text::String> idToken;
		Bool registered;
		NN<Text::String> refreshToken;
		Int32 expiresIn;
		if (jret->GetValueString(CSTR("localId")).SetTo(localId) &&
			jret->GetValueString(CSTR("email")).SetTo(email) &&
			jret->GetValueString(CSTR("displayName")).SetTo(displayName) &&
			jret->GetValueString(CSTR("idToken")).SetTo(idToken) &&
			jret->GetValueString(CSTR("refreshToken")).SetTo(refreshToken) &&
			jret->GetValueAsInt32(CSTR("expiresIn"), expiresIn))
		{
			registered = jret->GetValueAsBool(CSTR("registered"));
			NEW_CLASSNN(res, VerifyPasswordResponse(idToken, refreshToken, expiresIn, localId, email, displayName, registered));
			ret = res;
		}
		jret->EndUse();
	}
	cli.Delete();
	return ret;
}

Optional<Net::Google::SignUpResponse> Net::Google::GoogleIdentityToolkit::SignInAnonymous()
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://identitytoolkit.googleapis.com/v1/accounts:signUp?key="));
	sb.Append(this->apiKey);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddBool(CSTR("returnSecureToken"), true);
	cli->AddContentType(CSTR("application/json"));
	Text::CStringNN req = json.Build();
	cli->AddContentLength(req.leng);
	cli->WriteCont(req.v, req.leng);
	sb.ClearStr();
	cli->ReadAllContent(sb, 65536, 1048576);
	Optional<SignUpResponse> ret = nullptr;
	NN<SignUpResponse> res;
	NN<Text::JSONBase> jret;
	printf("%s\r\n", sb.v.Ptr());
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK && Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jret))
	{
		NN<Text::String> localId;
		NN<Text::String> email;
		NN<Text::String> idToken;
		NN<Text::String> refreshToken;
		Int32 expiresIn;
		if (jret->GetValueString(CSTR("localId")).SetTo(localId) &&
			jret->GetValueString(CSTR("email")).SetTo(email) &&
			jret->GetValueString(CSTR("idToken")).SetTo(idToken) &&
			jret->GetValueString(CSTR("refreshToken")).SetTo(refreshToken) &&
			jret->GetValueAsInt32(CSTR("expiresIn"), expiresIn))
		{
			NEW_CLASSNN(res, SignUpResponse(idToken, refreshToken, expiresIn, localId, email));
			ret = res;
		}
		jret->EndUse();
	}
	cli.Delete();
	return ret;
}
