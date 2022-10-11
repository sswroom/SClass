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

Crypto::Cert::X509Cert *Crypto::Cert::CurlCert::CreateX509Cert() const
{
	curl_slist *slist = (curl_slist*)this->certinfo;
	while (slist)
	{
		if (Text::StrStartsWith(slist->data, "Cert:"))
		{
			Crypto::Cert::X509File *pobjCert;
			UOSInt len = Text::StrCharCnt(slist->data);
			Parser::FileParser::X509Parser parser;
			IO::StmData::MemoryDataRef mdata((const UInt8*)slist->data + 5, (UOSInt)len - 5);
			pobjCert = (Crypto::Cert::X509File*)parser.ParseFile(&mdata, 0, IO::ParserType::ASN1Data);
			return (Crypto::Cert::X509Cert*)pobjCert;
		}
		slist = slist->next;
	}
	return 0;
}

void Crypto::Cert::CurlCert::ToString(Text::StringBuilderUTF8 *sb) const
{
	curl_slist *slist = (curl_slist*)this->certinfo;
	while (slist)
	{
		sb->AppendSlow((const UTF8Char*)slist->data);
		sb->AppendC(UTF8STRC("\r\n"));
		slist = slist->next;
	}
}
