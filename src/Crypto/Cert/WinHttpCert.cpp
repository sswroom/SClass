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

Bool Crypto::Cert::WinHttpCert::GetNotBefore(Data::DateTime *dt)
{
	dt->SetValueFILETIME(&this->clsData->certInfo->ftStart);
	return true;
}

Bool Crypto::Cert::WinHttpCert::GetNotAfter(Data::DateTime *dt)
{
	dt->SetValueFILETIME(&this->clsData->certInfo->ftExpiry);
	return true;
}

Bool Crypto::Cert::WinHttpCert::IsSelfSigned()
{
	if (this->clsData->certInfo->lpszIssuerInfo == 0 || this->clsData->certInfo->lpszSubjectInfo == 0)
	{
		return false;
	}
	return Text::StrEquals(this->clsData->certInfo->lpszSubjectInfo, this->clsData->certInfo->lpszIssuerInfo);
}

void Crypto::Cert::WinHttpCert::ToString(Text::StringBuilderUTF8 *sb)
{
	const UTF8Char *csptr;
	Data::DateTime dt;
	sb->AppendC(UTF8STRC("Not Before: "));
	this->GetNotBefore(&dt);
	sb->AppendDate(&dt);
	this->GetNotAfter(&dt);
	sb->AppendC(UTF8STRC("\r\nNot After: "));
	sb->AppendDate(&dt);
	if (this->clsData->certInfo->lpszSubjectInfo)
	{
		csptr = Text::StrToUTF8New(this->clsData->certInfo->lpszSubjectInfo);
		sb->AppendC(UTF8STRC("\r\nSubject: "));
		sb->Append(csptr);
		Text::StrDelNew(csptr);
	}
	if (this->clsData->certInfo->lpszIssuerInfo)
	{
		csptr = Text::StrToUTF8New(this->clsData->certInfo->lpszIssuerInfo);
		sb->AppendC(UTF8STRC("\r\nIssuer: "));
		sb->Append(csptr);
		Text::StrDelNew(csptr);
	}
	if (this->clsData->certInfo->lpszProtocolName)
	{
		csptr = Text::StrToUTF8New(this->clsData->certInfo->lpszProtocolName);
		sb->AppendC(UTF8STRC("\r\nProtocol Name: "));
		sb->Append(csptr);
		Text::StrDelNew(csptr);
	}
	if (this->clsData->certInfo->lpszSignatureAlgName)
	{
		csptr = Text::StrToUTF8New(this->clsData->certInfo->lpszSignatureAlgName);
		sb->AppendC(UTF8STRC("\r\nSignature Alg: "));
		sb->Append(csptr);
		Text::StrDelNew(csptr);
	}
	if (this->clsData->certInfo->lpszEncryptionAlgName)
	{
		csptr = Text::StrToUTF8New(this->clsData->certInfo->lpszEncryptionAlgName);
		sb->AppendC(UTF8STRC("\r\nEncryption Alg: "));
		sb->Append(csptr);
		Text::StrDelNew(csptr);
	}
	sb->AppendC(UTF8STRC("\r\nKey Size: "));
	sb->AppendU32(this->clsData->certInfo->dwKeySize);
	sb->AppendC(UTF8STRC("\r\n"));
}
