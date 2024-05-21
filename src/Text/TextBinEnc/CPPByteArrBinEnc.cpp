#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/CPPByteArrBinEnc.h"

Text::TextBinEnc::CPPByteArrBinEnc::CPPByteArrBinEnc()
{
}

Text::TextBinEnc::CPPByteArrBinEnc::~CPPByteArrBinEnc()
{
}

UOSInt Text::TextBinEnc::CPPByteArrBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize) const
{
	UOSInt size = sb->GetCharCnt();
	UOSInt i = 0;
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

UOSInt Text::TextBinEnc::CPPByteArrBinEnc::CalcBinSize(const UTF8Char *str, UOSInt strLen) const
{
	UOSInt cnt = 0;
	UTF8Char c;
	while ((c = *str++) != 0)
	{
		if (c == ',')
			cnt++;
	}
	return cnt + 1;
}

UOSInt Text::TextBinEnc::CPPByteArrBinEnc::DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff) const
{
	UInt8 *startPtr = dataBuff;
	UTF8Char c;
	UInt8 b = 0;
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
	return (UOSInt)(dataBuff - startPtr);
}

Text::CStringNN Text::TextBinEnc::CPPByteArrBinEnc::GetName() const
{
	return CSTR("CPP Byte Arr");
}
