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

UOSInt Text::TextBinEnc::IntegerMSBEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UOSInt buffSize) const
{
	Math::BigIntLSB bint(buffSize);
	bint.FromBytesMSB(dataBuff, buffSize);
	UOSInt len = sb->GetLength();
	bint.ToString(sb);
	return sb->GetLength() - len;
}

UOSInt Text::TextBinEnc::IntegerMSBEnc::CalcBinSize(Text::CStringNN str) const
{
	Math::BigIntLSB bint(str.leng / 2 + 1, str);
	return bint.GetOccupiedSize();
}

UOSInt Text::TextBinEnc::IntegerMSBEnc::DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const
{
	Math::BigIntLSB bint(str.leng / 2 + 1, str);
	return bint.GetBytesMSB(dataBuff, true);
}

Text::CStringNN Text::TextBinEnc::IntegerMSBEnc::GetName() const
{
	return CSTR("Integer MSB");
}
