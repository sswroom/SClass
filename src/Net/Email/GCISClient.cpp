#include "Stdafx.h"
#include "Crypto/Cert/X509FileList.h"
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
			"ChanAddr":"sswroom@yahoo.com",
			"CcAddr":null,
			"BccAddr":null,
			"RecipientAtthFile":[]
		}
	]
}
{"ChanType":"EM","IsEncrypt":false,"IsSign":false,"IsRestricted":false,"IsUrgent":false,"ValidityPeriod":-1,"ContentDetail":{"CharSet":"UTF-8","ContentType":"text/html","Subject":"Email Testing","Content":"This is a test email","AtthFile":[]},"RecipientDetail":[{"ChanAddr":"sswroom@yahoo.com","CcAddr":null,"BccAddr":null,"RecipientAtthFile":[]}]}
*/
Net::Email::GCISClient::GCISClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN notifyURL, NN<Crypto::Cert::X509Cert> cert, NN<Crypto::Cert::X509File> key)
{
	this->clif = clif;
	this->ssl = ssl;
	this->notifyURL = Text::String::New(notifyURL);
	this->cert = NN<Crypto::Cert::X509Cert>::ConvertFrom(cert->Clone());
	this->key = NN<Crypto::Cert::X509File>::ConvertFrom(key->Clone());
	this->svrCert = 0;
}

Net::Email::GCISClient::~GCISClient()
{
	this->notifyURL->Release();
	this->cert.Delete();
	this->key.Delete();
	this->svrCert.Delete();
}

Bool Net::Email::GCISClient::SendMessage(Bool intranetChannel, Text::CString charset, Text::CStringNN contentType, Text::CStringNN subject, Text::CStringNN content, Text::CStringNN toList, Text::CString ccList, Text::CString bccList, Optional<Text::StringBuilderUTF8> sbError)
{
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->clif, ssl, CSTR("GCISClient/1.0"), false, true);
	NN<Text::StringBuilderUTF8> nnsb;
	if (!cli->SetClientCert(this->cert, this->key))
	{
		if (sbError.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Error in adding client cert"));
		cli.Delete();
		return false;
	}
	cli->Connect(this->notifyURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, 0, 0, true);
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
		if (sbError.SetTo(nnsb)) nnsb->AppendC(UTF8STRC("Error in sending json"));
		cli.Delete();
		return false;
	}
	Net::WebStatus::StatusCode code = cli->GetRespStatus();
	if (code != Net::WebStatus::SC_OK)
	{
		if (sbError.SetTo(nnsb))
		{
			nnsb->AppendC(UTF8STRC("Return HTTP status not success: "));
			nnsb->AppendI32((Int32)code);
		}
		cli.Delete();
		return false;
	}
	this->svrCert.Delete();
	NN<const Data::ReadingListNN<Crypto::Cert::Certificate>> svrCert;
	if (!cli->GetServerCerts().SetTo(svrCert) || svrCert->GetCount() == 0)
	{

	}
	else if (svrCert->GetCount() == 1)
	{
		this->svrCert = svrCert->GetItemNoCheck(0)->CreateX509Cert();
	}
	else
	{
		NN<Crypto::Cert::X509Cert> cert;
		Crypto::Cert::X509FileList *certList = 0;
		UOSInt i = 0;
		UOSInt j = svrCert->GetCount();
		while (i < j)
		{
			if (svrCert->GetItemNoCheck(i)->CreateX509Cert().SetTo(cert))
			{
				if (certList == 0)
				{
					NEW_CLASS(certList, Crypto::Cert::X509FileList(CSTR("ServerCert.crt"), cert));
				}
				else
				{
					certList->AddFile(cert);
				}
			}
			i++;
		}
		this->svrCert = certList;
	}
	if (sbError.SetTo(nnsb))
		cli->ReadAllContent(nnsb, 4096, 1048576);
	cli.Delete();
	return true;
}

Optional<Crypto::Cert::X509File> Net::Email::GCISClient::GetServerCertChain() const
{
	return this->svrCert;
}

void Net::Email::GCISClient::ParseEmailAddresses(NN<Text::JSONBuilder> builder, Text::CStringNN toList, Text::CString ccList, Text::CString bccList)
{
	Text::StringBuilderUTF8 sb;
	Text::PString sarr[10];
	sb.Append(toList);
	Text::CStringNN nns;
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

	if (ccList.SetTo(nns) && nns.leng > 0)
	{
		sb.ClearStr();
		sb.Append(nns);
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

	if (bccList.SetTo(nns) && nns.leng > 0)
	{
		sb.ClearStr();
		sb.Append(nns);
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
