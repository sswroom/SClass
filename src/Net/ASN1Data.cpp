#include "Stdafx.h"
#include "Net/ASN1Data.h"
#include "Net/ASN1Util.h"

Net::ASN1Data::ASN1Data(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : IO::ParsedObject(sourceName)
{
	this->buff = MemAlloc(UInt8, buffSize);
	this->buffSize = buffSize;
	MemCopyNO(this->buff, buff, buffSize);
}

Net::ASN1Data::~ASN1Data()
{
	MemFree(this->buff);
}

IO::ParsedObject::ParserType Net::ASN1Data::GetParserType()
{
	return IO::ParsedObject::PT_ASN1_DATA;
}

Bool Net::ASN1Data::ToASN1String(Text::StringBuilderUTF *sb)
{
	return Net::ASN1Util::PDUToString(this->buff, this->buff + this->buffSize, sb, 0);
}

const UInt8 *Net::ASN1Data::GetASN1Buff()
{
	return this->buff;
}

UOSInt Net::ASN1Data::GetASN1BuffSize()
{
	return this->buffSize;
}
