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

UIntOS Text::TextBinEnc::IntegerMSBEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const
{
	Math::BigIntLSB bint(buffSize);
	bint.FromBytesMSB(dataBuff, buffSize);
	UIntOS len = sb->GetLength();
	bint.ToString(sb);
	return sb->GetLength() - len;
}

UIntOS Text::TextBinEnc::IntegerMSBEnc::CalcBinSize(Text::CStringNN str) const
{
	Math::BigIntLSB bint(str.leng / 2 + 1, str);
	return bint.GetOccupiedSize();
}

UIntOS Text::TextBinEnc::IntegerMSBEnc::DecodeBin(Text::CStringNN str, UnsafeArray<UInt8> dataBuff) const
{
	Math::BigIntLSB bint(str.leng / 2 + 1, str);
	return bint.GetBytesMSB(dataBuff, true);
}

Text::CStringNN Text::TextBinEnc::IntegerMSBEnc::GetName() const
{
	return CSTR("Integer MSB");
}
