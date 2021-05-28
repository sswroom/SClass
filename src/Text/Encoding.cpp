#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#if  defined(_MSC_VER) || defined(__MINGW32__)
#include <windows.h>
#else
#endif

Text::Encoding::Encoding(Int32 codePage)
{
	if (codePage)
	{
		this->codePage = codePage;
	}
	else
	{
		this->codePage = Text::EncodingFactory::GetSystemCodePage();
	}
	this->lastHigh = false;
}

Text::Encoding::Encoding()
{
	this->codePage = Text::EncodingFactory::GetSystemCodePage();
	this->lastHigh = false;
}

Text::Encoding::~Encoding()
{
}

void Text::Encoding::SetCodePage(Int32 codePage)
{
	this->codePage = codePage;
}

Int32 Text::Encoding::GetEncCodePage()
{
	return this->codePage;
}

UTF8Char *Text::Encoding::ToString(UTF8Char *buff)
{
	return Text::EncodingFactory::GetName(buff, this->codePage);
}

UOSInt Text::Encoding::CountWChars(const UInt8 *bytes, UOSInt byteSize)
{
	if (this->codePage == 65001)
	{
		return StrUTF8_WCharCntC(bytes, byteSize);
	}
	else if (this->codePage == 1200)
	{
		return byteSize >> 1;
	}
	else if (this->codePage == 1201)
	{
		return byteSize >> 1;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		return MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes, (Int32)byteSize, 0, 0);
#else
		return byteSize;
#endif
	}
}

WChar *Text::Encoding::WFromBytes(WChar *buff, const UInt8 *bytes, UOSInt byteSize, UOSInt *byteConv)
{
	UOSInt tmp;
	UOSInt size = byteSize * 2 + 2;
	if (byteConv == 0)
		byteConv = &tmp;

	if (this->codePage == 65001)
	{
		return Text::StrUTF8_WCharC(buff, bytes, byteSize, byteConv);
	}
	if (size <= 0)
	{
		size = 1024;
		//size = Text::StrByteLen((Char*)bytes);
	}
	if (this->codePage == 1200)
	{
		if (byteSize == (UOSInt)-1)
		{
			WChar *oriBuff = buff;
			while ((*buff++ = ReadUInt16(bytes)) != 0)
			{
				bytes += 2;
			}
			*byteConv = (UOSInt)(buff - oriBuff) * 2;
			return buff - 1;
		}
		else
		{
			size = byteSize >> 1;
			*byteConv = size << 1;
			while (size-- > 0)
			{
				*buff++ = ReadUInt16(bytes);
				bytes += 2;
			}
			*buff = 0;
			return buff;
		}
	}
	else if (this->codePage == 1201)
	{
		if (byteSize == (UOSInt)-1)
		{
			WChar *oriBuff = buff;
			while ((*buff++ = ReadMUInt16(bytes)) != 0)
			{
				bytes += 2;
			}
			*byteConv = (UOSInt)(buff - oriBuff) * 2;
			return buff - 1;
		}
		else
		{
			size = byteSize >> 1;
			*byteConv = size << 1;
			while (size-- > 0)
			{
				*buff++ = ReadMUInt16(bytes);
				bytes += 2;
			}
			*buff = 0;
			return buff;
		}
	}
#if defined(_MSC_VER) || defined(__MINGW32__)
	else if (this->codePage == 50222) //ISO-2022-JP
	{
		UInt8 dataBuff[512];
		UInt8 *tmpPtr;
		const UInt8 *oriBytes = bytes;
		tmpPtr = dataBuff;
		UInt8 b;
		Int32 currCP = 850;
		while (byteSize > 0)
		{
			b = *bytes;
			if (b == 0x1b)
			{
				if (tmpPtr > dataBuff)
				{
					Int32 iRet = 0;
					iRet = MultiByteToWideChar(currCP, 0, (LPCSTR)dataBuff, (Int32)(tmpPtr - dataBuff), buff, (Int32)((tmpPtr - dataBuff) * 2 + 2));
					if (iRet > 0)
					{
						buff += iRet;
						*buff = 0;
					}
				}
				if (bytes[1] == '$' && bytes[2] == 'B')
				{
					currCP = 20932;
					bytes += 3;
					byteSize -= 3;
				}
				else if (bytes[1] == '(' && bytes[2] == 'B')
				{
					currCP = 850;
					bytes += 3;
					byteSize -= 3;
				}
				else
				{
					bytes++;
					byteSize--;
				}
				tmpPtr = dataBuff;
			}
			else
			{
				if (currCP == 20932)
				{
					*tmpPtr++ = b | 0x80;
				}
				else
				{
					*tmpPtr++ = b;
				}
				bytes++;
				byteSize--;
			}
		}
		if (tmpPtr > dataBuff)
		{
			Int32 iRet = 0;
			iRet = MultiByteToWideChar(currCP, 0, (LPCSTR)dataBuff, (Int32)(tmpPtr - dataBuff), buff, (Int32)((tmpPtr - dataBuff) * 2 + 2));
			if (iRet > 0)
			{
				buff += iRet;
				*buff = 0;
			}
		}
		if (byteConv)
		{
			*byteConv = (Int32)(bytes - oriBytes);
		}
		return buff;
	}
#endif
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		if (byteConv && byteConv != &tmp)
		{
			OSInt convSize = byteSize;
			Int32 iRet = 0;
			while (iRet <= 0)
			{
				iRet = MultiByteToWideChar(this->codePage, MB_ERR_INVALID_CHARS, (LPCSTR)bytes, (Int32)convSize, buff, (Int32)size);
				
				if (iRet <= 0)
				{
					UInt32 err = GetLastError();
					iRet = MultiByteToWideChar(this->codePage, MB_ERR_INVALID_CHARS, (LPCSTR)bytes, (Int32)--convSize, buff, (Int32)size);
				}
				if (iRet <= 0)
				{
					iRet = MultiByteToWideChar(this->codePage, MB_ERR_INVALID_CHARS, (LPCSTR)bytes, (Int32)--convSize, buff, (Int32)size);
				}
				if (iRet <= 0)
				{
					if (this->codePage == 65001)
						return 0;
					else
						this->codePage = 65001;
				}
			}

			buff[iRet] = 0;
			buff = &buff[Text::StrCharCnt(buff)];
			*byteConv = convSize;
			return buff;
		}
		else
		{
			Int32 iRet = MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes, (Int32)byteSize, buff, (Int32)size);
			if (iRet <= 0)
				return 0;
			else
			{
				buff += iRet;
				*buff = 0;
				return buff;
			}
		}
