#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/Google/GoogleFCMv1.h"
#include "Text/JSONBuilder.h"

Bool Net::Google::GoogleFCMv1::SendMessage(Text::CStringNN accessToken, Text::CStringNN devToken, Text::CStringNN message, Optional<Text::StringBuilderUTF8> sbResult)
{
	NN<Text::StringBuilderUTF8> sbRes;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://fcm.googleapis.com/v1/projects/"));
	sb.Append(this->projectId);
	sb.Append(CSTR("/messages:send"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	sb.ClearStr();
	sb.Append(CSTR("Bearer "));
	sb.Append(accessToken);
	cli->AddHeaderC(CSTR("Authorization"), sb.ToCString());
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectAddBool(CSTR("validate_only"), false);
	json.ObjectBeginObject(CSTR("message"));
	json.ObjectAddStr(CSTR("token"), devToken);
	json.ObjectBeginObject(CSTR("data"));
	json.ObjectAddStr(CSTR("message"), message);
	json.ObjectEnd();
	json.ObjectBeginObject(CSTR("android"));
	json.ObjectAddStr(CSTR("collapse_key"), CSTR("optional"));
	json.ObjectEnd();
	Text::CStringNN j = json.Build();
	cli->AddContentType(CSTR("application/json"));
	cli->AddContentLength(j.leng);
	cli->WriteCont(j.v, j.leng);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	Bool succ = false;
	if (status == Net::WebStatus::SC_OK)
	{
		sb.ClearStr();
		cli->ReadAllContent(sb, 2048, 1048576);
		NN<Text::JSONBase> jobj;
		if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jobj))
		{
			NN<Text::String> name;
			if (jobj->GetValueString(CSTR("name")).SetTo(name))
			{
				succ = true;
				if (sbResult.SetTo(sbRes))
				{
					sbRes->AppendC(UTF8STRC("Success, name = "));
					sbRes->Append(name);
				}
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
