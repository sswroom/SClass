#include "Stdafx.h"
#include "Text/TextBinEnc/ASCII85Enc.h"

Text::TextBinEnc::ASCII85Enc::ASCII85Enc()
{
}

Text::TextBinEnc::ASCII85Enc::~ASCII85Enc()
{
}

UOSInt Text::TextBinEnc::ASCII85Enc::EncodeBin(NN<Text::StringBuilderUTF8> sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	UOSInt initLen = sb->GetCharCnt();
	UInt32 v;
	while (buffSize >= 4)
	{
		v = ReadMUInt32(dataBuff);
		dataBuff += 4;
		buffSize -= 4;
		sb->AppendUTF8Char((UTF8Char)(v / 52200625 + 33));
		v %= 52200625;
		sb->AppendUTF8Char((UTF8Char)(v / 614125 + 33));
		v %= 614125;
		sb->AppendUTF8Char((UTF8Char)(v / 7225 + 33));
		v %= 7225;
		sb->AppendUTF8Char((UTF8Char)(v / 85 + 33));
		v %= 85;
		sb->AppendUTF8Char((UTF8Char)(v + 33));
	}
	switch (buffSize)
	{
	case 1:
		v = ((UInt32)*dataBuff) << 24;
		sb->AppendUTF8Char((UTF8Char)(v / 52200625 + 33));
		v %= 52200625;
		sb->AppendUTF8Char((UTF8Char)(v / 614125 + 33));
//		sb->AppendChar('u', 3);
		break;
	case 2:
		v = ((UInt32)ReadMUInt16(dataBuff)) << 16;
		sb->AppendUTF8Char((UTF8Char)(v / 52200625 + 33));
		v %= 52200625;
		sb->AppendUTF8Char((UTF8Char)(v / 614125 + 33));
		v %= 614125;
		sb->AppendUTF8Char((UTF8Char)(v / 7225 + 33));
//		sb->AppendChar('u', 2);
		break;
	case 3:
		v = (ReadMUInt24(dataBuff)) << 8;
		sb->AppendUTF8Char((UTF8Char)(v / 52200625 + 33));
		v %= 52200625;
		sb->AppendUTF8Char((UTF8Char)(v / 614125 + 33));
		v %= 614125;
		sb->AppendUTF8Char((UTF8Char)(v / 7225 + 33));
		v %= 7225;
		sb->AppendUTF8Char((UTF8Char)(v / 85 + 33));
//		sb->AppendUTF8Char('u');
		break;
	}
	return sb->GetCharCnt() - initLen;
}

UTF8Char *Text::TextBinEnc::ASCII85Enc::EncodeBin(UTF8Char *sbuff, const UInt8 *dataBuff, UOSInt buffSize)
{
	UInt32 v;
	while (buffSize >= 4)
	{
		v = ReadMUInt32(dataBuff);
		dataBuff += 4;
		buffSize -= 4;
		*sbuff++ = (UTF8Char)(v / 52200625 + 33);
		v %= 52200625;
		*sbuff++ = (UTF8Char)(v / 614125 + 33);
		v %= 614125;
		*sbuff++ = (UTF8Char)(v / 7225 + 33);
		v %= 7225;
		*sbuff++ = (UTF8Char)(v / 85 + 33);
		v %= 85;
		*sbuff++ = (UTF8Char)(v + 33);
	}
	switch (buffSize)
	{
	case 1:
		v = ((UInt32)*dataBuff) << 24;
		*sbuff++ = (UTF8Char)(v / 52200625 + 33);
		v %= 52200625;
		*sbuff++ = (UTF8Char)(v / 614125 + 33);
//		*sbuff++ = 'u';
//		*sbuff++ = 'u';
//		*sbuff++ = 'u';
		break;
	case 2:
		v = ((UInt32)ReadMUInt16(dataBuff)) << 16;
		*sbuff++ = (UTF8Char)(v / 52200625 + 33);
		v %= 52200625;
		*sbuff++ = (UTF8Char)(v / 614125 + 33);
		v %= 614125;
		*sbuff++ = (UTF8Char)(v / 7225 + 33);
//		*sbuff++ = 'u';
//		*sbuff++ = 'u';
		break;
	case 3:
		v = (ReadMUInt24(dataBuff)) << 8;
		*sbuff++ = (UTF8Char)(v / 52200625 + 33);
		v %= 52200625;
		*sbuff++ = (UTF8Char)(v / 614125 + 33);
		v %= 614125;
		*sbuff++ = (UTF8Char)(v / 7225 + 33);
		v %= 7225;
		*sbuff++ = (UTF8Char)(v / 85 + 33);
//		*sbuff++ = 'u';
		break;
	}
	return sbuff;
}

