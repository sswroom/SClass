#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ASN1Util.h"
#include "Text/TextBinEnc/ASN1OIDBinEnc.h"

Text::TextBinEnc::ASN1OIDBinEnc::ASN1OIDBinEnc()
{
}

Text::TextBinEnc::ASN1OIDBinEnc::~ASN1OIDBinEnc()
{
}

UIntOS Text::TextBinEnc::ASN1OIDBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const
{
	UIntOS size = sb->GetCharCnt();
	Net::ASN1Util::OIDToString(Data::ByteArrayR(dataBuff, buffSize), sb);
	return sb->GetCharCnt() - size;
}

UIntOS Text::TextBinEnc::ASN1OIDBinEnc::CalcBinSize(Text::CStringNN str) const
{
	return Net::ASN1Util::OIDCalcPDUSize(str);
}

UIntOS Text::TextBinEnc::ASN1OIDBinEnc::DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const
{
	return Net::ASN1Util::OIDText2PDU(str, dataBuff);
}

Text::CStringNN Text::TextBinEnc::ASN1OIDBinEnc::GetName() const
{
	return CSTR("SNMP OID");
}
