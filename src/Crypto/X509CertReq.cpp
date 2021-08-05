#include "Stdafx.h"
#include "Crypto/X509CertReq.h"

Crypto::X509CertReq::X509CertReq(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::X509File(sourceName, buff, buffSize)
{

}

Crypto::X509CertReq::~X509CertReq()
{

}

Crypto::X509File::FileType Crypto::X509CertReq::GetFileType()
{
	return FT_CERT_REQ;
}

void Crypto::X509CertReq::ToString(Text::StringBuilderUTF *sb)
{
}
