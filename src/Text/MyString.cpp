#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

Char MyString_STRHEXARR[] = "0123456789ABCDEF";
Char MyString_STRhexarr[] = "0123456789abcdef";

UTF8Char MyString_StrDigit100U8[] = {
	'0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9',
	'1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9',
	'2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9',
	'3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9',
	'4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9',
	'5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9',
	'6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9',
	'7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9',
	'8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9',
	'9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9'};

UTF16Char MyString_StrDigit100U16[] = {
	'0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9',
	'1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9',
	'2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9',
	'3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9',
	'4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9',
	'5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9',
	'6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9',
	'7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9',
	'8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9',
	'9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9'};

UTF32Char MyString_StrDigit100U32[] = {
	'0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9',
	'1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9',
	'2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9',
	'3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9',
	'4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9',
	'5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9',
	'6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9',
	'7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9',
	'8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9',
	'9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9'};

UTF16Char MyString_StrHexArrU16[] = {
	'0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9','0','A','0','B','0','C','0','D','0','E','0','F',
	'1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9','1','A','1','B','1','C','1','D','1','E','1','F',
	'2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9','2','A','2','B','2','C','2','D','2','E','2','F',
	'3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9','3','A','3','B','3','C','3','D','3','E','3','F',
	'4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9','4','A','4','B','4','C','4','D','4','E','4','F',
	'5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9','5','A','5','B','5','C','5','D','5','E','5','F',
	'6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9','6','A','6','B','6','C','6','D','6','E','6','F',
	'7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9','7','A','7','B','7','C','7','D','7','E','7','F',
	'8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9','8','A','8','B','8','C','8','D','8','E','8','F',
	'9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9','9','A','9','B','9','C','9','D','9','E','9','F',
	'A','0','A','1','A','2','A','3','A','4','A','5','A','6','A','7','A','8','A','9','A','A','A','B','A','C','A','D','A','E','A','F',
	'B','0','B','1','B','2','B','3','B','4','B','5','B','6','B','7','B','8','B','9','B','A','B','B','B','C','B','D','B','E','B','F',
	'C','0','C','1','C','2','C','3','C','4','C','5','C','6','C','7','C','8','C','9','C','A','C','B','C','C','C','D','C','E','C','F',
	'D','0','D','1','D','2','D','3','D','4','D','5','D','6','D','7','D','8','D','9','D','A','D','B','D','C','D','D','D','E','D','F',
	'E','0','E','1','E','2','E','3','E','4','E','5','E','6','E','7','E','8','E','9','E','A','E','B','E','C','E','D','E','E','E','F',
	'F','0','F','1','F','2','F','3','F','4','F','5','F','6','F','7','F','8','F','9','F','A','F','B','F','C','F','D','F','E','F','F'};

UTF32Char MyString_StrHexArrU32[] = {
	'0','0','0','1','0','2','0','3','0','4','0','5','0','6','0','7','0','8','0','9','0','A','0','B','0','C','0','D','0','E','0','F',
	'1','0','1','1','1','2','1','3','1','4','1','5','1','6','1','7','1','8','1','9','1','A','1','B','1','C','1','D','1','E','1','F',
	'2','0','2','1','2','2','2','3','2','4','2','5','2','6','2','7','2','8','2','9','2','A','2','B','2','C','2','D','2','E','2','F',
	'3','0','3','1','3','2','3','3','3','4','3','5','3','6','3','7','3','8','3','9','3','A','3','B','3','C','3','D','3','E','3','F',
	'4','0','4','1','4','2','4','3','4','4','4','5','4','6','4','7','4','8','4','9','4','A','4','B','4','C','4','D','4','E','4','F',
	'5','0','5','1','5','2','5','3','5','4','5','5','5','6','5','7','5','8','5','9','5','A','5','B','5','C','5','D','5','E','5','F',
	'6','0','6','1','6','2','6','3','6','4','6','5','6','6','6','7','6','8','6','9','6','A','6','B','6','C','6','D','6','E','6','F',
	'7','0','7','1','7','2','7','3','7','4','7','5','7','6','7','7','7','8','7','9','7','A','7','B','7','C','7','D','7','E','7','F',
	'8','0','8','1','8','2','8','3','8','4','8','5','8','6','8','7','8','8','8','9','8','A','8','B','8','C','8','D','8','E','8','F',
	'9','0','9','1','9','2','9','3','9','4','9','5','9','6','9','7','9','8','9','9','9','A','9','B','9','C','9','D','9','E','9','F',
	'A','0','A','1','A','2','A','3','A','4','A','5','A','6','A','7','A','8','A','9','A','A','A','B','A','C','A','D','A','E','A','F',
	'B','0','B','1','B','2','B','3','B','4','B','5','B','6','B','7','B','8','B','9','B','A','B','B','B','C','B','D','B','E','B','F',
	'C','0','C','1','C','2','C','3','C','4','C','5','C','6','C','7','C','8','C','9','C','A','C','B','C','C','C','D','C','E','C','F',
	'D','0','D','1','D','2','D','3','D','4','D','5','D','6','D','7','D','8','D','9','D','A','D','B','D','C','D','D','D','E','D','F',
	'E','0','E','1','E','2','E','3','E','4','E','5','E','6','E','7','E','8','E','9','E','A','E','B','E','C','E','D','E','E','E','F',
	'F','0','F','1','F','2','F','3','F','4','F','5','F','6','F','7','F','8','F','9','F','A','F','B','F','C','F','D','F','E','F','F'};

Char *Text::StrConcat(Char *oriStr, const Char *strToJoin)
{
	while ((*oriStr++ = *strToJoin++) != 0);
	return oriStr - 1;
}

Char *Text::StrConcatS(Char *oriStr, const Char *strToJoin, UOSInt buffSize)
{
	if (buffSize <= 1)
	{
		*oriStr = 0;
		return oriStr;
	}
	buffSize--;
	while ((*oriStr++ = *strToJoin++) != 0)
		if (buffSize-- <= 0)
		{
			*oriStr++ = 0;
			break;
		}
	return oriStr - 1;
}

Char *Text::StrConcatC(Char *oriStr, const Char *strToJoin, UOSInt charCnt)
{
	MemCopyNO(oriStr, strToJoin, charCnt);
	oriStr[charCnt] = 0;
	return &oriStr[charCnt];
}

UTF8Char *Text::StrConcatASCII(UTF8Char *oriStr, const Char *strToJoin)
{
	while ((*oriStr++ = (UTF8Char)*strToJoin++) != 0);
	return oriStr - 1;
}