#else
		if (byteConv)
			*byteConv = 0;
		return 0;
#endif
	}
}

UOSInt Text::Encoding::CountUTF8Chars(const UInt8 *bytes, UOSInt byteSize)
{
	if (this->codePage == 65001)
	{
		return byteSize;
	}
	else if (this->codePage == 1200)
	{
		UOSInt byteCnt = 0;
		UTF16Char c;
		UTF16Char c2;
		byteSize = byteSize >> 1;
		while (byteSize-- > 0)
		{
			c = ReadUInt16(bytes);
			bytes += 2;
			if (c < 0x80)
				byteCnt++;
			else if (c < 0x800)
				byteCnt += 2;
			else if (byteSize > 0)
			{
				c2 = ReadUInt16(bytes);
				if (c >= 0xd800 && c < 0xdc00 && c2 >= 0xdc00 && c2 < 0xe000)
				{
					UInt32 code = (UInt32)(0x10000 + ((c - 0xd800) << 10) + (c2 - 0xdc00));
					bytes += 2;
					byteSize--;
					if (code < 0x200000)
					{
						byteCnt += 4;
					}
					else if (code < 0x4000000)
					{
						byteCnt += 5;
					}
					else
					{
						byteCnt += 6;
					}
				}
				else
				{
					byteCnt += 3;
				}
			}
			else
			{
				byteCnt += 3;
			}
		}
		return byteCnt;
	}
	else if (this->codePage == 1201)
	{
		UOSInt byteCnt = 0;
		UTF16Char c;
		UTF16Char c2;
		byteSize = byteSize >> 1;
		while (byteSize-- > 0)
		{
			c = ReadMUInt16(bytes);
			bytes += 2;
			if (c < 0x80)
				byteCnt++;
			else if (c < 0x800)
				byteCnt += 2;
			else if (byteSize > 0)
			{
				c2 = ReadMUInt16(bytes);
				if (c >= 0xd800 && c < 0xdc00 && c2 >= 0xdc00 && c2 < 0xe000)
				{
					UInt32 code = 0x10000 + ((UInt32)(c - 0xd800) << 10) + (UInt32)(c2 - 0xdc00);
					bytes += 2;
					byteSize--;
					if (code < 0x200000)
					{
						byteCnt += 4;
					}
					else if (code < 0x4000000)
					{
						byteCnt += 5;
					}
					else
					{
						byteCnt += 6;
					}
				}
				else
				{
					byteCnt += 3;
				}
			}
			else
			{
				byteCnt += 3;
			}
		}
		return byteCnt;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		OSInt charCnt = MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes, (Int32)byteSize, 0, 0);
		WChar *buff = MemAlloc(WChar, charCnt + 1);
		MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes, (Int32)byteSize, buff, (Int32)charCnt);
		charCnt = Text::StrWChar_UTF8Cnt(buff, -1);
		MemFree(buff);
		return charCnt;
