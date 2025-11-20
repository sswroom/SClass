#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#if  defined(_MSC_VER) || defined(__MINGW32__)
#include <windows.h>
#else
#endif

Text::Encoding::Encoding(UInt32 codePage)
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

void Text::Encoding::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

UInt32 Text::Encoding::GetEncCodePage() const
{
	return this->codePage;
}

UnsafeArray<UTF8Char> Text::Encoding::ToString(UnsafeArray<UTF8Char> buff)
{
	return Text::EncodingFactory::GetName(buff, this->codePage);
}

UOSInt Text::Encoding::CountWChars(UnsafeArray<const UInt8> bytes, UOSInt byteSize)
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
		return (UOSInt)MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes.Ptr(), (Int32)byteSize, 0, 0);
#else
		return byteSize;
#endif
	}
}

UnsafeArray<WChar> Text::Encoding::WFromBytes(UnsafeArray<WChar> buff, UnsafeArray<const UInt8> bytes, UOSInt byteSize, OptOut<UOSInt> byteConv)
{
	UOSInt size = byteSize * 2 + 2;

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
			UnsafeArray<WChar> oriBuff = buff;
			while ((*buff++ = ReadUInt16(&bytes[0])) != 0)
			{
				bytes += 2;
			}
			byteConv.Set((UOSInt)(buff - oriBuff) * 2);
			return buff - 1;
		}
		else
		{
			size = byteSize >> 1;
			byteConv.Set(size << 1);
			while (size-- > 0)
			{
				*buff++ = ReadUInt16(&bytes[0]);
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
			UnsafeArray<WChar> oriBuff = buff;
			while ((*buff++ = ReadMUInt16(&bytes[0])) != 0)
			{
				bytes += 2;
			}
			byteConv.Set((UOSInt)(buff - oriBuff) * 2);
			return buff - 1;
		}
		else
		{
			size = byteSize >> 1;
			byteConv.Set(size << 1);
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
		UnsafeArray<const UInt8> oriBytes = bytes;
		tmpPtr = dataBuff;
		UInt8 b;
		UInt32 currCP = 850;
		while (byteSize > 0)
		{
			b = *bytes;
			if (b == 0x1b)
			{
				if (tmpPtr > dataBuff)
				{
					Int32 iRet = 0;
					iRet = MultiByteToWideChar(currCP, 0, (LPCSTR)dataBuff, (Int32)(tmpPtr - dataBuff), buff.Ptr(), (Int32)((tmpPtr - dataBuff) * 2 + 2));
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
			iRet = MultiByteToWideChar(currCP, 0, (LPCSTR)dataBuff, (Int32)(tmpPtr - dataBuff), buff.Ptr(), (Int32)((tmpPtr - dataBuff) * 2 + 2));
			if (iRet > 0)
			{
				buff += iRet;
				*buff = 0;
			}
		}
		byteConv.Set((UOSInt)(bytes - oriBytes));
		return buff;
	}
#endif
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		if (byteConv.IsNotNull())
		{
			UOSInt convSize = byteSize;
			Int32 iRet = 0;
			while (iRet <= 0)
			{
				iRet = MultiByteToWideChar(this->codePage, MB_ERR_INVALID_CHARS, (LPCSTR)bytes.Ptr(), (Int32)convSize, buff.Ptr(), (Int32)size);
				
				if (iRet <= 0)
				{
					//UInt32 err = GetLastError();
					iRet = MultiByteToWideChar(this->codePage, MB_ERR_INVALID_CHARS, (LPCSTR)bytes.Ptr(), (Int32)--convSize, buff.Ptr(), (Int32)size);
				}
				if (iRet <= 0)
				{
					iRet = MultiByteToWideChar(this->codePage, MB_ERR_INVALID_CHARS, (LPCSTR)bytes.Ptr(), (Int32)--convSize, buff.Ptr(), (Int32)size);
				}
				if (iRet <= 0)
				{
					if (this->codePage == 65001)
					{
						return Text::StrUTF8_WChar(buff, bytes, 0);
					}
					else
						this->codePage = 65001;
				}
			}

			buff[iRet] = 0;
			buff = &buff[Text::StrCharCnt(UnsafeArray<const WChar>(buff))];
			byteConv.SetNoCheck(convSize);
			return buff;
		}
		else
		{
			Int32 iRet = MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes.Ptr(), (Int32)byteSize, buff.Ptr(), (Int32)size);
			if (iRet <= 0)
				return Text::StrUTF8_WChar(buff, bytes, 0);
			else
			{
				buff += iRet;
				*buff = 0;
				return buff;
			}
		}
#else
		byteConv.Set(0);
		*buff = 0;
		return buff;
#endif
	}
}