Char *Text::StrInt16(Char *oriStr, Int16 val)
{
	if (val < 0)
	{
		val = (Int16)-val;
		*oriStr++ = '-';
	}
	if (val < 10)
	{
		*oriStr++ = (Char)MyString_StrDigit100U8[val * 2 + 1];
	}
	else if (val < 100)
	{
		WriteNInt16((UInt8*)oriStr, ReadNInt16(&MyString_StrDigit100U8[val * 2]));
		oriStr += 2;
	}
	else if (val < 1000)
	{
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		*oriStr = (Char)MyString_StrDigit100U8[(val / 100) * 2 + 1];
		oriStr += 3;
	}
	else if (val < 10000)
	{
		WriteNInt16((UInt8*)oriStr, ReadNInt16(&MyString_StrDigit100U8[(val / 100) * 2]));
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		oriStr += 4;
	}
	else
	{
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		val = val / 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		*oriStr = (Char)MyString_StrDigit100U8[(val / 100) * 2 + 1];
		oriStr += 5;
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrUInt16(Char *oriStr, UInt16 val)
{
	if (val < 10)
	{
		*oriStr++ = (Char)MyString_StrDigit100U8[val * 2 + 1];
	}
	else if (val < 100)
	{
		WriteNInt16((UInt8*)oriStr, ReadNInt16(&MyString_StrDigit100U8[val * 2]));
		oriStr += 2;
	}
	else if (val < 1000)
	{
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		*oriStr = (Char)MyString_StrDigit100U8[(val / 100) * 2 + 1];
		oriStr += 3;
	}
	else if (val < 10000)
	{
		WriteNInt16((UInt8*)oriStr, ReadNInt16(&MyString_StrDigit100U8[(val / 100) * 2]));
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		oriStr += 4;
	}
	else
	{
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		val = val / 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		*oriStr = (Char)MyString_StrDigit100U8[(val / 100) * 2 + 1];
		oriStr += 5;
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrInt32(Char *oriStr, Int32 val)
{
	Char buff[10];
	Char *str;
	if (val < 0)
	{
		val = -val;
		*oriStr++ = '-';
	}
	str = &buff[10];
	if (val == 0)
	{
		*--str = 0x30;
	}
	else
	{
#if 1
		UInt32 uval = (UInt32)val;
		while (uval)
		{
			str -= 2;
			WriteNInt16((UInt8*)str, ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(uval % 100) * 2]));
			uval = uval / 100;
		}
		if (*str == '0')
		{
			str++;
		}
#else
		while (val >= 100)
		{
			str -= 2;
			WriteNInt16(str, ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2];
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt16(str, ReadNInt16(&MyString_StrDigit100U8[val * 2];
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U8[val * 2 + 1];
		}
#endif
	}
	while (str < &buff[10])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrUInt32(Char *oriStr, UInt32 val)
{
	Char buff[10];
	Char *str;
	str = &buff[10];
	if (val == 0)
	{
		*--str = 0x30;
	}
	else
	{
#if 1
		while (val)
		{
			str -= 2;
			WriteNInt16((UTF8Char*)str, ReadNInt16((const UTF8Char*)&MyString_StrDigit100U8[(val % 100) * 2]));
			val = val / 100;
		}
		if (*str == '0')
		{
			str++;
		}
#else
		while (val >= 100)
		{
			str -= 2;
			WriteNInt16(str, ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2];
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt16(str, ReadNInt16(&MyString_StrDigit100U8[val * 2];
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U8[val * 2 + 1];
		}
#endif
	}
	while (str < &buff[10])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrInt32S(Char *oriStr, Int32 val, Char seperator, UOSInt sepCnt)
{
	Char buff[20];
	Char *str;
	UOSInt i;
	if (val < 0)
	{
		val = -val;
		*oriStr++ = '-';
	}
	str = &buff[20];
	if (val == 0)
		*--str = 0x30;
	i = sepCnt;
	while (val)
	{
		if (i == 0)
		{
			i = sepCnt;
			*--str = seperator;
		}
		*--str = (Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}


Char *Text::StrUInt32S(Char *oriStr, UInt32 val, Char seperator, UOSInt sepCnt)
{
	Char buff[20];
	Char *str;
	UOSInt i;
	str = &buff[20];
	if (val == 0)
		*--str = 0x30;
	i = sepCnt;
	while (val)
	{
		if (i == 0)
		{
			i = sepCnt;
			*--str = seperator;
		}
		*--str = (Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

#ifdef HAS_INT64
#if _OSINT_SIZE == 64
Char *Text::StrInt64(Char *oriStr, Int64 val)
{
	Char buff[20];
	Char *str;
	if (val < 0)
	{
		val = -val;
		*oriStr++ = '-';
	}
	str = &buff[20];
	if (val == 0)
	{
		*--str = 0x30;
	}
	else
	{
#if 0
		while (val)
		{
			str -= 2;
			WriteNInt16(str, ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2];
			val = val / 100;
		}
		if (*str == '0')
		{
			str++;
		}
#else
		while (val >= 100)
		{
			str -= 2;
			WriteNInt16(str, ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt16(str, ReadNInt16(&MyString_StrDigit100U8[val * 2]));
		}
		else
		{
			str -= 1;
			*str = (Char)MyString_StrDigit100U8[val * 2 + 1];
		}
#endif
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrUInt64(Char *oriStr, UInt64 val)
{
	Char buff[20];
	Char *str;
	str = &buff[20];
	if (val == 0)
	{
		*--str = 0x30;
	}
	else
	{
		while (val)
		{
			str -= 2;
			WriteNInt16(str, ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
			val = val / 100;
		}
		if (*str == '0')
		{
			str++;
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrInt64S(Char *oriStr, Int64 val, Char seperator, UOSInt sepCnt)
{
	Char buff[40];
	Char *str;
	UOSInt i;
	if (val < 0)
	{
		val = -val;
		*oriStr++ = '-';
	}
	str = &buff[40];
	if (val == 0)
		*--str = 0x30;
	i = sepCnt;
	while (val)
	{
		if (i == 0)
		{
			i = sepCnt;
			*--str = seperator;
		}
		*--str = (Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str < &buff[40])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrUInt64S(Char *oriStr, UInt64 val, Char seperator, UOSInt sepCnt)
{
	Char buff[40];
	Char *str;
	UOSInt i;
	str = &buff[40];
	if (val == 0)
		*--str = 0x30;
	i = sepCnt;
	while (val)
	{
		if (i == 0)
		{
			i = sepCnt;
			*--str = seperator;
		}
		*--str = (Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str < &buff[40])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}
#else
Char *Text::StrInt64(Char *oriStr, Int64 val)
{
	Char buff[20];
	Char *str;
	if (val < 0)
	{
		val = -val;
		*oriStr++ = '-';
	}
	str = &buff[20];
	if (val == 0)
	{
		*--str = 0x30;
	}
	else
	{
		while (val >= 0x100000000LL)
		{
			str -= 2;
			WriteNInt16((UInt8*)str, ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
			val = val / 100;
		}
		UInt32 i32Val = (UInt32)val;
		while (i32Val >= 100)
		{
			str -= 2;
			WriteNInt16((UInt8*)str, ReadNInt16(&MyString_StrDigit100U8[(i32Val % 100) * 2]));
			i32Val = i32Val / 100;
		}
		if (i32Val >= 10)
		{
			str -= 2;
			WriteNInt16((UInt8*)str, ReadNInt16(&MyString_StrDigit100U8[i32Val * 2]));
		}
		else
		{
			str -= 1;
			*str = (Char)MyString_StrDigit100U8[i32Val * 2 + 1];
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrUInt64(Char *oriStr, UInt64 val)
{
	Char buff[20];
	Char *str;
	str = &buff[20];
	if (val == 0)
	{
		*--str = 0x30;
	}
	else
	{
		while (val >= 0x100000000LL)
		{
			str -= 2;
			WriteNInt16((UInt8*)str, ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
			val = val / 100;
		}
		UInt32 i32Val = (UInt32)val;
		while (i32Val >= 100)
		{
			str -= 2;
			WriteNInt16((UInt8*)str, ReadNInt16(&MyString_StrDigit100U8[(i32Val % 100) * 2]));
			i32Val = i32Val / 100;
		}
		if (i32Val >= 10)
		{
			str -= 2;
			WriteNInt16((UInt8*)str, ReadNInt16(&MyString_StrDigit100U8[i32Val * 2]));
		}
		else
		{
			str -= 1;
			*str = (Char)MyString_StrDigit100U8[i32Val * 2 + 1];
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrInt64S(Char *oriStr, Int64 val, Char seperator, UOSInt sepCnt)
{
	Char buff[40];
	Char *str;
	UOSInt i;
	if (val < 0)
	{
		val = -val;
		*oriStr++ = '-';
	}
	str = &buff[40];
	if (val == 0)
		*--str = 0x30;
	i = sepCnt;
	while (val)
	{
		if (i == 0)
		{
			i = sepCnt;
			*--str = seperator;
		}
		*--str = (Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str < &buff[40])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrUInt64S(Char *oriStr, UInt64 val, Char seperator, UOSInt sepCnt)
{
	Char buff[40];
	Char *str;
	UOSInt i;
	str = &buff[40];
	if (val == 0)
		*--str = 0x30;
	i = sepCnt;
	while (val)
	{
		if (i == 0)
		{
			i = sepCnt;
			*--str = seperator;
		}
		*--str = (Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str < &buff[40])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}
#endif
#endif

Char *Text::StrToUpper(Char *oriStr, const Char *strToJoin)
{
	Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 'a' && c <= 'z')
		{
			*oriStr++ = (Char)(c - 32);
		}
		else
		{
			*oriStr++ = c;
		}
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrToLower(Char *oriStr, const Char *strToJoin)
{
	Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			*oriStr++ = (Char)(c + 32);
		}
		else
		{
			*oriStr++ = c;
		}
	}
	*oriStr = 0;
	return oriStr;
}

Char *Text::StrToCapital(Char *oriStr, const Char *strToJoin)
{
	Bool lastLetter = false;
	Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			if (lastLetter)
			{
				*oriStr++ = (Char)(c + 32);
			}
			else
			{
				*oriStr++ = c;
				lastLetter = true;
			}
		}
		else if (c >= 'a' && c <= 'z')
		{
			if (!lastLetter)
			{
				*oriStr++ = (Char)(c - 32);
				lastLetter = true;
			}
			else
			{
				*oriStr++ = c;
			}
		}
		else
		{
			*oriStr++ = c;
			lastLetter = false;
		}
	}
	*oriStr = 0;
	return oriStr;
}

Bool Text::StrEquals(const Char *str1, const Char *str2)
{
	Char c;
	while ((c = *str1++) != 0)
	{
		if (c != *str2++)
			return false;
	}
	return *str2 == 0;
}

Bool Text::StrEqualsN(const Char *str1, const Char *str2)
{
	if (str1 == str2)
		return true;
	if (str1 == 0 || str2 == 0)
		return false;
	return StrEquals(str1, str2);
}

Bool Text::StrEqualsICase(const Char *str1, const Char *str2)
{
	Char c1;
	Char c2;
	while ((c1 = *str1++) != 0)
	{
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 = (Char)(c1 - 0x20);
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (Char)(c2 - 0x20);
		}
		if (c1 != c2)
			return false;
	}
	return *str2 == 0;
}

Bool Text::StrEqualsICase(const Char *str1, const Char *str2, UOSInt str2Len)
{
	Char c1;
	Char c2;
	while (str2Len-- > 0)
	{
		c1 = *str1++;
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
			c1 = (Char)(c1 - 32);
		if (c2 >= 'a' && c2 <= 'z')
			c2 = (Char)(c2 - 32);
		if (c1 != c2)
			return false;
	}
	if (*str1 == 0)
		return true;
	return false;
}

UOSInt Text::StrCharCntS(const Char *str, UOSInt maxLen)
{
	const Char *currPtr = str;
	const Char *endPtr = str + maxLen;
	while (currPtr < endPtr && *currPtr++);
	return (UOSInt)(currPtr - str);
}

Bool Text::StrHasUpperCase(const Char *str)
{
	Char c;
	while ((c = *str++) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			return true;
		}
	}
	return false;
}

Char *Text::StrHexVal64V(Char *oriStr, UInt64 val)
{
	UInt32 v1 = (UInt32)((UInt64)val >> 32);
	UInt32 v2 = (UInt32)(val & 0xffffffff);
	if (v1 == 0)
	{
		return StrHexVal32V(oriStr, v2);
	}
	if ((v1 & 0xfffffff0) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(&oriStr[1], v2);
	}
	else if ((v1 & 0xffffff00) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[1] = MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(&oriStr[2], v2);
	}
	else if ((v1 & 0xfffff000) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[2] = MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(&oriStr[3], v2);
	}
	else if ((v1 & 0xffff0000) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(v1 >> 12) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[2] = MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[3] = MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(&oriStr[4], v2);
	}
	else if ((v1 & 0xfff00000) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(v1 >> 16) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(v1 >> 12) & 0xf];
		oriStr[2] = MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[3] = MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[4] = MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(&oriStr[5], v2);
	}
	else if ((v1 & 0xff000000) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(v1 >> 20) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(v1 >> 16) & 0xf];
		oriStr[2] = MyString_STRHEXARR[(v1 >> 12) & 0xf];
		oriStr[3] = MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[4] = MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[5] = MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(&oriStr[6], v2);
	}
	else if ((v1 & 0xf0000000) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(v1 >> 24) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(v1 >> 20) & 0xf];
		oriStr[2] = MyString_STRHEXARR[(v1 >> 16) & 0xf];
		oriStr[3] = MyString_STRHEXARR[(v1 >> 12) & 0xf];
		oriStr[4] = MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[5] = MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[6] = MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(&oriStr[7], v2);
	}
	else
	{
		oriStr[0] = MyString_STRHEXARR[(v1 >> 28) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(v1 >> 24) & 0xf];
		oriStr[2] = MyString_STRHEXARR[(v1 >> 20) & 0xf];
		oriStr[3] = MyString_STRHEXARR[(v1 >> 16) & 0xf];
		oriStr[4] = MyString_STRHEXARR[(v1 >> 12) & 0xf];
		oriStr[5] = MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[6] = MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[7] = MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(&oriStr[8], v2);
	}
}

Char *Text::StrHexVal64(Char *oriStr, UInt64 val)
{
	Char *tmp = &oriStr[16];
	OSInt i = 16;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[16] = 0;
	return &oriStr[16];
}

Char *Text::StrHexVal32V(Char *oriStr, UInt32 val)
{
	if ((val & 0xfffffff0) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[val & 0xf];
		oriStr[1] = 0;
		return &oriStr[1];
	}
	else if ((val & 0xffffff00) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[1] = MyString_STRHEXARR[val & 0xf];
		oriStr[2] = 0;
		return &oriStr[2];
	}
	else if ((val & 0xfffff000) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[2] = MyString_STRHEXARR[val & 0xf];
		oriStr[3] = 0;
		return &oriStr[3];
	}
	else if ((val & 0xffff0000) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(val >> 12) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[2] = MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[3] = MyString_STRHEXARR[val & 0xf];
		oriStr[4] = 0;
		return &oriStr[4];
	}
	else if ((val & 0xfff00000) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(val >> 16) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(val >> 12) & 0xf];
		oriStr[2] = MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[3] = MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[4] = MyString_STRHEXARR[val & 0xf];
		oriStr[5] = 0;
		return &oriStr[5];
	}
	else if ((val & 0xff000000) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(val >> 20) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(val >> 16) & 0xf];
		oriStr[2] = MyString_STRHEXARR[(val >> 12) & 0xf];
		oriStr[3] = MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[4] = MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[5] = MyString_STRHEXARR[val & 0xf];
		oriStr[6] = 0;
		return &oriStr[6];
	}
	else if ((val & 0xf0000000) == 0)
	{
		oriStr[0] = MyString_STRHEXARR[(val >> 24) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(val >> 20) & 0xf];
		oriStr[2] = MyString_STRHEXARR[(val >> 16) & 0xf];
		oriStr[3] = MyString_STRHEXARR[(val >> 12) & 0xf];
		oriStr[4] = MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[5] = MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[6] = MyString_STRHEXARR[val & 0xf];
		oriStr[7] = 0;
		return &oriStr[7];
	}
	else
	{
		oriStr[0] = MyString_STRHEXARR[(val >> 28) & 0xf];
		oriStr[1] = MyString_STRHEXARR[(val >> 24) & 0xf];
		oriStr[2] = MyString_STRHEXARR[(val >> 20) & 0xf];
		oriStr[3] = MyString_STRHEXARR[(val >> 16) & 0xf];
		oriStr[4] = MyString_STRHEXARR[(val >> 12) & 0xf];
		oriStr[5] = MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[6] = MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[7] = MyString_STRHEXARR[val & 0xf];
		oriStr[8] = 0;
		return &oriStr[8];
	}
}

Char *Text::StrHexVal32(Char *oriStr, UInt32 val)
{
	Char *tmp = &oriStr[8];
	UOSInt i = 8;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[8] = 0;
	return &oriStr[8];
}

Char *Text::StrHexVal24(Char *oriStr, UInt32 val)
{
	Char *tmp = &oriStr[6];
	UInt32 i = 6;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[6] = 0;
	return &oriStr[6];
}

Char *Text::StrHexVal16(Char *oriStr, UInt16 val)
{
	Char *tmp = &oriStr[4];
	Int32 i = 4;
	while (i-- > 0)
	{
		*--tmp = MyString_STRHEXARR[val & 0xf];
		val = (UInt16)(val >> 4);
	}
	oriStr[4] = 0;
	return &oriStr[4];
}

Char *Text::StrHexByte(Char *oriStr, UInt8 val)
{
	oriStr[0] = MyString_STRHEXARR[val >> 4];
	oriStr[1] = MyString_STRHEXARR[val & 15];
	oriStr[2] = 0;
	return &oriStr[2];
}

Char *Text::StrHexBytes(Char *oriStr, const UInt8 *buff, UOSInt buffSize, Char seperator)
{
	if (seperator == 0)
	{
		while (buffSize-- > 0)
		{
			oriStr = Text::StrHexByte(oriStr, *buff++);
		}
	}
	else
	{
		while (buffSize-- > 0)
		{
			oriStr = Text::StrHexByte(oriStr, *buff++);
			*oriStr++ = seperator;
		}
		*--oriStr = 0;
	}
	return oriStr;
}

Int64 Text::StrHex2Int64C(const Char *str)
{
	OSInt i = 16;
	Int64 outVal = 0;
	Char c;
	while (i-- > 0)
	{
		c = *str++;
		if (c == 0)
			return outVal;
		if (c >= '0' && c <= '9')
		{
			outVal = (outVal << 4) | (c - 48);
		}
		else if (c >= 'A' && c <= 'F')
		{
			outVal = (outVal << 4) | (c - 0x37);
		}
		else if (c >= 'a' && c <= 'f')
		{
			outVal = (outVal << 4) | (c - 0x57);
		}
		else
		{
			return 0;
		}
	}
	return outVal;
}

Bool Text::StrHex2Int64C(const Char *str, Int64 *outVal)
{
	UOSInt i = 0;
	Int64 currVal = 0;
	Char c;
	while (true)
	{
		c = *str++;
		if (c == 0)
		{
			*outVal = currVal;
			return true;
		}
		i++;
		if (i >= 17)
		{
			return false;
		}
		if (c >= '0' && c <= '9')
		{
			currVal = (currVal << 4) | (c - 48);
		}
		else if (c >= 'A' && c <= 'F')
		{
			currVal = (currVal << 4) | (c - 0x37);
		}
		else if (c >= 'a' && c <= 'f')
		{
			currVal = (currVal << 4) | (c - 0x57);
		}
		else
		{
			return false;
		}
	}
}

Bool Text::StrHex2Int64S(const Char *str, Int64 *outVal, Int64 failVal)
{
	UOSInt i = 0;
	Int64 currVal = 0;
	Char c;
	while (true)
	{
		c = *str++;
		if (c == 0)
		{
			*outVal = currVal;
			return true;
		}
		i++;
		if (i >= 17)
		{
			*outVal = failVal;
			return false;
		}
		if (c >= '0' && c <= '9')
		{
			currVal = (currVal << 4) | (c - 48);
		}
		else if (c >= 'A' && c <= 'F')
		{
			currVal = (currVal << 4) | (c - 0x37);
		}
		else if (c >= 'a' && c <= 'f')
		{
			currVal = (currVal << 4) | (c - 0x57);
		}
		else
		{
			*outVal = failVal;
			return false;
		}
	}
}

Int32 Text::StrHex2Int32C(const Char *str)
{
	OSInt i = 8;
	Int32 outVal = 0;
	Char c;
	while (i-- > 0)
	{
		c = *str++;
		if (c == 0)
			return outVal;
		if (c >= '0' && c <= '9')
		{
			outVal = (outVal << 4) | (c - 48);
		}
		else if (c >= 'A' && c <= 'F')
		{
			outVal = (outVal << 4) | (c - 0x37);
		}
		else if (c >= 'a' && c <= 'f')
		{
			outVal = (outVal << 4) | (c - 0x57);
		}
		else
		{
			return 0;
		}
	}
	return outVal;
}

Int16 Text::StrHex2Int16C(const Char *str)
{
	UOSInt i = 4;
	Int32 outVal = 0;
	Char c;
	while (i-- > 0)
	{
		c = *str++;
		if (c == 0)
			return (Int16)outVal;
		if (c >= '0' && c <= '9')
		{
			outVal = (outVal << 4) | (c - 48);
		}
		else if (c >= 'A' && c <= 'F')
		{
			outVal = (outVal << 4) | (c - 0x37);
		}
		else if (c >= 'a' && c <= 'f')
		{
			outVal = (outVal << 4) | (c - 0x57);
		}
		else
		{
			return 0;
		}
	}
	return (Int16)outVal;
}

UInt8 Text::StrHex2UInt8C(const Char *str)
{
	UInt8 outVal = 0;
	Char c;
	c = *str++;
	if (c == 0)
		return outVal;
	if (c >= '0' && c <= '9')
	{
		outVal = (UInt8)(c - 48);
	}
	else if (c >= 'A' && c <= 'F')
	{
		outVal = (UInt8)(c - 0x37);
	}
	else if (c >= 'a' && c <= 'f')
	{
		outVal = (UInt8)(c - 0x57);
	}
	else
	{
		return 0;
	}
	c = *str++;
	if (c == 0)
		return outVal;
	if (c >= '0' && c <= '9')
	{
		outVal = (UInt8)((outVal << 4) | (c - 48));
	}
	else if (c >= 'A' && c <= 'F')
	{
		outVal = (UInt8)((outVal << 4) | (c - 0x37));
	}
	else if (c >= 'a' && c <= 'f')
	{
		outVal = (UInt8)((outVal << 4) | (c - 0x57));
	}
	else
	{
		return 0;
	}
	return outVal;
}

UOSInt Text::StrHex2Bytes(const Char *str, UInt8 *buff)
{
	UOSInt outVal = 0;
	UInt8 tmpVal;
	Char c;
	while (true)
	{
		c = *str++;
		if (c == 0)
			return outVal;
		if (c >= '0' && c <= '9')
		{
			tmpVal = (UInt8)((c - 48) << 4);
		}
		else if (c >= 'A' && c <= 'F')
		{
			tmpVal = (UInt8)((c - 0x37) << 4);
		}
		else if (c >= 'a' && c <= 'f')
		{
			tmpVal = (UInt8)((c - 0x57) << 4);
		}
		else if (c == ' ')
		{
			continue;
		}
		else
		{
			return outVal;
		}

		c = *str++;
		if (c == 0)
		{
			*buff++ = 'c';
			outVal++;
			return outVal;
		}
		if (c >= '0' && c <= '9')
		{
			tmpVal |= (UInt8)(c - 48);
		}
		else if (c >= 'A' && c <= 'F')
		{
			tmpVal |= (UInt8)(c - 0x37);
		}
		else if (c >= 'a' && c <= 'f')
		{
			tmpVal |= (UInt8)(c - 0x57);
		}
		else
		{
			return outVal;
		}
		outVal++;
		*buff++ = tmpVal;
	}
	return 0;
}

Int64 Text::StrOct2Int64(const Char *str)
{
	Int32 i = 22;
	Int32 outVal = 0;
	Char c;
	while (i-- > 0)
	{
		c = *str++;
		if (c == 0)
			return outVal;
		if (c >= '0' && c <= '7')
		{
			outVal = (outVal << 3) | (c - 48);
		}
		else
		{
			return outVal;
		}
	}
	return outVal;
}

UOSInt Text::StrSplit(Char **strs, UOSInt maxStrs, Char *strToSplit, Char splitChar)
{
	UOSInt i = 0;
	Char c;
	strs[i++] = strToSplit;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
			break;
		if (c == splitChar)
		{
			strToSplit[-1] = 0;
			strs[i++] = strToSplit;
		}
	}
	return i;
}

UOSInt Text::StrSplitTrim(Char **strs, UOSInt maxStrs, Char *strToSplit, Char splitChar)
{
	UOSInt i = 0;
	Char c;
	Char *lastPtr;
	Char *thisPtr;
	while (*strToSplit == ' ' || *strToSplit == '\r' || *strToSplit == '\n' || *strToSplit == '\t')
		strToSplit++;
	strs[i++] = lastPtr = strToSplit;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
		{
			thisPtr = strToSplit - 1;
			while (lastPtr < thisPtr)
			{
				c = *--thisPtr;
				if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
					*thisPtr = 0;
				else
					break;
			}
			break;
		}
		if (c == splitChar)
		{
			strToSplit[-1] = 0;

			thisPtr = strToSplit - 1;
			while (lastPtr < thisPtr)
			{
				c = *--thisPtr;
				if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
					*thisPtr = 0;
				else
					break;
			}

			while (*strToSplit == ' ' || *strToSplit == '\r' || *strToSplit == '\n' || *strToSplit == '\t')
				strToSplit++;
			strs[i++] = lastPtr = strToSplit;
		}
	}
	return i;
}

UOSInt Text::StrSplitLine(Char **strs, UOSInt maxStrs, Char *strToSplit)
{
	UOSInt i = 0;
	Char c;
	strs[i++] = strToSplit;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
			break;
		if (c == 13)
		{
			strToSplit[-1] = 0;
			if (*strToSplit == 10)
			{
				strToSplit++;
				strs[i++] = strToSplit;
			}
			else
			{
				strs[i++] = strToSplit;
			}
		}
		else if (c == 10)
		{
			strToSplit[-1] = 0;
			strs[i++] = strToSplit;
		}
	}
	return i;
}

UOSInt Text::StrSplitWS(Char **strs, UOSInt maxStrs, Char *strToSplit)
{
	UOSInt i = 0;
	Char c;
	while (true)
	{
		c = *strToSplit++;
		if (c == 0)
		{
			return 0;
		}
		if (c != 32 && c != '\t')
		{
			strs[i++] = strToSplit - 1;
			break;
		}
	}
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
			break;
		if (c == 32 || c == '\t')
		{
			strToSplit[-1] = 0;
			while (true)
			{
				c = *strToSplit++;
				if (c == 0)
				{
					return i;
				}
				if (c != 32 && c != '\t')
				{
					strs[i++] = strToSplit - 1;
					break;
				}
			}
		}
	}
	return i;
}

Bool Text::StrToUInt8(const Char *intStr, UInt8 *outVal)
{
	UInt32 retVal = 0;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return false;
		retVal = retVal * 10 + (UInt32)*intStr - 48;
		intStr++;
		if (retVal & 0xffffff00)
			return false;
	}
	*outVal = (UInt8)retVal;
	return true;
}

UInt8 Text::StrToUInt8(const Char *intStr)
{
	UInt32 retVal = 0;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return 0;
		retVal = retVal * 10 + (UInt32)*intStr - 48;
		intStr++;
		if (retVal & 0xffffff00)
			return 0;
	}
	return (UInt8)retVal;
}

Bool Text::StrToUInt16(const Char *intStr, UInt16 *outVal)
{
	UInt32 retVal = 0;
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		retVal = (UInt16)StrHex2Int16C(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return false;
			retVal = retVal * 10 + (UInt32)*intStr - 48;
			intStr++;
			if (retVal & 0xffff0000)
				return false;
		}
	}
	*outVal = (UInt16)retVal;
	return true;
}

Bool Text::StrToUInt16S(const Char *intStr, UInt16 *outVal, UInt16 failVal)
{
	UInt32 retVal = 0;
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		retVal = (UInt16)StrHex2Int16C(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
			{
				*outVal = failVal;
				return false;
			}
			retVal = retVal * 10 + (UInt32)*intStr - 48;
			intStr++;
			if (retVal & 0xffff0000)
			{
				*outVal = failVal;
				return false;
			}
		}
	}
	*outVal = (UInt16)retVal;
	return true;
}

Bool Text::StrToInt16(const Char *intStr, Int16 *outVal)
{
	Bool sign;
	Int32 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int16C(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return false;
			retVal = retVal * 10 + (Int32)*intStr - 48;
			intStr++;
		}
	}
	if (sign)
		*outVal = (Int16)-retVal;
	else
		*outVal = (Int16)retVal;
	return true;
}

Int16 Text::StrToInt16(const Char *intStr)
{
	Bool sign;
	Int32 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int16C(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return 0;
			retVal = retVal * 10 + (Int32)*intStr - 48;
			intStr++;
		}
	}
	if (sign)
		return (Int16)-retVal;
	else
		return (Int16)retVal;
}

Bool Text::StrToUInt32(const Char *intStr, UInt32 *outVal)
{
	UInt32 retVal = 0;
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2UInt32C(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return false;
			retVal = retVal * 10 + (UInt32)*intStr - 48;
			intStr++;
		}
	}
	*outVal = retVal;
	return true;
}

Bool Text::StrToUInt32S(const Char *intStr, UInt32 *outVal, UInt32 failVal)
{
	UInt32 retVal = 0;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
		{
			*outVal = failVal;
			return false;
		}
		retVal = retVal * 10 + (UInt32)*intStr - 48;
		intStr++;
	}
	*outVal = retVal;
	return true;
}

UInt32 Text::StrToUInt32(const Char *intStr)
{
	UInt32 retVal = 0;
	UInt32 newVal;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return 0;
		newVal = retVal * 10 + (UInt32)*intStr - 48;
		intStr++;
		if (newVal < retVal)
			return 0;
		retVal = newVal;
	}
	return retVal;
}

Bool Text::StrToInt32(const Char *intStr, Int32 *outVal)
{
	Bool sign;
	Int32 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int32C(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return false;
			retVal = retVal * 10 + (Int32)*intStr - 48;
			intStr++;
		}
	}
	if (sign)
		*outVal = -retVal;
	else
		*outVal = retVal;
	return true;
}

Int32 Text::StrToInt32(const Char *intStr)
{
	Bool sign;
	Int32 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int32C(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return 0;
			retVal = retVal * 10 + (Int32)*intStr - 48;
			intStr++;
		}
	}
	if (sign)
		return -retVal;
	else
		return retVal;
}

Bool Text::StrToInt64(const Char *intStr, Int64 *outVal)
{
	Int64 ret = Text::StrToInt64(intStr);
	if (ret == 0)
	{
		if (intStr[0] != '0' || intStr[1] != 0)
			return false;
	}
	*outVal = ret;
	return true;
}

Bool Text::StrToUInt64(const Char *intStr, UInt64 *outVal)
{
	if (intStr[0] == 0)
		return false;
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		return Text::StrHex2UInt64C(intStr + 2, outVal);
	}
	Char c;
	UInt64 v = 0;
	while ((c = *intStr++) != 0)
	{
		if (c < '0' || c > '9')
			return false;
		v = v * 10 + (UInt64)(c - '0');
	}
	*outVal = v;
	return true;
}

Bool Text::StrToUInt64S(const Char *intStr, UInt64 *outVal, UInt64 failVal)
{
	if (intStr[0] == 0)
	{
		*outVal = failVal;
		return false;
	}
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		return Text::StrHex2UInt64S(intStr + 2, outVal, failVal);
	}
	Char c;
	UInt64 v = 0;
	while ((c = *intStr++) != 0)
	{
		if (c < '0' || c > '9')
		{
			*outVal = failVal;
			return false;
		}
		v = v * 10 + (UInt64)(c - '0');
	}
	*outVal = v;
	return true;
}

OSInt Text::StrToOSInt(const Char *str)
{
#if _OSINT_SIZE == 64
	return Text::StrToInt64(str);
#elif _OSINT_SIZE == 32
	return Text::StrToInt32(str);
#else
	return Text::StrToInt16(str);
#endif
}

Bool Text::StrToOSInt(const Char *intStr, OSInt *outVal)
{
#if _OSINT_SIZE == 64
	return Text::StrToInt64(intStr, outVal);
#elif _OSINT_SIZE == 32
	return Text::StrToInt32(intStr, outVal);
#else
	return Text::StrToInt16(intStr, outVal);
#endif
}


UOSInt Text::StrToUOSInt(const Char *str)
{
#if _OSINT_SIZE == 64
	return Text::StrToUInt64(str);
#elif _OSINT_SIZE == 32
	return Text::StrToUInt32(str);
#else
	return Text::StrToUInt16(str);
#endif
}

Bool Text::StrToUOSInt(const Char *intStr, UOSInt *outVal)
{
#if _OSINT_SIZE == 64
	return Text::StrToUInt64(intStr, outVal);
#elif _OSINT_SIZE == 32
	return Text::StrToUInt32(intStr, outVal);
#else
	return Text::StrToUInt16(intStr, outVal);
#endif
}

Bool Text::StrToBool(const Char *str)
{
	if (str == 0)
	{
		return false;
	}
	else if (str[0] == 'T' || str[0] == 't')
	{
		return true;
	}
	else if (str[0] == 'F' || str[0] == 'f')
	{
		return false;
	}
	else
	{
		return Text::StrToInt32(str) != 0;
	}
}

UOSInt Text::StrIndexOf(const Char *str1, const Char *str2)
{
	const Char *ptr = str1;
	const Char *ptr2;
	const Char *ptr3;
	Int32 i;
	while (*ptr)
	{
		ptr2 = ptr;
		ptr3 = str2;
		i = 0;
		while (*ptr3)
		{
			if (*ptr2++ != *ptr3++)
			{
				i = 1;
				break;
			}
		}
		if (i == 0)
			return (UOSInt)(ptr - str1);
		ptr++;
	}
	return INVALID_INDEX;
}

UOSInt Text::StrIndexOf(const Char *str1, Char c)
{
	const Char *ptr = str1;
	while (*ptr)
		if (*ptr == c)
			return (UOSInt)(ptr - str1);
		else
			ptr++;
	return INVALID_INDEX;
}

UOSInt Text::StrIndexOfICase(const Char *str1, const Char *str2)
{
	const Char *ptr = str1;
	const Char *ptr2;
	const Char *ptr3;
	Char c2;
	Char c3;
	Int32 i;
	while (*ptr)
	{
		ptr2 = ptr;
		ptr3 = str2;
		i = 0;
		while (*ptr3)
		{
			c2 = *ptr2++;
			c3 = *ptr3++;
			if (c2 != c3)
			{
				if (c2 >= 'a' && c2 <= 'z')
					c2 = (Char)(c2 - 32);
				if (c3 >= 'a' && c3 <= 'z')
					c3 = (Char)(c3 - 32);
				if (c2 != c3)
				{
					i = 1;
					break;
				}
			}
		}
		if (i == 0)
			return (UOSInt)(ptr - str1);
		ptr++;
	}
	return INVALID_INDEX;
}

UOSInt Text::StrLastIndexOf(const Char *str1, Char c)
{
	const Char *sptr;
	const Char *cpos = str1 - 1;
	Char ch;
	sptr = str1;
	while ((ch = *sptr++) != 0)
	{
		if (ch == c)
			cpos = &sptr[-1];
	}
	return (UOSInt)(cpos - str1);
}

UOSInt Text::StrLastIndexOf(const Char *str1, const Char *str2)
{
	UOSInt leng1 = Text::StrCharCnt(str1);
	UOSInt leng2 = Text::StrCharCnt(str2);
	if (leng2 > leng1)
		return INVALID_INDEX;
	const Char *ptr = str1 + leng1 - leng2;
	const Char *ptr2;
	const Char *ptr3;
	Bool found;
	while (ptr >= str1)
	{
		ptr2 = ptr;
		ptr3 = str2;
		found = false;
		while (*ptr3)
		{
			if (*ptr2++ != *ptr3++)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			return (UOSInt)(ptr - str1);
		}
		ptr--;
	}
	return INVALID_INDEX;
}

Bool Text::StrContainChars(const Char *str, const Char *chars)
{
	const Char *cptr;
	Char c;
	while ((c = *str++) != 0)
	{
		cptr = chars;
		while (true)
		{
			if (c == *cptr)
			{
				return true;
			}
			if (*cptr++ == 0)
			{
				break;
			}
		}
	}
	return false;
}

Char *Text::StrRTrim(Char* str1)
{
	Char *sp = str1;
	while (*str1)
	{
		if (*str1 != ' ' && *str1 != '\t')
		{
			sp = str1 + 1;
		}
		str1++;
	}
	*sp = 0;
	return sp;
}

Char *Text::StrLTrim(Char* str1)
{
	Char *sptr;
	if (str1[0] != ' ' && str1[0] != '\t')
	{
		return &str1[Text::StrCharCnt(str1)];
	}
	else
	{
		sptr = str1;
		while (*sptr == ' ' || *sptr == '\t')
			sptr++;
		return Text::StrConcat(str1, sptr);
	}
}

Char *Text::StrTrim(Char* str1)
{
	Char *sptr = str1;
	Char *sptr2;
	Char c;
	if (str1[0] != ' ' && str1[0] != '\t')
	{
		while (*str1++)
			if (str1[-1] != ' ' && str1[-1] != '\t')
				sptr = str1;
		*sptr = 0;
		return sptr;
	}
	else
	{
		sptr = str1;
		while (*sptr == ' ' || *sptr == '\t')
			sptr++;
		sptr2 = str1;
		while ((*str1++ = c = *sptr++) != 0)
		{
			if (c != ' ' && c != '\t')
				sptr2 = str1;
		}
		*sptr2 = 0;
		return sptr2;
	}
}

Char *Text::StrTrimWSCRLF(Char* str1)
{
	Char *sptr = str1;
	Char *sptr2;
	Char c;
	if (str1[0] != ' ' && str1[0] != '\t' && str1[0] != '\r' && str1[0] != '\n')
	{
		while (*str1++)
			if (str1[-1] != ' ' && str1[-1] != '\t' && str1[-1] != '\r' && str1[-1] != '\n')
				sptr = str1;
		*sptr = 0;
		return sptr;
	}
	else
	{
		sptr = str1;
		while (*sptr == ' ' || *sptr == '\t' || *sptr == '\r' || *sptr == '\n')
			sptr++;
		sptr2 = str1;
		while ((*str1++ = c = *sptr++) != 0)
		{
			if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
				sptr2 = str1;
		}
		*sptr2 = 0;
		return sptr2;
	}
}

Char *Text::StrRemoveChar(Char *str1, Char c)
{
	Char *sp = str1;
	Char c2;
	while (*str1)
		if ((c2 = *str1++) != c)
			*sp++ = c2;
	*sp = 0;
	return sp;
}

const Char *Text::StrCopyNew(const Char *str1)
{
	Char *s = MemAlloc(Char, Text::StrCharCnt(str1) + 1);
	Text::StrConcat(s, str1);
	return s;
}

const Char *Text::StrCopyNewC(const Char *str1, UOSInt strLen)
{
	Char *s = MemAlloc(Char, strLen + 1);
	Text::StrConcatC(s, str1, strLen);
	return s;
}

void Text::StrDelNew(const Char *newStr)
{
	MemFree((void*)newStr);
}

Bool Text::StrStartsWith(const Char *str1, const Char *str2)
{
	while (*str2)
	{
		if (*str1++ != *str2++)
			return false;
	}
	return true;
}

Bool Text::StrStartsWithICase(const Char *str1, const Char *str2)
{
	Char c1;
	Char c2;
	while (*str2)
	{
		c1 = *str1++;
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 = (Char)(c1 - 32);
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (Char)(c2 - 32);
		}
		if (c1 != c2)
			return false;
	}
	return true;
}

Bool Text::StrEndsWith(const Char *str1, const Char *str2)
{
	const Char *ptr1 = str1;
	const Char *ptr2 = str2;
	while (*ptr1++) ;
	while (*ptr2++) ;
	if ((ptr1 - str1) < (ptr2 - str2))
		return false;
	ptr1--;
	ptr2--;
	while (ptr2 > str2)
	{
		if (*--ptr2 != *--ptr1)
			return false;
	}
	return true;
}

Bool Text::StrEndsWithICase(const Char *str1, const Char *str2)
{
	const Char *ptr1 = str1;
	const Char *ptr2 = str2;
	Char c1;
	Char c2;
	while (*ptr1++) ;
	while (*ptr2++) ;
	if ((ptr1 - str1) < (ptr2 - str2))
		return false;
	ptr1--;
	ptr2--;
	while (ptr2 > str2)
	{
		c1 = *--ptr1;
		c2 = *--ptr2;
		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 = (Char)(c1 - 32);
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (Char)(c2 - 32);
		}
		if (c2 != c1)
			return false;
	}
	return true;
}

Bool Text::StrIsInt32(const Char *intStr)
{
	Bool sign;
	Int32 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	if (*intStr == 0)
		return false;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return false;
		if (retVal > 214748364 || retVal < 0)
			return false;
		else if (retVal == 214748364)
		{
			if (*intStr > '8')
				return false;
			else if (*intStr == '8' && !sign)
				return false;
		}

		retVal = retVal * 10 + *intStr - 48;
		intStr++;
	}
	return true;
}

UOSInt Text::StrReplace(Char *str1, Char oriC, Char destC)
{
	Char c;
	UOSInt chrCnt = 0;
	while ((c = *str1++) != 0)
	{
		if (c == oriC)
		{
			str1[-1] = destC;
			chrCnt++;
		}
	}
	return chrCnt;
}

UOSInt Text::StrReplace(Char *str1, const Char *replaceFrom, const Char *replaceTo)
{
	UOSInt cnt;
	UOSInt fromCharCnt;
	UOSInt toCharCnt;
	UOSInt charCnt;
	Char *sptr;
	charCnt = Text::StrCharCnt(str1);
	sptr = &str1[charCnt];
	if ((fromCharCnt = Text::StrCharCnt(replaceFrom)) == 0)
		return 0;
	toCharCnt = Text::StrCharCnt(replaceTo);

	cnt = 0;
	sptr -= fromCharCnt;
	while (sptr >= str1)
	{
		if (Text::StrStartsWith(sptr, replaceFrom))
		{
			if (fromCharCnt != toCharCnt)
			{
				MemCopyO(&sptr[toCharCnt], &sptr[fromCharCnt], (charCnt - (UOSInt)(sptr - str1) - fromCharCnt + 1) * sizeof(Char));
			}
			MemCopyNO(sptr, replaceTo, toCharCnt * sizeof(Char));
			sptr -= fromCharCnt;
			cnt++;
			charCnt = charCnt + toCharCnt - fromCharCnt;
		}
		else
		{
			sptr -= 1;
		}
	}
	return cnt;
}

UOSInt Text::StrReplaceICase(Char *str1, const Char *replaceFrom, const Char *replaceTo)
{
	UOSInt cnt;
	UOSInt fromCharCnt;
	UOSInt toCharCnt;
	UOSInt charCnt;
	Char *sptr;
	charCnt = Text::StrCharCnt(str1);
	sptr = &str1[charCnt];
	if ((fromCharCnt = Text::StrCharCnt(replaceFrom)) == 0)
		return 0;
	toCharCnt = Text::StrCharCnt(replaceTo);

	cnt = 0;
	sptr -= fromCharCnt;
	while (sptr >= str1)
	{
		if (Text::StrStartsWithICase(sptr, replaceFrom))
		{
			if (fromCharCnt != toCharCnt)
			{
				MemCopyO(&sptr[toCharCnt], &sptr[fromCharCnt], (charCnt - (UOSInt)(sptr - str1) - fromCharCnt + 1) * sizeof(Char));
			}
			MemCopyNO(sptr, replaceTo, toCharCnt * sizeof(Char));
			sptr -= fromCharCnt;
			cnt++;
			charCnt = charCnt + toCharCnt - fromCharCnt;
		}
		else
		{
			sptr -= 1;
		}
	}
	return cnt;
}

Char *Text::StrToCSVRec(Char *oriStr, const Char *str1)
{
	Char c;
	*oriStr++ = '"';
	while ((c = *str1++) != 0)
	{
		if (c == '"')
		{
			*oriStr++ = '"';
			*oriStr++ = '"';
		}
		else
		{
			*oriStr++ = c;
		}
	}
	*oriStr++ = '"';
	*oriStr = 0;
	return oriStr;
}

const Char *Text::StrToNewCSVRec(const Char *str1)
{
	UOSInt len = 2;
	Char c;
	const Char *sptr = str1;
	Char *sptr2;
	Char *outPtr;
	while ((c = *sptr++) != 0)
	{
		if (c == '"')
		{
			len += 2;
		}
		else
		{
			len += 1;
		}
	}
	outPtr = MemAlloc(Char, len + 1);
	sptr2 = outPtr;
	*sptr2++ = '"';
	while ((c = *str1++) != 0)
	{
		if (c == '"')
		{
			*sptr2++ = '"';
			*sptr2++ = '"';
		}
		else
		{
			*sptr2++ = c;
		}
	}
	*sptr2++ = '"';
	*sptr2 = 0;
	return outPtr;
}

UOSInt Text::StrCSVSplit(Char **strs, UOSInt maxStrs, Char *strToSplit)
{
	Bool quoted = false;
	Bool first = true;
	UOSInt i = 0;
	Char *strCurr;
	Char c;
	strs[i++] = strCurr = strToSplit;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
		{
			*strCurr = 0;
			break;
		}
		if (c == '"')
		{
			if (!quoted)
			{
				quoted = true;
				first = false;
			}
			else if (*strToSplit == '"')
			{
				strToSplit++;
				*strCurr++ = '"';
				first = false;
			}
			else
			{
				quoted = false;
			}
		}
		else if (c == ',' && !quoted)
		{
			*strCurr = 0;
			strs[i++] = strCurr = strToSplit;
			first = true;
		}
		else
		{
			if (c == ' ' && first)
			{
			}
			else
			{
				*strCurr++ = c;
				first = false;
			}
		}
	}
	return i;
}

Char *Text::StrCSVJoin(Char *oriStr, const Char **strs, UOSInt nStrs)
{
	UOSInt i = 0;
	const Char *sptr;
	Char c;
	while (i < nStrs)
	{
		if (i)
			*oriStr++ = ',';
		*oriStr++ = '"';
		if ((sptr = strs[i]) != 0)
		{
			while ((c = *sptr++) != 0)
			{
				if (c == '"')
				{
					*oriStr++ = '"';
				}
				*oriStr++ = c;
			}
		}
		*oriStr++ = '"';
		i++;
	}
	*oriStr = 0;
	return oriStr;
}

UOSInt Text::StrCountChar(const Char *str1, Char c)
{
	UOSInt cnt = 0;
	Char c2;
	while ((c2 = *str1++) != 0)
		if (c2 == c)
			cnt++;
	return cnt;
}

Char *Text::StrRemoveANSIEscapes(Char *str1)
{
	Char c;
	Char *dest = str1;
	while (true)
	{
		c = *str1++;
		if (c == 0)
		{
			*dest = 0;
			return dest;
		}
		else if (c == 1 || c == 2)
		{
			
		}
		else if (c == 27)
		{
			switch (*str1)
			{
			case '[': //Control Sequence Introducer
				str1++;
				if (*str1 == '?')
				{
					str1++;
				}
				while (true)
				{
					c = *str1++;
					if (c == 0)
					{
						str1--;
						break;
					}
					else if (c == ';')
					{

					}
					else if (c >= '0' && c <= '9')
					{

					}
					else
					{
						break;
					}
				}
				break;
			default:
			case 'M': //Single Shift Two
			case 'O': //Single Shift Three
			case 'P': //Device Control String
			case '\\': //String Terminator
			case 'X': //Start Of String
			case '^': //Privacy Message
			case '_': //Application Program Command
				str1++;
				break;
			}
		}
		else
		{
			*dest++ = c;
		}
	}
}

const UTF8Char *Text::StrCopyNew(const UTF8Char *str1)
{
	UTF8Char *s = MemAlloc(UTF8Char, Text::StrCharCnt(str1) + 1);
	Text::StrConcat(s, str1);
	return s;
}

const UTF8Char *Text::StrCopyNewC(const UTF8Char *str1, UOSInt strLen)
{
	UTF8Char *s = MemAlloc(UTF8Char, strLen + 1);
	Text::StrConcatC(s, str1, strLen);
	return s;
}

void Text::StrDelNew(const UTF8Char *newStr)
{
	MemFree((void*)newStr);
}
