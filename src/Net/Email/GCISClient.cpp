#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/Email/GCISClient.h"
/*
{
	"ChanType":"EM",
	"IsEncrypt":false,
	"IsSign":false,
	"IsRestricted":false,
	"IsUrgent":false,
	"ValidityPeriod":-1,
	"ContentDetail":{
		"CharSet":"UTF-8",
		"ContentType":"text/html",
		"Subject":"Email Testing",
		"Content":"This is a test email",
		"AtthFile":[]
	},
	"RecipientDetail":[
		{
			"ChanAddr":"test@abc.com",
			"CcAddr":null,
			"BccAddr":null,
			"RecipientAtthFile":[]
		},
		{
			"ChanAddr":"sswroom@yahoo.com",
			"CcAddr":null,
			"BccAddr":null,
			"RecipientAtthFile":[]
		}
	]
}
*/
Net::Email::GCISClient::GCISClient(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CStringNN notifyURL, NotNullPtr<Crypto::Cert::X509Cert> cert, NotNullPtr<Crypto::Cert::X509File> key)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->notifyURL = Text::String::New(notifyURL);
	this->cert = NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(cert->Clone());
	this->key = NotNullPtr<Crypto::Cert::X509File>::ConvertFrom(key->Clone());
}

Net::Email::GCISClient::~GCISClient()
{
	this->notifyURL->Release();
	this->cert.Delete();
	this->key.Delete();
}

Bool Net::Email::GCISClient::SendMessage(Bool intranetChannel, Text::CString charset, Text::CStringNN contentType, Text::CStringNN subject, Text::CStringNN content, Text::CStringNN toList, Text::CString ccList, Text::CString bccList, Text::StringBuilderUTF8 *sbError)
{
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->sockf, ssl, CSTR("GCISClient/1.0"), false, true);
	cli->Connect(this->notifyURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, 0, 0, true);
	cli->SetClientCert(this->cert, this->key);
	Text::JSONBuilder builder(Text::JSONBuilder::OT_OBJECT);
	builder.ObjectAddStr(CSTR("ChanType"), intranetChannel?CSTR("BD"):CSTR("EM"));
	builder.ObjectAddBool(CSTR("IsEncrypt"), false);
	builder.ObjectAddBool(CSTR("IsSign"), false);
	builder.ObjectAddBool(CSTR("IsRestricted"), false);
	builder.ObjectAddBool(CSTR("IsUrgent"), false);
	builder.ObjectAddInt32(CSTR("ValidityPeriod"), -1);
	builder.ObjectBeginObject(CSTR("ContentDetail"));
	builder.ObjectAddStr(CSTR("CharSet"), charset);
	builder.ObjectAddStr(CSTR("ContentType"), contentType);
	builder.ObjectAddStr(CSTR("Subject"), subject);
	builder.ObjectAddStr(CSTR("Content"), content);
	builder.ObjectBeginArray(CSTR("AtthFile"));
	builder.ArrayEnd();
	builder.ObjectEnd();
	builder.ObjectBeginArray(CSTR("RecipientDetail"));
	ParseEmailAddresses(builder, toList, ccList, bccList);
	builder.ArrayEnd();
	builder.ObjectEnd();
	if (!cli->WriteContent(CSTR("application/json"), builder.Build()))
	{
		if (sbError) sbError->AppendC(UTF8STRC("Error in sending json"));
		cli.Delete();
		return false;
	}
	Net::WebStatus::StatusCode code = cli->GetRespStatus();
	if (code != Net::WebStatus::SC_OK)
	{
		if (sbError)
		{
			sbError->AppendC(UTF8STRC("Return HTTP status not success: "));
			sbError->AppendI32((Int32)code);
		}
		cli.Delete();
		return false;
	}
	NotNullPtr<Text::StringBuilderUTF8> sb;
	if (sb.Set(sbError))
		cli->ReadAllContent(sb, 4096, 1048576);
	return true;
}

void Net::Email::GCISClient::ParseEmailAddresses(NotNullPtr<Text::JSONBuilder> builder, Text::CStringNN toList, Text::CString ccList, Text::CString bccList)
{
	Text::StringBuilderUTF8 sb;
	Text::PString sarr[10];
	sb.Append(toList);
	UOSInt strCnt = Text::StrSplitTrimP(sarr, 10, sb, ',');
	UOSInt i = 0;
	while (i < strCnt)
	{
		builder->ArrayBeginObject();
		builder->ObjectAddStr(CSTR("ChanAddr"), sarr[i].ToCString());
		builder->ObjectAddNull(CSTR("CcAddr"));
		builder->ObjectAddNull(CSTR("BccAddr"));
		builder->ObjectBeginArray(CSTR("RecipientAtthFile"));
		builder->ArrayEnd();
		builder->ObjectEnd();
		i++;
	}

	if (ccList.leng > 0)
	{
		sb.ClearStr();
		sb.Append(ccList);
		strCnt = Text::StrSplitTrimP(sarr, 10, sb, ',');
		i = 0;
		while (i < strCnt)
		{
			builder->ArrayBeginObject();
			builder->ObjectAddNull(CSTR("ChanAddr"));
			builder->ObjectAddStr(CSTR("CcAddr"), sarr[i].ToCString());
			builder->ObjectAddNull(CSTR("BccAddr"));
			builder->ObjectBeginArray(CSTR("RecipientAtthFile"));
			builder->ArrayEnd();
			builder->ObjectEnd();
			i++;
		}
	}

	if (bccList.leng > 0)
	{
		sb.ClearStr();
		sb.Append(bccList);
		strCnt = Text::StrSplitTrimP(sarr, 10, sb, ',');
		i = 0;
		while (i < strCnt)
		{
			builder->ArrayBeginObject();
			builder->ObjectAddNull(CSTR("ChanAddr"));
			builder->ObjectAddNull(CSTR("CcAddr"));
			builder->ObjectAddStr(CSTR("BccAddr"), sarr[i].ToCString());
			builder->ObjectBeginArray(CSTR("RecipientAtthFile"));
			builder->ArrayEnd();
			builder->ObjectEnd();
			i++;
		}
	}
}
