#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/Base32Enc.h"

const UInt8 Text::TextBinEnc::Base32Enc::decArr[] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
		0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
		0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

const UTF8Char *Text::TextBinEnc::Base32Enc::GetEncArr()
{
	return (const UTF8Char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
}

Text::TextBinEnc::Base32Enc::Base32Enc()
{
}

Text::TextBinEnc::Base32Enc::~Base32Enc()
{
}

UOSInt Text::TextBinEnc::Base32Enc::EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	const UTF8Char *encArr = GetEncArr();
	UOSInt outSize;
	UTF8Char sptr[8];
	UOSInt tmp1 = buffSize * 8 / 5;
	UOSInt tmp2 = buffSize * 8 % 5;
	if (tmp2)
	{
		outSize = tmp1 + 1;
	}
	else
	{
		outSize = tmp1;
	}
	if (outSize == 0)
		return 0;
	sb->AllocLeng(outSize);
	tmp1 = buffSize / 5;
	tmp2 = buffSize - tmp1 * 5;
	while (tmp1-- > 0)
	{
		sptr[0] = encArr[dataBuff[0] >> 3];
		sptr[1] = encArr[((dataBuff[0] << 2) | (dataBuff[1] >> 6)) & 0x1f];
		sptr[2] = encArr[(dataBuff[1] >> 1) & 0x1f];
		sptr[3] = encArr[((dataBuff[1] << 4) | (dataBuff[2] >> 4)) & 0x1f];
		sptr[4] = encArr[((dataBuff[2] << 1) | (dataBuff[3] >> 7)) & 0x1f];
		sptr[5] = encArr[(dataBuff[3] >> 2) & 0x1f];
		sptr[6] = encArr[((dataBuff[3] << 3) | (dataBuff[4] >> 5)) & 0x1f];
		sptr[7] = encArr[dataBuff[4] & 0x1f];
		sb->AppendC(sptr, 8);
		dataBuff += 5;
	}
	if (tmp2 == 1)
	{
		sptr[0] = encArr[dataBuff[0] >> 3];
		sptr[1] = encArr[(dataBuff[0] << 2) & 0x1f];
		sb->AppendC(sptr, 2);
	}
	else if (tmp2 == 2)
	{
		sptr[0] = encArr[dataBuff[0] >> 3];
		sptr[1] = encArr[((dataBuff[0] << 2) | (dataBuff[1] >> 6)) & 0x1f];
		sptr[2] = encArr[(dataBuff[1] >> 1) & 0x1f];
		sptr[3] = encArr[((dataBuff[1] << 4)) & 0x1f];
		sb->AppendC(sptr, 4);
	}
	else if (tmp2 == 3)
	{
		sptr[0] = encArr[dataBuff[0] >> 3];
		sptr[1] = encArr[((dataBuff[0] << 2) | (dataBuff[1] >> 6)) & 0x1f];
		sptr[2] = encArr[(dataBuff[1] >> 1) & 0x1f];
		sptr[3] = encArr[((dataBuff[1] << 4) | (dataBuff[2] >> 4)) & 0x1f];
		sptr[4] = encArr[((dataBuff[2] << 1)) & 0x1f];
		sb->AppendC(sptr, 5);
	}
	else if (tmp2 == 4)
	{
		sptr[0] = encArr[dataBuff[0] >> 3];
		sptr[1] = encArr[((dataBuff[0] << 2) | (dataBuff[1] >> 6)) & 0x1f];
		sptr[2] = encArr[(dataBuff[1] >> 1) & 0x1f];
		sptr[3] = encArr[((dataBuff[1] << 4) | (dataBuff[2] >> 4)) & 0x1f];
		sptr[4] = encArr[((dataBuff[2] << 1) | (dataBuff[3] >> 7)) & 0x1f];
		sptr[5] = encArr[(dataBuff[3] >> 2) & 0x1f];
		sptr[6] = encArr[((dataBuff[3] << 3)) & 0x1f];
		sb->AppendC(sptr, 7);
	}
	return outSize;
}

UOSInt Text::TextBinEnc::Base32Enc::CalcBinSize(const UTF8Char *sbuff, UOSInt strLen)
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
	return cnt * 5 / 8;
}

UOSInt Text::TextBinEnc::Base32Enc::CalcBinSize(const WChar *sbuff)
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
	return cnt * 5 / 8;
}

UOSInt Text::TextBinEnc::Base32Enc::DecodeBin(const UTF8Char *b64Str, UOSInt strLen, UInt8 *dataBuff)
{
	UOSInt decSize = 0;
	UInt8 b = 0;
	UInt8 b2 = 0;
	UInt8 code;
	UTF8Char c;
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
					b2 = (UInt8)(code << 3);
					b = 1;
					break;
				case 1:
					*dataBuff = (UInt8)(b2 | (code >> 2));
					b2 = (UInt8)(code << 6);
					dataBuff++;
					decSize++;
					b = 2;
					break;
				case 2:
					b2 = (UInt8)(b2 | (code << 1));
					b = 3;
					break;
				case 3:
					*dataBuff = (UInt8)(b2 | (code >> 4));
					b2 = (UInt8)(code << 4);
					dataBuff++;
					decSize++;
					b = 4;
					break;
				case 4:
					*dataBuff = (UInt8)(b2 | (code >> 1));
					b2 = (UInt8)(code << 7);
					dataBuff++;
					decSize++;
					b = 5;
					break;
				case 5:
					b2 = (UInt8)(b2 | (code << 2));
					b = 6;
					break;
				case 6:
					*dataBuff = (UInt8)(b2 | (code >> 3));
					b2 = (UInt8)(code << 5);
					dataBuff++;
					decSize++;
					b = 7;
					break;
				case 7:
					*dataBuff = (UInt8)(b2 | code);
					b2 = 0;
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

Text::CStringNN Text::TextBinEnc::Base32Enc::GetName() const
{
	return CSTR("Base32");
}

Bool Text::TextBinEnc::Base32Enc::IsValid(const UTF8Char *b32Str)
{
	UTF8Char c;
	while ((c = *b32Str++) != 0)
	{
		if ((c >= 'A' && c <= 'Z') || (c >= '2' && c <= '7'))
		{

		}
		else
		{
			return false;
		}
	}
	return true;
}