#else
		return byteSize;
#endif
	}
}

UTF8Char *Text::Encoding::UTF8FromBytes(UTF8Char *buff, const UInt8 *bytes, UOSInt byteSize, UOSInt *byteConv)
{
	if (this->codePage == 65001)
	{
		if (byteConv)
			*byteConv = byteSize;
		return Text::StrConcatC(buff, bytes, byteSize);
	}
	else if (this->codePage == 1200)
	{
		UTF8Char *dest = buff;
		UTF16Char c;
		UTF16Char c2;
		byteSize = byteSize >> 1;
		while (byteSize-- > 0)
		{
			c = ReadUInt16(bytes);
			bytes += 2;
			if (c < 0x80)
			{
				*dest++ = (UTF8Char)c;
			}
			else if (c < 0x800)
			{
				*dest++ = (UTF8Char)(0xc0 | (c >> 6));
				*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
			}
			else if (byteSize > 0)
			{
				c2 = ReadUInt16(bytes);
				if (c >= 0xd800 && c < 0xdc00 && c2 >= 0xdc00 && c2 < 0xe000)
				{
					UInt32 code = 0x10000 + ((UInt32)(c - 0xd800) << 10) + (UInt32)(c2 - 0xdc00);
					bytes += 2;
					byteSize--;
					if (code < 0x200000)
					{
						*dest++ = (UTF8Char)(0xf0 | (code >> 18));
						*dest++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | (code & 0x3f));
					}
					else if (code < 0x4000000)
					{
						*dest++ = (UTF8Char)(0xf8 | (code >> 24));
						*dest++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | (code & 0x3f));
					}
					else
					{
						*dest++ = (UTF8Char)(0xfc | (code >> 30));
						*dest++ = (UTF8Char)(0x80 | ((code >> 24) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | (code & 0x3f));
					}
				}
				else
				{
					*dest++ = (UTF8Char)(0xe0 | (c >> 12));
					*dest++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
					*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
				}
			}
			else
			{
				*dest++ = (UTF8Char)(0xe0 | (c >> 12));
				*dest++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
			}
		}
		*dest = 0;
		if (byteConv)
		{
			*byteConv = (UOSInt)(dest - buff);
		}
		return dest;
	}
	else if (this->codePage == 1201)
	{
		UTF8Char *dest = buff;
		UTF16Char c;
		UTF16Char c2;
		byteSize = byteSize >> 1;
		while (byteSize-- > 0)
		{
			c = ReadMUInt16(bytes);
			bytes += 2;
			if (c < 0x80)
			{
				*dest++ = (UTF8Char)c;
			}
			else if (c < 0x800)
			{
				*dest++ = (UTF8Char)(0xc0 | (c >> 6));
				*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
			}
			else if (byteSize > 0)
			{
				c2 = ReadMUInt16(bytes);
				if (c >= 0xd800 && c < 0xdc00 && c2 >= 0xdc00 && c2 < 0xe000)
				{
					UInt32 code = 0x10000 + ((UInt32)(c - 0xd800) << 10) + (UInt32)(c2 - 0xdc00);
					bytes += 2;
					byteSize--;
					if (code < 0x200000)
					{
						*dest++ = (UTF8Char)(0xf0 | (code >> 18));
						*dest++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | (code & 0x3f));
					}
					else if (code < 0x4000000)
					{
						*dest++ = (UTF8Char)(0xf8 | (code >> 24));
						*dest++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | (code & 0x3f));
					}
					else
					{
						*dest++ = (UTF8Char)(0xfc | (code >> 30));
						*dest++ = (UTF8Char)(0x80 | ((code >> 24) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
						*dest++ = (UTF8Char)(0x80 | (code & 0x3f));
					}
				}
				else
				{
					*dest++ = (UTF8Char)(0xe0 | (c >> 12));
					*dest++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
					*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
				}
			}
			else
			{
				*dest++ = (UTF8Char)(0xe0 | (c >> 12));
				*dest++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
			}
		}
		*dest = 0;
		if (byteConv)
		{
			*byteConv = (UOSInt)(dest - buff);
		}
		return dest;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		OSInt charCnt = MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes, (Int32)byteSize, 0, 0);
		WChar *wbuff = MemAlloc(WChar, charCnt + 1);
		MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes, (Int32)byteSize, wbuff, (Int32)charCnt);
		wbuff[charCnt] = 0;
		UTF8Char *dest = Text::StrWChar_UTF8(buff, wbuff, -1);
		MemFree(wbuff);
		if (byteConv)
		{
			*byteConv = dest - buff;
		}
		return dest;
