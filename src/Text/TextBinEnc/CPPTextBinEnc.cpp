#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/CPPTextBinEnc.h"
//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Text::TextBinEnc::CPPTextBinEnc::CPPTextBinEnc()
{
}

Text::TextBinEnc::CPPTextBinEnc::~CPPTextBinEnc()
{
}

UIntOS Text::TextBinEnc::CPPTextBinEnc::EncodeBin(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> dataBuff, UIntOS buffSize) const
{
	Bool lineStart = true;
	UInt32 code;
	UInt8 b;
	UIntOS ret = 0;
	while (buffSize-- > 0)
	{
		b = *dataBuff++;
		if (lineStart)
		{
			sb->AppendC(UTF8STRC("\""));
			lineStart = false;
			ret += 1;
		}
		if (b == 0)
		{
			sb->AppendC(UTF8STRC("\\0"));
			ret += 2;
		}
		else if (b == '\r')
		{
			sb->AppendC(UTF8STRC("\\r"));
			ret += 2;
		}
		else if (b == '\n')
		{
			sb->AppendC(UTF8STRC("\\n\"\r\n"));
			ret += 5;
			lineStart = true;
		}
		else if (b == '\\')
		{
			sb->AppendC(UTF8STRC("\\\\"));
			ret += 2;
		}
		else if (b == '\"')
		{
			sb->AppendC(UTF8STRC("\\\""));
			ret += 2;
		}
		else if (b < 0x80)
		{
			sb->AppendChar(b, 1);
			ret += 1;
		}
		else if ((b & 0xe0) == 0xc0)
		{
			sb->AppendChar((UTF32Char)(((b & 0x1f) << 6) | (*dataBuff & 0x3f)), 1);
			dataBuff++;
			buffSize--;
			ret += 1;
		}
		else if ((b & 0xf0) == 0xe0)
		{
			sb->AppendChar((UTF32Char)(((b & 0x0f) << 12) | ((dataBuff[0] & 0x3f) << 6) | (dataBuff[1] & 0x3f)), 1);
			dataBuff += 2;
			buffSize -= 2;
			ret += 1;
		}
		else if ((b & 0xf8) == 0xf0)
		{
			code = (UInt32)(((b & 0x7) << 18) | ((dataBuff[0] & 0x3f) << 12) | ((dataBuff[1] & 0x3f) << 6) | (dataBuff[2] & 0x3f));
			if (code >= 0x10000)
			{
				sb->AppendChar((UTF32Char)((code - 0x10000) >> 10) + 0xd800, 1);
				sb->AppendChar((UTF32Char)(code & 0x3ff) + 0xdc00, 1);
				ret += 2;
			}
			else
			{
				sb->AppendChar((UTF32Char)code, 1);
				ret += 1;
			}
			dataBuff += 3;
			buffSize -= 3;
		}
		else if ((b & 0xfc) == 0xf8)
		{
			code = (UInt32)(((b & 0x3) << 24) | ((dataBuff[0] & 0x3f) << 18) | ((dataBuff[1] & 0x3f) << 12) | ((dataBuff[2] & 0x3f) << 6) | (dataBuff[3] & 0x3f));
			if (code >= 0x10000)
			{
				sb->AppendChar((UTF32Char)((code - 0x10000) >> 10) + 0xd800, 1);
				sb->AppendChar((UTF32Char)(code & 0x3ff) + 0xdc00, 1);
				ret += 2;
			}
			else
			{
				sb->AppendChar((UTF32Char)code, 1);
				ret += 1;
			}
			dataBuff += 4;
			buffSize -= 4;
		}
		else if ((b & 0xfe) == 0xfc)
		{
			code = (UInt32)(((b & 0x1) << 30) | ((dataBuff[0] & 0x3f) << 24) | ((dataBuff[1] & 0x3f) << 18) | ((dataBuff[2] & 0x3f) << 12) | ((dataBuff[3] & 0x3f) << 6) | (dataBuff[4] & 0x3f));
			if (code >= 0x10000)
			{
				sb->AppendChar((UTF32Char)((code - 0x10000) >> 10) + 0xd800, 1);
				sb->AppendChar((UTF32Char)(code & 0x3ff) + 0xdc00, 1);
				ret += 2;
			}
			else
			{
				sb->AppendChar((UTF32Char)code, 1);
				ret += 1;
			}
			dataBuff += 5;
			buffSize -= 5;
		}
	}
	if (!lineStart)
	{
		sb->AppendUTF8Char('\"');
	}
	return ret;
}

