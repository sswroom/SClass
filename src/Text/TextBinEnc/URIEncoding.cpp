#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/URIEncoding.h"

static UInt8 URIAllowRes[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

UTF8Char *Text::TextBinEnc::URIEncoding::URIEncode(UTF8Char *buff, const UTF8Char *uri)
{
	UOSInt b;
	UTF8Char *dest;

	dest = buff;
	while ((b = *uri) != 0)
	{
		if (URIAllowRes[b])
		{
			*dest++ = (UTF8Char)b;
		}
		else
		{
			dest[0] = '%';
			dest[1] = (UTF8Char)MyString_STRHEXARR[b >> 4];
			dest[2] = (UTF8Char)MyString_STRHEXARR[b & 15];
			dest += 3;
		}
		uri++;
	}
	*dest = 0;
	return dest;
}

UTF8Char *Text::TextBinEnc::URIEncoding::URIDecode(UTF8Char *buff, const UTF8Char *uri)
{
	UTF8Char *dest;
	UTF8Char v;
	UTF8Char c;
	dest = buff;

	while ((c = *uri++) != 0)
	{
		if (c == '%')
		{
			if (uri[0] == 0 || uri[1] == 0)
			{
				*dest++ = (UInt8)c;
			}
			else
			{
				c = *uri++;
				if (c >= 0x30 && c <= 0x39)
				{
					v = (UTF8Char)(c - 0x30);
				}
				else if (c >= 0x41 && c <= 0x46)
				{
					v = (UTF8Char)(c - 0x37);
				}
				else if (c >= 0x61 && c <= 0x66)
				{
					v = (UTF8Char)(c - 0x57);
				}
				else
				{
					dest[0] = '%';
					dest[1] = c;
					c = 0;
					dest += 2;
				}
				if (c)
				{
					c = *uri++;
					if (c >= 0x30 && c <= 0x39)
					{
						*dest++ = (UTF8Char)((v << 4) + (c - 0x30));
					}
					else if (c >= 0x41 && c <= 0x46)
					{
						*dest++ = (UTF8Char)((v << 4) + (c - 0x37));
					}
					else if (c >= 0x61 && c <= 0x66)
					{
						*dest++ = (UTF8Char)((v << 4) + (c - 0x57));
					}
					else
					{
						dest[0] = '%';
						dest[1] = uri[-2];
						dest[2] = c;
						dest += 3;
					}
				}
			}
		}
		else
		{
			*dest++ = c;
		}
	}
	*dest = 0;
	return dest;
}

Text::TextBinEnc::URIEncoding::URIEncoding()
{
}

Text::TextBinEnc::URIEncoding::~URIEncoding()
{
}

UOSInt Text::TextBinEnc::URIEncoding::EncodeBin(Text::StringBuilderUTF8 *sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	UInt8 b;
	UOSInt initLen = sb->GetCharCnt();

	while (buffSize-- > 0)
	{
		b = *dataBuff++;
		if (URIAllowRes[b])
		{
			sb->AppendChar(b, 1);
		}
		else
		{
			sb->AppendUTF8Char('%');
			sb->AppendHex8(b);
		}
	}
	return sb->GetCharCnt() - initLen;

}

UOSInt Text::TextBinEnc::URIEncoding::CalcBinSize(const UTF8Char *str, UOSInt strLen)
{
	UTF8Char c;
	UOSInt retSize = 0;

	while (strLen-- > 0)
	{
		c = *str++;
		if (c == '%' && strLen >= 2)
		{
			c = str[0];
			if ((c >= 0x30 && c <= 0x39) || (c >= 0x41 && c <= 0x46) || (c >= 0x61 && c <= 0x66))
			{
				c = str[1];
				if ((c >= 0x30 && c <= 0x39) || (c >= 0x41 && c <= 0x46) || (c >= 0x61 && c <= 0x66))
				{
					strLen -= 2;
					str += 2;
				}
			}
		}
		retSize++;
	}
	return retSize;

}

UOSInt Text::TextBinEnc::URIEncoding::DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff)
{
	UInt8 v;
	UTF8Char c;
	UOSInt retSize = 0;

	while (strLen-- > 0)
	{
		c = *str++;
		if (c == '%' && strLen >= 2)
		{
			c = str[0];
			if (c >= 0x30 && c <= 0x39)
			{
				v = (UInt8)(c - 0x30);
			}
			else if (c >= 0x41 && c <= 0x46)
			{
				v = (UInt8)(c - 0x37);
			}
			else if (c >= 0x61 && c <= 0x66)
			{
				v = (UInt8)(c - 0x57);
			}
			else
			{
				*dataBuff++ = '%';
				c = 0;
			}
			if (c)
			{
				c = str[1];
				if (c >= 0x30 && c <= 0x39)
				{
					*dataBuff++ = (UInt8)((v << 4) + (c - 0x30));
					str += 2;
					strLen -= 2;
				}
				else if (c >= 0x41 && c <= 0x46)
				{
					*dataBuff++ = (UInt8)((v << 4) + (c - 0x37));
					str += 2;
					strLen -= 2;
				}
				else if (c >= 0x61 && c <= 0x66)
				{
					*dataBuff++ = (UInt8)((v << 4) + (c - 0x57));
					str += 2;
					strLen -= 2;
				}
				else
				{
					*dataBuff++ = '%';
				}
			}
		}
		else
		{
			*dataBuff++ = (UInt8)c;
		}
		retSize++;
	}
	return retSize;
}

Text::CString Text::TextBinEnc::URIEncoding::GetName()
{
	return {UTF8STRC("URI Encoding")};
}