#else
		if (byteConv)
			*byteConv = byteSize;
		return Text::StrConcatC(buff, bytes, byteSize);
#endif
	}
}

UOSInt Text::Encoding::WCountBytes(const WChar *stri, OSInt strLen)
{
	if (this->codePage == 65001)
	{
		if (strLen < 0)
		{
			return StrWChar_UTF8Cnt(stri, strLen) + 1;
		}
		else
		{
			return StrWChar_UTF8Cnt(stri, strLen);
		}
	}
	else if (this->codePage == 1200)
	{
		return (UOSInt)strLen << 1;
	}
	else if (this->codePage == 1201)
	{
		return (UOSInt)strLen << 1;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		return WideCharToMultiByte(this->codePage, 0, stri, (Int32)strLen, 0, 0, 0, 0);
#else
		return (UOSInt)strLen;
#endif
	}
}

UOSInt Text::Encoding::WToBytes(UInt8 *bytes, const WChar *wstr, OSInt strLen)
{
	UOSInt size;
	if (this->codePage == 65001)
	{
		if (strLen < 0)
		{
			return (UOSInt)(Text::StrWChar_UTF8(bytes, wstr, strLen) - bytes + 1);
		}
		else
		{
			return (UOSInt)(Text::StrWChar_UTF8(bytes, wstr, strLen) - bytes);
		}
	}
	else if (this->codePage == 1200)
	{
		WChar c;
		if (strLen == -1)
		{
			size = 0;
			while (true)
			{
				c = *wstr++;
				size += 2;
				WriteInt16(bytes, c);
				if (c == 0)
					break;
				bytes += 2;
			}
		}
		else
		{
			size = (UOSInt)strLen << 1;
			while (strLen-- > 0)
			{
				c = *wstr++;
				size += 2;
				WriteInt16(bytes, c);
				bytes += 2;
			}
		}
		return size;
	}
	else if (this->codePage == 1201)
	{
		if (strLen == -1)
		{
			WChar c;
			size = 0;
			while (true)
			{
				c = *wstr++;
				size += 2;
				WriteMInt16(bytes, c);
				if (c == 0)
					break;
				bytes += 2;
			}
		}
		else
		{
			WChar c;
			size = (UOSInt)strLen << 1;
			while (strLen-- > 0)
			{
				c = *wstr++;
				size += 2;
				WriteMInt16(bytes, c);
				bytes += 2;
			}
		}
		return size;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		size = (Int32)(strLen * 3);
		if (strLen == -1)
			size = 1024;
		Int32 iRet = WideCharToMultiByte(this->codePage, 0, wstr, (Int32)strLen, (LPSTR)bytes, (Int32)size, 0, 0);
		return iRet;
#else
		return 0;
#endif
	}
}

