#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Text/JSText.h"
#include "Text/MyStringW.h"

UnsafeArray<UTF8Char> Text::JSText::ToJSText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> s)
{
	UTF8Char c;
	*buff++ = '\'';
	while ((c = *s++) != 0)
	{
		switch (c)
		{
		case '\\':
			*buff++ = '\\';
			*buff++ = '\\';
			break;
		case '\'':
			*buff++ = '\\';
			*buff++ = '\'';
			break;
		case '\n':
			*buff++ = '\\';
			*buff++ = 'n';
			break;
		case '\r':
			*buff++ = '\\';
			*buff++ = 'n';
			break;
		case '\0':
			*buff++ = '\\';
			*buff++ = '0';
			break;
		default:
			if (c < 32)
			{
				*buff++ = '\\';
				*buff++ = 'u';
				buff = Text::StrHexVal16(buff, c);
			}
			else
			{
				*buff++ = c;
			}
			break;
		}
	}
	*buff++ = '\'';
	*buff = 0;
	return buff;
}

UnsafeArray<UTF8Char> Text::JSText::ToJSTextDQuote(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> s)
{
	UTF8Char c;
	*buff++ = '\"';
	while ((c = *s++) != 0)
	{
		switch (c)
		{
		case '\"':
			*buff++ = '\\';
			*buff++ = '\"';
			break;
		case '\n':
			*buff++ = '\\';
			*buff++ = 'n';
			break;
		case '\r':
			*buff++ = '\\';
			*buff++ = 'n';
			break;
		case '\\':
			*buff++ = '\\';
			*buff++ = '\\';
			break;
		case '\0':
			*buff++ = '\\';
			*buff++ = '0';
			break;
		default:
			if (c < 32)
			{
				*buff++ = '\\';
				*buff++ = 'u';
				buff = Text::StrHexVal16(buff, c);
			}
			else
			{
				*buff++ = c;
			}
			break;
		}
	}
	*buff++ = '\"';
	*buff = 0;
	return buff;
}

void Text::JSText::ToJSTextDQuote(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> s)
{
	UTF8Char c;
	UTF8Char buff[256];
	UnsafeArray<UTF8Char> sptr;
	sb->AppendUTF8Char('\"');
	sptr = buff;
	while ((c = *s++) != 0)
	{
		switch (c)
		{
		case '\"':
			*sptr++ = '\\';
			*sptr++ = '\"';
			break;
		case '\n':
			*sptr++ = '\\';
			*sptr++ = 'n';
			break;
		case '\r':
			*sptr++ = '\\';
			*sptr++ = 'n';
			break;
		case '\\':
			*sptr++ = '\\';
			*sptr++ = '\\';
			break;
		case '\0':
			sptr[0] = '\\';
			sptr[1] = '0';
			sptr += 2;
			break;
		default:
			if (c < 32)
			{
				sptr[0] = '\\';
				sptr[1] = 'u';
				sptr = Text::StrHexVal16(sptr + 2, c);
			}
			else
			{
				*sptr++ = c;
			}
			break;
		}
		if (sptr - buff >= 250)
		{
			sb->AppendC(buff, (UOSInt)(sptr - buff));
			sptr = buff;
		}
	}
	if (sptr > buff)
	{
		sb->AppendC(buff, (UOSInt)(sptr - buff));
	}
	sb->AppendUTF8Char('\"');
}

UnsafeArray<WChar> Text::JSText::ToJSTextW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> s)
{
	WChar c;
	*buff++ = '\'';
	while ((c = *s++) != 0)
	{
		switch (c)
		{
		case '\'':
			*buff++ = '\\';
			*buff++ = '\'';
			break;
		case '\n':
			*buff++ = '\\';
			*buff++ = 'n';
			break;
		case '\r':
			*buff++ = '\\';
			*buff++ = 'n';
			break;
		default:
			*buff++ = c;
			break;
		}
	}
	*buff++ = '\'';
	*buff = 0;
	return buff;
}

