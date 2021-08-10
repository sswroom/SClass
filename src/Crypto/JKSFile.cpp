#include "Stdafx.h"
#include "Crypto/JKSFile.h"

Crypto::JKSFile::JKSFile(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Crypto::X509File(sourceName, buff, buffSize)
{

}

Crypto::JKSFile::~JKSFile()
{

}

Crypto::X509File::FileType Crypto::JKSFile::GetFileType()
{
	return FT_JKS;
}

void Crypto::JKSFile::ToString(Text::StringBuilderUTF *sb)
{
}