UOSInt Text::Encoding::UTF8CountBytes(const UTF8Char *str, OSInt strLen)
{
	if (this->codePage == 65001)
	{
		if (strLen < 0)
		{
			return Text::StrCharCnt(str) + 1;
		}
		else
		{
			return (UOSInt)strLen;
		}
	}
	else if (this->codePage == 1200 || this->codePage == 1201)
	{
		UOSInt charCnt = 0;
		if (strLen < 0)
		{
			while (str[0])
			{
				if ((str[0] & 0x80) == 0)
				{
					charCnt++;
					str++;
				}
				else if ((str[0] & 0xe0) == 0xc0)
				{
					charCnt++;
					str += 2;
				}
				else if ((str[0] & 0xf0) == 0xe0)
				{
					charCnt++;
					str += 3;
				}
				else if ((str[0] & 0xf8) == 0xf0)
				{
					charCnt += 2;
					str += 4;
				}
				else if ((str[0] & 0xfc) == 0xf8)
				{
					charCnt += 2;
					str += 5;
				}
				else
				{
					charCnt += 2;
					str += 6;
				}
			}
			return (charCnt + 1) << 1;
		}
		else
		{
			while (strLen > 0)
			{
				if ((str[0] & 0x80) == 0)
				{
					charCnt++;
					strLen--;
					str++;
				}
				else if ((str[0] & 0xe0) == 0xc0)
				{
					charCnt++;
					strLen -= 2;
					str += 2;
				}
				else if ((str[0] & 0xf0) == 0xe0)
				{
					charCnt++;
					strLen -= 3;
					str += 3;
				}
				else if ((str[0] & 0xf8) == 0xf0)
				{
					charCnt += 2;
					strLen -= 4;
					str += 4;
				}
				else if ((str[0] & 0xfc) == 0xf8)
				{
					charCnt += 2;
					strLen -= 5;
					str += 5;
				}
				else
				{
					charCnt += 2;
					strLen -= 6;
					str += 6;
				}
			}
			return charCnt << 1;
		}
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		if (strLen < 0)
		{
			const WChar *wptr = Text::StrToWCharNew(str);
			strLen = Text::StrCharCnt(wptr);
			UOSInt ret = WideCharToMultiByte(this->codePage, 0, wptr, (Int32)strLen, 0, 0, 0, 0) + 1;
			Text::StrDelNew(wptr);
			return ret;
		}
		else
		{
			OSInt ret = Text::StrUTF8_WCharCntC(str, strLen);
			WChar *wptr = MemAlloc(WChar, ret + 1);
			Text::StrUTF8_WCharC(wptr, str, strLen, 0);
			ret = WideCharToMultiByte(this->codePage, 0, wptr, (Int32)ret, 0, 0, 0, 0) + 1;
			MemFree(wptr);
			return ret;
		}
#else
		if (strLen < 0)
		{
			return Text::StrCharCnt(str) + 1;
		}
		else
		{
			return (UOSInt)strLen;
		}
#endif
	}
}

