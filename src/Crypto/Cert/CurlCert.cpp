#include "Stdafx.h"
#include "Crypto/Cert/CurlCert.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Parser/FileParser/X509Parser.h"
#include <curl/curl.h>

Crypto::Cert::CurlCert::CurlCert(AnyType certinfo)
{
	this->certinfo = certinfo;
}

Crypto::Cert::CurlCert::~CurlCert()
{

}

Data::Timestamp Crypto::Cert::CurlCert::GetNotBefore() const
{
	curl_slist *slist = (curl_slist*)this->certinfo.p;
	UOSInt slen;
	while (slist)
	{
		slen = Text::StrCharCntCh(slist->data);
		if (Text::StrStartsWithC((const UTF8Char*)slist->data, slen, UTF8STRC("Start date:")))
		{
			return Data::Timestamp::FromStr(Text::CStringNN((const UTF8Char*)slist->data + 11, slen - 11), Data::DateTimeUtil::GetLocalTzQhr());
		}
	}

	return 0;
}

Data::Timestamp Crypto::Cert::CurlCert::GetNotAfter() const
{
	curl_slist *slist = (curl_slist*)this->certinfo.p;
	UOSInt slen;
	while (slist)
	{
		slen = Text::StrCharCntCh(slist->data);
		if (Text::StrStartsWithC((const UTF8Char*)slist->data, slen, UTF8STRC("Expire date:")))
		{
			return Data::Timestamp::FromStr(Text::CStringNN((const UTF8Char*)slist->data + 12, slen - 12), Data::DateTimeUtil::GetLocalTzQhr());
		}
	}

	return 0;
}

Bool Crypto::Cert::CurlCert::IsSelfSigned() const
{
	const Char *subj = 0;
	const Char *issuer = 0;
	curl_slist *slist = (curl_slist*)this->certinfo.p;
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
	if (subj && issuer && Text::StrEqualsCh(subj, issuer))
	{
		return true;
	}
	return false;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::CurlCert::CreateX509Cert() const
{
	NN<Crypto::Cert::X509Cert> pobjCert;
	curl_slist *slist = (curl_slist*)this->certinfo.p;
	while (slist)
	{
		if (Text::StrStartsWith(slist->data, "Cert:"))
		{
			UOSInt len = Text::StrCharCntCh(slist->data);
			NN<Text::String> fileName = Text::String::New(UTF8STRC("Certificate.crt"));
			if (Optional<Crypto::Cert::X509Cert>::ConvertFrom(Parser::FileParser::X509Parser::ParseBuff(Data::ByteArrayR((const UInt8*)slist->data + 5, (UOSInt)len - 5), fileName)).SetTo(pobjCert))
			{
				fileName->Release();
				return pobjCert;
			}
			fileName->Release();
		}
		slist = slist->next;
	}
	return 0;
}

void Crypto::Cert::CurlCert::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	curl_slist *slist = (curl_slist*)this->certinfo.p;
	while (slist)
	{
		sb->AppendSlow((const UTF8Char*)slist->data);
		sb->AppendC(UTF8STRC("\r\n"));
		slist = slist->next;
	}
}