UnsafeArray<WChar> Text::JSText::ToJSTextDQuoteW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> s)
{
	WChar c;
	*buff++ = '\"';
	while ((c = *s++) != 0)
	{
		switch (c)
		{
		case '\"':
			*buff++ = '\\';
			*buff++ = '\"';
			break;
		case '\n':
			*buff++ = '\\';
			*buff++ = 'n';
			break;
		case '\r':
			*buff++ = '\\';
			*buff++ = 'r';
			break;
		default:
			*buff++ = c;
			break;
		}
	}
	*buff++ = '\"';
	*buff = 0;
	return buff;
}

NN<Text::String> Text::JSText::ToNewJSText(Optional<Text::String> s)
{
	return ToNewJSText(OPTSTR_CSTR(s).v);
}

NN<Text::String> Text::JSText::ToNewJSText(NN<Text::String> s)
{
	return ToNewJSText(UnsafeArray<const UTF8Char>(s->v));
}

NN<Text::String> Text::JSText::ToNewJSText(UnsafeArrayOpt<const UTF8Char> s)
{
	UnsafeArray<const UTF8Char> srcPtr;
	if (!s.SetTo(srcPtr))
	{
		return Text::String::New(UTF8STRC("null"));
	}
	return ToNewJSText(srcPtr);
}
NN<Text::String> Text::JSText::ToNewJSText(UnsafeArray<const UTF8Char> srcPtr)
{
	UnsafeArray<const UTF8Char> nns = srcPtr;
	UTF8Char c;
	UOSInt chCnt;
	chCnt = 2;
	while ((c = *srcPtr++) != 0)
	{
		switch(c)
		{
		case '\\':
		case '\'':
		case '\n':
		case '\r':
		case '\0':
			chCnt += 2;
			break;
		default:
			if (c < 32)
			{
				chCnt += 6;
			}
			else
			{
				chCnt++;
			}
			break;
		}
	}
	NN<Text::String> retS = Text::String::New(chCnt);
	ToJSText(retS->v, nns);
	return retS;
}

NN<Text::String> Text::JSText::ToNewJSTextDQuote(UnsafeArrayOpt<const UTF8Char> s)
{
	UnsafeArray<const UTF8Char> nns;
	if (!s.SetTo(nns))
	{
		return Text::String::New(UTF8STRC("null"));
	}
	return ToNewJSTextDQuote(nns);
}

NN<Text::String> Text::JSText::ToNewJSTextDQuote(UnsafeArray<const UTF8Char> s)
{
	UnsafeArray<const UTF8Char> srcPtr;
	UTF8Char c;
	UOSInt chCnt;
	srcPtr = s;
	chCnt = 2;
	while ((c = *srcPtr++) != 0)
	{
		switch(c)
		{
		case '\"':
		case '\n':
		case '\r':
		case '\\':
		case '\0':
			chCnt += 2;
			break;
		default:
			if (c < 32)
			{
				chCnt += 6;
			}
			else
			{
				chCnt++;
			}
			break;
		}
	}
	NN<Text::String> retS = Text::String::New(chCnt);
	ToJSTextDQuote(retS->v, s);
	return retS;
}

UnsafeArray<const WChar> Text::JSText::ToNewJSTextW(UnsafeArrayOpt<const WChar> s)
{
	UnsafeArray<WChar> destStr;
	UnsafeArray<const WChar> srcPtr;
	UnsafeArray<const WChar> nns;
	if (!s.SetTo(srcPtr))
	{
		destStr = MemAlloc(WChar, 5);
		Text::StrConcat(destStr, L"null");
		return destStr;
	}
	nns = srcPtr;
	WChar c;
	UOSInt chCnt;
	chCnt = 3;
	while ((c = *srcPtr++) != 0)
	{
		switch(c)
		{
		case '\'':
		case '\n':
		case '\r':
			chCnt += 2;
			break;
		default:
			chCnt++;
			break;
		}
	}
	destStr = MemAlloc(WChar, chCnt);
	ToJSTextW(destStr, nns);
	return destStr;
}