UOSInt Text::Encoding::CountUTF8Chars(UnsafeArray<const UInt8> bytes, UOSInt byteSize)
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
			c = ReadUInt16(bytes.Ptr());
			bytes += 2;
			if (c < 0x80)
				byteCnt++;
			else if (c < 0x800)
				byteCnt += 2;
			else if (byteSize > 0)
			{
				c2 = ReadUInt16(bytes.Ptr());
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
		UOSInt charCnt = (UOSInt)MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes.Ptr(), (Int32)byteSize, 0, 0);
		WChar *buff = MemAlloc(WChar, charCnt + 1);
		MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes.Ptr(), (Int32)byteSize, buff, (Int32)charCnt);
		charCnt = Text::StrWChar_UTF8Cnt(buff);
		MemFree(buff);
		return charCnt;
#else
		return byteSize;
#endif
	}
}

UnsafeArray<UTF8Char> Text::Encoding::UTF8FromBytes(UnsafeArray<UTF8Char> buff, UnsafeArray<const UInt8> bytes, UOSInt byteSize, OptOut<UOSInt> byteConv)
{
	if (this->codePage == 65001)
	{
		byteConv.Set(byteSize);
		return Text::StrConcatC(buff, bytes, byteSize);
	}
	else if (this->codePage == 1200)
	{
		UnsafeArray<UTF8Char> dest = buff;
		UTF16Char c;
		UTF16Char c2;
		byteSize = byteSize >> 1;
		UOSInt retSize = byteSize << 1;
		while (byteSize-- > 0)
		{
			c = ReadUInt16(bytes.Ptr());
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
				c2 = ReadUInt16(bytes.Ptr());
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
		byteConv.Set(retSize);
		return dest;
	}
	else if (this->codePage == 1201)
	{
		UnsafeArray<UTF8Char> dest = buff;
		UTF16Char c;
		UTF16Char c2;
		byteSize = byteSize >> 1;
		UOSInt retSize = byteSize << 1;
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
		byteConv.Set(retSize);
		return dest;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		UOSInt charCnt = (UOSInt)MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes.Ptr(), (Int32)byteSize, 0, 0);
		WChar *wbuff = MemAlloc(WChar, charCnt + 1);
		MultiByteToWideChar(this->codePage, 0, (LPCSTR)bytes.Ptr(), (Int32)byteSize, wbuff, (Int32)charCnt);
		wbuff[charCnt] = 0;
		UnsafeArray<UTF8Char> dest = Text::StrWChar_UTF8(buff, wbuff);
		MemFree(wbuff);
		byteConv.Set((UOSInt)(dest - buff));
		return dest;
#else
		byteConv.Set(byteSize);
		return Text::StrConcatC(buff, bytes, byteSize);
#endif
	}
}

UOSInt Text::Encoding::WCountBytes(UnsafeArray<const WChar> stri)
{
	if (this->codePage == 65001)
	{
		return StrWChar_UTF8Cnt(stri) + 1;
	}
	else if (this->codePage == 1200)
	{
		return (Text::StrCharCnt(stri) + 1) << 1;
	}
	else if (this->codePage == 1201)
	{
		return (Text::StrCharCnt(stri) + 1) << 1;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		return (UOSInt)WideCharToMultiByte(this->codePage, 0, stri.Ptr(), -1, 0, 0, 0, 0);
#else
		return StrWChar_UTF8Cnt(stri) + 1;
#endif
	}
}

UOSInt Text::Encoding::WCountBytesC(UnsafeArray<const WChar> stri, UOSInt strLen)
{
	if (this->codePage == 65001)
	{
		return StrWChar_UTF8CntC(stri, (UOSInt)strLen);
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
		return (UOSInt)WideCharToMultiByte(this->codePage, 0, stri.Ptr(), (Int32)strLen, 0, 0, 0, 0);
#else
		return (UOSInt)strLen;
#endif
	}
}

