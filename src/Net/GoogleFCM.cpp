#include "Stdafx.h"
#include "Net/GoogleFCM.h"
#include "Net/HTTPClient.h"
#include "Text/JSON.h"
#include "Text/JSONBuilder.h"

Bool Net::GoogleFCM::SendMessage(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CString apiKey, Text::CString devToken, Text::CString message, Text::StringBuilderUTF8 *sbResult)
{
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(sockf, ssl, CSTR("https://fcm.googleapis.com/fcm/send"), Net::WebUtil::RequestMethod::HTTP_POST, true);
	if (cli->IsError())
	{
		cli.Delete();
		if (sbResult)
			sbResult->AppendC(UTF8STRC("Error connecting to server"));
		return false;
	}
	Text::StringBuilderUTF8 sb;
	cli->AddContentType(CSTR("application/json"));
	sb.AppendC(UTF8STRC("key="));
	sb.Append(apiKey);
	cli->AddHeaderC(CSTR("Authorization"), sb.ToCString());

	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectBeginArray(CSTR("registration_ids"));
	json.ArrayAddStrUTF8(devToken.v);
	json.ArrayEnd();
	json.ObjectAddStr(CSTR("collapse_key"), CSTR("optional"));
	json.ObjectBeginObject(CSTR("data"));
	json.ObjectAddStr(CSTR("message"), message);
	json.ObjectEnd();
	Text::CStringNN j = json.Build();
	cli->AddContentLength(j.leng);
	cli->Write(j.v, j.leng);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	Bool succ = false;
	if (status == Net::WebStatus::SC_OK)
	{
		sb.ClearStr();
		cli->ReadAllContent(sb, 2048, 1048576);
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
	cli.Delete();
	return succ;
}

Bool Net::GoogleFCM::SendMessages(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CString apiKey, Data::ArrayList<Text::String*> *devTokens, Text::CString message, Text::StringBuilderUTF8 *sbResult)
{
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(sockf, ssl, CSTR("https://fcm.googleapis.com/fcm/send"), Net::WebUtil::RequestMethod::HTTP_POST, true);
	if (cli->IsError())
	{
		cli.Delete();
		if (sbResult)
			sbResult->AppendC(UTF8STRC("Error in connecting to server"));
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

	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
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
	Text::CStringNN js = json.Build();
	cli->AddContentLength(js.leng);
	cli->Write(js.v, js.leng);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	Bool succ = false;
	if (status == Net::WebStatus::SC_OK)
	{
		sb.ClearStr();
		cli->ReadAllContent(sb, 2048, 1048576);
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
	cli.Delete();
	return succ;
}
