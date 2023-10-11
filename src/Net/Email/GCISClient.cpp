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

Bool Net::Email::GCISClient::SendMessage(Bool internalChannel, Text::CString charset, Text::CStringNN contentType, Text::CStringNN subject, Data::ByteArrayR content, Text::CStringNN toList, Text::CString ccList, Text::StringBuilderUTF8 *sb)
{
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->sockf, ssl, CSTR("GCISClient/1.0"), false, true);
	cli->Connect(this->notifyURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, 0, 0, true);
	cli->SetClientCert(this->cert, this->key);
	
}
