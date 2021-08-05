#include "Stdafx.h"
#include "Crypto/X509Cert.h"

Crypto::X509Cert::X509Cert(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::X509File(sourceName, buff, buffSize)
{

}

Crypto::X509Cert::~X509Cert()
{

}

Crypto::X509File::FileType Crypto::X509Cert::GetFileType()
{
	return FT_CERT;
}

void Crypto::X509Cert::ToString(Text::StringBuilderUTF *sb)
{
	if (IsCertificate(this->buff, this->buff + this->buffSize, "1"))
	{
		AppendCertificate(this->buff, this->buff + this->buffSize, "1", sb);
	}
}
