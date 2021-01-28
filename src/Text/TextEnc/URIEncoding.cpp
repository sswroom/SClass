#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/TextEnc/URIEncoding.h"

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

UTF8Char *Text::TextEnc::URIEncoding::URIEncode(UTF8Char *buff, const UTF8Char *uri)
{
	UInt8 b;
	UTF8Char *dest;

	dest = buff;
	while (*uri)
	{
		if (URIAllowRes[*uri])
		{
			*dest++ = *uri++;
		}
		else
		{
			*dest++ = '%';
			b = *uri++;
			*dest++ = MyString_STRHEXARR[b >> 4];
			*dest++ = MyString_STRHEXARR[b & 15];
		}
	}
	*dest = 0;
	return dest;
}

UTF8Char *Text::TextEnc::URIEncoding::URIDecode(UTF8Char *buff, const UTF8Char *uri)
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
					v = c - 0x30;
				}
				else if (c >= 0x41 && c <= 0x46)
				{
					v = c - 0x37;
				}
				else if (c >= 0x61 && c <= 0x66)
				{
					v = c - 0x57;
				}
				else
				{
					*dest++ = '%';
					*dest++ = c;
					c = 0;
				}
				if (c)
				{
					c = *uri++;
					if (c >= 0x30 && c <= 0x39)
					{
						*dest++ = (v << 4) + (c - 0x30);
					}
					else if (c >= 0x41 && c <= 0x46)
					{
						*dest++ = (v << 4) + (c - 0x37);
					}
					else if (c >= 0x61 && c <= 0x66)
					{
						*dest++ = (v << 4) + (c - 0x57);
					}
					else
					{
						*dest++ = '%';
						*dest++ = uri[-2];
						*dest++ = c;
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