UOSInt Text::Encoding::WToBytes(UnsafeArray<UInt8> bytes, UnsafeArray<const WChar> wstr)
{
	UOSInt size;
	if (this->codePage == 65001)
	{
		return (UOSInt)(Text::StrWChar_UTF8(bytes, wstr) - bytes + 1);
	}
	else if (this->codePage == 1200)
	{
		WChar c;
		size = 0;
		while (true)
		{
			c = *wstr++;
			size += 2;
			WriteInt16(&bytes[0], c);
			if (c == 0)
				break;
			bytes += 2;
		}
		return size;
	}
	else if (this->codePage == 1201)
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
		return size;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		size = 1024;
		Int32 iRet = WideCharToMultiByte(this->codePage, 0, wstr.Ptr(), -1, (LPSTR)bytes.Ptr(), (Int32)size, 0, 0);
		return (UOSInt)iRet;
#else
		return (UOSInt)(Text::StrWChar_UTF8(bytes, wstr) - bytes + 1);
#endif
	}
}

UOSInt Text::Encoding::WToBytesC(UnsafeArray<UInt8> bytes, UnsafeArray<const WChar> wstr, UOSInt strLen)
{
	UOSInt size;
	if (this->codePage == 65001)
	{
		return (UOSInt)(Text::StrWChar_UTF8C(bytes, wstr, strLen) - bytes);
	}
	else if (this->codePage == 1200)
	{
		WChar c;
		size = strLen << 1;
		while (strLen-- > 0)
		{
			c = *wstr++;
			WriteInt16(&bytes[0], c);
			bytes += 2;
		}
		return size;
	}
	else if (this->codePage == 1201)
	{
		WChar c;
		size = strLen << 1;
		while (strLen-- > 0)
		{
			c = *wstr++;
			WriteMInt16(&bytes[0], c);
			bytes += 2;
		}
		return size;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		size = (UOSInt)(strLen * 3);
		Int32 iRet = WideCharToMultiByte(this->codePage, 0, wstr.Ptr(), (Int32)strLen, (LPSTR)bytes.Ptr(), (Int32)size, 0, 0);
		return (UOSInt)iRet;
#else
		return (UOSInt)(Text::StrWChar_UTF8C(bytes, wstr, strLen) - bytes);
#endif
	}
}

UOSInt Text::Encoding::UTF8CountBytes(UnsafeArray<const UTF8Char> str)
{
	if (this->codePage == 65001)
	{
		return Text::StrCharCnt(str) + 1;
	}
	else if (this->codePage == 1200 || this->codePage == 1201)
	{
		UOSInt charCnt = 0;
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
#if defined(_MSC_VER) || defined(__MINGW32__)
		UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str);
		OSInt strLen = (OSInt)Text::StrCharCnt(wptr);
		UOSInt ret = (UOSInt)WideCharToMultiByte(this->codePage, 0, wptr.Ptr(), (Int32)strLen, 0, 0, 0, 0) + 1;
		Text::StrDelNew(wptr);
		return ret;
#else
		return Text::StrCharCnt(str) + 1;
#endif
	}
}

UOSInt Text::Encoding::UTF8CountBytesC(UnsafeArray<const UTF8Char> str, UOSInt strLen)
{
	if (this->codePage == 65001)
	{
		return strLen;
	}
	else if (this->codePage == 1200 || this->codePage == 1201)
	{
		UOSInt charCnt = 0;
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
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		UOSInt ret = Text::StrUTF8_WCharCntC(str, strLen);
		WChar *wptr = MemAlloc(WChar, ret + 1);
		Text::StrUTF8_WCharC(wptr, str, strLen, 0);
		ret = (UOSInt)WideCharToMultiByte(this->codePage, 0, wptr, (Int32)ret, 0, 0, 0, 0) + 1;
		MemFree(wptr);
		return ret;
#else
		return strLen;
#endif
	}
}

