#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/XML.h"

UOSInt Text::XML::GetXMLTextLen(const UTF8Char *text)
{
	UOSInt cnt = 0;
	const UTF8Char *sptr = text;
	UTF8Char c;
	while ((c = *sptr++) != 0)
	{
		switch (c)
		{
		case '&':
			cnt += 5;
			break;
		case '<':
			cnt += 4;
			break;
		case '>':
			cnt += 4;
			break;
		case '\'':
			cnt += 6;
			break;
		case '"':
			cnt += 6;
			break;
		case '\n':
			cnt += 5;
			break;
		default:
			cnt++;
			break;
		}
	}
	return cnt;
}

UOSInt Text::XML::GetXMLTextLen(const WChar *text)
{
	UOSInt cnt = 0;
	const WChar *sptr = text;
	WChar c;
	while ((c = *sptr++) != 0)
	{
		switch (c)
		{
		case '&':
			cnt += 5;
			break;
		case '<':
			cnt += 4;
			break;
		case '>':
			cnt += 4;
			break;
		case '\'':
			cnt += 6;
			break;
		case '"':
			cnt += 6;
			break;
		case '\n':
			cnt += 5;
			break;
		default:
			cnt++;
			break;
		}
	}
	return cnt;
}

UOSInt Text::XML::GetXMLTextLiteLen(const UTF8Char *text)
{
	UOSInt cnt = 0;
	const UTF8Char *sptr = text;
	UTF8Char c;
	while ((c = *sptr++) != 0)
	{
		switch (c)
		{
		case '&':
			cnt += 5;
			break;
		case '<':
			cnt += 4;
			break;
		case '>':
			cnt += 4;
			break;
		default:
			cnt++;
			break;
		}
	}
	return cnt;
}

UOSInt Text::XML::GetXMLTextLiteLen(const WChar *text)
{
	UOSInt cnt = 0;
	const WChar *sptr = text;
	WChar c;
	while ((c = *sptr++) != 0)
	{
		switch (c)
		{
		case '&':
			cnt += 5;
			break;
		case '<':
			cnt += 4;
			break;
		case '>':
			cnt += 4;
			break;
		default:
			cnt++;
			break;
		}
	}
	return cnt;
}

UOSInt Text::XML::GetHTMLTextLen(const UTF8Char *text)
{
	UOSInt cnt = 0;
	const UTF8Char *sptr = text;
	UTF8Char c;
	while ((c = *sptr++) != 0)
	{
		switch (c)
		{
		case '&':
			cnt += 5;
			break;
		case '<':
			cnt += 4;
			break;
		case '>':
			cnt += 4;
			break;
		case '\'':
			cnt += 6;
			break;
		case '"':
			cnt += 6;
			break;
		case '\r':
			break;
		case '\n':
			cnt += 5;
			break;
		default:
			cnt++;
			break;
		}
	}
	return cnt;
}

UOSInt Text::XML::GetHTMLTextLen(const WChar *text)
{
	UOSInt cnt = 0;
	const WChar *sptr = text;
	WChar c;
	while ((c = *sptr++) != 0)
	{
		switch (c)
		{
		case '&':
			cnt += 5;
			break;
		case '<':
			cnt += 4;
			break;
		case '>':
			cnt += 4;
			break;
		case '\'':
			cnt += 6;
			break;
		case '"':
			cnt += 6;
			break;
		case '\r':
			break;
		case '\n':
			cnt += 5;
			break;
		default:
			cnt++;
			break;
		}
	}
	return cnt;
}