UOSInt Text::TextBinEnc::ASCII85Enc::CalcBinSize(const UTF8Char *a85Str, UOSInt strLen)
{
	UOSInt zCnt = 0;
	UOSInt validCnt = 0;
	UOSInt lastU = 0;
	UTF8Char c;
	while (strLen-- > 0)
	{
		c = *a85Str++;
		if (c == 'z')
		{
			zCnt++;
			lastU = 0;
		}
		else if (c == '~')
		{
			if (strLen > 0 && *a85Str == '>')
			{
				lastU = 0;
				break;
			}
		}
		else if (c == 'u')
		{
			lastU++;
			validCnt++;
		}
		else if (c < 33 || c > 'u')
		{
		}
		else
		{
			lastU = 0;
			validCnt++;
		}
	}
	if (lastU > 0)
	{
		validCnt -= lastU;
	}
	UOSInt left = validCnt % 5;
	if (left > 0)
	{
		return zCnt * 4 + validCnt / 5 * 4 + left - 1;
	}
	else
	{
		return zCnt * 4 + validCnt / 5 * 4;
	}
}

UOSInt Text::TextBinEnc::ASCII85Enc::DecodeBin(const UTF8Char *a85Str, UOSInt strLen, UInt8 *dataBuff)
{
	UInt8 *destBuff = dataBuff;
	UTF8Char sbuff[5];
	UOSInt validCnt = 0;
	UOSInt lastU = 0;
	UTF8Char c;
	while (strLen-- > 0)
	{
		c = *a85Str++;
		if (c == 'z')
		{
			WriteNInt32(destBuff, 0);
			destBuff += 4;
			lastU = 0;
		}
		else if (c == '~')
		{
			if (strLen > 0 && *a85Str == '>')
			{
				lastU = 0;
				break;
			}
		}
		else if (c == 'u')
		{
			lastU++;
			sbuff[validCnt++] = c;
		}
		else if (c < 33 || c > 'u')
		{
		}
		else
		{
			lastU = 0;
			sbuff[validCnt++] = c;
		}
		if (validCnt == 5)
		{
			UInt32 v;
			v  = (UInt32)(sbuff[4] - 33);
			v += (UInt32)(sbuff[3] - 33) * 85;
			v += (UInt32)(sbuff[2] - 33) * 7225;
			v += (UInt32)(sbuff[1] - 33) * 614125;
			v += (UInt32)(sbuff[0] - 33) * 52200625;
			WriteMUInt32(destBuff, v);
			destBuff += 4;
			validCnt = 0;
		}
	}
	if (validCnt > 0)
	{
		UInt32 v;
		v = 5;
		while (v-- > validCnt)
		{
			sbuff[v] = 'u';
		}
		v  = (UInt32)(sbuff[4] - 33);
		v += (UInt32)(sbuff[3] - 33) * 85;
		v += (UInt32)(sbuff[2] - 33) * 7225;
		v += (UInt32)(sbuff[1] - 33) * 614125;
		v += (UInt32)(sbuff[0] - 33) * 52200625;
		WriteMUInt32(sbuff, v);
		validCnt--;
		v = 0;
		while (v < validCnt)
		{
			destBuff[v] = sbuff[v];
			v++;
		}
		destBuff += validCnt;
		validCnt = 0;
	}
	else if (lastU > 0)
	{
		destBuff -= lastU;
	}
	return (UOSInt)(destBuff - dataBuff);
}

Text::CStringNN Text::TextBinEnc::ASCII85Enc::GetName() const
{
	return CSTR("ASCII85");
}
