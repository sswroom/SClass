#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/CPPTextBinEnc.h"

Text::TextBinEnc::CPPTextBinEnc::CPPTextBinEnc()
{
}

Text::TextBinEnc::CPPTextBinEnc::~CPPTextBinEnc()
{
}

UOSInt Text::TextBinEnc::CPPTextBinEnc::EncodeBin(Text::StringBuilderUTF *sb, const UInt8 *dataBuff, UOSInt buffSize)
{
	Bool lineStart = true;
	UInt32 code;
	UInt8 b;
	UOSInt ret = 0;
	while (buffSize-- > 0)
	{
		b = *dataBuff++;
		if (lineStart)
		{
			sb->Append((const UTF8Char*)"\"");
			lineStart = false;
			ret += 1;
		}
		if (b == 0)
		{
			sb->Append((const UTF8Char*)"\\0");
			ret += 2;
		}
		else if (b == '\r')
		{
			sb->Append((const UTF8Char*)"\\r");
			ret += 2;
		}
		else if (b == '\n')
		{
			sb->Append((const UTF8Char*)"\\n\"\r\n");
			ret += 5;
			lineStart = true;
		}
		else if (b == '\\')
		{
			sb->Append((const UTF8Char*)"\\\\");
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
		sb->AppendChar('\"', 1);
	}
	return ret;
}

UOSInt Text::TextBinEnc::CPPTextBinEnc::CalcBinSize(const UTF8Char *sbuff)
{
	Bool isQuote = false;
	UOSInt ret = 0;
	UTF8Char c;
	while (true)
	{
		c = *sbuff++;
		if (c == 0)
		{
			if (isQuote)
				return 0;
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
				return 0;
			}
		}
		else if (c == '\\')
		{
			c = *sbuff++;
			if (c == 0)
			{
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
			else if (c == '0')
			{
				ret++;
			}
			else
			{
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

UOSInt Text::TextBinEnc::CPPTextBinEnc::DecodeBin(const UTF8Char *sbuff, UInt8 *dataBuff)
{
	Bool isQuote = false;
	UOSInt ret = 0;
	UTF8Char c;
	while (true)
	{
		c = *sbuff++;
		if (c == 0)
		{
			if (isQuote)
				return 0;
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
				return 0;
			}
		}
		else if (c == '\\')
		{
			c = *sbuff++;
			if (c == 0)
			{
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
			else
			{
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

const UTF8Char *Text::TextBinEnc::CPPTextBinEnc::GetName()
{
	return (const UTF8Char*)"CPP String";
}