Bool Text::XML::WriteUTF8Char(IO::Stream *stm, UTF32Char c)
{
	UInt8 buff[6];
	if (c < 0x80)
	{
		buff[0] = (UInt8)c;
		return stm->Write(buff, 1) == 1;
	}
	else if (c < 0x800)
	{
		buff[0] = (UInt8)(0xc0 | (c >> 6));
		buff[1] = (UInt8)(0x80 | (c & 0x3f));
		return stm->Write(buff, 2) == 2;
	}
	else if (c < 0x10000)
	{
		buff[0] = (UInt8)(0xe0 | (c >> 12));
		buff[1] = (UInt8)(0x80 | ((c >> 6) & 0x3f));
		buff[2] = (UInt8)(0x80 | (c & 0x3f));
		return stm->Write(buff, 3) == 3;
	}
	else if (c < 0x200000)
	{
		buff[0] = (UInt8)(0xf0 | (c >> 18));
		buff[1] = (UInt8)(0x80 | ((c >> 12) & 0x3f));
		buff[2] = (UInt8)(0x80 | ((c >> 6) & 0x3f));
		buff[3] = (UInt8)(0x80 | (c & 0x3f));
		return stm->Write(buff, 4) == 4;
	}
	else if (c < 0x4000000)
	{
		buff[0] = (UInt8)(0xf8 | (c >> 24));
		buff[1] = (UInt8)(0x80 | ((c >> 18) & 0x3f));
		buff[2] = (UInt8)(0x80 | ((c >> 12) & 0x3f));
		buff[3] = (UInt8)(0x80 | ((c >> 6) & 0x3f));
		buff[4] = (UInt8)(0x80 | (c & 0x3f));
		return stm->Write(buff, 5) == 5;
	}
	else
	{
		buff[0] = (UInt8)(0xfc | (c >> 30));
		buff[1] = (UInt8)(0x80 | ((c >> 24) & 0x3f));
		buff[2] = (UInt8)(0x80 | ((c >> 18) & 0x3f));
		buff[3] = (UInt8)(0x80 | ((c >> 12) & 0x3f));
		buff[4] = (UInt8)(0x80 | ((c >> 6) & 0x3f));
		buff[5] = (UInt8)(0x80 | (c & 0x3f));
		return stm->Write(buff, 6) == 6;
	}
}