UOSInt Text::Encoding::UTF8ToBytes(UnsafeArray<UInt8> bytes, UnsafeArray<const UTF8Char> str)
{
	if (this->codePage == 65001)
	{
		return (UOSInt)(Text::StrConcat(bytes, str) - bytes + 1);
	}
	else if (this->codePage == 1200)
	{
		UnsafeArray<UInt8> oriBytes = bytes;
		UInt16 c;
		UInt32 code;
		while (str[0])
		{
			if ((str[0] & 0x80) == 0)
			{
				c = str[0];
				WriteInt16(&bytes[0], c);
				bytes += 2;
				str++;
			}
			else if ((str[0] & 0xe0) == 0xc0)
			{
				c = (UInt16)(((str[0] & 0x1f) << 6) | (str[1] & 0x3f));
				WriteInt16(&bytes[0], c);
				bytes += 2;
				str += 2;
			}
			else if ((str[0] & 0xf0) == 0xe0)
			{
				c = (UInt16)(((str[0] & 0x0f) << 12) | ((str[1] & 0x3f) << 6) | (str[2] & 0x3f));
				WriteInt16(&bytes[0], c);
				bytes += 2;
				str += 3;
			}
			else if ((str[0] & 0xf8) == 0xf0)
			{
				code = (UInt32)((((UTF32Char)str[0] & 0x7) << 18) | (((UTF32Char)str[1] & 0x3f) << 12) | ((UTF32Char)(str[2] & 0x3f) << 6) | (UTF32Char)(str[3] & 0x3f));
				WriteInt16(&bytes[0], ((code - 0x10000) >> 10) + 0xd800);
				WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 4;
			}
			else if ((str[0] & 0xfc) == 0xf8)
			{
				code = (UInt32)((((UTF32Char)str[0] & 0x3) << 24) | (((UTF32Char)str[1] & 0x3f) << 18) | (((UTF32Char)str[2] & 0x3f) << 12) | ((UTF32Char)(str[3] & 0x3f) << 6) | (UTF32Char)(str[4] & 0x3f));
				WriteInt16(&bytes[0], ((code - 0x10000) >> 10) + 0xd800);
				WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 5;
			}
			else
			{
				code = (UInt32)((((UTF32Char)str[0] & 0x1) << 30) | (((UTF32Char)str[1] & 0x3f) << 24) | (((UTF32Char)str[2] & 0x3f) << 18) | (((UTF32Char)str[3] & 0x3f) << 12) | ((UTF32Char)(str[4] & 0x3f) << 6) | (UTF32Char)(str[5] & 0x3f));
				WriteInt16(&bytes[0], ((code - 0x10000) >> 10) + 0xd800);
				WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 6;
			}
		}
		WriteInt16(&bytes[0], 0);
		bytes += 2;
		return (UOSInt)(bytes - oriBytes);
	}
	else if (this->codePage == 1201)
	{
		UnsafeArray<UInt8> oriBytes = bytes;
		UInt16 c;
		UInt32 code;
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
				code = (UInt32)((((UTF32Char)str[0] & 0x7) << 18) | (((UTF32Char)str[1] & 0x3f) << 12) | ((UTF32Char)(str[2] & 0x3f) << 6) | (UTF32Char)(str[3] & 0x3f));
				WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
				WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 4;
			}
			else if ((str[0] & 0xfc) == 0xf8)
			{
				code = (UInt32)((((UTF32Char)str[0] & 0x3) << 24) | (((UTF32Char)str[1] & 0x3f) << 18) | (((UTF32Char)str[2] & 0x3f) << 12) | ((UTF32Char)(str[3] & 0x3f) << 6) | (UTF32Char)(str[4] & 0x3f));
				WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
				WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 5;
			}
			else
			{
				code = (UInt32)((((UTF32Char)str[0] & 0x1) << 30) | (((UTF32Char)str[1] & 0x3f) << 24) | (((UTF32Char)str[2] & 0x3f) << 18) | (((UTF32Char)str[3] & 0x3f) << 12) | ((UTF32Char)(str[4] & 0x3f) << 6) | (UTF32Char)(str[5] & 0x3f));
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
#if defined(_MSC_VER) || defined(__MINGW32__)
		UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str);
		OSInt strLen = (OSInt)Text::StrCharCnt(wptr);
		UOSInt ret = (UOSInt)WideCharToMultiByte(this->codePage, 0, wptr.Ptr(), (Int32)strLen, (LPSTR)bytes.Ptr(), (Int32)(strLen * 3), 0, 0);
		Text::StrDelNew(wptr);
		return ret;
#else
		return (UOSInt)(Text::StrConcat(bytes, str) - bytes + 1);
#endif
	}
}

