#include "Stdafx.h"
#include "Data/ByteTool.h"
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

IO::ParserType Net::ASN1Data::GetParserType()
{
	return IO::ParserType::ASN1Data;
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

void Net::ASN1Data::AppendInteger(Text::StringBuilderUTF *sb, const UInt8 *pdu, UOSInt len)
{
	if (len == 1)
	{
		sb->AppendU16(pdu[0]);
	}
	else if (len == 2)
	{
		sb->AppendI16(ReadMInt16(pdu));
	}
	else if (len == 3)
	{
		sb->AppendI32(ReadMInt24(pdu));
	}
	else if (len == 4)
	{
		sb->AppendI32(ReadMInt32(pdu));
	}
	else if (len == 8)
	{
		sb->AppendI64(ReadMInt64(pdu));
	}
	else
	{
		sb->AppendHexBuff(pdu, len, ' ', Text::LineBreakType::None);
	}
}