UTF8Char *Text::XML::ToXMLText(UTF8Char *buff, const UTF8Char *text)
{
	UTF8Char *dptr = buff;
	const UTF8Char *sptr = text;
	UTF8Char c;
	while ((c = *sptr++) != 0)
	{
		if (c == '&')
		{
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'm';
			dptr[3] = 'p';
			dptr[4] = ';';
			dptr += 5;
		}
		else if (c == '<')
		{
			dptr[0] = '&';
			dptr[1] = 'l';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else if (c == '>')
		{
			dptr[0] = '&';
			dptr[1] = 'g';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else if (c == '\'')
		{
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'p';
			dptr[3] = 'o';
			dptr[4] = 's';
			dptr[5] = ';';
			dptr += 6;
		}
		else if (c == '"')
		{
			dptr[0] = '&';
			dptr[1] = 'q';
			dptr[2] = 'u';
			dptr[3] = 'o';
			dptr[4] = 't';
			dptr[5] = ';';
			dptr += 6;
		}
		else if (c == '\n')
		{
			dptr[0] = '&';
			dptr[1] = '#';
			dptr[2] = '1';
			dptr[3] = '0';
			dptr[4] = ';';
			dptr += 5;
		}
		else
		{
			*dptr++ = c;
		}
	}
	*dptr = 0;
	return dptr;
}

WChar *Text::XML::ToXMLText(WChar *buff, const WChar *text)
{
	WChar *dptr = buff;
	const WChar *sptr = text;
	WChar c;
	while ((c = *sptr++) != 0)
	{
		if (c == '&')
		{
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'm';
			dptr[3] = 'p';
			dptr[4] = ';';
			dptr += 5;
		}
		else if (c == '<')
		{
			dptr[0] = '&';
			dptr[1] = 'l';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else if (c == '>')
		{
			dptr[0] = '&';
			dptr[1] = 'g';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else if (c == '\'')
		{
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'p';
			dptr[3] = 'o';
			dptr[4] = 's';
			dptr[5] = ';';
			dptr += 6;
		}
		else if (c == '"')
		{
			dptr[0] = '&';
			dptr[1] = 'q';
			dptr[2] = 'u';
			dptr[3] = 'o';
			dptr[4] = 't';
			dptr[5] = ';';
			dptr += 6;
		}
		else if (c == '\n')
		{
			dptr[0] = '&';
			dptr[1] = '#';
			dptr[2] = '1';
			dptr[3] = '0';
			dptr[4] = ';';
			dptr += 5;
		}
		else
		{
			*dptr++ = c;
		}
	}
	*dptr = 0;
	return dptr;
}

UTF8Char *Text::XML::ToXMLTextLite(UTF8Char *buff, const UTF8Char *text)
{
	UTF8Char *dptr = buff;
	const UTF8Char *sptr = text;
	UTF8Char c;
	while ((c = *sptr++) != 0)
	{
		if (c == '&')
		{
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'm';
			dptr[3] = 'p';
			dptr[4] = ';';
			dptr += 5;
		}
		else if (c == '<')
		{
			dptr[0] = '&';
			dptr[1] = 'l';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else if (c == '>')
		{
			dptr[0] = '&';
			dptr[1] = 'g';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else
		{
			*dptr++ = c;
		}
	}
	*dptr = 0;
	return dptr;
}

WChar *Text::XML::ToXMLTextLite(WChar *buff, const WChar *text)
{
	WChar *dptr = buff;
	const WChar *sptr = text;
	WChar c;
	while ((c = *sptr++) != 0)
	{
		if (c == '&')
		{
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'm';
			dptr[3] = 'p';
			dptr[4] = ';';
			dptr += 5;
		}
		else if (c == '<')
		{
			dptr[0] = '&';
			dptr[1] = 'l';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else if (c == '>')
		{
			dptr[0] = '&';
			dptr[1] = 'g';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else
		{
			*dptr++ = c;
		}
	}
	*dptr = 0;
	return dptr;
}

UTF8Char *Text::XML::ToHTMLText(UTF8Char *buff, const UTF8Char *text)
{
	UTF8Char *dptr = buff;
	const UTF8Char *sptr = text;
	UTF8Char c;
	while ((c = *sptr++) != 0)
	{
		if (c == '&')
		{
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'm';
			dptr[3] = 'p';
			dptr[4] = ';';
			dptr += 5;
		}
		else if (c == '<')
		{
			dptr[0] = '&';
			dptr[1] = 'l';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else if (c == '>')
		{
			dptr[0] = '&';
			dptr[1] = 'g';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else if (c == '\'')
		{
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'p';
			dptr[3] = 'o';
			dptr[4] = 's';
			dptr[5] = ';';
			dptr += 6;
		}
		else if (c == '"')
		{
			dptr[0] = '&';
			dptr[1] = 'q';
			dptr[2] = 'u';
			dptr[3] = 'o';
			dptr[4] = 't';
			dptr[5] = ';';
			dptr += 6;
		}
		else if (c == '\r')
		{

		}
		else if (c == '\n')
		{
			dptr[0] = '<';
			dptr[1] = 'b';
			dptr[2] = 'r';
			dptr[3] = '/';
			dptr[4] = '>';
			dptr += 5;
		}
		else
		{
			*dptr++ = c;
		}
	}
	*dptr = 0;
	return dptr;
}

WChar *Text::XML::ToHTMLText(WChar *buff, const WChar *text)
{
	WChar *dptr = buff;
	const WChar *sptr = text;
	WChar c;
	while ((c = *sptr++) != 0)
	{
		if (c == '&')
		{
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'm';
			dptr[3] = 'p';
			dptr[4] = ';';
			dptr += 5;
		}
		else if (c == '<')
		{
			dptr[0] = '&';
			dptr[1] = 'l';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else if (c == '>')
		{
			dptr[0] = '&';
			dptr[1] = 'g';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
		}
		else if (c == '\'')
		{
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'p';
			dptr[3] = 'o';
			dptr[4] = 's';
			dptr[5] = ';';
			dptr += 6;
		}
		else if (c == '"')
		{
			dptr[0] = '&';
			dptr[1] = 'q';
			dptr[2] = 'u';
			dptr[3] = 'o';
			dptr[4] = 't';
			dptr[5] = ';';
			dptr += 6;
		}
		else if (c == '\r')
		{

		}
		else if (c == '\n')
		{
			dptr[0] = '<';
			dptr[1] = 'b';
			dptr[2] = 'r';
			dptr[3] = '/';
			dptr[4] = '>';
			dptr += 5;
		}
		else
		{
			*dptr++ = c;
		}
	}
	*dptr = 0;
	return dptr;
}

UTF8Char *Text::XML::ToAttrText(UTF8Char *buff, const UTF8Char *text)
{
	*buff++ = '"';
	buff = ToXMLText(buff, text);
	*buff++ = '"';
	*buff = 0;
	return buff;
}

WChar *Text::XML::ToAttrText(WChar *buff, const WChar *text)
{
	*buff++ = '"';
	buff = ToXMLText(buff, text);
	*buff++ = '"';
	*buff = 0;
	return buff;
}

const UTF8Char *Text::XML::ToNewXMLText(const UTF8Char *text)
{
	UOSInt cnt = GetXMLTextLen(text) + 1;
	UTF8Char *dptr = MemAlloc(UTF8Char, cnt);
	ToXMLText(dptr, text);
	return dptr;
}

const WChar *Text::XML::ToNewXMLText(const WChar *text)
{
	UOSInt cnt = GetXMLTextLen(text) + 1;
	WChar *dptr = MemAlloc(WChar, cnt);
	ToXMLText(dptr, text);
	return dptr;
}

const UTF8Char *Text::XML::ToNewXMLTextLite(const UTF8Char *text)
{
	UOSInt cnt = GetXMLTextLiteLen(text) + 1;
	UTF8Char *dptr = MemAlloc(UTF8Char, cnt);
	ToXMLTextLite(dptr, text);
	return dptr;
}

const WChar *Text::XML::ToNewXMLTextLite(const WChar *text)
{
	UOSInt cnt = GetXMLTextLiteLen(text) + 1;
	WChar *dptr = MemAlloc(WChar, cnt);
	ToXMLTextLite(dptr, text);
	return dptr;
}

const UTF8Char *Text::XML::ToNewHTMLText(const UTF8Char *text)
{
	UOSInt cnt = GetHTMLTextLen(text) + 1;
	UTF8Char *dptr = MemAlloc(UTF8Char, cnt);
	ToHTMLText(dptr, text);
	return dptr;
}

const WChar *Text::XML::ToNewHTMLText(const WChar *text)
{
	UOSInt cnt = GetHTMLTextLen(text) + 1;
	WChar *dptr = MemAlloc(WChar, cnt);
	ToHTMLText(dptr, text);
	return dptr;
}

const UTF8Char *Text::XML::ToNewAttrText(const UTF8Char *text)
{
	UOSInt cnt = GetXMLTextLen(text) + 3;
	UTF8Char *dptr = MemAlloc(UTF8Char, cnt);
	UTF8Char *buff = dptr;
	*buff++ = '"';
	buff = ToXMLText(buff, text);
	*buff++ = '"';
	*buff = 0;
	return dptr;
}

const WChar *Text::XML::ToNewAttrText(const WChar *text)
{
	UOSInt cnt = GetXMLTextLen(text) + 3;
	WChar *dptr = MemAlloc(WChar, cnt);
	WChar *buff = dptr;
	*buff++ = '"';
	buff = ToXMLText(buff, text);
	*buff++ = '"';
	*buff = 0;
	return dptr;
}

void Text::XML::FreeNewText(const UTF8Char *text)
{
	MemFree((void*)text);
}

void Text::XML::FreeNewText(const WChar *text)
{
	MemFree((void*)text);
}

void Text::XML::ParseStr(UTF8Char *out, const UTF8Char *xmlStart, const UTF8Char *xmlEnd)
{
	UTF8Char *currPtr = out;
	UTF8Char c;
	while (xmlStart < xmlEnd)
	{
		c = *xmlStart;
		if (c == '&')
		{
			if (xmlStart[1] == 'a' && xmlStart[2] == 'm' && xmlStart[3] == 'p' && xmlStart[4] == ';')
			{
				*currPtr++ = '&';
				xmlStart += 5;
			}
			else if (xmlStart[1] == 'l' && xmlStart[2] == 't' && xmlStart[3] == ';')
			{
				*currPtr++ = '<';
				xmlStart += 4;
			}
			else if (xmlStart[1] == 'g' && xmlStart[2] == 't' && xmlStart[3] == ';')
			{
				*currPtr++ = '>';
				xmlStart += 4;
			}
			else if (xmlStart[1] == 'a' && xmlStart[2] == 'p' && xmlStart[3] == 'o' && xmlStart[4] == 's' && xmlStart[5] == ';')
			{
				*currPtr++ = '\'';
				xmlStart += 6;
			}
			else if (xmlStart[1] == 'a' && xmlStart[2] == 'p' && xmlStart[3] == 'o' && xmlStart[4] == 's' && xmlStart[5] == ';')
			{
				*currPtr++ = '"';
				xmlStart += 6;
			}
			else if (xmlStart[1] == '#' && xmlStart[2] == 'x')
			{
				Bool valid = true;
				UInt32 v = 0;
				const UTF8Char *tmp = xmlStart + 3;
				while (true)
				{
					c = *tmp++;
					if (c >= '0' && c <= '9')
					{
						v = (v << 4) + (UInt32)(c - 48);
					}
					else if (c >= 'A' && c <= 'F')
					{
						v = (v << 4) + (UInt32)(c - 0x37);
					}
					else if (c >= 'a' && c <= 'f')
					{
						v = (v << 4) + (UInt32)(c - 0x57);
					}
					else if (c == ';')
					{
						currPtr = Text::StrWriteChar(currPtr, (UTF32Char)v);
						xmlStart = tmp;
						break;
					}
					else
					{
						valid = false;
						break;
					}
				}
				if (!valid)
				{
					*currPtr++ = '&';
					xmlStart++;
				}
			}
			else if (xmlStart[1] == '#')
			{
				Bool valid = true;
				UInt32 v = 0;
				const UTF8Char *tmp = xmlStart + 2;
				while (true)
				{
					c = *tmp++;
					if (c >= '0' && c <= '9')
					{
						v = v * 10 + (UInt32)(c - 48);
					}
					else if (c == ';')
					{
						currPtr = Text::StrWriteChar(currPtr, (UTF32Char)v);
						xmlStart = tmp;
						break;
					}
					else
					{
						valid = false;
						break;
					}
				}
				if (!valid)
				{
					*currPtr++ = '&';
					xmlStart++;
				}
			}
			else
			{
				*currPtr++ = '&';
				xmlStart++;
			}
		}
		else
		{
			*currPtr++ = c;
			xmlStart++;
		}
	}
	*currPtr = 0;
}

void Text::XML::ParseStr(WChar *out, const WChar *xmlStart, const WChar *xmlEnd)
{
	WChar *currPtr = out;
	WChar c;
	while (xmlStart < xmlEnd)
	{
		c = *xmlStart;
		if (c == '&')
		{
			if (xmlStart[1] == 'a' && xmlStart[2] == 'm' && xmlStart[3] == 'p' && xmlStart[4] == ';')
			{
				*currPtr++ = '&';
				xmlStart += 5;
			}
			else if (xmlStart[1] == 'l' && xmlStart[2] == 't' && xmlStart[3] == ';')
			{
				*currPtr++ = '<';
				xmlStart += 4;
			}
			else if (xmlStart[1] == 'g' && xmlStart[2] == 't' && xmlStart[3] == ';')
			{
				*currPtr++ = '>';
				xmlStart += 4;
			}
			else if (xmlStart[1] == 'a' && xmlStart[2] == 'p' && xmlStart[3] == 'o' && xmlStart[4] == 's' && xmlStart[5] == ';')
			{
				*currPtr++ = '\'';
				xmlStart += 6;
			}
			else if (xmlStart[1] == 'a' && xmlStart[2] == 'p' && xmlStart[3] == 'o' && xmlStart[4] == 's' && xmlStart[5] == ';')
			{
				*currPtr++ = '"';
				xmlStart += 6;
			}
			else if (xmlStart[1] == '#' && xmlStart[2] == 'x')
			{
				Bool valid = true;
				UInt32 v = 0;
				const WChar *tmp = xmlStart + 3;
				while (true)
				{
					c = *tmp++;
					if (c >= '0' && c <= '9')
					{
						v = (v << 4) + (UInt32)(c - 48);
					}
					else if (c >= 'A' && c <= 'F')
					{
						v = (v << 4) + (UInt32)(c - 0x37);
					}
					else if (c >= 'a' && c <= 'f')
					{
						v = (v << 4) + (UInt32)(c - 0x57);
					}
					else if (c == ';')
					{
						currPtr = Text::StrWriteChar(currPtr, (UTF32Char)v);
						xmlStart = tmp;
						break;
					}
					else
					{
						valid = false;
						break;
					}
				}
				if (!valid)
				{
					*currPtr++ = '&';
					xmlStart++;
				}
			}
			else if (xmlStart[1] == '#')
			{
				Bool valid = true;
				UInt32 v = 0;
				const WChar *tmp = xmlStart + 2;
				while (true)
				{
					c = *tmp++;
					if (c >= '0' && c <= '9')
					{
						v = v * 10 + (UInt32)(c - 48);
					}
					else if (c == ';')
					{
						currPtr = Text::StrWriteChar(currPtr, (UTF32Char)v);
						xmlStart = tmp;
						break;
					}
					else
					{
						valid = false;
						break;
					}
				}
				if (!valid)
				{
					*currPtr++ = '&';
					xmlStart++;
				}
			}
			else
			{
				*currPtr++ = '&';
				xmlStart++;
			}
		}
		else
		{
			*currPtr++ = c;
			xmlStart++;
		}
	}
	*currPtr = 0;
}

Bool Text::XML::HTMLAppendCharRef(const UTF8Char *chrRef, OSInt refSize, IO::Stream *stm)
{
	UTF8Char sbuff[6];
	UTF32Char wcs;
	if (chrRef[0] != '&')
	{
		return false;
	}
	if (refSize == 4)
	{
		if (chrRef[1] == '#')
		{
			sbuff[0] = chrRef[2];
			sbuff[1] = 0;
			sbuff[0] = (UInt8)Text::StrToUInt32(sbuff);
			stm->Write(sbuff, 1);
			return true;
		}
		else if (Text::StrStartsWith(chrRef, (const UTF8Char*)"&lt;"))
		{
			stm->Write((const UInt8*)"<", 1);
			return true;
		}
		else if (Text::StrStartsWith(chrRef, (const UTF8Char*)"&gt;"))
		{
			stm->Write((const UInt8*)">", 1);
			return true;
		}
	}
	else if (refSize == 5)
	{
		if (chrRef[1] == '#')
		{
			if (chrRef[2] == 'x')
			{
				sbuff[0] = Text::StrHex2UInt8C(&chrRef[3]);
				stm->Write(sbuff, 1);
				return true;
			}
			else
			{
				sbuff[0] = chrRef[2];
				sbuff[1] = chrRef[3];
				sbuff[2] = 0;
				wcs = (UTF32Char)Text::StrToUInt32(sbuff);
				return WriteUTF8Char(stm, wcs);
			}
		}
		else if (Text::StrStartsWith(chrRef, (const UTF8Char*)"&amp;"))
		{
			stm->Write((const UInt8*)"&", 1);
			return true;
		}
	}
	else if (refSize == 6)
	{
		if (chrRef[1] == '#')
		{
			if (chrRef[2] == 'x')
			{
				wcs = Text::StrHex2UInt8C(&chrRef[3]);
			}
			else
			{
				sbuff[0] = chrRef[2];
				sbuff[1] = chrRef[3];
				sbuff[2] = chrRef[4];
				sbuff[3] = 0;
				wcs = (UTF32Char)Text::StrToInt32(sbuff);
			}
			return WriteUTF8Char(stm, wcs);
		}
		else if (Text::StrStartsWith(chrRef, (const UTF8Char*)"&apos;"))
		{
			stm->Write((const UInt8*)"\'", 1);
			return true;
		}
		else if (Text::StrStartsWith(chrRef, (const UTF8Char*)"&bull;"))
		{
			return WriteUTF8Char(stm, 0x2022);
		}
		else if (Text::StrStartsWith(chrRef, (const UTF8Char*)"&quot;"))
		{
			stm->Write((const UInt8*)"\"", 1);
			return true;
		}
	}
	else if (refSize == 7)
	{
		if (chrRef[1] == '#')
		{
			if (chrRef[2] == 'x')
			{
				wcs = Text::StrHex2UInt8C(&chrRef[4]);
				if (chrRef[3] <= '9')
				{
					wcs += (UTF32Char)(chrRef[3] - '0') << 8;
				}
				else if (chrRef[3] <= 'F')
				{
					wcs += (UTF32Char)(chrRef[3] - 0x37) << 8;
				}
				else if (chrRef[3] <= 'f')
				{
					wcs += (UTF32Char)(chrRef[3] - 0x57) << 8;
				}
			}
			else
			{
				sbuff[0] = chrRef[2];
				sbuff[1] = chrRef[3];
				sbuff[2] = chrRef[4];
				sbuff[3] = chrRef[5];
				sbuff[4] = 0;
				wcs = (UTF32Char)Text::StrToInt32(sbuff);
			}
			return WriteUTF8Char(stm, wcs);
		}
		else if (Text::StrStartsWith(chrRef, (const UTF8Char*)"&raquo;"))
		{
			return WriteUTF8Char(stm, 0xbb);
		}
	}
	return false;
}
