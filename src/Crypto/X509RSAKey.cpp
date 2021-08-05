#include "Stdafx.h"
#include "Crypto/X509RSAKey.h"

Crypto::X509RSAKey::X509RSAKey(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::X509File(sourceName, buff, buffSize)
{

}

Crypto::X509RSAKey::~X509RSAKey()
{

}

Crypto::X509File::FileType Crypto::X509RSAKey::GetFileType()
{
	return FT_RSA_KEY;
}

void Crypto::X509RSAKey::ToString(Text::StringBuilderUTF *sb)
{
}
