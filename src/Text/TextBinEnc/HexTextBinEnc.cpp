#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/HexTextBinEnc.h"

Text::TextBinEnc::HexTextBinEnc::HexTextBinEnc()
{
}

Text::TextBinEnc::HexTextBinEnc::~HexTextBinEnc()
{
}

UIntOS Text::TextBinEnc::HexTextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const
{
	UIntOS size = sb->GetCharCnt();
	sb->AppendHexBuff(dataBuff, buffSize, ' ', Text::LineBreakType::CRLF);
	return sb->GetCharCnt() - size;
}

UIntOS Text::TextBinEnc::HexTextBinEnc::CalcBinSize(Text::CStringNN s) const
{
	UIntOS cnt = 0;
	UTF8Char c;
	UnsafeArray<const UTF8Char> str = s.v;
	while ((c = *str++) != 0)
	{
		if (c >= '0' && c <= '9')
		{
			cnt++;
		}
		else if (c >= 'A' && c <= 'F')
		{
			cnt++;
		}
		else if (c >= 'a' && c <= 'f')
		{
			cnt++;
		}
	}
	return cnt >> 1;
}

UIntOS Text::TextBinEnc::HexTextBinEnc::DecodeBin(Text::CStringNN s, UnsafeArray<UInt8> dataBuff) const
{
	UIntOS cnt = 0;
	UTF8Char c;
	UInt8 b = 0;
	Bool exist = false;
	UnsafeArray<const UTF8Char> str = s.v;
	while ((c = *str++) != 0)
	{
		if (c >= '0' && c <= '9')
		{
			b = (UInt8)(b | (c - 0x30));
		}
		else if (c >= 'A' && c <= 'F')
		{
			b = (UInt8)(b | (c - 0x37));
		}
		else if (c >= 'a' && c <= 'f')
		{
			b = (UInt8)(b | (c - 0x57));
		}
		else
		{
			continue;
		}
		if (exist)
		{
			*dataBuff++ = b;
			exist = false;
			b = 0;
			cnt++;
		}
		else
		{
			b = (UInt8)(b << 4);
			exist = true;
		}
	}
	return cnt;
}

Text::CStringNN Text::TextBinEnc::HexTextBinEnc::GetName() const
{
	return CSTR("Hex");
}
