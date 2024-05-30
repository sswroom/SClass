#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/XML.h"

UOSInt Text::XML::GetXMLTextLen(UnsafeArray<const UTF8Char> text)
{
	UOSInt cnt = 0;
	UnsafeArray<const UTF8Char> sptr = text;
	while (true)
	{
		switch (*sptr++)
		{
		case 0:
			return cnt;
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
}

UOSInt Text::XML::GetXMLTextLen(const WChar *text)
{
	UOSInt cnt = 0;
	const WChar *wptr = text;
	WChar c;
	while ((c = *wptr++) != 0)
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

UOSInt Text::XML::GetXMLTextLiteLen(UnsafeArray<const UTF8Char> text)
{
	UOSInt cnt = 0;
	UnsafeArray<const UTF8Char> sptr = text;
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
	const WChar *wptr = text;
	WChar c;
	while ((c = *wptr++) != 0)
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

UOSInt Text::XML::GetHTMLBodyTextLen(UnsafeArray<const UTF8Char> text)
{
	UOSInt cnt = 0;
	UnsafeArray<const UTF8Char> sptr = text;
	UTF8Char c;
	while (true)
	{
		switch ((c = *sptr++))
		{
		case 0:
			return cnt;
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
			cnt += 5;
			break;
		case '"':
			cnt += 6;
			break;
		case '\r':
			break;
		case '\n':
			cnt += 5;
			break;
		case '\t':
			cnt += 24;
			break;
		default:
			cnt++;
			break;
		}
	}
}

UOSInt Text::XML::GetHTMLElementTextLen(UnsafeArray<const UTF8Char> text)
{
	UOSInt cnt = 0;
	UnsafeArray<const UTF8Char> sptr = text;
	UTF8Char c;
	while (true)
	{
		switch ((c = *sptr++))
		{
		case 0:
			return cnt;
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
			cnt += 5;
			break;
		case '"':
			cnt += 6;
			break;
		case '\t':
			cnt += 24;
			break;
		default:
			cnt++;
			break;
		}
	}
}

UOSInt Text::XML::GetHTMLBodyTextLen(const WChar *text)
{
	UOSInt cnt = 0;
	const WChar *wptr = text;
	WChar c;
	while ((c = *wptr++) != 0)
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
			cnt += 5;
			break;
		case '"':
			cnt += 6;
			break;
		case '\r':
			break;
		case '\n':
			cnt += 5;
			break;
		case '\t':
			cnt += 24;
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

UnsafeArray<UTF8Char> Text::XML::ToXMLText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text)
{
	UnsafeArray<UTF8Char> dptr = buff;
	UnsafeArray<const UTF8Char> sptr = text;
	UTF8Char c;
	while (true)
	{
		switch ((c = *sptr++))
		{
		case 0:
			*dptr = 0;
			return dptr;
		case '&':
			WriteNUInt32(&dptr[0], ReadNUInt32((const UInt8*)"&amp"));
			dptr[4] = ';';
			dptr += 5;
			break;
		case '<':
			WriteNUInt32(&dptr[0], ReadNUInt32((const UInt8*)"&lt;"));
			dptr += 4;
			break;
		case '>':
			WriteNUInt32(&dptr[0], ReadNUInt32((const UInt8*)"&gt;"));
			dptr += 4;
			break;
		case '\'':
			WriteNUInt32(&dptr[0], ReadNUInt32((const UInt8*)"&apo"));
			WriteNUInt16(&dptr[4], ReadNUInt16((const UInt8*)"s;"));
			dptr += 6;
			break;
		case '"':
			WriteNUInt32(&dptr[0], ReadNUInt32((const UInt8*)"&quo"));
			WriteNUInt16(&dptr[4], ReadNUInt16((const UInt8*)"t;"));
			dptr += 6;
			break;
		case '\n':
			WriteNUInt32(&dptr[0], ReadNUInt32((const UInt8*)"&#10"));
			dptr[4] = ';';
			dptr += 5;
			break;
		default:
			*dptr++ = c;
			break;
		}
	}
}

WChar *Text::XML::ToXMLText(WChar *buff, const WChar *text)
{
	WChar *dptr = buff;
	const WChar *wptr = text;
	WChar c;
	while ((c = *wptr++) != 0)
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

UnsafeArray<UTF8Char> Text::XML::ToXMLTextLite(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text)
{
	UnsafeArray<UTF8Char> dptr = buff;
	UnsafeArray<const UTF8Char> sptr = text;
	UTF8Char c;
	while (true)
	{
		switch ((c = *sptr++))
		{
		case 0:
			*dptr = 0;
			return dptr;
		case '&':
			dptr[0] = '&';
			dptr[1] = 'a';
			dptr[2] = 'm';
			dptr[3] = 'p';
			dptr[4] = ';';
			dptr += 5;
			break;
		case '<':
			dptr[0] = '&';
			dptr[1] = 'l';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
			break;
		case '>':
			dptr[0] = '&';
			dptr[1] = 'g';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
			break;
		default:
			*dptr++ = c;
			break;
		}
	}
}

WChar *Text::XML::ToXMLTextLite(WChar *buff, const WChar *text)
{
	WChar *dptr = buff;
	const WChar *wptr = text;
	WChar c;
	while ((c = *wptr++) != 0)
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

UnsafeArray<UTF8Char> Text::XML::ToHTMLBodyText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text)
{
	UnsafeArray<UTF8Char> dptr = buff;
	UnsafeArray<const UTF8Char> sptr = text;
	UTF8Char c;
	while (true)
	{
		switch (c = *sptr++)
		{
		case 0:
			*dptr = 0;
			return dptr;
		case '&':
			dptr[0] = '&';
			dptr[1] = '#';
			dptr[2] = '3';
			dptr[3] = '8';
			dptr[4] = ';';
			dptr += 5;
			break;
		case '<':
			dptr[0] = '&';
			dptr[1] = 'l';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
			break;
		case '>':
			dptr[0] = '&';
			dptr[1] = 'g';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
			break;
		case '\'':
			dptr[0] = '&';
			dptr[1] = '#';
			dptr[2] = '3';
			dptr[3] = '9';
			dptr[4] = ';';
			dptr += 5;
			break;
		case '"':
			dptr[0] = '&';
			dptr[1] = 'q';
			dptr[2] = 'u';
			dptr[3] = 'o';
			dptr[4] = 't';
			dptr[5] = ';';
			dptr += 6;
			break;
		case '\r':
			break;
		case '\n':
			dptr[0] = '<';
			dptr[1] = 'b';
			dptr[2] = 'r';
			dptr[3] = '/';
			dptr[4] = '>';
			dptr += 5;
			break;
		case '\t':
			dptr = Text::StrConcatC(dptr, UTF8STRC("&nbsp;&nbsp;&nbsp;&nbsp;"));
			break;
		default:
			*dptr++ = c;
			break;
		}
	}
}

UnsafeArray<UTF8Char> Text::XML::ToHTMLElementText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text)
{
	UnsafeArray<UTF8Char> dptr = buff;
	UnsafeArray<const UTF8Char> sptr = text;
	UTF8Char c;
	while (true)
	{
		switch (c = *sptr++)
		{
		case 0:
			*dptr = 0;
			return dptr;
		case '&':
			dptr[0] = '&';
			dptr[1] = '#';
			dptr[2] = '3';
			dptr[3] = '8';
			dptr[4] = ';';
			dptr += 5;
			break;
		case '<':
			dptr[0] = '&';
			dptr[1] = 'l';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
			break;
		case '>':
			dptr[0] = '&';
			dptr[1] = 'g';
			dptr[2] = 't';
			dptr[3] = ';';
			dptr += 4;
			break;
		case '\'':
			dptr[0] = '&';
			dptr[1] = '#';
			dptr[2] = '3';
			dptr[3] = '9';
			dptr[4] = ';';
			dptr += 5;
			break;
		case '"':
			dptr[0] = '&';
			dptr[1] = 'q';
			dptr[2] = 'u';
			dptr[3] = 'o';
			dptr[4] = 't';
			dptr[5] = ';';
			dptr += 6;
			break;
		case '\t':
			dptr = Text::StrConcatC(dptr, UTF8STRC("&nbsp;&nbsp;&nbsp;&nbsp;"));
			break;
		default:
			*dptr++ = c;
			break;
		}
	}
}

WChar *Text::XML::ToHTMLBodyText(WChar *buff, const WChar *text)
{
	WChar *dptr = buff;
	const WChar *wptr = text;
	WChar c;
	while ((c = *wptr++) != 0)
	{
		if (c == '&')
		{
			dptr[0] = '&';
			dptr[1] = '#';
			dptr[2] = '3';
			dptr[3] = '8';
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
			dptr[1] = '#';
			dptr[2] = '3';
			dptr[3] = '9';
			dptr[4] = ';';
			dptr += 5;
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
		else if (c == '\t')
		{
			dptr = Text::StrConcat(dptr, L"&nbsp;&nbsp;&nbsp;&nbsp;");
		}
		else
		{
			*dptr++ = c;
		}
	}
	*dptr = 0;
	return dptr;
}

UnsafeArray<UTF8Char> Text::XML::ToAttrText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text)
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

NN<Text::String> Text::XML::ToNewXMLText(UnsafeArray<const UTF8Char> text)
{
	UOSInt cnt = GetXMLTextLen(text);
	NN<Text::String> s = Text::String::New(cnt);
	ToXMLText(s->v, text);
	return s;
}

const WChar *Text::XML::ToNewXMLText(const WChar *text)
{
	UOSInt cnt = GetXMLTextLen(text) + 1;
	WChar *dptr = MemAlloc(WChar, cnt);
	ToXMLText(dptr, text);
	return dptr;
}

NN<Text::String> Text::XML::ToNewXMLTextLite(UnsafeArray<const UTF8Char> text)
{
	UOSInt cnt = GetXMLTextLiteLen(text);
	NN<Text::String> s = Text::String::New(cnt);
	ToXMLTextLite(s->v, text);
	return s;
}

const WChar *Text::XML::ToNewXMLTextLite(const WChar *text)
{
	UOSInt cnt = GetXMLTextLiteLen(text) + 1;
	WChar *dptr = MemAlloc(WChar, cnt);
	ToXMLTextLite(dptr, text);
	return dptr;
}

NN<Text::String> Text::XML::ToNewHTMLBodyText(UnsafeArray<const UTF8Char> text)
{
	UOSInt cnt = GetHTMLBodyTextLen(text);
	NN<Text::String> s = Text::String::New(cnt);
	ToHTMLBodyText(s->v, text);
	return s;
}

NN<Text::String> Text::XML::ToNewHTMLElementText(UnsafeArray<const UTF8Char> text)
{
	UOSInt cnt = GetHTMLElementTextLen(text);
	NN<Text::String> s = Text::String::New(cnt);
	ToHTMLElementText(s->v, text);
	return s;
}

const WChar *Text::XML::ToNewHTMLBodyText(const WChar *text)
{
	UOSInt cnt = GetHTMLBodyTextLen(text) + 1;
	WChar *dptr = MemAlloc(WChar, cnt);
	ToHTMLBodyText(dptr, text);
	return dptr;
}

NN<Text::String> Text::XML::ToNewAttrText(UnsafeArrayOpt<const UTF8Char> text)
{
	NN<Text::String> s;
	UnsafeArray<const UTF8Char> nntext;
	UnsafeArray<UTF8Char> buff;
	if (!text.SetTo(nntext))
	{
		return Text::String::New(UTF8STRC("\"\""));
	}
	else
	{
		UOSInt cnt = GetXMLTextLen(nntext) + 2;
		s = Text::String::New(cnt);
		buff = s->v;
		*buff++ = '"';
		buff = ToXMLText(buff, nntext);
		*buff++ = '"';
		*buff = 0;
		return s;
	}
}

const WChar *Text::XML::ToNewAttrText(const WChar *text)
{
	WChar *dptr;
	WChar *buff;
	if (text == 0)
	{
		dptr = MemAlloc(WChar, 3);
		buff = dptr;
		*buff++ = '"';
		*buff++ = '"';
		*buff = 0;
		return dptr;
	}
	else
	{
		UOSInt cnt = GetXMLTextLen(text) + 3;
		dptr = MemAlloc(WChar, cnt);
		buff = dptr;
		*buff++ = '"';
		buff = ToXMLText(buff, text);
		*buff++ = '"';
		*buff = 0;
		return dptr;
	}
}

NN<Text::String> Text::XML::ToNewHTMLTextXMLColor(UnsafeArray<const UTF8Char> text)
{
	Text::StringBuilderUTF8 sb;
	Bool elementStarted = false;
	Bool beginAttr = false;
	Bool fontStarted = false;
	UTF8Char quoteChar = 0;
	UTF8Char c;
	while ((c = *text++) != 0)
	{
		if (quoteChar != 0 || !elementStarted)
		{
			if (c == '&')
			{
				sb.AppendC(UTF8STRC("&#38;"));
			}
			else if (c == '<')
			{
				if (quoteChar == 0)
				{
					elementStarted = true;
					fontStarted = true;
				}
				sb.AppendC(UTF8STRC("<font color=\"red\">&lt;"));
			}
			else if (c == '>')
			{
				sb.AppendC(UTF8STRC("&gt;"));
			}
			else if (c == '\'')
			{
				sb.AppendC(UTF8STRC("&#39;"));
			}
			else if (c == '"')
			{
				sb.AppendC(UTF8STRC("&quot;"));
			}
			else if (c == '\r')
			{
			}
			else if (c == '\n')
			{
				sb.AppendC(UTF8STRC("<br/>"));
			}
			else if (c == '\t')
			{
				sb.AppendC(UTF8STRC("&nbsp;&nbsp;&nbsp;&nbsp;"));
			}
			else
			{
				sb.AppendUTF8Char(c);
			}
			if (c == quoteChar)
			{
				quoteChar = 0;
			}
		}
		else
		{
			if (c == '=' && beginAttr)
			{
				if (!fontStarted)
				{
					sb.AppendC(UTF8STRC("<font color=\"blue\">"));
					fontStarted = true;
				}
				sb.AppendC(UTF8STRC("=</font><font color=\"green\">"));
			}
			else if (c == ' ')
			{
				if (fontStarted)
				{
					sb.AppendC(UTF8STRC(" </font>"));
					fontStarted = false;
				}
				else
				{
					sb.AppendUTF8Char(' ');
				}
				beginAttr = true;
			}
			else if (c == '>')
			{
				if (fontStarted)
				{
					sb.AppendC(UTF8STRC("</font>"));
				}
				sb.AppendC(UTF8STRC("<font color=\"red\">&gt;</font>"));
				beginAttr = false;
				fontStarted = false;
				elementStarted = false;
			}
			else if (c == '\'')
			{
				sb.AppendC(UTF8STRC("&#39;"));
				quoteChar = '\'';
			}
			else if (c == '"')
			{
				sb.AppendC(UTF8STRC("&quot;"));
				quoteChar = '\"';
			}
			else if (c == '/')
			{
				if (*text == '>')
				{
					if (fontStarted)
					{
						sb.AppendC(UTF8STRC("</font>"));
						fontStarted = false;
					}
					sb.AppendC(UTF8STRC("<font color=\"red\">/&gt;</font>"));
					text++;
					elementStarted = false;
				}
				else
				{
					sb.AppendUTF8Char(c);
				}
			}
			else if (c == '\r')
			{
			}
			else if (c == '\n')
			{
				sb.AppendC(UTF8STRC("<br/>"));
			}
			else if (c == '\t')
			{
				sb.AppendC(UTF8STRC("&nbsp;&nbsp;&nbsp;&nbsp;"));
			}
			else if (c == '&')
			{
				sb.AppendC(UTF8STRC("&#38;"));
			}
			else
			{
				if (!fontStarted)
				{
					fontStarted = true;
					sb.AppendC(UTF8STRC("<font color=\"blue\">"));
				}
				sb.AppendUTF8Char(c);
			}
		}
	}
	if (fontStarted)
	{
		sb.AppendC(UTF8STRC("</font>"));
	}
	return Text::String::New(sb.ToCString());
}

void Text::XML::FreeNewText(const WChar *text)
{
	MemFree((void*)text);
}

void Text::XML::ParseStr(Text::String *out, UnsafeArray<const UTF8Char> xmlStart, UnsafeArray<const UTF8Char> xmlEnd)
{
	UnsafeArray<UTF8Char> currPtr = out->v;
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
				UnsafeArray<const UTF8Char> tmp = xmlStart + 3;
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
				UnsafeArray<const UTF8Char> tmp = xmlStart + 2;
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
	out->leng = (UOSInt)(currPtr - out->v);
}

void Text::XML::ParseStr(UnsafeArray<UTF8Char> out, UnsafeArray<const UTF8Char> xmlStart, UnsafeArray<const UTF8Char> xmlEnd)
{
	UnsafeArray<UTF8Char> currPtr = out;
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
				UnsafeArray<const UTF8Char> tmp = xmlStart + 3;
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
				UnsafeArray<const UTF8Char> tmp = xmlStart + 2;
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

Bool Text::XML::HTMLAppendCharRef(UnsafeArray<const UTF8Char> chrRef, UOSInt refSize, IO::Stream *stm)
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
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&lt;")))
		{
			stm->Write((const UInt8*)"<", 1);
			return true;
		}
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&gt;")))
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
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&amp;")))
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
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&apos;")))
		{
			stm->Write((const UInt8*)"\'", 1);
			return true;
		}
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&bull;")))
		{
			return WriteUTF8Char(stm, 0x2022);
		}
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&quot;")))
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
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&raquo;")))
		{
			return WriteUTF8Char(stm, 0xbb);
		}
	}
	return false;
}

Bool Text::XML::HTMLAppendCharRef(UnsafeArray<const UTF8Char> chrRef, UOSInt refSize, NN<Text::StringBuilderUTF8> sb)
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
			sb->AppendUTF8Char((UInt8)Text::StrToUInt32(sbuff));
			return true;
		}
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&lt;")))
		{
			sb->AppendUTF8Char('<');
			return true;
		}
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&gt;")))
		{
			sb->AppendUTF8Char('>');
			return true;
		}
	}
	else if (refSize == 5)
	{
		if (chrRef[1] == '#')
		{
			if (chrRef[2] == 'x')
			{
				sb->AppendChar(Text::StrHex2UInt8C(&chrRef[3]), 1);
				return true;
			}
			else
			{
				sbuff[0] = chrRef[2];
				sbuff[1] = chrRef[3];
				sbuff[2] = 0;
				wcs = (UTF32Char)Text::StrToUInt32(sbuff);
				sb->AppendChar(wcs, 1);
				return true;
			}
		}
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&amp;")))
		{
			sb->AppendUTF8Char('&');
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
			sb->AppendChar(wcs, 1);
			return true;
		}
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&apos;")))
		{
			sb->AppendUTF8Char('\'');
			return true;
		}
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&bull;")))
		{
			sb->AppendChar(0x2022, 1);
			return true;
		}
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&quot;")))
		{
			sb->AppendUTF8Char('\"');
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
			sb->AppendChar(wcs, 1);
			return true;
		}
		else if (Text::StrStartsWithC(chrRef, refSize, UTF8STRC("&raquo;")))
		{
			sb->AppendChar(0xbb, 1);
			return true;
		}
	}
	return false;
}
