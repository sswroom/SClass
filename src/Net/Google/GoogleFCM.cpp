#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/Google/GoogleFCM.h"
#include "Text/JSON.h"
#include "Text/JSONBuilder.h"

Bool Net::Google::GoogleFCM::SendMessage(Text::CStringNN devToken, Text::CStringNN message, Optional<Text::StringBuilderUTF8> sbResult)
{
	NN<Text::StringBuilderUTF8> sbRes;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, CSTR("https://fcm.googleapis.com/fcm/send"), Net::WebUtil::RequestMethod::HTTP_POST, true);
	if (cli->IsError())
	{
		cli.Delete();
		if (sbResult.SetTo(sbRes))
			sbRes->AppendC(UTF8STRC("Error connecting to server"));
		return false;
	}
	Text::StringBuilderUTF8 sb;
	cli->AddContentType(CSTR("application/json"));
	sb.AppendC(UTF8STRC("key="));
	sb.Append(this->apiKey);
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
	cli->Write(j.ToByteArray());
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	Bool succ = false;
	if (status == Net::WebStatus::SC_OK)
	{
		sb.ClearStr();
		cli->ReadAllContent(sb, 2048, 1048576);
		NN<Text::JSONBase> jobj;
		if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jobj))
		{
			succ = jobj->GetValueAsInt32(CSTR("success"));
			if (succ)
			{
				if (sbResult.SetTo(sbRes))
					sbRes->AppendC(UTF8STRC("Success"));
			}
			else
			{
				if (sbResult.SetTo(sbRes))
					sbRes->AppendC(UTF8STRC("Server response failed"));
			}
			jobj->EndUse();
		}
		else
		{
			if (sbResult.SetTo(sbRes))
			{
				sbRes->AppendC(UTF8STRC("Cannot parse response as JSON"));
			}
		}
	}
	else
	{
		if (sbResult.SetTo(sbRes))
		{
			sbRes->AppendC(UTF8STRC("Response Status is not 200: "));
			sbRes->AppendU32((UInt32)status);
		}
	}
	cli.Delete();
	return succ;
}

Bool Net::Google::GoogleFCM::SendMessages(NN<Data::ArrayListNN<Text::String>> devTokens, Text::CStringNN message, Optional<Text::StringBuilderUTF8> sbResult)
{
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, CSTR("https://fcm.googleapis.com/fcm/send"), Net::WebUtil::RequestMethod::HTTP_POST, true);
	NN<Text::StringBuilderUTF8> sbRes;
	if (cli->IsError())
	{
		cli.Delete();
		if (sbResult.SetTo(sbRes))
			sbRes->AppendC(UTF8STRC("Error in connecting to server"));
		return false;
	}
	if (devTokens->GetCount() == 0)
	{
		if (sbResult.SetTo(sbRes))
			sbRes->AppendC(UTF8STRC("No device found"));
		return false;
	}
	Text::StringBuilderUTF8 sb;
	cli->AddContentType(CSTR("application/json"));
	sb.AppendC(UTF8STRC("key="));
	sb.Append(this->apiKey);
	cli->AddHeaderC(CSTR("Authorization"), sb.ToCString());

	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectBeginArray(CSTR("registration_ids"));
	UIntOS i = 0;
	UIntOS j = devTokens->GetCount();
	while (i < j)
	{
		json.ArrayAddStr(devTokens->GetItemNoCheck(i));
		i++;
	}
	json.ArrayEnd();
	json.ObjectAddStr(CSTR("collapse_key"), CSTR("optional"));
	json.ObjectBeginObject(CSTR("data"));
	json.ObjectAddStr(CSTR("message"), message);
	json.ObjectEnd();
	Text::CStringNN js = json.Build();
	cli->AddContentLength(js.leng);
	cli->Write(js.ToByteArray());
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	Bool succ = false;
	if (status == Net::WebStatus::SC_OK)
	{
		sb.ClearStr();
		cli->ReadAllContent(sb, 2048, 1048576);
		NN<Text::JSONBase> jobj;
		if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jobj))
		{
			succ = jobj->GetValueAsInt32(CSTR("success"));
			if (succ)
			{
				if (sbResult.SetTo(sbRes))
					sbRes->AppendC(UTF8STRC("Success"));
			}
			else
			{
				if (sbResult.SetTo(sbRes))
					sbRes->AppendC(UTF8STRC("Server response failed"));
			}
			jobj->EndUse();
		}
		else
		{
			if (sbResult.SetTo(sbRes))
			{
				sbRes->AppendC(UTF8STRC("Cannot parse response as JSON"));
			}
		}
	}
	else
	{
		if (sbResult.SetTo(sbRes))
		{
			sbRes->AppendC(UTF8STRC("Response Status is not 200: "));
			sbRes->AppendU32((UInt32)status);
		}
	}
	cli.Delete();
	return succ;
}
