#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/X509File.h"
#include "Net/ASN1Util.h"

Crypto::X509File::X509File(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize, FileType fileType) : Net::ASN1Data(sourceName, buff, buffSize)
{
	this->fileType =fileType;
}

Crypto::X509File::~X509File()
{
}

Net::ASN1Data::ASN1Type Crypto::X509File::GetASN1Type()
{
	return AT_X509;
}

Crypto::X509File::FileType Crypto::X509File::GetFileType()
{
	return this->fileType;
}

Crypto::X509File *Crypto::X509File::LoadFile(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize, Crypto::X509File::FileType fileType)
{
	Crypto::X509File *file;
	NEW_CLASS(file, Crypto::X509File(sourceName, buff, buffSize, fileType));
	return file;
}
