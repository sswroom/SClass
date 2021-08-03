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

UOSInt Text::TextBinEnc::ASN1OIDBinEnc::EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	UOSInt size = sb->GetCharCnt();
	Net::ASN1Util::OIDToString(dataBuff, buffSize, sb);
	return sb->GetCharCnt() - size;
}

UOSInt Text::TextBinEnc::ASN1OIDBinEnc::CalcBinSize(const UTF8Char *sbuff)
{
	return Net::ASN1Util::OIDCalcPDUSize(sbuff);
}

UOSInt Text::TextBinEnc::ASN1OIDBinEnc::DecodeBin(const UTF8Char *sbuff, UInt8 *dataBuff)
{
	return Net::ASN1Util::OIDUText2PDU(sbuff, dataBuff);
}

const UTF8Char *Text::TextBinEnc::ASN1OIDBinEnc::GetName()
{
	return (const UTF8Char*)"SNMP OID";
}
