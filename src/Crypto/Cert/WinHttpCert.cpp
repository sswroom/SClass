#include "Stdafx.h"
#include "Crypto/Cert/WinHttpCert.h"
#include "Text/MyStringW.h"
#include <windows.h>
#include <winhttp.h>

struct Crypto::Cert::WinHttpCert::ClassData
{
	WINHTTP_CERTIFICATE_INFO *certInfo;
};

Crypto::Cert::WinHttpCert::WinHttpCert(void *certInfo)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->certInfo = (WINHTTP_CERTIFICATE_INFO*)certInfo;
}

Crypto::Cert::WinHttpCert::~WinHttpCert()
{
	LocalFree(this->clsData->certInfo->lpszSubjectInfo);
	LocalFree(this->clsData->certInfo->lpszIssuerInfo);
	MemFree(this->clsData->certInfo);
	MemFree(this->clsData);
}

Data::Timestamp Crypto::Cert::WinHttpCert::GetNotBefore() const
{
	return Data::Timestamp::FromFILETIME(&this->clsData->certInfo->ftStart, Data::DateTimeUtil::GetLocalTzQhr());
}

Data::Timestamp Crypto::Cert::WinHttpCert::GetNotAfter() const
{
	return Data::Timestamp::FromFILETIME(&this->clsData->certInfo->ftExpiry, Data::DateTimeUtil::GetLocalTzQhr());
}

Bool Crypto::Cert::WinHttpCert::IsSelfSigned() const
{
	if (this->clsData->certInfo->lpszIssuerInfo == 0 || this->clsData->certInfo->lpszSubjectInfo == 0)
	{
		return false;
	}
	return Text::StrEquals(this->clsData->certInfo->lpszSubjectInfo, this->clsData->certInfo->lpszIssuerInfo);
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::WinHttpCert::CreateX509Cert() const
{
	return 0;
}

void Crypto::Cert::WinHttpCert::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	Data::DateTime dt;
	sb->AppendC(UTF8STRC("Not Before: "));
	sb->AppendTSNoZone(this->GetNotBefore());
	sb->AppendC(UTF8STRC("\r\nNot After: "));
	sb->AppendTSNoZone(this->GetNotAfter());
	if (this->clsData->certInfo->lpszSubjectInfo)
	{
		sb->AppendC(UTF8STRC("\r\nSubject: "));
		sb->AppendW(this->clsData->certInfo->lpszSubjectInfo);
	}
	if (this->clsData->certInfo->lpszIssuerInfo)
	{
		sb->AppendC(UTF8STRC("\r\nIssuer: "));
		sb->AppendW(this->clsData->certInfo->lpszIssuerInfo);
	}
	if (this->clsData->certInfo->lpszProtocolName)
	{
		sb->AppendC(UTF8STRC("\r\nProtocol Name: "));
		sb->AppendW(this->clsData->certInfo->lpszProtocolName);
	}
	if (this->clsData->certInfo->lpszSignatureAlgName)
	{
		sb->AppendC(UTF8STRC("\r\nSignature Alg: "));
		sb->AppendW(this->clsData->certInfo->lpszSignatureAlgName);
	}
	if (this->clsData->certInfo->lpszEncryptionAlgName)
	{
		sb->AppendC(UTF8STRC("\r\nEncryption Alg: "));
		sb->AppendW(this->clsData->certInfo->lpszEncryptionAlgName);
	}
	sb->AppendC(UTF8STRC("\r\nKey Size: "));
	sb->AppendU32(this->clsData->certInfo->dwKeySize);
	sb->AppendC(UTF8STRC("\r\n"));
}
