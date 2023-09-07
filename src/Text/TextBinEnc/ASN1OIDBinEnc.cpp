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

UOSInt Text::TextBinEnc::ASN1OIDBinEnc::EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	UOSInt size = sb->GetCharCnt();
	Net::ASN1Util::OIDToString(dataBuff, buffSize, sb);
	return sb->GetCharCnt() - size;
}

UOSInt Text::TextBinEnc::ASN1OIDBinEnc::CalcBinSize(const UTF8Char *str, UOSInt strLen)
{
	return Net::ASN1Util::OIDCalcPDUSize(str, strLen);
}

UOSInt Text::TextBinEnc::ASN1OIDBinEnc::DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff)
{
	return Net::ASN1Util::OIDText2PDU(str, strLen, dataBuff);
}

Text::CStringNN Text::TextBinEnc::ASN1OIDBinEnc::GetName() const
{
	return CSTR("SNMP OID");
}
