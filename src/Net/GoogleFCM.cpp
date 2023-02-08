#include "Stdafx.h"
#include "Net/GoogleFCM.h"
#include "Net/HTTPClient.h"
#include "Text/JSON.h"
#include "Text/JSONBuilder.h"

Bool Net::GoogleFCM::SendMessage(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString apiKey, Text::CString devToken, Text::CString message, Text::StringBuilderUTF8 *sbResult)
{
	Data::ArrayList<Text::String*> tokenList;
	tokenList.Add(Text::String::New(devToken));
	Bool ret = SendMessage(sockf, ssl, apiKey, &tokenList, message, sbResult);
	tokenList.GetItem(0)->Release();
	return ret;
}

Bool Net::GoogleFCM::SendMessage(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString apiKey, Data::ArrayList<Text::String*> *devTokens, Text::CString message, Text::StringBuilderUTF8 *sbResult)
{
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(sockf, ssl, CSTR("https://fcm.googleapis.com/fcm/send"), Net::WebUtil::RequestMethod::HTTP_POST, true);
	if (cli == 0)
	{
		if (sbResult)
			sbResult->AppendC(UTF8STRC("Error in creating client"));
		return false;
	}
	if (devTokens->GetCount() == 0)
	{
		if (sbResult)
			sbResult->AppendC(UTF8STRC("No device found"));
		return false;
	}
	Text::StringBuilderUTF8 sb;
	cli->AddContentType(CSTR("application/json"));
	sb.AppendC(UTF8STRC("key="));
	sb.Append(apiKey);
	cli->AddHeaderC(CSTR("Authorization"), sb.ToCString());

	sb.ClearStr();
	{
		Text::JSONBuilder json(&sb, Text::JSONBuilder::OT_OBJECT);
		json.ObjectBeginArray(CSTR("registration_ids"));
		UOSInt i = 0;
		UOSInt j = devTokens->GetCount();
		while (i < j)
		{
			json.ArrayAddStr(devTokens->GetItem(i));
			i++;
		}
		json.ArrayEnd();
		json.ObjectAddStr(CSTR("collapse_key"), CSTR("optional"));
		json.ObjectBeginObject(CSTR("data"));
		json.ObjectAddStr(CSTR("message"), message);
		json.ObjectEnd();
	}
	cli->AddContentLength(sb.leng);
	cli->Write(sb.v, sb.leng);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	Bool succ = false;
	if (status == Net::WebStatus::SC_OK)
	{
		sb.ClearStr();
		cli->ReadAllContent(&sb, 2048, 1048576);
		Text::JSONBase *jobj = Text::JSONBase::ParseJSONStr(sb.ToCString());
		if (jobj)
		{
			succ = jobj->GetValueAsInt32(CSTR("success"));
			if (succ)
			{
				if (sbResult)
					sbResult->AppendC(UTF8STRC("Success"));
			}
			else
			{
				if (sbResult)
					sbResult->AppendC(UTF8STRC("Server response failed"));
			}
			jobj->EndUse();
		}
		else
		{
			if (sbResult)
			{
				sbResult->AppendC(UTF8STRC("Cannot parse response as JSON"));
			}
		}
	}
	else
	{
		if (sbResult)
		{
			sbResult->AppendC(UTF8STRC("Response Status is not 200: "));
			sbResult->AppendU32((UInt32)status);
		}
	}
	DEL_CLASS(cli);
	return succ;
}