UOSInt Text::Encoding::UTF8ToBytes(UInt8 *bytes, const UTF8Char *str, OSInt strLen)
{
	if (this->codePage == 65001)
	{
		if (strLen < 0)
		{
			return (UOSInt)(Text::StrConcat(bytes, str) - bytes + 1);
		}
		else
		{
			MemCopyNO(bytes, str, (UOSInt)strLen);
			return (UOSInt)strLen;
		}
	}
	else if (this->codePage == 1200)
	{
		UInt8 *oriBytes = bytes;
		UInt16 c;
		UInt32 code;
		if (strLen < 0)
		{
			while (str[0])
			{
				if ((str[0] & 0x80) == 0)
				{
					c = str[0];
					WriteInt16(bytes, c);
					bytes += 2;
					str++;
				}
				else if ((str[0] & 0xe0) == 0xc0)
				{
					c = (UInt16)(((str[0] & 0x1f) << 6) | (str[1] & 0x3f));
					WriteInt16(bytes, c);
					bytes += 2;
					str += 2;
				}
				else if ((str[0] & 0xf0) == 0xe0)
				{
					c = (UInt16)(((str[0] & 0x0f) << 12) | ((str[1] & 0x3f) << 6) | (str[2] & 0x3f));
					WriteInt16(bytes, c);
					bytes += 2;
					str += 3;
				}
				else if ((str[0] & 0xf8) == 0xf0)
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x7) << 18) | (((UTF32Char)str[1] & 0x3f) << 12) | ((str[2] & 0x3f) << 6) | (str[3] & 0x3f));
					WriteInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 4;
				}
				else if ((str[0] & 0xfc) == 0xf8)
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x3) << 24) | (((UTF32Char)str[1] & 0x3f) << 18) | (((UTF32Char)str[2] & 0x3f) << 12) | ((str[3] & 0x3f) << 6) | (str[4] & 0x3f));
					WriteInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 5;
				}
				else
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x1) << 30) | (((UTF32Char)str[1] & 0x3f) << 24) | (((UTF32Char)str[2] & 0x3f) << 18) | (((UTF32Char)str[3] & 0x3f) << 12) | ((str[4] & 0x3f) << 6) | (str[5] & 0x3f));
					WriteInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 6;
				}
			}
			WriteInt16(bytes, 0);
			bytes += 2;
			return (UOSInt)(bytes - oriBytes);
		}
		else
		{
			while (strLen > 0)
			{
				if ((str[0] & 0x80) == 0)
				{
					c = str[0];
					WriteInt16(bytes, c);
					bytes += 2;
					str++;
				}
				else if ((str[0] & 0xe0) == 0xc0)
				{
					c = (UInt16)(((str[0] & 0x1f) << 6) | (str[1] & 0x3f));
					WriteInt16(bytes, c);
					bytes += 2;
					str += 2;
				}
				else if ((str[0] & 0xf0) == 0xe0)
				{
					c = (UInt16)(((str[0] & 0x0f) << 12) | ((str[1] & 0x3f) << 6) | (str[2] & 0x3f));
					WriteInt16(bytes, c);
					bytes += 2;
					str += 3;
				}
				else if ((str[0] & 0xf8) == 0xf0)
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x7) << 18) | (((UTF32Char)str[1] & 0x3f) << 12) | ((str[2] & 0x3f) << 6) | (str[3] & 0x3f));
					WriteInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 4;
				}
				else if ((str[0] & 0xfc) == 0xf8)
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x3) << 24) | (((UTF32Char)str[1] & 0x3f) << 18) | (((UTF32Char)str[2] & 0x3f) << 12) | ((str[3] & 0x3f) << 6) | (str[4] & 0x3f));
					WriteInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 5;
				}
				else
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x1) << 30) | (((UTF32Char)str[1] & 0x3f) << 24) | (((UTF32Char)str[2] & 0x3f) << 18) | (((UTF32Char)str[3] & 0x3f) << 12) | ((str[4] & 0x3f) << 6) | (str[5] & 0x3f));
					WriteInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 6;
				}
			}
			return (UOSInt)(bytes - oriBytes);
		}
	}
	else if (this->codePage == 1201)
	{
		UInt8 *oriBytes = bytes;
		UInt16 c;
		UInt32 code;
		if (strLen < 0)
		{
			while (str[0])
			{
				if ((str[0] & 0x80) == 0)
				{
					c = str[0];
					WriteMInt16(bytes, c);
					bytes += 2;
					str++;
				}
				else if ((str[0] & 0xe0) == 0xc0)
				{
					c = (UInt16)(((str[0] & 0x1f) << 6) | (str[1] & 0x3f));
					WriteMInt16(bytes, c);
					bytes += 2;
					str += 2;
				}
				else if ((str[0] & 0xf0) == 0xe0)
				{
					c = (UInt16)(((str[0] & 0x0f) << 12) | ((str[1] & 0x3f) << 6) | (str[2] & 0x3f));
					WriteMInt16(bytes, c);
					bytes += 2;
					str += 3;
				}
				else if ((str[0] & 0xf8) == 0xf0)
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x7) << 18) | (((UTF32Char)str[1] & 0x3f) << 12) | ((str[2] & 0x3f) << 6) | (str[3] & 0x3f));
					WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 4;
				}
				else if ((str[0] & 0xfc) == 0xf8)
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x3) << 24) | (((UTF32Char)str[1] & 0x3f) << 18) | (((UTF32Char)str[2] & 0x3f) << 12) | ((str[3] & 0x3f) << 6) | (str[4] & 0x3f));
					WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 5;
				}
				else
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x1) << 30) | (((UTF32Char)str[1] & 0x3f) << 24) | (((UTF32Char)str[2] & 0x3f) << 18) | (((UTF32Char)str[3] & 0x3f) << 12) | ((str[4] & 0x3f) << 6) | (str[5] & 0x3f));
					WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 6;
				}
			}
			WriteMInt16(bytes, 0);
			bytes += 2;
			return (UOSInt)(bytes - oriBytes);
		}
		else
		{
			while (strLen > 0)
			{
				if ((str[0] & 0x80) == 0)
				{
					c = str[0];
					WriteMInt16(bytes, c);
					bytes += 2;
					str++;
				}
				else if ((str[0] & 0xe0) == 0xc0)
				{
					c = (UInt16)(((str[0] & 0x1f) << 6) | (str[1] & 0x3f));
					WriteMInt16(bytes, c);
					bytes += 2;
					str += 2;
				}
				else if ((str[0] & 0xf0) == 0xe0)
				{
					c = (UInt16)(((str[0] & 0x0f) << 12) | ((str[1] & 0x3f) << 6) | (str[2] & 0x3f));
					WriteMInt16(bytes, c);
					bytes += 2;
					str += 3;
				}
				else if ((str[0] & 0xf8) == 0xf0)
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x7) << 18) | (((UTF32Char)str[1] & 0x3f) << 12) | ((str[2] & 0x3f) << 6) | (str[3] & 0x3f));
					WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 4;
				}
				else if ((str[0] & 0xfc) == 0xf8)
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x3) << 24) | (((UTF32Char)str[1] & 0x3f) << 18) | (((UTF32Char)str[2] & 0x3f) << 12) | ((str[3] & 0x3f) << 6) | (str[4] & 0x3f));
					WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 5;
				}
				else
				{
					code = (UInt32)((((UTF32Char)str[0] & 0x1) << 30) | (((UTF32Char)str[1] & 0x3f) << 24) | (((UTF32Char)str[2] & 0x3f) << 18) | (((UTF32Char)str[3] & 0x3f) << 12) | ((str[4] & 0x3f) << 6) | (str[5] & 0x3f));
					WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
					WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
					bytes += 4;
					str += 6;
				}
			}
			return (UOSInt)(bytes - oriBytes);
		}
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		if (strLen < 0)
		{
			const WChar *wptr = Text::StrToWCharNew(str);
			strLen = Text::StrCharCnt(wptr);
			UOSInt ret = WideCharToMultiByte(this->codePage, 0, wptr, (Int32)strLen, (LPSTR)bytes, (Int32)(strLen * 3), 0, 0);
			Text::StrDelNew(wptr);
			return ret;
		}
		else
		{
			UOSInt ret = Text::StrUTF8_WCharCntC(str, strLen);
			WChar *wptr = MemAlloc(WChar, ret + 1);
			Text::StrUTF8_WCharC(wptr, str, strLen, 0);
			ret = WideCharToMultiByte(this->codePage, 0, wptr, (Int32)ret, (LPSTR)bytes, (Int32)(ret * 3), 0, 0) + 1;
			MemFree(wptr);
			return ret;
		}
