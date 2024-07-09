#include "Stdafx.h"
#include "Net/ASN1UnknownData.h"

Net::ASN1UnknownData::ASN1UnknownData(Text::CStringNN sourceName, Data::ByteArrayR buff) : ASN1Data(sourceName, buff)
{
}

Net::ASN1UnknownData::~ASN1UnknownData()
{
}

Net::ASN1Data::ASN1Type Net::ASN1UnknownData::GetASN1Type() const
{
	return ASN1Type::Unknown;
}

NN<Net::ASN1Data> Net::ASN1UnknownData::Clone() const
{
	NN<Net::ASN1UnknownData> data;
	NEW_CLASSNN(data, Net::ASN1UnknownData(this->sourceName->ToCString(), this->buff));
	return data;
}

void Net::ASN1UnknownData::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	Net::ASN1Util::PDUToString(this->buff.Arr(), this->buff.ArrEnd(), sb, 0, 0, 0);
}

NN<Net::ASN1Names> Net::ASN1UnknownData::CreateNames() const
{
	NN<Net::ASN1Names> names;
	NEW_CLASSNN(names, Net::ASN1Names());
	return names;
}
