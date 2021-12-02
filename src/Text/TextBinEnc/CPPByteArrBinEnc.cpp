#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/CPPByteArrBinEnc.h"

Text::TextBinEnc::CPPByteArrBinEnc::CPPByteArrBinEnc()
{
}

Text::TextBinEnc::CPPByteArrBinEnc::~CPPByteArrBinEnc()
{
}

UOSInt Text::TextBinEnc::CPPByteArrBinEnc::EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	UOSInt size = sb->GetCharCnt();
	UOSInt i = 0;
	while (i < buffSize)
	{
		if (i > 0)
		{
			sb->Append((const UTF8Char*)", ");
			if ((i & 15) == 0)
			{
				sb->Append((const UTF8Char*)"\r\n");
			}
		}
		sb->Append((const UTF8Char*)"0x");
		sb->AppendHex8(dataBuff[i]);
		i++;
	}
	return sb->GetCharCnt() - size;
}

UOSInt Text::TextBinEnc::CPPByteArrBinEnc::CalcBinSize(const UTF8Char *sbuff)
{
	UOSInt cnt = 0;
	UTF8Char c;
	while ((c = *sbuff++) != 0)
	{
		if (c == ',')
			cnt++;
	}
	return cnt + 1;
}

UOSInt Text::TextBinEnc::CPPByteArrBinEnc::DecodeBin(const UTF8Char *sbuff, UInt8 *dataBuff)
{
	UInt8 *startPtr = dataBuff;
	UTF8Char c;
	UInt8 b = 0;
	while ((c = *sbuff++) != 0)
	{
		if (c == '0' && sbuff[0] == 'x')
		{
			sbuff++;
			c = *sbuff++;
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
				sbuff--;
				continue;
			}
			c = *sbuff++;
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
				sbuff--;
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
				c = *sbuff++;
				if (c >= '0' && c <= '9')
				{
					b = (UInt8)(b * 10 + (c - 0x30));
				}
				else
				{
					*dataBuff++ = b;
					sbuff--;
					break;
				}
			}
		}
	}
	return (UOSInt)(dataBuff - startPtr);
}

const UTF8Char *Text::TextBinEnc::CPPByteArrBinEnc::GetName()
{
	return (const UTF8Char*)"CPP Byte Arr";
}
