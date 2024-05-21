#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/HexTextBinEnc.h"

Text::TextBinEnc::HexTextBinEnc::HexTextBinEnc()
{
}

Text::TextBinEnc::HexTextBinEnc::~HexTextBinEnc()
{
}

UOSInt Text::TextBinEnc::HexTextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize) const
{
	UOSInt size = sb->GetCharCnt();
	sb->AppendHexBuff(dataBuff, buffSize, ' ', Text::LineBreakType::CRLF);
	return sb->GetCharCnt() - size;
}

UOSInt Text::TextBinEnc::HexTextBinEnc::CalcBinSize(const UTF8Char *str, UOSInt strLen) const
{
	UOSInt cnt = 0;
	UTF8Char c;
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

UOSInt Text::TextBinEnc::HexTextBinEnc::DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff) const
{
	UOSInt cnt = 0;
	UTF8Char c;
	UInt8 b = 0;
	Bool exist = false;
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