UIntOS Text::TextBinEnc::CPPTextBinEnc::CalcBinSize(Text::CStringNN s) const
{
	Bool isQuote = false;
	UIntOS ret = 0;
	UTF8Char c;
	UnsafeArray<const UTF8Char> str = s.v;
	while (true)
	{
		c = *str++;
		if (c == 0)
		{
			if (isQuote)
			{
#if defined(VERBOSE)
				printf("End quote not found\r\n");
#endif
				return 0;
			}
			break;
		}
		else if (!isQuote)
		{
			if (c == '"')
			{
				isQuote = true;
			}
			else if (c == 0x20 || c == '\t' || c == 13 || c == 10)
			{
			}
			else
			{
#if defined(VERBOSE)
				printf("Unknown character: %c\r\n", c);
#endif
				return 0;
			}
		}
		else if (c == '\\')
		{
			c = *str++;
			if (c == 0)
			{
#if defined(VERBOSE)
				printf("End with \\\r\n");
#endif
				return 0;
			}
			else if (c == 'r')
			{
				ret++;
			}
			else if (c == 'n')
			{
				ret++;
			}
			else if (c == 't')
			{
				ret++;
			}
			else if (c == '\\')
			{
				ret++;
			}
			else if (c == '"')
			{
				ret++;
			}
			else if (c == '0')
			{
				ret++;
			}
			else
			{
#if defined(VERBOSE)
				printf("Unknown escape sequence: \\%c\r\n", c);
#endif
				return 0;
			}
		}
		else if (c == '"')
		{
			isQuote = false;
		}
		else
		{
			ret++;
		}
	}
	return ret;
}

UIntOS Text::TextBinEnc::CPPTextBinEnc::DecodeBin(Text::CStringNN s, UnsafeArray<UInt8> dataBuff) const
{
	Bool isQuote = false;
	UIntOS ret = 0;
	UTF8Char c;
	UnsafeArray<const UTF8Char> str = s.v;
	while (true)
	{
		c = *str++;
		if (c == 0)
		{
			if (isQuote)
			{
#if defined(VERBOSE)
				printf("End quote not found\r\n");
#endif
				return 0;
			}
			break;
		}
		else if (!isQuote)
		{
			if (c == '"')
			{
				isQuote = true;
			}
			else if (c == 0x20 || c == '\t' || c == 13 || c == 10)
			{
			}
			else
			{
#if defined(VERBOSE)
				printf("Unknown character: %c\r\n", c);
#endif
				return 0;
			}
		}
		else if (c == '\\')
		{
			c = *str++;
			if (c == 0)
			{
#if defined(VERBOSE)
				printf("End with \\\r\n");
#endif
				return 0;
			}
			else if (c == 'r')
			{
				*dataBuff++ = '\r';
				ret++;
			}
			else if (c == 'n')
			{
				*dataBuff++ = '\n';
				ret++;
			}
			else if (c == 't')
			{
				*dataBuff++ = '\t';
				ret++;
			}
			else if (c == '\\')
			{
				*dataBuff++ = '\\';
				ret++;
			}
			else if (c == '0')
			{
				*dataBuff++ = '\0';
				ret++;
			}
			else if (c == '"')
			{
				*dataBuff++ = '\"';
				ret++;
			}
			else
			{
#if defined(VERBOSE)
				printf("Unknown escape sequence: \\%c\r\n", c);
#endif
				return 0;
			}
		}
		else if (c == '"')
		{
			isQuote = false;
		}
		else
		{
			*dataBuff++ = (UInt8)c;
			ret++;
		}
	}
	return ret;
}

Text::CStringNN Text::TextBinEnc::CPPTextBinEnc::GetName() const
{
	return CSTR("CPP String");
}
