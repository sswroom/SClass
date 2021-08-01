#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/X509File.h"
#include "Net/ASN1Util.h"

Crypto::X509File::X509File(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize, FileType fileType) : IO::ParsedObject(sourceName)
{
	this->buff = MemAlloc(UInt8, buffSize);
	this->buffSize = buffSize;
	this->fileType =fileType;
	MemCopyNO(this->buff, buff, buffSize);
}

Crypto::X509File::~X509File()
{
	MemFree(this->buff);
}

IO::ParsedObject::ParserType Crypto::X509File::GetParserType()
{
	return IO::ParsedObject::PT_X509_FILE;
}

Bool Crypto::X509File::ToASN1String(Text::StringBuilderUTF *sb)
{
	return Net::ASN1Util::PDUToString(this->buff, this->buff + this->buffSize, sb, 0);
}

Crypto::X509File::FileType Crypto::X509File::GetFileType()
{
	return this->fileType;
}

const UInt8 *Crypto::X509File::GetASN1Buff()
{
	return this->buff;
}

UOSInt Crypto::X509File::GetASN1BuffSize()
{
	return this->buffSize;
}

Crypto::X509File *Crypto::X509File::LoadFile(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize, Crypto::X509File::FileType fileType)
{
	Crypto::X509File *file;
	NEW_CLASS(file, Crypto::X509File(sourceName, buff, buffSize, fileType));
	return file;
}
