#include "Stdafx.h"
#include "Crypto/Cert/CurlCert.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Parser/FileParser/X509Parser.h"
#include <curl/curl.h>

Crypto::Cert::CurlCert::CurlCert(void *certinfo)
{
	this->certinfo = certinfo;
}

Crypto::Cert::CurlCert::~CurlCert()
{

}

Bool Crypto::Cert::CurlCert::GetNotBefore(Data::DateTime *dt) const
{
	curl_slist *slist = (curl_slist*)this->certinfo;
	UOSInt slen;
	while (slist)
	{
		slen = Text::StrCharCnt(slist->data);
		if (Text::StrStartsWithC((const UTF8Char*)slist->data, slen, UTF8STRC("Start date:")))
		{
			dt->SetValue(Text::CString((const UTF8Char*)slist->data + 11, slen - 11));
			return true;
		}
	}

	return false;
}

Bool Crypto::Cert::CurlCert::GetNotAfter(Data::DateTime *dt) const
{
	curl_slist *slist = (curl_slist*)this->certinfo;
	UOSInt slen;
	while (slist)
	{
		slen = Text::StrCharCnt(slist->data);
		if (Text::StrStartsWithC((const UTF8Char*)slist->data, slen, UTF8STRC("Expire date:")))
		{
			dt->SetValue(Text::CString((const UTF8Char*)slist->data + 12, slen - 12));
			return true;
		}
	}

	return false;
}

Bool Crypto::Cert::CurlCert::IsSelfSigned() const
{
	const Char *subj = 0;
	const Char *issuer = 0;
	curl_slist *slist = (curl_slist*)this->certinfo;
	while (slist)
	{
		if (Text::StrStartsWith(slist->data, "Subject:"))
		{
			subj = slist->data + 8;
		}
		else if (Text::StrStartsWith(slist->data, "Issuer:"))
		{
			issuer = slist->data + 7;
		}
		slist = slist->next;
	}
	if (subj && issuer && Text::StrEquals(subj, issuer))
	{
		return true;
	}
	return false;
}

NotNullPtr<Crypto::Cert::X509Cert> Crypto::Cert::CurlCert::CreateX509Cert() const
{
	NotNullPtr<Crypto::Cert::X509Cert> pobjCert;
	curl_slist *slist = (curl_slist*)this->certinfo;
	while (slist)
	{
		if (Text::StrStartsWith(slist->data, "Cert:"))
		{
			UOSInt len = Text::StrCharCnt(slist->data);
			NotNullPtr<Text::String> fileName = Text::String::New(UTF8STRC("Certificate.crt"));
			if (pobjCert.Set((Crypto::Cert::X509Cert*)Parser::FileParser::X509Parser::ParseBuff(Data::ByteArrayR((const UInt8*)slist->data + 5, (UOSInt)len - 5), fileName)))
			{
				fileName->Release();
				return pobjCert;
			}
			fileName->Release();
		}
		slist = slist->next;
	}
	NEW_CLASSNN(pobjCert, Crypto::Cert::X509Cert(CSTR("Certificate.crt"), Data::ByteArrayR(0, 0)));
	return pobjCert;
}

void Crypto::Cert::CurlCert::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	curl_slist *slist = (curl_slist*)this->certinfo;
	while (slist)
	{
		sb->AppendSlow((const UTF8Char*)slist->data);
		sb->AppendC(UTF8STRC("\r\n"));
		slist = slist->next;
	}
}
