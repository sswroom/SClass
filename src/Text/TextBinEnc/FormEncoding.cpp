#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/TextBinEnc/FormEncoding.h"

static UInt8 URIAllow[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
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

void Text::TextBinEnc::FormEncoding::FormEncode(Text::StringBuilderUTF *sb, const UTF8Char *uri, UOSInt uriLen)
{
	UInt8 b;
	while (uriLen-- > 0)
	{
		b = *uri++;
		if (URIAllow[b])
		{
			sb->AppendChar(b, 1);
		}
		else
		{
			if (b == ' ')
			{
				sb->AppendChar('+', 1);
			}
			else
			{
				sb->AppendChar('%', 1);
				sb->AppendHex8(b);
			}
		}
	}
}

UTF8Char *Text::TextBinEnc::FormEncoding::FormEncode(UTF8Char *buff, const UTF8Char *uri)
{
	const UTF8Char *src;
	UInt8 b;
	UTF8Char *dest;

	src = uri;
	dest = buff;
	while (*src)
	{
		if (URIAllow[*src])
		{
			*dest++ = *src++;
		}
		else
		{
			b = *src++;
			if (b == ' ')
			{
				*dest++ = '+';
			}
			else
			{
				*dest++ = '%';
				*dest++ = (UTF8Char)MyString_STRHEXARR[b >> 4];
				*dest++ = (UTF8Char)MyString_STRHEXARR[b & 15];
			}
		}
	}
	*dest = 0;
	return dest;
}

UTF8Char *Text::TextBinEnc::FormEncoding::FormDecode(UTF8Char *buff, const UTF8Char *uri)
{
	UInt8 *dest;
	UInt8 v;
	UTF8Char c;
	dest = buff;

	while ((c = *uri++) != 0)
	{
		if (c == '+')
		{
			*dest++ = ' ';
		}
		else if (c == '%')
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
					*dest++ = '%';
					*dest++ = (UInt8)c;
					c = 0;
				}
				if (c)
				{
					c = *uri++;
					if (c >= 0x30 && c <= 0x39)
					{
						*dest++ = (UInt8)((v << 4) + (c - 0x30));
					}
					else if (c >= 0x41 && c <= 0x46)
					{
						*dest++ = (UInt8)((v << 4) + (c - 0x37));
					}
					else if (c >= 0x61 && c <= 0x66)
					{
						*dest++ = (UInt8)((v << 4) + (c - 0x57));
					}
					else
					{
						*dest++ = '%';
						*dest++ = (UInt8)uri[-2];
						*dest++ = (UInt8)c;
					}
				}
			}
		}
		else
		{
			*dest++ = (UInt8)c;
		}
	}
	*dest = 0;
	return dest;
}

Text::TextBinEnc::FormEncoding::FormEncoding()
{
}

Text::TextBinEnc::FormEncoding::~FormEncoding()
{
}

UOSInt Text::TextBinEnc::FormEncoding::EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	UOSInt initLen = sb->GetCharCnt();
	FormEncode(sb, dataBuff, buffSize);
	return sb->GetCharCnt() - initLen;
}

UOSInt Text::TextBinEnc::FormEncoding::CalcBinSize(const UTF8Char *str, UOSInt strLen)
{
	UTF8Char c;
	UOSInt retSize = 0;

	while (strLen-- > 0)
	{
		c = *str++;
		if (c == '+')
		{
		}
		else if (c == '%' && strLen >= 2)
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

UOSInt Text::TextBinEnc::FormEncoding::DecodeBin(const UTF8Char *str, UOSInt strLen, UInt8 *dataBuff)
{
	UInt8 v;
	UTF8Char c;
	UOSInt retSize = 0;

	while (strLen-- > 0)
	{
		c = *str++;
		if (c == '+')
		{
			*dataBuff++ = ' ';
		}
		else if (c == '%' && strLen >= 2)
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

Text::CString Text::TextBinEnc::FormEncoding::GetName()
{
	return {UTF8STRC("Form Encoding")};
}