UnsafeArray<const WChar> Text::JSText::ToNewJSTextDQuoteW(UnsafeArrayOpt<const WChar> s)
{
	UnsafeArray<WChar> destStr;
	UnsafeArray<const WChar> nns;
	if (!s.SetTo(nns))
	{
		destStr = MemAllocArr(WChar, 5);
		Text::StrConcat(destStr, L"null");
		return destStr;
	}
	UnsafeArray<const WChar> srcPtr;
	WChar c;
	UOSInt chCnt;

	srcPtr = nns;
	chCnt = 3;
	while ((c = *srcPtr++) != 0)
	{
		switch(c)
		{
		case '\'':
		case '\n':
		case '\r':
			chCnt += 2;
			break;
		default:
			chCnt++;
			break;
		}
	}
	destStr = MemAllocArr(WChar, chCnt);
	ToJSTextDQuoteW(destStr, nns);
	return destStr;
}


Optional<Text::String> Text::JSText::FromNewJSText(UnsafeArray<const UTF8Char> s)
{
	UnsafeArray<const UTF8Char> srcPtr;
	UnsafeArray<UTF8Char> destPtr;
	NN<Text::String> outS;
	UOSInt chCnt;
	UTF8Char c;
	UTF8Char startC;
	UInt32 v;
	srcPtr = s;
	chCnt = 0;
	startC = *srcPtr++;
	if (startC != '\'' && startC != '"')
		return 0;
	while (true)
	{
		c = *srcPtr++;
		if (c == startC)
			break;
		if (c == 0)
			return 0;
		if (c == '\\')
		{
			c = *srcPtr++;
			switch (c)
			{
			case 'u':
				v = 0;
				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (UInt32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (UInt32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (UInt32)(c - 0x57);
				}
				else
				{
					return 0;
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (UInt32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (UInt32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (UInt32)(c - 0x57);
				}
				else
				{
					return 0;
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (UInt32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (UInt32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (UInt32)(c - 0x57);
				}
				else
				{
					return 0;
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (UInt32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (UInt32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (UInt32)(c - 0x57);
				}
				else
				{
					return 0;
				}

				if (v < 0x80)
				{
					chCnt++;
				}
				else if (v < 0x800)
				{
					chCnt += 2;
				}
				else
				{
					chCnt += 3;
				}
				break;
			case '\"':
			case '\\':
			case '/':
			case 'b':
			case 'r':
			case 'n':
			case 'f':
			case 't':
				chCnt++;
				break;
			default:
				return 0;
			}
		}
		else
		{
			chCnt++;
		}
	}
	if (*srcPtr != 0)
		return 0;

	outS = Text::String::New(chCnt);
	destPtr = outS->v;
	srcPtr = s;
	srcPtr++;
	while (true)
	{
		c = *srcPtr++;
		if (c == startC)
			break;
		if (c == '\\')
		{
			c = *srcPtr++;
			switch (c)
			{
			case 'u':
				v = 0;
				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (UInt32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (UInt32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (UInt32)(c - 0x57);
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (UInt32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (UInt32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (UInt32)(c - 0x57);
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (UInt32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (UInt32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (UInt32)(c - 0x57);
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (UInt32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (UInt32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (UInt32)(c - 0x57);
				}
				if (v < 0x80)
				{
					*destPtr++ = (UInt8)v;
				}
				else if (v < 0x800)
				{
					*destPtr++ = (UTF8Char)(0xc0 | (v >> 6));
					*destPtr++ = (UTF8Char)(0x80 | (v & 0x3f));
				}
				else
				{
					*destPtr++ = (UTF8Char)(0xe0 | (v >> 12));
					*destPtr++ = (UTF8Char)(0x80 | ((v >> 6) & 0x3f));
					*destPtr++ =(UTF8Char)( 0x80 | (v & 0x3f));
				}
				break;
			case '\"':
				*destPtr++ = '\"';
				break;
			case '\\':
				*destPtr++ = '\\';
				break;
			case '/':
				*destPtr++ = '/';
				break;
			case 'b':
				*destPtr++ = '\b';
				break;
			case 'r':
				*destPtr++ = '\r';
				break;
			case 'n':
				*destPtr++ = '\n';
				break;
			case 'f':
				*destPtr++ = '\f';
				break;
			case 't':
				*destPtr++ = '\t';
				break;
			default:
				return 0;
			}
		}
		else
		{
			*destPtr++ = c;
		}
	}
	*destPtr = 0;
	return outS;
}

UnsafeArrayOpt<const WChar> Text::JSText::FromNewJSTextW(UnsafeArray<const WChar> s)
{
	UnsafeArray<const WChar> srcPtr;
	UnsafeArray<WChar> destPtr;
	UnsafeArray<WChar> outStr;
	UOSInt chCnt;
	WChar c;
	WChar startC;
	Int32 v;
	srcPtr = s;
	chCnt = 0;
	startC = *srcPtr++;
	if (startC != '\'' && startC != '"')
		return 0;
	while (true)
	{
		c = *srcPtr++;
		if (c == startC)
			break;
		if (c == 0)
			return 0;
		if (c == '\\')
		{
			c = *srcPtr++;
			switch (c)
			{
			case 'u':
				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
				}
				else if (c >= 'A' && c <= 'F')
				{
				}
				else if (c >= 'a' && c <= 'f')
				{
				}
				else
				{
					return 0;
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
				}
				else if (c >= 'A' && c <= 'F')
				{
				}
				else if (c >= 'a' && c <= 'f')
				{
				}
				else
				{
					return 0;
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
				}
				else if (c >= 'A' && c <= 'F')
				{
				}
				else if (c >= 'a' && c <= 'f')
				{
				}
				else
				{
					return 0;
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
				}
				else if (c >= 'A' && c <= 'F')
				{
				}
				else if (c >= 'a' && c <= 'f')
				{
				}
				else
				{
					return 0;
				}
				break;
			case '\"':
			case '\\':
			case '/':
			case 'b':
			case 'r':
			case 'n':
			case 'f':
			case 't':
				chCnt++;
				break;
			default:
				return 0;
			}
		}
		else
		{
			chCnt++;
		}
	}
	if (*srcPtr != 0)
		return 0;

	outStr = MemAllocArr(WChar, chCnt + 1);
	destPtr = outStr;
	srcPtr = s;
	srcPtr++;
	while (true)
	{
		c = *srcPtr++;
		if (c == startC)
			break;
		if (c == '\\')
		{
			c = *srcPtr++;
			switch (c)
			{
			case 'u':
				v = 0;
				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (Int32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (Int32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (Int32)(c - 0x57);
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (Int32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (Int32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (Int32)(c - 0x57);
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (Int32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (Int32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (Int32)(c - 0x57);
				}

				c = *srcPtr++;
				if (c >= '0' && c <= '9')
				{
					v = (v << 4) | (Int32)(c - 0x30);
				}
				else if (c >= 'A' && c <= 'F')
				{
					v = (v << 4) | (Int32)(c - 0x37);
				}
				else if (c >= 'a' && c <= 'f')
				{
					v = (v << 4) | (Int32)(c - 0x57);
				}
				*destPtr++ = (WChar)v;
				break;
			case '\"':
				*destPtr++ = '\"';
				break;
			case '\\':
				*destPtr++ = '\\';
				break;
			case '/':
				*destPtr++ = '/';
				break;
			case 'b':
				*destPtr++ = '\b';
				break;
			case 'r':
				*destPtr++ = '\r';
				break;
			case 'n':
				*destPtr++ = '\n';
				break;
			case 'f':
				*destPtr++ = '\f';
				break;
			case 't':
				*destPtr++ = '\t';
				break;
			default:
				return 0;
			}
		}
		else
		{
			*destPtr++ = c;
		}
	}
	*destPtr = 0;
	return UnsafeArray<const WChar>(outStr);
}

void Text::JSText::FreeNewText(UnsafeArray<const WChar> s)
{
	MemFreeArr(s);
}

UnsafeArray<UTF8Char> Text::JSText::JSDouble(UnsafeArray<UTF8Char> buff, Double val)
{
	if (Math::IsNAN(val))
		return Text::StrConcatC(buff, UTF8STRC("null"));
	else
		return Text::StrDouble(buff, val);
}

Bool Text::JSText::JSONWellFormat(UnsafeArray<const UTF8Char> buff, UOSInt buffSize, UOSInt initLev, NN<Text::StringBuilderUTF8> sb)
{
	UOSInt lev = initLev;
	UOSInt i = 0;
	UOSInt startOfst = 0;
	OSInt endOfst = -1;
	UTF8Char c;
	while (i < buffSize)
	{
		c = buff[i];
		if (c == '{' || c == '[')
		{
			if (lev > 0)
				sb->AppendChar('\t', lev);
			sb->AppendC(&buff[startOfst], i + 1 - startOfst);
			sb->AppendC(UTF8STRC("\r\n"));
			lev++;
			startOfst = i + 1;
			endOfst = -1;
		}
		else if (c == ']' || c == '}')
		{
			if (startOfst < i)
			{
				if (lev > 0)
					sb->AppendChar('\t', lev);
				if (endOfst != -1)
				{
					sb->AppendC(&buff[startOfst], (UOSInt)endOfst + 1 - startOfst);
				}
				else
				{
					sb->AppendC(&buff[startOfst], i - startOfst);
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			lev--;
			startOfst = i;
			endOfst = (OSInt)i;
		}
		else if (c == ',')
		{
			if (lev > 0)
				sb->AppendChar('\t', lev);
			sb->AppendC(&buff[startOfst], i + 1 - startOfst);
			sb->AppendC(UTF8STRC("\r\n"));
			startOfst = i + 1;
			endOfst = -1;
		}
		else if (c == '"')
		{
			i++;
			while (i < buffSize)
			{
				c = buff[i];
				if (c == '\\')
				{
					i++;
					if (i >= buffSize)
						break;
				}
				if (c == '"')
					break;
				i++;
			}
			endOfst = (OSInt)i;
		}
		else if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			if (i == startOfst)
			{
				startOfst++;
			}
		}
		else
		{
			endOfst = (OSInt)i;
		}
		
		i++;
	}

	if (startOfst < buffSize)
	{
		if (lev > 0)
			sb->AppendChar('\t', lev);
		sb->AppendC(&buff[startOfst], buffSize - startOfst);
	}
	return lev == initLev;
}

Bool Text::JSText::JSWellFormat(UnsafeArray<const UTF8Char> buff, UOSInt buffSize, UOSInt initLev, NN<Text::StringBuilderUTF8> sb)
{
	Data::ArrayList<Int32> lastType;
	UOSInt lev = initLev;
	UOSInt i = 0;
	UOSInt startOfst = 0;
	OSInt endOfst = -1;
	UTF8Char c;
	UTF8Char lastC = 0;
	OSInt jsonLev = -1;
	lastType.Add(0);
	while (i < buffSize)
	{
		c = buff[i];
		if (c == '{')
		{
			if ((lastC == '=' || lastC == '(') && jsonLev == -1)
			{
				jsonLev = (OSInt)lev;
			}
			if (lev > 0)
				sb->AppendChar('\t', lev);
			sb->AppendC(&buff[startOfst], i + 1 - startOfst);
			sb->AppendC(UTF8STRC("\r\n"));
			lev++;
			startOfst = i + 1;
			endOfst = -1;
			lastC = c;
		}
		else if (c == '}')
		{
			if (startOfst < i)
			{
				if (lev > 0)
					sb->AppendChar('\t', lev);
				if (endOfst != -1)
				{
					sb->AppendC(&buff[startOfst], (UOSInt)endOfst + 1 - startOfst);
				}
				else
				{
					sb->AppendC(&buff[startOfst], i - startOfst);
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			lev--;
			startOfst = i;
			endOfst = (OSInt)i;
			if ((OSInt)lev <= jsonLev)
			{
				jsonLev = -1;
			}
			lastC = c;
		}
		else if (c == '[')
		{
			if (jsonLev >= 0)
			{
				if (lev > 0)
					sb->AppendChar('\t', lev);
				sb->AppendC(&buff[startOfst], i + 1 - startOfst);
				sb->AppendC(UTF8STRC("\r\n"));
				lev++;
				startOfst = i + 1;
				endOfst = -1;
			}
			else
			{
				endOfst = (OSInt)i;
			}
			lastC = c;
		}
		else if (c == ']')
		{
			if (jsonLev >= 0)
			{
				if (startOfst < i)
				{
					if (lev > 0)
						sb->AppendChar('\t', lev);
					if (endOfst != -1)
					{
						sb->AppendC(&buff[startOfst], (UOSInt)endOfst + 1 - startOfst);
					}
					else
					{
						sb->AppendC(&buff[startOfst], i - startOfst);
					}
					sb->AppendC(UTF8STRC("\r\n"));
				}
				lev--;
				startOfst = i;
				endOfst = (OSInt)i;
			}
			else
			{
				endOfst = (OSInt)i;
			}
			lastC = c;
		}
		else if (c == ',')
		{
			if (jsonLev >= 0)
			{
				if (lev > 0)
					sb->AppendChar('\t', lev);
				sb->AppendC(&buff[startOfst], i + 1 - startOfst);
				sb->AppendC(UTF8STRC("\r\n"));
				startOfst = i + 1;
				endOfst = -1;
			}
			else
			{
				endOfst = (OSInt)i;
			}
			lastC = c;
		}
		else if (c == ';')
		{
			if (lev > 0)
				sb->AppendChar('\t', lev);
			sb->AppendC(&buff[startOfst], i + 1 - startOfst);
			sb->AppendC(UTF8STRC("\r\n"));
			startOfst = i + 1;
			endOfst = -1;
			lastC = c;
		}
		else if (c == '"')
		{
			i++;
			while (i < buffSize)
			{
				c = buff[i];
				if (c == '\\')
				{
					i++;
				}
				else if (c == '"')
				{
					break;
				}
				i++;
			}
			endOfst = (OSInt)i;
			lastC = c;
		}
		else if (c == '\'')
		{
			i++;
			while (i < buffSize)
			{
				c = buff[i];
				if (c == '\\')
				{
					i++;
				}
				else if (c == '\'')
				{
					break;
				}
				i++;
			}
			endOfst = (OSInt)i;
			lastC = c;
		}
		else if (c == '\r' || c == '\n')
		{
			if (i == startOfst)
			{
				startOfst++;
			}
			else
			{
				if (lev > 0)
					sb->AppendChar('\t', lev);
				sb->AppendC(&buff[startOfst], i + 1 - startOfst);
				sb->AppendC(UTF8STRC("\r\n"));
				startOfst = i + 1;
				endOfst = -1;
				lastC = c;
			}
		}
		else if (c == ' ' || c == '\t')
		{
			if (i == startOfst)
			{
				startOfst++;
			}
		}
		else
		{
			endOfst = (OSInt)i;
			if (c == '=' || c == '(' || c == ')')
			{
				lastC = c;
			}
		}
		
		i++;
	}

	if (startOfst < buffSize)
	{
		if (lev > 0)
			sb->AppendChar('\t', lev);
		sb->AppendC(&buff[startOfst], buffSize - startOfst);
	}
	return lev == initLev;
}