#else
		if (strLen < 0)
		{
			return (UOSInt)(Text::StrConcat(bytes, str) - bytes + 1);
		}
		else
		{
			return (UOSInt)(Text::StrConcatC(bytes, str, (UOSInt)strLen) - bytes);
		}
#endif
	}
}


const UInt8 *Text::Encoding::NextWChar(const UInt8 *buff, WChar *outputChar)
{
	if (this->codePage == 65001)
	{
		if ((buff[0] & 0xfe) == 0xfc)
		{
			UInt32 code = (UInt32)((buff[5] & 0x3f) | (((UInt32)buff[4] & 0x3f) << 6) | (((UInt32)buff[3] & 0x3f) << 12) | (((UInt32)buff[2] & 0x3f) << 18) | (((UInt32)buff[1] & 0x3f) << 24) | (((UInt32)buff[0] & 0x01) << 30));
			if (this->lastHigh)
			{
				*outputChar = (WChar)(((code - 0x10000) & 0x3ff) + 0xdc00);
				this->lastHigh = false;
				return &buff[6];
			}
			else
			{
				*outputChar = (WChar)(((code - 0x10000) >> 10) + 0xd800);
				this->lastHigh = true;
				return buff;
			}
		}
		else if ((buff[0] & 0xfc) == 0xf8)
		{
			UInt32 code = (buff[4] & 0x3f) | (((UInt32)buff[3] & 0x3f) << 6) | (((UInt32)buff[2] & 0x3f) << 12) | (((UInt32)buff[1] & 0x3f) << 18) | (((UInt32)buff[0] & 0x03) << 24);
			if (this->lastHigh)
			{
				*outputChar = (WChar)(((code - 0x10000) & 0x3ff) + 0xdc00);
				this->lastHigh = false;
				return &buff[5];
			}
			else
			{
				*outputChar = (WChar)(((code - 0x10000) >> 10) + 0xd800);
				this->lastHigh = true;
				return buff;
			}
		}
		else if ((buff[0] & 0xf8) == 0xf0)
		{
			UInt32 code = (buff[3] & 0x3f) | (((UInt32)buff[2] & 0x3f) << 6) | (((UInt32)buff[1] & 0x3f) << 12) | (((UInt32)buff[0] & 0x03) << 18);
			if (this->lastHigh)
			{
				*outputChar = (WChar)(((code - 0x10000) & 0x3ff) + 0xdc00);
				this->lastHigh = false;
				return &buff[4];
			}
			else
			{
				*outputChar = (WChar)(((code - 0x10000) >> 10) + 0xd800);
				this->lastHigh = true;
				return buff;
			}
		}
		else if ((buff[0] & 0xf0) == 0xe0)
		{
			this->lastHigh = false;
			*outputChar = (buff[2] & 0x3f) | ((buff[1] & 0x3f) << 6) | ((buff[0] & 0x0f) << 12);
			return &buff[3];
		}
		else if ((buff[0] & 0xe0) == 0xc0)
		{
			this->lastHigh = false;
			*outputChar = (buff[1] & 0x3f) | ((buff[0] & 0x1f) << 6);
			return &buff[2];
		}
		else
		{
			this->lastHigh = false;
			*outputChar = buff[0];
			return &buff[1];
		}
	}
	else if (this->codePage == 1200)
	{
		*outputChar = *(WChar*)buff;
		return buff + 2;
	}
	else if (this->codePage == 1201)
	{
		*outputChar = (buff[0] << 8) | buff[1];
		return buff + 2;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		if (IsDBCSLeadByteEx(this->codePage, buff[0]))
		{
			MultiByteToWideChar(this->codePage, 0, (LPCSTR)buff, 2, outputChar, 1);
			return &buff[2];
		}
		else
		{
			MultiByteToWideChar(this->codePage, 0, (LPCSTR)buff, 1, outputChar, 1);
			return &buff[1];
		}
#else
		*outputChar = buff[0];
		return buff + 1;
#endif
	}
}