UOSInt Text::Encoding::UTF8ToBytesC(UnsafeArray<UInt8> bytes, UnsafeArray<const UTF8Char> str, UOSInt strLen)
{
	if (this->codePage == 65001)
	{
		MemCopyNO(bytes.Ptr(), str.Ptr(), strLen);
		return strLen;
	}
	else if (this->codePage == 1200)
	{
		UnsafeArray<UInt8> oriBytes = bytes;
		UInt16 c;
		UInt32 code;

		while (strLen > 0)
		{
			if ((str[0] & 0x80) == 0)
			{
				c = str[0];
				WriteInt16(&bytes[0], c);
				bytes += 2;
				str++;
			}
			else if ((str[0] & 0xe0) == 0xc0)
			{
				c = (UInt16)(((str[0] & 0x1f) << 6) | (str[1] & 0x3f));
				WriteInt16(&bytes[0], c);
				bytes += 2;
				str += 2;
			}
			else if ((str[0] & 0xf0) == 0xe0)
			{
				c = (UInt16)(((str[0] & 0x0f) << 12) | ((str[1] & 0x3f) << 6) | (str[2] & 0x3f));
				WriteInt16(&bytes[0], c);
				bytes += 2;
				str += 3;
			}
			else if ((str[0] & 0xf8) == 0xf0)
			{
				code = (UInt32)((((UTF32Char)str[0] & 0x7) << 18) | (((UTF32Char)str[1] & 0x3f) << 12) | ((UTF32Char)(str[2] & 0x3f) << 6) | (UTF32Char)(str[3] & 0x3f));
				WriteInt16(&bytes[0], ((code - 0x10000) >> 10) + 0xd800);
				WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 4;
			}
			else if ((str[0] & 0xfc) == 0xf8)
			{
				code = (UInt32)((((UTF32Char)str[0] & 0x3) << 24) | (((UTF32Char)str[1] & 0x3f) << 18) | (((UTF32Char)str[2] & 0x3f) << 12) | ((UTF32Char)(str[3] & 0x3f) << 6) | (UTF32Char)(str[4] & 0x3f));
				WriteInt16(&bytes[0], ((code - 0x10000) >> 10) + 0xd800);
				WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 5;
			}
			else
			{
				code = (UInt32)((((UTF32Char)str[0] & 0x1) << 30) | (((UTF32Char)str[1] & 0x3f) << 24) | (((UTF32Char)str[2] & 0x3f) << 18) | (((UTF32Char)str[3] & 0x3f) << 12) | ((UTF32Char)(str[4] & 0x3f) << 6) | (UTF32Char)(str[5] & 0x3f));
				WriteInt16(&bytes[0], ((code - 0x10000) >> 10) + 0xd800);
				WriteInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 6;
			}
		}
		return (UOSInt)(bytes - oriBytes);
	}
	else if (this->codePage == 1201)
	{
		UnsafeArray<UInt8> oriBytes = bytes;
		UInt16 c;
		UInt32 code;
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
				code = (UInt32)((((UTF32Char)str[0] & 0x7) << 18) | (((UTF32Char)str[1] & 0x3f) << 12) | ((UTF32Char)(str[2] & 0x3f) << 6) | (UTF32Char)(str[3] & 0x3f));
				WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
				WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 4;
			}
			else if ((str[0] & 0xfc) == 0xf8)
			{
				code = (UInt32)((((UTF32Char)str[0] & 0x3) << 24) | (((UTF32Char)str[1] & 0x3f) << 18) | (((UTF32Char)str[2] & 0x3f) << 12) | ((UTF32Char)(str[3] & 0x3f) << 6) | (UTF32Char)(str[4] & 0x3f));
				WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
				WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 5;
			}
			else
			{
				code = (UInt32)((((UTF32Char)str[0] & 0x1) << 30) | (((UTF32Char)str[1] & 0x3f) << 24) | (((UTF32Char)str[2] & 0x3f) << 18) | (((UTF32Char)str[3] & 0x3f) << 12) | ((UTF32Char)(str[4] & 0x3f) << 6) | (UTF32Char)(str[5] & 0x3f));
				WriteMInt16(bytes, ((code - 0x10000) >> 10) + 0xd800);
				WriteMInt16(&bytes[2], (code & 0x3ff) + 0xdc00);
				bytes += 4;
				str += 6;
			}
		}
		return (UOSInt)(bytes - oriBytes);
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		UOSInt ret = Text::StrUTF8_WCharCntC(str, strLen);
		WChar *wptr = MemAlloc(WChar, ret + 1);
		Text::StrUTF8_WCharC(wptr, str, strLen, 0);
		ret = (UOSInt)WideCharToMultiByte(this->codePage, 0, wptr, (Int32)ret, (LPSTR)bytes.Ptr(), (Int32)(ret * 3), 0, 0) + 1;
		MemFree(wptr);
		return ret;
