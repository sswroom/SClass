#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/Radix64Enc.h"

Text::TextBinEnc::Radix64Enc::Radix64Enc(const Char *encArr)
{
	UOSInt i = 256;
	UOSInt j;
	while (i-- > 0)
	{
		this->decArr[i] = 0xFF;
	}
	Char c;
	i = 0;
	j = 64;
	while (i < j)
	{
		c = encArr[i];
		this->encArr[i] = c;
		this->decArr[(UInt8)c] = (UInt8)i;
		i++;
	}
}

Text::TextBinEnc::Radix64Enc::~Radix64Enc()
{
}

UOSInt Text::TextBinEnc::Radix64Enc::EncodeBin(NotNullPtr<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	UOSInt outSize;
	UTF8Char sptr[4];
	UOSInt tmp1 = buffSize % 3;
	UOSInt tmp2 = buffSize / 3;
	if (tmp1)
	{
		outSize = tmp2 * 4 + 4;
	}
	else
	{
		outSize = tmp2 * 4;
	}
	if (outSize == 0)
		return 0;
	sb->AllocLeng(outSize);
	while (tmp2-- > 0)
	{
		sptr[0] = (UTF8Char)encArr[dataBuff[0] >> 2];
		sptr[1] = (UTF8Char)encArr[((dataBuff[0] << 4) | (dataBuff[1] >> 4)) & 0x3f];
		sptr[2] = (UTF8Char)encArr[((dataBuff[1] << 2) | (dataBuff[2] >> 6)) & 0x3f];
		sptr[3] = (UTF8Char)encArr[dataBuff[2] & 0x3f];
		sb->AppendC(sptr, 4);
		dataBuff += 3;
	}
	if (tmp1 == 1)
	{
		sptr[0] = (UTF8Char)encArr[dataBuff[0] >> 2];
		sptr[1] = (UTF8Char)encArr[(dataBuff[0] << 4) & 0x3f];
		sb->AppendC(sptr, 2);
	}
	else if (tmp1 == 2)
	{
		sptr[0] = (UTF8Char)encArr[dataBuff[0] >> 2];
		sptr[1] = (UTF8Char)encArr[((dataBuff[0] << 4) | (dataBuff[1] >> 4)) & 0x3f];
		sptr[2] = (UTF8Char)encArr[(dataBuff[1] << 2) & 0x3f];
		sb->AppendC(sptr, 3);
	}
	return outSize;
}

UOSInt Text::TextBinEnc::Radix64Enc::CalcBinSize(const UTF8Char *sbuff, UOSInt strLen)
{
	UOSInt cnt = 0;
	UTF8Char c;
	while ((c = *sbuff++) != 0)
	{
		if (c < 0x80 && decArr[c] != 0xff)
		{
			cnt++;
		}
	}
	return cnt * 3 / 4;
}

UOSInt Text::TextBinEnc::Radix64Enc::CalcBinSize(const WChar *sbuff)
{
	UOSInt cnt = 0;
	WChar c;
	while ((c = *sbuff++) != 0)
	{
		if (c < 0x80 && decArr[c] != 0xff)
		{
			cnt++;
		}
	}
	return cnt * 3 / 4;
}

UOSInt Text::TextBinEnc::Radix64Enc::DecodeBin(const UTF8Char *b64Str, UOSInt len, UInt8 *dataBuff)
{
	UOSInt decSize = 0;
	UInt8 b = 0;
	UInt8 b2 = 0;
	UInt8 code;
	UTF8Char c;
	while (len-- > 0)
	{
		c = *b64Str++;
		if (c < 0x80)
		{
			code = decArr[c];
			if (code != 0xff)
			{
				switch (b)
				{
				case 0:
					b2 = (UInt8)(code << 2);
					b = 1;
					break;
				case 1:
					*dataBuff = (UInt8)(b2 | (code >> 4));
					b2 = (UInt8)(code << 4);
					dataBuff++;
					decSize++;
					b = 2;
					break;
				case 2:
					*dataBuff = (UInt8)(b2 | (code >> 2));
					b2 = (UInt8)(code << 6);
					dataBuff++;
					decSize++;
					b = 3;
					break;
				case 3:
					*dataBuff = (UInt8)(b2 | code);
					dataBuff++;
					decSize++;
					b = 0;
					break;
				}
			}
		}
	}
	return decSize;
}

UOSInt Text::TextBinEnc::Radix64Enc::DecodeBin(const WChar *b64Str, UInt8 *dataBuff)
{
	UOSInt decSize = 0;
	UInt8 b = 0;
	UInt8 b2 = 0;
	UInt8 code;
	WChar c;
	while ((c = *b64Str++) != 0)
	{
		if (c < 0x80)
		{
			code = decArr[c];
			if (code != 0xff)
			{
				switch (b)
				{
				case 0:
					b2 = (UInt8)(code << 2);
					b = 1;
					break;
				case 1:
					*dataBuff = (UInt8)(b2 | (code >> 4));
					b2 = (UInt8)(code << 4);
					dataBuff++;
					decSize++;
					b = 2;
					break;
				case 2:
					*dataBuff = (UInt8)(b2 | (code >> 2));
					b2 = (UInt8)(code << 6);
					dataBuff++;
					decSize++;
					b = 3;
					break;
				case 3:
					*dataBuff = (UInt8)(b2 | code);
					dataBuff++;
					decSize++;
					b = 0;
					break;
				}
			}
		}
	}
	return decSize;
}

Text::CStringNN Text::TextBinEnc::Radix64Enc::GetName() const
{
	return CSTR("Radix64");
}
