#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/CPPByteArrBinEnc.h"

Text::TextBinEnc::CPPByteArrBinEnc::CPPByteArrBinEnc()
{
}

Text::TextBinEnc::CPPByteArrBinEnc::~CPPByteArrBinEnc()
{
}

UIntOS Text::TextBinEnc::CPPByteArrBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const
{
	UIntOS size = sb->GetCharCnt();
	UIntOS i = 0;
	while (i < buffSize)
	{
		if (i > 0)
		{
			sb->AppendC(UTF8STRC(", "));
			if ((i & 15) == 0)
			{
				sb->AppendC(UTF8STRC("\r\n"));
			}
		}
		sb->AppendC(UTF8STRC("0x"));
		sb->AppendHex8(dataBuff[i]);
		i++;
	}
	return sb->GetCharCnt() - size;
}

UIntOS Text::TextBinEnc::CPPByteArrBinEnc::CalcBinSize(Text::CStringNN s) const
{
	UIntOS cnt = 0;
	UTF8Char c;
	UnsafeArray<const UTF8Char> str = s.v;
	while ((c = *str++) != 0)
	{
		if (c == ',')
			cnt++;
	}
	return cnt + 1;
}

UIntOS Text::TextBinEnc::CPPByteArrBinEnc::DecodeBin(Text::CStringNN s, UnsafeArray<UInt8> dataBuff) const
{
	UnsafeArray<UInt8> startPtr = dataBuff;
	UTF8Char c;
	UInt8 b = 0;
	UnsafeArray<const UTF8Char> str = s.v;
	while ((c = *str++) != 0)
	{
		if (c == '0' && str[0] == 'x')
		{
			str++;
			c = *str++;
			if (c >= '0' && c <= '9')
			{
				b = (UInt8)(c - 0x30);
			}
			else if (c >= 'A' && c <= 'F')
			{
				b = (UInt8)(c - 0x37);
			}
			else if (c >= 'a' && c <= 'f')
			{
				b = (UInt8)(c - 0x57);
			}
			else
			{
				*dataBuff++ = b;
				str--;
				continue;
			}
			c = *str++;
			if (c >= '0' && c <= '9')
			{
				b = (UInt8)((b << 4) | (c - 0x30));
			}
			else if (c >= 'A' && c <= 'F')
			{
				b = (UInt8)((b << 4) | (c - 0x37));
			}
			else if (c >= 'a' && c <= 'f')
			{
				b = (UInt8)((b << 4) | (c - 0x57));
			}
			else
			{
				*dataBuff++ = b;
				str--;
				continue;
			}
			*dataBuff++ = b;
			continue;
		}
		else if (c >= '0' && c <= '9')
		{
			b = (UInt8)(c - 0x30);
			while (true)
			{
				c = *str++;
				if (c >= '0' && c <= '9')
				{
					b = (UInt8)(b * 10 + (c - 0x30));
				}
				else
				{
					*dataBuff++ = b;
					str--;
					break;
				}
			}
		}
	}
	return (UIntOS)(dataBuff - startPtr);
}

Text::CStringNN Text::TextBinEnc::CPPByteArrBinEnc::GetName() const
{
	return CSTR("CPP Byte Arr");
}
