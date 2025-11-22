#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Net/ASN1Data.h"
#include "Net/ASN1Util.h"

Net::ASN1Data::ASN1Data(NN<Text::String> sourceName, Data::ByteArrayR buff) : IO::ParsedObject(sourceName), buff(buff.GetSize())
{
	this->buff.CopyFrom(buff);
}

Net::ASN1Data::ASN1Data(Text::CStringNN sourceName, Data::ByteArrayR buff) : IO::ParsedObject(sourceName), buff(buff.GetSize())
{
	this->buff.CopyFrom(buff);
}

Net::ASN1Data::~ASN1Data()
{
}

IO::ParserType Net::ASN1Data::GetParserType() const
{
	return IO::ParserType::ASN1Data;
}

Bool Net::ASN1Data::ToASN1String(NN<Text::StringBuilderUTF8> sb) const
{
	NN<Net::ASN1Names> names = this->CreateNames();
	Bool ret = Net::ASN1Util::PDUToString(this->buff.Arr(), this->buff.ArrEnd(), sb, 0, 0, names);
	names.Delete();
	return ret;
}

UnsafeArray<const UInt8> Net::ASN1Data::GetASN1Buff() const
{
	return this->buff.Arr();
}

UOSInt Net::ASN1Data::GetASN1BuffSize() const
{
	return this->buff.GetSize();
}

Data::ByteArrayR Net::ASN1Data::GetASN1Array() const
{
	return this->buff;
}

void Net::ASN1Data::AppendInteger(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> pdu, UOSInt len)
{
	if (len == 1)
	{
		sb->AppendU16(pdu[0]);
	}
	else if (len == 2)
	{
		sb->AppendI16(ReadMInt16(&pdu[0]));
	}
	else if (len == 3)
	{
		sb->AppendI32(ReadMInt24(&pdu[0]));
	}
	else if (len == 4)
	{
		sb->AppendI32(ReadMInt32(&pdu[0]));
	}
	else if (len == 8)
	{
		sb->AppendI64(ReadMInt64(&pdu[0]));
	}
	else
	{
		sb->AppendHexBuff(pdu, len, ' ', Text::LineBreakType::None);
	}
}
