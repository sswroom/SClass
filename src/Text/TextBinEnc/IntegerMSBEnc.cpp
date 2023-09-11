#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/BigIntLSB.h"
#include "Text/TextBinEnc/IntegerMSBEnc.h"

Text::TextBinEnc::IntegerMSBEnc::IntegerMSBEnc()
{
}

Text::TextBinEnc::IntegerMSBEnc::~IntegerMSBEnc()
{
}

UOSInt Text::TextBinEnc::IntegerMSBEnc::EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	Math::BigIntLSB bint(buffSize);
	bint.FromBytesMSB(dataBuff, buffSize);
	UOSInt len = sb->GetLength();
	bint.ToString(sb);
	return sb->GetLength() - len;
}

UOSInt Text::TextBinEnc::IntegerMSBEnc::CalcBinSize(const UTF8Char *str, UOSInt strLen)
{
	Math::BigIntLSB bint(strLen / 2 + 1, Text::CString(str, strLen));
	return bint.GetOccupiedSize();
}

UOSInt Text::TextBinEnc::IntegerMSBEnc::DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff)
{
	Math::BigIntLSB bint(strLen / 2 + 1, Text::CString(str, strLen));
	return bint.GetBytesMSB(dataBuff, true);
}

Text::CStringNN Text::TextBinEnc::IntegerMSBEnc::GetName() const
{
	return CSTR("Integer MSB");
}