#else
		return (UOSInt)(Text::StrConcatC(bytes, str, strLen) - bytes);
#endif
	}
}

UnsafeArray<const UInt8> Text::Encoding::NextWChar(UnsafeArray<const UInt8> buff, OutParam<WChar> outputChar)
{
	if (this->codePage == 65001)
	{
		if ((buff[0] & 0xfe) == 0xfc)
		{
			UInt32 code = (UInt32)((buff[5] & 0x3f) | (((UInt32)buff[4] & 0x3f) << 6) | (((UInt32)buff[3] & 0x3f) << 12) | (((UInt32)buff[2] & 0x3f) << 18) | (((UInt32)buff[1] & 0x3f) << 24) | (((UInt32)buff[0] & 0x01) << 30));
			if (this->lastHigh)
			{
				outputChar.Set((WChar)(((code - 0x10000) & 0x3ff) + 0xdc00));
				this->lastHigh = false;
				return &buff[6];
			}
			else
			{
				outputChar.Set((WChar)(((code - 0x10000) >> 10) + 0xd800));
				this->lastHigh = true;
				return buff;
			}
		}
		else if ((buff[0] & 0xfc) == 0xf8)
		{
			UInt32 code = (buff[4] & 0x3f) | (((UInt32)buff[3] & 0x3f) << 6) | (((UInt32)buff[2] & 0x3f) << 12) | (((UInt32)buff[1] & 0x3f) << 18) | (((UInt32)buff[0] & 0x03) << 24);
			if (this->lastHigh)
			{
				outputChar.Set((WChar)(((code - 0x10000) & 0x3ff) + 0xdc00));
				this->lastHigh = false;
				return &buff[5];
			}
			else
			{
				outputChar.Set((WChar)(((code - 0x10000) >> 10) + 0xd800));
				this->lastHigh = true;
				return buff;
			}
		}
		else if ((buff[0] & 0xf8) == 0xf0)
		{
			UInt32 code = (buff[3] & 0x3f) | (((UInt32)buff[2] & 0x3f) << 6) | (((UInt32)buff[1] & 0x3f) << 12) | (((UInt32)buff[0] & 0x03) << 18);
			if (this->lastHigh)
			{
				outputChar.Set((WChar)(((code - 0x10000) & 0x3ff) + 0xdc00));
				this->lastHigh = false;
				return &buff[4];
			}
			else
			{
				outputChar.Set((WChar)(((code - 0x10000) >> 10) + 0xd800));
				this->lastHigh = true;
				return buff;
			}
		}
		else if ((buff[0] & 0xf0) == 0xe0)
		{
			this->lastHigh = false;
			outputChar.Set((WChar)((buff[2] & 0x3f) | ((buff[1] & 0x3f) << 6) | ((buff[0] & 0x0f) << 12)));
			return &buff[3];
		}
		else if ((buff[0] & 0xe0) == 0xc0)
		{
			this->lastHigh = false;
			outputChar.Set((WChar)((buff[1] & 0x3f) | ((buff[0] & 0x1f) << 6)));
			return &buff[2];
		}
		else
		{
			this->lastHigh = false;
			outputChar.Set(buff[0]);
			return &buff[1];
		}
	}
	else if (this->codePage == 1200)
	{
		outputChar.Set(*(WChar*)buff.Ptr());
		return buff + 2;
	}
	else if (this->codePage == 1201)
	{
		outputChar.Set((WChar)((buff[0] << 8) | buff[1]));
		return buff + 2;
	}
	else
	{
#if defined(_MSC_VER) || defined(__MINGW32__)
		if (IsDBCSLeadByteEx(this->codePage, buff[0]))
		{
			MultiByteToWideChar(this->codePage, 0, (LPCSTR)buff.Ptr(), 2, outputChar.Ptr(), 1);
			return &buff[2];
		}
		else
		{
			MultiByteToWideChar(this->codePage, 0, (LPCSTR)buff.Ptr(), 1, outputChar.Ptr(), 1);
			return &buff[1];
		}
#else
		outputChar.Set(buff[0]);
		return buff + 1;
#endif
	}
}

