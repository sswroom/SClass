#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/SNMPUtil.h"
#include "Text/TextBinEnc/SNMPOIDBinEnc.h"

Text::TextBinEnc::SNMPOIDBinEnc::SNMPOIDBinEnc()
{
}

Text::TextBinEnc::SNMPOIDBinEnc::~SNMPOIDBinEnc()
{
}

UOSInt Text::TextBinEnc::SNMPOIDBinEnc::EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	UOSInt size = sb->GetCharCnt();
	Net::SNMPUtil::OIDToString(dataBuff, buffSize, sb);
	return sb->GetCharCnt() - size;
}

UOSInt Text::TextBinEnc::SNMPOIDBinEnc::CalcBinSize(const UTF8Char *sbuff)
{
	return Net::SNMPUtil::OIDCalcPDUSize(sbuff);
}

UOSInt Text::TextBinEnc::SNMPOIDBinEnc::DecodeBin(const UTF8Char *sbuff, UInt8 *dataBuff)
{
	return Net::SNMPUtil::OIDText2PDU(sbuff, dataBuff);
}

const UTF8Char *Text::TextBinEnc::SNMPOIDBinEnc::GetName()
{
	return (const UTF8Char*)"SNMP OID";
}
