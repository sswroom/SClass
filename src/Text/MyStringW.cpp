#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

UnsafeArray<UTF16Char> Text::StrConcat(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToJoin)
{
	while ((*oriStr++ = *strToJoin++) != 0);
	return oriStr - 1;
}

UnsafeArray<UTF16Char> Text::StrConcatS(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToJoin, UIntOS buffSize)
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

UnsafeArray<UTF16Char> Text::StrConcatC(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToJoin, UIntOS charCnt)
{
	MemCopyNO(oriStr.Ptr(), strToJoin.Ptr(), charCnt * sizeof(UTF16Char));
	oriStr[charCnt] = 0;
	return &oriStr[charCnt];
}

UnsafeArray<UTF32Char> Text::StrConcat(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToJoin)
{
	while ((*oriStr++ = *strToJoin++) != 0);
	return oriStr - 1;
}

UnsafeArray<UTF32Char> Text::StrConcatS(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToJoin, UIntOS buffSize)
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

UnsafeArray<UTF32Char> Text::StrConcatC(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToJoin, UIntOS charCnt)
{
	MemCopyNO(oriStr.Ptr(), strToJoin.Ptr(), charCnt * sizeof(UTF32Char));
	oriStr[charCnt] = 0;
	return &oriStr[charCnt];
}

UnsafeArray<UTF16Char> Text::StrConcatASCII(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const Char> strToJoin)
{
	while ((*oriStr++ = (UTF16Char)*strToJoin++) != 0);
	return oriStr - 1;
}

UnsafeArray<UTF32Char> Text::StrConcatASCII(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const Char> strToJoin)
{
	while ((*oriStr++ = (UTF32Char)*strToJoin++) != 0);
	return oriStr - 1;
}

UnsafeArray<UTF16Char> Text::StrInt16(UnsafeArray<UTF16Char> oriStr, Int16 val)
{
	UInt16 uval;
	if (val < 0)
	{
		uval = (UInt16)-val;
		*oriStr++ = '-';
	}
	else
	{
		uval = (UInt16)val;
	}
	if (uval < 10)
	{
		*oriStr++ = MyString_StrDigit100U16[uval * 2 + 1];
	}
	else if (uval < 100)
	{
		WriteNInt32((UTF8Char*)&oriStr[0], ReadNInt32((const UTF8Char*)&MyString_StrDigit100U16[uval * 2]));
		oriStr += 2;
	}
	else if (uval < 1000)
	{
		WriteNInt32((UTF8Char*)&oriStr[1], ReadNInt32((const UTF8Char*)&MyString_StrDigit100U16[(uval % 100) * 2]));
		*oriStr = MyString_StrDigit100U16[(uval / 100) * 2 + 1];
		oriStr += 3;
	}
	else if (uval < 10000)
	{
		WriteNInt32((UTF8Char*)&oriStr[0], ReadNInt32((const UTF8Char*)&MyString_StrDigit100U16[(uval / 100) * 2]));
		WriteNInt32((UTF8Char*)&oriStr[2], ReadNInt32((const UTF8Char*)&MyString_StrDigit100U16[(uval % 100) * 2]));
		oriStr += 4;
	}
	else
	{
		WriteNInt32((UTF8Char*)&oriStr[3], ReadNInt32((const UTF8Char*)&MyString_StrDigit100U16[(uval % 100) * 2]));
		uval = uval / 100;
		WriteNInt32((UTF8Char*)&oriStr[1], ReadNInt32((const UTF8Char*)&MyString_StrDigit100U16[(uval % 100) * 2]));
		*oriStr = MyString_StrDigit100U16[(uval / 100) * 2 + 1];
		oriStr += 5;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF16Char> Text::StrUInt16(UnsafeArray<UTF16Char> oriStr, UInt16 val)
{
	if (val < 10)
	{
		*oriStr++ = MyString_StrDigit100U16[val * 2 + 1];
	}
	else if (val < 100)
	{
		WriteNInt32((UInt8*)&oriStr[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[val * 2]));
		oriStr += 2;
	}
	else if (val < 1000)
	{
		WriteNInt32((UInt8*)&oriStr[1], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(val % 100) * 2]));
		*oriStr = MyString_StrDigit100U16[(val / 100) * 2 + 1];
		oriStr += 3;
	}
	else if (val < 10000)
	{
		WriteNInt32((UInt8*)&oriStr[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(val / 100) * 2]));
		WriteNInt32((UInt8*)&oriStr[2], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(val % 100) * 2]));
		oriStr += 4;
	}
	else
	{
		WriteNInt32((UInt8*)&oriStr[3], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(val % 100) * 2]));
		val = val / 100;
		WriteNInt32((UInt8*)&oriStr[1], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(val % 100) * 2]));
		*oriStr = MyString_StrDigit100U16[(val / 100) * 2 + 1];
		oriStr += 5;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF32Char> Text::StrInt16(UnsafeArray<UTF32Char> oriStr, Int16 val)
{
	UInt16 uval;
	if (val < 0)
	{
		uval = (UInt16)-val;
		*oriStr++ = '-';
	}
	else
	{
		uval = (UInt16)val;
	}
	if (uval < 10)
	{
		*oriStr++ = MyString_StrDigit100U32[uval * 2 + 1];
	}
	else if (uval < 100)
	{
		WriteNInt64((UInt8*)&oriStr[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[uval * 2]));
		oriStr += 2;
	}
	else if (uval < 1000)
	{
		WriteNInt64((UInt8*)&oriStr[1], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(uval % 100) * 2]));
		*oriStr = MyString_StrDigit100U32[(uval / 100) * 2 + 1];
		oriStr += 3;
	}
	else if (uval < 10000)
	{
		WriteNInt64((UInt8*)&oriStr[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(uval / 100) * 2]));
		WriteNInt64((UInt8*)&oriStr[2], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(uval % 100) * 2]));
		oriStr += 4;
	}
	else
	{
		WriteNInt64((UInt8*)&oriStr[3], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(uval % 100) * 2]));
		uval = uval / 100;
		WriteNInt64((UInt8*)&oriStr[1], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(uval % 100) * 2]));
		*oriStr = MyString_StrDigit100U32[(uval / 100) * 2 + 1];
		oriStr += 5;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF32Char> Text::StrUInt16(UnsafeArray<UTF32Char> oriStr, UInt16 val)
{
	if (val < 10)
	{
		*oriStr++ = MyString_StrDigit100U32[val * 2 + 1];
	}
	else if (val < 100)
	{
		WriteNInt64((UInt8*)&oriStr[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[val * 2]));
		oriStr += 2;
	}
	else if (val < 1000)
	{
		WriteNInt64((UInt8*)&oriStr[1], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(val % 100) * 2]));
		*oriStr = MyString_StrDigit100U8[(val / 100) * 2 + 1];
		oriStr += 3;
	}
	else if (val < 10000)
	{
		WriteNInt64((UInt8*)&oriStr[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(val / 100) * 2]));
		WriteNInt64((UInt8*)&oriStr[2], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(val % 100) * 2]));
		oriStr += 4;
	}
	else
	{
		WriteNInt64((UInt8*)&oriStr[3], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(val % 100) * 2]));
		val = val / 100;
		WriteNInt64((UInt8*)&oriStr[1], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(val % 100) * 2]));
		*oriStr = MyString_StrDigit100U32[(val / 100) * 2 + 1];
		oriStr += 5;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF16Char> Text::StrInt32(UnsafeArray<UTF16Char> oriStr, Int32 val)
{
	UTF16Char buff[10];
	UnsafeArray<UTF16Char> str;
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
			WriteNInt32((UInt8*)str.Ptr(), ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(uval % 100) * 2]));
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
			WriteNInt32((UInt8*)str.Ptr(), ReadNInt32(&MyString_StrDigit100U16[(val % 100) * 2]));
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt32((UInt8*)str.Ptr(), ReadNInt32(&MyString_StrDigit100U16[val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U16[val * 2 + 1];
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

UnsafeArray<UTF16Char> Text::StrUInt32(UnsafeArray<UTF16Char> oriStr, UInt32 val)
{
	UTF16Char buff[10];
	UnsafeArray<UTF16Char> str;
	str = &buff[10];
	if (val == 0)
	{
		*--str = 0x30;
	}
	else
	{
#if 1
		UInt32 uval = val;
		while (uval)
		{
			str -= 2;
			WriteNInt32((UInt8*)str.Ptr(), ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(uval % 100) * 2]));
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
			WriteNInt32((UInt8*)str.Ptr(), ReadNInt32(&MyString_StrDigit100U16[(val % 100) * 2]));
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt32((UInt8*)str.Ptr(), ReadNInt32(&MyString_StrDigit100U16[val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U16[val * 2 + 1];
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

UnsafeArray<UTF32Char> Text::StrInt32(UnsafeArray<UTF32Char> oriStr, Int32 val)
{
	UTF32Char buff[10];
	UnsafeArray<UTF32Char> str;
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
		while (val)
		{
			str -= 2;
			WriteNInt64((UInt8*)str.Ptr(), ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(val % 100) * 2]));
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
			WriteNInt64((UInt8*)str.Ptr(), ReadNInt64(&MyString_StrDigit100U32[(val % 100) * 2]));
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt64((UInt8*)str.Ptr(), ReadNInt64(&MyString_StrDigit100U32[val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U32[val * 2 + 1];
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

UnsafeArray<UTF32Char> Text::StrUInt32(UnsafeArray<UTF32Char> oriStr, UInt32 val)
{
	UTF32Char buff[10];
	UnsafeArray<UTF32Char> str;
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
			WriteNInt64((UInt8*)str.Ptr(), ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(val % 100) * 2]));
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
			WriteNInt64((UInt8*)str.Ptr(), ReadNInt64(&MyString_StrDigit100U32[(val % 100) * 2]));
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt64((UInt8*)str.Ptr(), ReadNInt64(&MyString_StrDigit100U32[val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U32[val * 2 + 1];
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

UnsafeArray<UTF16Char> Text::StrInt32S(UnsafeArray<UTF16Char> oriStr, Int32 val, UTF16Char seperator, IntOS sepCnt)
{
	UTF16Char buff[20];
	UnsafeArray<UTF16Char> str;
	IntOS i;
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
		*--str = (UTF16Char)(0x30 + val % 10);
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

UnsafeArray<UTF32Char> Text::StrInt32S(UnsafeArray<UTF32Char> oriStr, Int32 val, UTF32Char seperator, IntOS sepCnt)
{
	UTF32Char buff[20];
	UnsafeArray<UTF32Char> str;
	IntOS i;
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
		*--str = (UTF32Char)(0x30 + val % 10);
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
UnsafeArray<UTF16Char> Text::StrInt64(UnsafeArray<UTF16Char> oriStr, Int64 val)
{
	UTF16Char buff[20];
	UnsafeArray<UTF16Char> str;
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
			WriteNInt32(str, ReadNInt32(&MyString_StrDigit100U16[(val % 100) * 2]));
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
			WriteNInt32(str.Ptr(), ReadNInt32(&MyString_StrDigit100U16[(val % 100) * 2]));
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt32(str.Ptr(), ReadNInt32(&MyString_StrDigit100U16[val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U16[val * 2 + 1];
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

UnsafeArray<UTF16Char> Text::StrUInt64(UnsafeArray<UTF16Char> oriStr, UInt64 val)
{
	UTF16Char buff[20];
	UnsafeArray<UTF16Char> str;
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
			WriteNInt32(str.Ptr(), ReadNInt32(&MyString_StrDigit100U16[(val % 100) * 2]));
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
			WriteNInt32(str.Ptr(), ReadNInt32(&MyString_StrDigit100U16[(val % 100) * 2]));
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt32(str.Ptr(), ReadNInt32(&MyString_StrDigit100U16[val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U16[val * 2 + 1];
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

UnsafeArray<UTF32Char> Text::StrInt64(UnsafeArray<UTF32Char> oriStr, Int64 val)
{
	UTF32Char buff[20];
	UnsafeArray<UTF32Char> str;
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
		while (val >= 100)
		{
			str -= 2;
			WriteNInt64(str.Ptr(), ReadNInt64(&MyString_StrDigit100U32[(val % 100) * 2]));
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt64(str.Ptr(), ReadNInt64(&MyString_StrDigit100U32[val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U32[val * 2 + 1];
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF32Char> Text::StrUInt64(UnsafeArray<UTF32Char> oriStr, UInt64 val)
{
	UTF32Char buff[20];
	UnsafeArray<UTF32Char> str;
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
			WriteNInt64(str.Ptr(), ReadNInt64(&MyString_StrDigit100U32[(val % 100) * 2]));
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
			WriteNInt64(str.Ptr(), ReadNInt64(&MyString_StrDigit100U32[(val % 100) * 2]));
			val = val / 100;
		}
		if (val >= 10)
		{
			str -= 2;
			WriteNInt64(str.Ptr(), ReadNInt64(&MyString_StrDigit100U32[val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U32[val * 2 + 1];
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

UnsafeArray<UTF16Char> Text::StrInt64S(UnsafeArray<UTF16Char> oriStr, Int64 val, UTF16Char seperator, IntOS sepCnt)
{
	UTF16Char buff[40];
	UnsafeArray<UTF16Char> str;
	IntOS i;
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
		*--str = (UTF16Char)(0x30 + val % 10);
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

UnsafeArray<UTF32Char> Text::StrInt64S(UnsafeArray<UTF32Char> oriStr, Int64 val, UTF32Char seperator, IntOS sepCnt)
{
	UTF32Char buff[40];
	UnsafeArray<UTF32Char> str;
	IntOS i;
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
		*--str = (UTF32Char)(0x30 + val % 10);
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
UnsafeArray<UTF16Char> Text::StrInt64(UnsafeArray<UTF16Char> oriStr, Int64 val)
{
	UTF16Char buff[20];
	UnsafeArray<UTF16Char> str;
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
			WriteNInt32((UInt8*)&str[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(val % 100) * 2]));
			val = val / 100;
		}
		UInt32 i32val = (UInt32)val;
		while (i32val >= 100)
		{
			str -= 2;
			WriteNInt32((UInt8*)&str[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(i32val % 100) * 2]));
			i32val = i32val / 100;
		}
		if (i32val >= 10)
		{
			str -= 2;
			WriteNInt32((UInt8*)&str[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[i32val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U16[i32val * 2 + 1];
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF16Char> Text::StrUInt64(UnsafeArray<UTF16Char> oriStr, UInt64 val)
{
	UTF16Char buff[20];
	UnsafeArray<UTF16Char> str;
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
			WriteNInt32((UInt8*)&str[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(val % 100) * 2]));
			val = val / 100;
		}
		UInt32 i32val = (UInt32)val;
		while (i32val >= 100)
		{
			str -= 2;
			WriteNInt32((UInt8*)&str[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[(i32val % 100) * 2]));
			i32val = i32val / 100;
		}
		if (i32val >= 10)
		{
			str -= 2;
			WriteNInt32((UInt8*)&str[0], ReadNInt32((const UInt8*)&MyString_StrDigit100U16[i32val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U16[i32val * 2 + 1];
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF32Char> Text::StrInt64(UnsafeArray<UTF32Char> oriStr, Int64 val)
{
	UTF32Char buff[20];
	UnsafeArray<UTF32Char> str;
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
			WriteNInt64((UInt8*)&str[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(val % 100) * 2]));
			val = val / 100;
		}
		UInt32 i32val = (UInt32)val;
		while (i32val >= 100)
		{
			str -= 2;
			WriteNInt64((UInt8*)&str[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(i32val % 100) * 2]));
			i32val = i32val / 100;
		}
		if (i32val >= 10)
		{
			str -= 2;
			WriteNInt64((UInt8*)&str[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[i32val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U32[i32val * 2 + 1];
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF32Char> Text::StrUInt64(UnsafeArray<UTF32Char> oriStr, UInt64 val)
{
	UTF32Char buff[20];
	UnsafeArray<UTF32Char> str;
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
			WriteNInt64((UInt8*)&str[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(val % 100) * 2]));
			val = val / 100;
		}
		UInt32 i32val = (UInt32)val;
		while (i32val >= 100)
		{
			str -= 2;
			WriteNInt64((UInt8*)&str[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[(i32val % 100) * 2]));
			i32val = i32val / 100;
		}
		if (i32val >= 10)
		{
			str -= 2;
			WriteNInt64((UInt8*)&str[0], ReadNInt64((const UInt8*)&MyString_StrDigit100U32[i32val * 2]));
		}
		else
		{
			str -= 1;
			*str = MyString_StrDigit100U32[i32val * 2 + 1];
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF16Char> Text::StrInt64S(UnsafeArray<UTF16Char> oriStr, Int64 val, UTF16Char seperator, IntOS sepCnt)
{
	UTF16Char buff[40];
	UnsafeArray<UTF16Char> str;
	IntOS i;
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
		*--str = (UTF16Char)(0x30 + val % 10);
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

UnsafeArray<UTF32Char> Text::StrInt64S(UnsafeArray<UTF32Char> oriStr, Int64 val, UTF32Char seperator, IntOS sepCnt)
{
	UTF32Char buff[40];
	UnsafeArray<UTF32Char> str;
	IntOS i;
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
		*--str = (UTF32Char)(0x30 + val % 10);
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

UnsafeArray<UTF16Char> Text::StrToUpper(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToJoin)
{
	UTF16Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 'a' && c <= 'z')
		{
			*oriStr++ = (UTF16Char)(c - 32);
		}
		else
		{
			*oriStr++ = c;
		}
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF32Char> Text::StrToUpper(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToJoin)
{
	UTF32Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 'a' && c <= 'z')
		{
			*oriStr++ = (c - 32);
		}
		else
		{
			*oriStr++ = c;
		}
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF16Char> Text::StrToLower(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToJoin)
{
	UTF16Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			*oriStr++ = (UTF16Char)(c + 32);
		}
		else
		{
			*oriStr++ = c;
		}
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF32Char> Text::StrToLower(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToJoin)
{
	UTF32Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			*oriStr++ = (UTF32Char)(c + 32);
		}
		else
		{
			*oriStr++ = c;
		}
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF16Char> Text::StrToCapital(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToJoin)
{
	Bool lastLetter = false;
	UTF16Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			if (lastLetter)
			{
				*oriStr++ = (UTF16Char)(c + 32);
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
				*oriStr++ = (UTF16Char)(c - 32);
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

UnsafeArray<UTF32Char> Text::StrToCapital(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToJoin)
{
	Bool lastLetter = false;
	UTF32Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			if (lastLetter)
			{
				*oriStr++ = (UTF32Char)(c + 32);
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
				*oriStr++ = (UTF32Char)(c - 32);
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

Bool Text::StrEquals(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2)
{
	UTF16Char c;
	while ((c = *str1++) != 0)
	{
		if (c != *str2++)
			return false;
	}
	return *str2 == 0;
}

Bool Text::StrEquals(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2)
{
	UTF32Char c;
	while ((c = *str1++) != 0)
	{
		if (c != *str2++)
			return false;
	}
	return *str2 == 0;
}

Bool Text::StrEqualsICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2)
{
	UTF16Char c1;
	UTF16Char c2;
	while ((c1 = *str1++) != 0)
	{
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 = (UTF16Char)(c1 - 0x20);
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (UTF16Char)(c2 - 0x20);
		}
		if (c1 != c2)
			return false;
	}
	return *str2 == 0;
}

Bool Text::StrEqualsICaseASCII(UnsafeArray<const UTF16Char> str1, UnsafeArray<const Char> str2)
{
	UTF16Char c1;
	Char c2;
	while ((c1 = *str1++) != 0)
	{
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 = (UTF16Char)(c1 - 0x20);
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (Char)(c2 - 0x20);
		}
		if (c1 != (UTF16Char)c2)
			return false;
	}
	return *str2 == 0;
}

Bool Text::StrEqualsICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2, IntOS str2Len)
{
	UTF16Char c1;
	UTF16Char c2;
	while (str2Len-- > 0)
	{
		c1 = *str1++;
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
			c1 = (UTF16Char)(c1 - 32);
		if (c2 >= 'a' && c2 <= 'z')
			c2 = (UTF16Char)(c2 - 32);
		if (c1 != c2)
			return false;
	}
	if (*str1 == 0)
		return true;
	return false;
}

Bool Text::StrEqualsICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2)
{
	UTF32Char c1;
	UTF32Char c2;
	while ((c1 = *str1++) != 0)
	{
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 = (UTF32Char)(c1 - 0x20);
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (UTF32Char)(c2 - 0x20);
		}
		if (c1 != c2)
			return false;
	}
	return *str2 == 0;
}

Bool Text::StrEqualsICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2, IntOS str2Len)
{
	UTF32Char c1;
	UTF32Char c2;
	while (str2Len-- > 0)
	{
		c1 = *str1++;
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
			c1 = (UTF32Char)(c1 - 32);
		if (c2 >= 'a' && c2 <= 'z')
			c2 = (UTF32Char)(c2 - 32);
		if (c1 != c2)
			return false;
	}
	if (*str1 == 0)
		return true;
	return false;
}

UIntOS Text::StrCharCntS(UnsafeArray<const UTF16Char> str, UIntOS maxLen)
{
	UnsafeArray<const UTF16Char> currPtr = str;
	UnsafeArray<const UTF16Char> endPtr = str + maxLen;
	while (currPtr < endPtr && *currPtr++);
	return (UIntOS)(currPtr - str);
}

UIntOS Text::StrCharCntS(UnsafeArray<const UTF32Char> str, UIntOS maxLen)
{
	UnsafeArray<const UTF32Char> currPtr = str;
	UnsafeArray<const UTF32Char> endPtr = str + maxLen;
	while (currPtr < endPtr && *currPtr++);
	return (UIntOS)(currPtr - str);
}

UnsafeArray<UTF16Char> Text::StrHexVal16(UnsafeArray<UTF16Char> oriStr, UInt16 val)
{
	WriteNInt32((UInt8*)&oriStr[0], ReadNInt32((const UInt8*)&MyString_StrHexArrU16[(val >> 8) * 2]));
	WriteNInt32((UInt8*)&oriStr[2], ReadNInt32((const UInt8*)&MyString_StrHexArrU16[(val & 0xff) * 2]));
	oriStr[4] = 0;
	return &oriStr[4];
}

UnsafeArray<UTF32Char> Text::StrHexVal16(UnsafeArray<UTF32Char> oriStr, UInt16 val)
{
	WriteNInt64((UInt8*)&oriStr[0], ReadNInt64((const UInt8*)&MyString_StrHexArrU32[(val >> 8) * 2]));
	WriteNInt64((UInt8*)&oriStr[2], ReadNInt64((const UInt8*)&MyString_StrHexArrU32[(val & 0xff) * 2]));
	oriStr[4] = 0;
	return &oriStr[4];
}

UnsafeArray<UTF16Char> Text::StrHexByte(UnsafeArray<UTF16Char> oriStr, UInt8 val)
{
	WriteNInt32((UInt8*)&oriStr[0], ReadNInt32((const UInt8*)&MyString_StrHexArrU16[val * 2]));
	oriStr[2] = 0;
	return &oriStr[2];
}

UnsafeArray<UTF32Char> Text::StrHexByte(UnsafeArray<UTF32Char> oriStr, UInt8 val)
{
	WriteNInt64((UInt8*)&oriStr[0], ReadNInt64((const UInt8*)&MyString_StrHexArrU32[val * 2]));
	oriStr[2] = 0;
	return &oriStr[2];
}

UnsafeArray<UTF16Char> Text::StrHexBytes(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UInt8> buff, UIntOS buffSize, UTF16Char seperator)
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

UnsafeArray<UTF32Char> Text::StrHexBytes(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UInt8> buff, UIntOS buffSize, UTF32Char seperator)
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

Int64 Text::StrHex2Int64WC(UnsafeArray<const UTF16Char> str)
{
	IntOS i = 16;
	Int64 outVal = 0;
	UTF16Char c;
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

Int64 Text::StrHex2Int64WC(UnsafeArray<const UTF32Char> str)
{
	IntOS i = 16;
	Int64 outVal = 0;
	UTF32Char c;
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

Int32 Text::StrHex2Int32WC(UnsafeArray<const UTF16Char> str)
{
	IntOS i = 8;
	Int32 outVal = 0;
	UTF16Char c;
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

Int32 Text::StrHex2Int32WC(UnsafeArray<const UTF32Char> str)
{
	IntOS i = 8;
	Int32 outVal = 0;
	UTF32Char c;
	while (i-- > 0)
	{
		c = *str++;
		if (c == 0)
			return outVal;
		if (c >= '0' && c <= '9')
		{
			outVal = (outVal << 4) | (Int32)(c - 48);
		}
		else if (c >= 'A' && c <= 'F')
		{
			outVal = (outVal << 4) | (Int32)(c - 0x37);
		}
		else if (c >= 'a' && c <= 'f')
		{
			outVal = (outVal << 4) | (Int32)(c - 0x57);
		}
		else
		{
			return 0;
		}
	}
	return outVal;
}

Int16 Text::StrHex2Int16WC(UnsafeArray<const UTF16Char> str)
{
	IntOS i = 8;
	Int16 outVal = 0;
	UTF16Char c;
	while (i-- > 0)
	{
		c = *str++;
		if (c == 0)
			return outVal;
		if (c >= '0' && c <= '9')
		{
			outVal = (Int16)((outVal << 4) | (c - 48));
		}
		else if (c >= 'A' && c <= 'F')
		{
			outVal = (Int16)((outVal << 4) | (c - 0x37));
		}
		else if (c >= 'a' && c <= 'f')
		{
			outVal = (Int16)((outVal << 4) | (c - 0x57));
		}
		else
		{
			return 0;
		}
	}
	return outVal;
}

Int16 Text::StrHex2Int16WC(UnsafeArray<const UTF32Char> str)
{
	IntOS i = 8;
	Int16 outVal = 0;
	UTF32Char c;
	while (i-- > 0)
	{
		c = *str++;
		if (c == 0)
			return outVal;
		if (c >= '0' && c <= '9')
		{
			outVal = (Int16)((outVal << 4) | (Int16)(c - 48));
		}
		else if (c >= 'A' && c <= 'F')
		{
			outVal = (Int16)((outVal << 4) | (Int16)(c - 0x37));
		}
		else if (c >= 'a' && c <= 'f')
		{
			outVal = (Int16)((outVal << 4) | (Int16)(c - 0x57));
		}
		else
		{
			return 0;
		}
	}
	return outVal;
}

UInt8 Text::StrHex2UInt8WC(UnsafeArray<const UTF16Char> str)
{
	UInt8 outVal = 0;
	UTF16Char c;
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
	if (*str == 0)
		return outVal;
	return 0;
}

UInt8 Text::StrHex2UInt8WC(UnsafeArray<const UTF32Char> str)
{
	UInt8 outVal = 0;
	UTF32Char c;
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
		outVal = (UInt8)((outVal << 4) | (UInt8)(c - 48));
	}
	else if (c >= 'A' && c <= 'F')
	{
		outVal = (UInt8)((outVal << 4) | (UInt8)(c - 0x37));
	}
	else if (c >= 'a' && c <= 'f')
	{
		outVal = (UInt8)((outVal << 4) | (UInt8)(c - 0x57));
	}
	else
	{
		return 0;
	}
	if (*str == 0)
		return outVal;
	return 0;
}

UIntOS Text::StrHex2BytesW(UnsafeArray<const UTF16Char> str, UnsafeArray<UInt8> buff)
{
	UIntOS outVal = 0;
	UInt8 tmpVal;
	UTF16Char c;
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
			tmpVal = (UInt8)(tmpVal | (c - 48));
		}
		else if (c >= 'A' && c <= 'F')
		{
			tmpVal = (UInt8)(tmpVal | (c - 0x37));
		}
		else if (c >= 'a' && c <= 'f')
		{
			tmpVal = (UInt8)(tmpVal | (c - 0x57));
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

UIntOS Text::StrHex2BytesW(UnsafeArray<const UTF32Char> str, UnsafeArray<UInt8> buff)
{
	UIntOS outVal = 0;
	UInt8 tmpVal;
	UTF32Char c;
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
			tmpVal = (UInt8)(tmpVal | (c - 48));
		}
		else if (c >= 'A' && c <= 'F')
		{
			tmpVal = (UInt8)(tmpVal | (c - 0x37));
		}
		else if (c >= 'a' && c <= 'f')
		{
			tmpVal = (UInt8)(tmpVal | (c - 0x57));
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

Int64 Text::StrOct2Int64W(UnsafeArray<const UTF16Char> str)
{
	Int32 i = 22;
	Int32 outVal = 0;
	UTF16Char c;
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

Int64 Text::StrOct2Int64W(UnsafeArray<const UTF32Char> str)
{
	Int32 i = 22;
	Int32 outVal = 0;
	UTF32Char c;
	while (i-- > 0)
	{
		c = *str++;
		if (c == 0)
			return outVal;
		if (c >= '0' && c <= '7')
		{
			outVal = (outVal << 3) | (Int32)(c - 48);
		}
		else
		{
			return outVal;
		}
	}
	return outVal;
}

UIntOS Text::StrSplit(UnsafeArray<UnsafeArray<UTF16Char>> strs, UIntOS maxStrs, UnsafeArray<UTF16Char> strToSplit, UTF16Char splitChar)
{
	UIntOS i = 0;
	UTF16Char c;
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

UIntOS Text::StrSplit(UnsafeArray<UnsafeArray<UTF32Char>> strs, UIntOS maxStrs, UnsafeArray<UTF32Char> strToSplit, UTF32Char splitChar)
{
	UIntOS i = 0;
	UTF32Char c;
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

UIntOS Text::StrSplitTrim(UnsafeArray<UnsafeArray<UTF16Char>> strs, UIntOS maxStrs, UnsafeArray<UTF16Char> strToSplit, UTF16Char splitChar)
{
	UIntOS i = 0;
	UTF16Char c;
	UnsafeArray<UTF16Char> lastPtr;
	UnsafeArray<UTF16Char> thisPtr;
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

UIntOS Text::StrSplitTrim(UnsafeArray<UnsafeArray<UTF32Char>> strs, UIntOS maxStrs, UnsafeArray<UTF32Char> strToSplit, UTF32Char splitChar)
{
	UIntOS i = 0;
	UTF32Char c;
	UnsafeArray<UTF32Char> lastPtr;
	UnsafeArray<UTF32Char> thisPtr;
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

UIntOS Text::StrSplitLine(UnsafeArray<UnsafeArray<UTF16Char>> strs, UIntOS maxStrs, UnsafeArray<UTF16Char> strToSplit)
{
	UIntOS i = 0;
	UTF16Char c;
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

UIntOS Text::StrSplitLine(UnsafeArray<UnsafeArray<UTF32Char>> strs, UIntOS maxStrs, UnsafeArray<UTF32Char> strToSplit)
{
	UIntOS i = 0;
	UTF32Char c;
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

UIntOS Text::StrSplitWS(UnsafeArray<UnsafeArray<UTF16Char>> strs, UIntOS maxStrs, UnsafeArray<UTF16Char> strToSplit)
{
	UIntOS i = 0;
	UTF16Char c;
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

UIntOS Text::StrSplitWS(UnsafeArray<UnsafeArray<UTF32Char>> strs, UIntOS maxStrs, UnsafeArray<UTF32Char> strToSplit)
{
	UIntOS i = 0;
	UTF32Char c;
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

Bool Text::StrToUInt8W(UnsafeArray<const UTF16Char> intStr, OutParam<UInt8> outVal)
{
	UInt32 retVal = 0;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return false;
		retVal = retVal * 10 + *intStr - 48;
		intStr++;
		if (retVal & 0xffffff00)
			return false;
	}
	outVal.Set((UInt8)retVal);
	return true;
}

Bool Text::StrToUInt8W(UnsafeArray<const UTF32Char> intStr, OutParam<UInt8> outVal)
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
	outVal.Set((UInt8)retVal);
	return true;
}

UInt8 Text::StrToUInt8W(UnsafeArray<const UTF16Char> intStr)
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

UInt8 Text::StrToUInt8W(UnsafeArray<const UTF32Char> intStr)
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

Bool Text::StrToUInt16W(UnsafeArray<const UTF16Char> intStr, OutParam<UInt16> outVal)
{
	UInt32 retVal = 0;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return false;
		retVal = retVal * 10 + (UInt32)*intStr - 48;
		intStr++;
		if (retVal & 0xffff0000)
			return false;
	}
	outVal.Set((UInt16)retVal);
	return true;
}

Bool Text::StrToUInt16W(UnsafeArray<const UTF32Char> intStr, OutParam<UInt16> outVal)
{
	UInt32 retVal = 0;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return false;
		retVal = retVal * 10 + (UInt32)*intStr - 48;
		intStr++;
		if (retVal & 0xffff0000)
			return false;
	}
	outVal.Set((UInt16)retVal);
	return true;
}

Bool Text::StrToInt16W(UnsafeArray<const UTF16Char> intStr, OutParam<Int16> outVal)
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
	if (*intStr == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int16WC(&intStr[2]);
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
		outVal.Set((Int16)-retVal);
	else
		outVal.Set((Int16)retVal);
	return true;
}

Bool Text::StrToInt16W(UnsafeArray<const UTF32Char> intStr, OutParam<Int16> outVal)
{
	Bool sign;
	Int16 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	if (*intStr == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int16WC(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return false;
			retVal = (Int16)(retVal * 10 + (Int32)*intStr - 48);
			intStr++;
		}
	}
	if (sign)
		outVal.Set((Int16)-retVal);
	else
		outVal.Set(retVal);
	return true;
}

Int16 Text::StrToInt16W(UnsafeArray<const UTF16Char> intStr)
{
	Bool sign;
	Int16 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	if (*intStr == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int16WC(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return 0;
			retVal = (Int16)(retVal * 10 + (Int32)*intStr - 48);
			intStr++;
		}
	}
	if (sign)
		return (Int16)-retVal;
	else
		return retVal;
}

Int16 Text::StrToInt16W(UnsafeArray<const UTF32Char> intStr)
{
	Bool sign;
	Int16 retVal = 0;
	if (*intStr == '-')
	{
		sign = true;
		intStr++;
	}
	else
	{
		sign = false;
	}
	if (*intStr == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int16WC(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return 0;
			retVal = (Int16)(retVal * 10 + (Int32)*intStr - 48);
			intStr++;
		}
	}
	if (sign)
		return (Int16)-retVal;
	else
		return retVal;
	return true;
}

Bool Text::StrToUInt32W(UnsafeArray<const UTF16Char> intStr, OutParam<UInt32> outVal)
{
	UInt32 retVal = 0;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return false;
		retVal = retVal * 10 + *intStr - 48;
		intStr++;
	}
	outVal.Set(retVal);
	return true;
}

Bool Text::StrToUInt32W(UnsafeArray<const UTF32Char> intStr, OutParam<UInt32> outVal)
{
	UInt32 retVal = 0;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
			return false;
		retVal = retVal * 10 + (UInt32)*intStr - 48;
		intStr++;
	}
	outVal.Set(retVal);
	return true;
}

UInt32 Text::StrToUInt32W(UnsafeArray<const UTF16Char> intStr)
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

UInt32 Text::StrToUInt32W(UnsafeArray<const UTF32Char> intStr)
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

Bool Text::StrToInt32W(UnsafeArray<const UTF16Char> intStr, OutParam<Int32> outVal)
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
	if (*intStr == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int32WC(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return false;
			retVal = retVal * 10 + *intStr - 48;
			intStr++;
		}
	}
	if (sign)
		outVal.Set(-retVal);
	else
		outVal.Set(retVal);
	return true;
}

Bool Text::StrToInt32W(UnsafeArray<const UTF32Char> intStr, OutParam<Int32> outVal)
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
	if (*intStr == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int32WC(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return false;
			retVal = retVal * 10 + (Int32)(*intStr - 48);
			intStr++;
		}
	}
	if (sign)
		outVal.Set(-retVal);
	else
		outVal.Set(retVal);
	return true;
}

Int32 Text::StrToInt32W(UnsafeArray<const UTF16Char> intStr)
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
	if (*intStr == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int32WC(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return 0;
			retVal = retVal * 10 + *intStr - 48;
			intStr++;
		}
	}
	if (sign)
		return -retVal;
	else
		return retVal;
}

Int32 Text::StrToInt32W(UnsafeArray<const UTF32Char> intStr)
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
	if (*intStr == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2Int32WC(&intStr[2]);
	}
	else
	{
		while (*intStr)
		{
			if (*intStr < '0' || *intStr > '9')
				return 0;
			retVal = retVal * 10 + (Int32)(*intStr - 48);
			intStr++;
		}
	}
	if (sign)
		return -retVal;
	else
		return retVal;
	return true;
}

Bool Text::StrToInt64W(UnsafeArray<const UTF16Char> intStr, OutParam<Int64> outVal)
{
	Int64 ret = Text::StrToInt64W(intStr);
	if (ret == 0)
	{
		if (intStr[0] != '0' || intStr[1] != 0)
			return false;
	}
	outVal.Set(ret);
	return true;
}

Bool Text::StrToInt64W(UnsafeArray<const UTF32Char> intStr, OutParam<Int64> outVal)
{
	Int64 ret = Text::StrToInt64W(intStr);
	if (ret == 0)
	{
		if (intStr[0] != '0' || intStr[1] != 0)
			return false;
	}
	outVal.Set(ret);
	return true;
}

Bool Text::StrToUInt64W(UnsafeArray<const UTF16Char> intStr, OutParam<UInt64> outVal)
{
	if (intStr[0] == 0)
		return false;
	UTF16Char c;
	UInt64 v = 0;
	while ((c = *intStr++) != 0)
	{
		if (c < '0' || c > '9')
			return false;
		v = v * 10 + (UInt64)(c - '0');
	}
	outVal.Set(v);
	return true;
}

Bool Text::StrToUInt64W(UnsafeArray<const UTF32Char> intStr, OutParam<UInt64> outVal)
{
	if (intStr[0] == 0)
		return false;
	UTF32Char c;
	UInt64 v = 0;
	while ((c = *intStr++) != 0)
	{
		if (c < '0' || c > '9')
			return false;
		v = v * 10 + (UInt64)(c - '0');
	}
	outVal.Set(v);
	return true;
}

Bool Text::StrToUInt64SW(UnsafeArray<const UTF16Char> intStr, OutParam<UInt64> outVal, UInt64 failVal)
{
	if (intStr[0] == 0)
	{
		outVal.Set(failVal);
		return false;
	}
	UTF16Char c;
	UInt64 v = 0;
	while ((c = *intStr++) != 0)
	{
		if (c < '0' || c > '9')
		{
			outVal.Set(failVal);
			return false;
		}
		v = v * 10 + (UInt64)(c - '0');
	}
	outVal.Set(v);
	return true;
}

Bool Text::StrToUInt64SW(UnsafeArray<const UTF32Char> intStr, OutParam<UInt64> outVal, UInt64 failVal)
{
	if (intStr[0] == 0)
	{
		outVal.Set(failVal);
		return false;
	}
	UTF32Char c;
	UInt64 v = 0;
	while ((c = *intStr++) != 0)
	{
		if (c < '0' || c > '9')
		{
			outVal.Set(failVal);
			return false;
		}
		v = v * 10 + (UInt64)(c - '0');
	}
	outVal.Set(v);
	return true;
}

UInt64 Text::StrToUInt64W(UnsafeArray<const UTF16Char> str)
{
	UInt64 v;
	StrToUInt64SW(str, v, 0);
	return v;
}

UInt64 Text::StrToUInt64W(UnsafeArray<const UTF32Char> str)
{
	UInt64 v;
	StrToUInt64SW(str, v, 0);
	return v;
}

IntOS Text::StrToIntOSW(UnsafeArray<const UTF16Char> str)
{
#if _OSINT_SIZE == 64
	return Text::StrToInt64W(str);
#elif _OSINT_SIZE == 32
	return Text::StrToInt32W(str);
#else
	return Text::StrToInt16W(str);
#endif
}

IntOS Text::StrToIntOSW(UnsafeArray<const UTF32Char> str)
{
#if _OSINT_SIZE == 64
	return Text::StrToInt64W(str);
#elif _OSINT_SIZE == 32
	return Text::StrToInt32W(str);
#else
	return Text::StrToInt16W(str);
#endif
}

Bool Text::StrToIntOSW(UnsafeArray<const UTF16Char> intStr, OutParam<IntOS> outVal)
{
#if _OSINT_SIZE == 64
	return Text::StrToInt64W(intStr, outVal);
#elif _OSINT_SIZE == 32
	return Text::StrToInt32W(intStr, outVal);
#else
	return Text::StrToInt16W(intStr, outVal);
#endif
}

Bool Text::StrToIntOSW(UnsafeArray<const UTF32Char> intStr, OutParam<IntOS> outVal)
{
#if _OSINT_SIZE == 64
	return Text::StrToInt64W(intStr, outVal);
#elif _OSINT_SIZE == 32
	return Text::StrToInt32W(intStr, outVal);
#else
	return Text::StrToInt16W(intStr, outVal);
#endif
}


UIntOS Text::StrToUIntOSW(UnsafeArray<const UTF16Char> str)
{
#if _OSINT_SIZE == 64
	return Text::StrToUInt64W(str);
#elif _OSINT_SIZE == 32
	return Text::StrToUInt32W(str);
#else
	return Text::StrToUInt16W(str);
#endif
}

UIntOS Text::StrToUIntOSW(UnsafeArray<const UTF32Char> str)
{
#if _OSINT_SIZE == 64
	return Text::StrToUInt64W(str);
#elif _OSINT_SIZE == 32
	return Text::StrToUInt32W(str);
#else
	return Text::StrToUInt16W(str);
#endif
}

Bool Text::StrToUIntOSW(UnsafeArray<const UTF16Char> intStr, OutParam<UIntOS> outVal)
{
#if _OSINT_SIZE == 64
	return Text::StrToUInt64W(intStr, outVal);
#elif _OSINT_SIZE == 32
	return Text::StrToUInt32W(intStr, outVal);
#else
	return Text::StrToUInt16W(intStr, outVal);
#endif
}

Bool Text::StrToUIntOSW(UnsafeArray<const UTF32Char> intStr, OutParam<UIntOS> outVal)
{
#if _OSINT_SIZE == 64
	return Text::StrToUInt64W(intStr, outVal);
#elif _OSINT_SIZE == 32
	return Text::StrToUInt32W(intStr, outVal);
#else
	return Text::StrToUInt16W(intStr, outVal);
#endif
}

Bool Text::StrToBoolW(UnsafeArrayOpt<const UTF16Char> str)
{
	UnsafeArray<const UTF16Char> nnstr;
	if (!str.SetTo(nnstr))
	{
		return false;
	}
	else if (nnstr[0] == 'T' || nnstr[0] == 't')
	{
		return true;
	}
	else if (nnstr[0] == 'F' || nnstr[0] == 'f')
	{
		return false;
	}
	else
	{
		return Text::StrToInt32W(nnstr) != 0;
	}
}

Bool Text::StrToBoolW(UnsafeArrayOpt<const UTF32Char> str)
{
	UnsafeArray<const UTF32Char> nnstr;
	if (!str.SetTo(nnstr))
	{
		return false;
	}
	else if (nnstr[0] == 'T' || nnstr[0] == 't')
	{
		return true;
	}
	else if (nnstr[0] == 'F' || nnstr[0] == 'f')
	{
		return false;
	}
	else
	{
		return Text::StrToInt32W(nnstr) != 0;
	}
}

UIntOS Text::StrIndexOfW(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2)
{
	UnsafeArray<const UTF16Char> ptr = str1;
	UnsafeArray<const UTF16Char> ptr2;
	UnsafeArray<const UTF16Char> ptr3;
	IntOS i;
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
			return (UIntOS)(ptr - str1);
		ptr++;
	}
	return INVALID_INDEX;
}

UIntOS Text::StrIndexOfCharW(UnsafeArray<const UTF16Char> str1, UTF16Char c)
{
	UnsafeArray<const UTF16Char> ptr = str1;
	while (*ptr)
		if (*ptr == c)
			return (UIntOS)(ptr - str1);
		else
			ptr++;
	return INVALID_INDEX;
}

UIntOS Text::StrIndexOfW(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2)
{
	UnsafeArray<const UTF32Char> ptr = str1;
	UnsafeArray<const UTF32Char> ptr2;
	UnsafeArray<const UTF32Char> ptr3;
	IntOS i;
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
			return (UIntOS)(ptr - str1);
		ptr++;
	}
	return INVALID_INDEX;
}

UIntOS Text::StrIndexOfCharW(UnsafeArray<const UTF32Char> str1, UTF32Char c)
{
	UnsafeArray<const UTF32Char> ptr = str1;
	while (*ptr)
		if (*ptr == c)
			return (UIntOS)(ptr - str1);
		else
			ptr++;
	return INVALID_INDEX;
}

UIntOS Text::StrIndexOfICaseW(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2)
{
	UnsafeArray<const UTF16Char> ptr = str1;
	UnsafeArray<const UTF16Char> ptr2;
	UnsafeArray<const UTF16Char> ptr3;
	UTF16Char c2;
	UTF16Char c3;
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
					c2 = (UTF16Char)(c2 - 32);
				if (c3 >= 'a' && c3 <= 'z')
					c3 = (UTF16Char)(c3 - 32);
				if (c2 != c3)
				{
					i = 1;
					break;
				}
			}
		}
		if (i == 0)
			return (UIntOS)(ptr - str1);
		ptr++;
	}
	return INVALID_INDEX;
}

UIntOS Text::StrIndexOfICaseW(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2)
{
	UnsafeArray<const UTF32Char> ptr = str1;
	UnsafeArray<const UTF32Char> ptr2;
	UnsafeArray<const UTF32Char> ptr3;
	UTF32Char c2;
	UTF32Char c3;
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
					c2 -= 32;
				if (c3 >= 'a' && c3 <= 'z')
					c3 -= 32;
				if (c2 != c3)
				{
					i = 1;
					break;
				}
			}
		}
		if (i == 0)
			return (UIntOS)(ptr - str1);
		ptr++;
	}
	return INVALID_INDEX;
}

UIntOS Text::StrLastIndexOfCharW(UnsafeArray<const UTF16Char> str1, UTF16Char c)
{
	UnsafeArray<const UTF16Char> sptr;
	UnsafeArray<const UTF16Char> cpos = str1 - 1;
	UTF16Char ch;
	sptr = str1;
	while ((ch = *sptr++) != 0)
	{
		if (ch == c)
			cpos = &sptr[-1];
	}
	return (UIntOS)(cpos - str1);
}

UIntOS Text::StrLastIndexOfCharW(UnsafeArray<const UTF32Char> str1, UTF32Char c)
{
	UnsafeArray<const UTF32Char> sptr;
	UnsafeArray<const UTF32Char> cpos = str1 - 1;
	UTF32Char ch;
	sptr = str1;
	while ((ch = *sptr++) != 0)
	{
		if (ch == c)
			cpos = &sptr[-1];
	}
	return (UIntOS)(cpos - str1);
}

UIntOS Text::StrLastIndexOfW(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2)
{
	UIntOS leng1 = Text::StrCharCnt(str1);
	UIntOS leng2 = Text::StrCharCnt(str2);
	if (leng2 > leng1)
		return INVALID_INDEX;
	UnsafeArray<const UTF16Char> ptr = str1 + leng1 - leng2;
	UnsafeArray<const UTF16Char> ptr2;
	UnsafeArray<const UTF16Char> ptr3;
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
			return (UIntOS)(ptr - str1);
		}
		ptr--;
	}
	return INVALID_INDEX;
}

UIntOS Text::StrLastIndexOfW(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2)
{
	UIntOS leng1 = Text::StrCharCnt(str1);
	UIntOS leng2 = Text::StrCharCnt(str2);
	if (leng2 > leng1)
		return INVALID_INDEX;
	UnsafeArray<const UTF32Char> ptr = str1 + leng1 - leng2;
	UnsafeArray<const UTF32Char> ptr2;
	UnsafeArray<const UTF32Char> ptr3;
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
			return (UIntOS)(ptr - str1);
		}
		ptr--;
	}
	return INVALID_INDEX;
}

UnsafeArray<UTF16Char> Text::StrRTrim(UnsafeArray<UTF16Char> str1)
{
	UnsafeArray<UTF16Char> sp = str1;
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

UnsafeArray<UTF32Char> Text::StrRTrim(UnsafeArray<UTF32Char> str1)
{
	UnsafeArray<UTF32Char> sp = str1;
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

UnsafeArray<UTF16Char> Text::StrLTrim(UnsafeArray<UTF16Char> str1)
{
	UnsafeArray<UTF16Char> sptr;
	if (str1[0] != ' ' && str1[0] != '\t')
	{
		return &str1[Text::StrCharCnt(UnsafeArray<const UTF16Char>(str1))];
	}
	else
	{
		sptr = str1;
		while (*sptr == ' ' || *sptr == '\t')
			sptr++;
		return Text::StrConcat(str1, sptr);
	}
}

UnsafeArray<UTF32Char> Text::StrLTrim(UnsafeArray<UTF32Char> str1)
{
	UnsafeArray<UTF32Char> sptr;
	if (str1[0] != ' ' && str1[0] != '\t')
	{
		return &str1[Text::StrCharCnt(UnsafeArray<const UTF32Char>(str1))];
	}
	else
	{
		sptr = str1;
		while (*sptr == ' ' || *sptr == '\t')
			sptr++;
		return Text::StrConcat(str1, sptr);
	}
}

UnsafeArray<UTF16Char> Text::StrTrim(UnsafeArray<UTF16Char> str1)
{
	UnsafeArray<UTF16Char> sptr = str1;
	UnsafeArray<UTF16Char> sptr2;
	UTF16Char c;
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

UnsafeArray<UTF32Char> Text::StrTrim(UnsafeArray<UTF32Char> str1)
{
	UnsafeArray<UTF32Char> sptr = str1;
	UnsafeArray<UTF32Char> sptr2;
	UTF32Char c;
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

UnsafeArray<UTF16Char> Text::StrTrimWSCRLF(UnsafeArray<UTF16Char> str1)
{
	UnsafeArray<UTF16Char> sptr = str1;
	UnsafeArray<UTF16Char> sptr2;
	UTF16Char c;
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

UnsafeArray<UTF32Char> Text::StrTrimWSCRLF(UnsafeArray<UTF32Char> str1)
{
	UnsafeArray<UTF32Char> sptr = str1;
	UnsafeArray<UTF32Char> sptr2;
	UTF32Char c;
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

UnsafeArray<UTF16Char> Text::StrRemoveChar(UnsafeArray<UTF16Char> str1, UTF16Char c)
{
	UnsafeArray<UTF16Char> sp = str1;
	UTF16Char c2;
	while (*str1)
		if ((c2 = *str1++) != c)
			*sp++ = c2;
	*sp = 0;
	return sp;
}

UnsafeArray<UTF32Char> Text::StrRemoveChar(UnsafeArray<UTF32Char> str1, UTF32Char c)
{
	UnsafeArray<UTF32Char> sp = str1;
	UTF32Char c2;
	while (*str1)
		if ((c2 = *str1++) != c)
			*sp++ = c2;
	*sp = 0;
	return sp;
}

UnsafeArray<const UTF16Char> Text::StrCopyNew(UnsafeArray<const UTF16Char> str1)
{
	UnsafeArray<UTF16Char> s = MemAllocArr(UTF16Char, Text::StrCharCnt(str1) + 1);
	Text::StrConcat(s, str1);
	return s;
}

UnsafeArray<const UTF32Char> Text::StrCopyNew(UnsafeArray<const UTF32Char> str1)
{
	UnsafeArray<UTF32Char> s = MemAllocArr(UTF32Char, Text::StrCharCnt(str1) + 1);
	Text::StrConcat(s, str1);
	return s;
}

UnsafeArray<const UTF16Char> Text::StrCopyNewC(UnsafeArray<const UTF16Char> str1, UIntOS strLen)
{
	UnsafeArray<UTF16Char> s = MemAllocArr(UTF16Char, strLen + 1);
	Text::StrConcatC(s, str1, strLen);
	return s;
}

UnsafeArray<const UTF32Char> Text::StrCopyNewC(UnsafeArray<const UTF32Char> str1, UIntOS strLen)
{
	UnsafeArray<UTF32Char> s = MemAllocArr(UTF32Char, strLen + 1);
	Text::StrConcatC(s, str1, strLen);
	return s;
}

#if _WCHAR_SIZE == 4
UnsafeArray<const WChar> Text::StrCopyNewUTF16_W(UnsafeArray<const UTF16Char> str1)
{
	UIntOS charCnt = Text::StrCharCnt(str1);
	UnsafeArray<WChar> s = MemAllocArr(WChar, charCnt + 1);
	Text::StrUTF16_UTF32(s, str1);
	return s;
}
#endif
UnsafeArray<const UTF8Char> Text::StrToUTF8New(UnsafeArray<const UTF16Char> str1)
{
	UIntOS charCnt = Text::StrUTF16_UTF8Cnt(str1);
	UnsafeArray<UTF8Char> s = MemAllocArr(UTF8Char, charCnt + 1);
	Text::StrUTF16_UTF8(s, str1);
	return s;
}

UnsafeArray<const UTF8Char> Text::StrToUTF8New(UnsafeArray<const UTF32Char> str1)
{
	UIntOS charCnt = Text::StrUTF32_UTF8Cnt(str1);
	UnsafeArray<UTF8Char> s = MemAllocArr(UTF8Char, charCnt + 1);
	Text::StrUTF32_UTF8(s, str1);
	return s;
}

UnsafeArray<const WChar> Text::StrToWCharNew(UnsafeArray<const UTF8Char> str1)
{
	UIntOS charCnt = Text::StrUTF8_WCharCnt(str1);
	UnsafeArray<WChar> s = MemAllocArr(WChar, charCnt + 1);
	Text::StrUTF8_WChar(s, str1, 0);
	return s;
}

void Text::StrDelNew(UnsafeArray<const UTF16Char> newStr)
{
	MemFreeArr(newStr);
}

void Text::StrDelNew(UnsafeArray<const UTF32Char> newStr)
{
	MemFreeArr(newStr);
}

Bool Text::StrStartsWith(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2)
{
	while (*str2)
	{
		if (*str1++ != *str2++)
			return false;
	}
	return true;
}

Bool Text::StrStartsWith(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2)
{
	while (*str2)
	{
		if (*str1++ != *str2++)
			return false;
	}
	return true;
}

Bool Text::StrStartsWithICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2)
{
	UTF16Char c1;
	UTF16Char c2;
	while (*str2)
	{
		c1 = *str1++;
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 = (UTF16Char)(c1 - 32);
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (UTF16Char)(c2 - 32);
		}
		if (c1 != c2)
			return false;
	}
	return true;
}

Bool Text::StrStartsWithICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2)
{
	UTF32Char c1;
	UTF32Char c2;
	while (*str2)
	{
		c1 = *str1++;
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 -= 32;
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 -= 32;
		}
		if (c1 != c2)
			return false;
	}
	return true;
}

Bool Text::StrStartsWithICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const Char> str2)
{
	UTF16Char c1;
	Char c2;
	while (*str2)
	{
		c1 = *str1++;
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 = (UTF16Char)(c1 - 32);
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (Char)(c2 - 32);
		}
		if (c1 != (UTF8Char)c2)
			return false;
	}
	return true;
}

Bool Text::StrStartsWithICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const Char> str2)
{
	UTF32Char c1;
	Char c2;
	while (*str2)
	{
		c1 = *str1++;
		c2 = *str2++;
		if (c1 >= 'a' && c1 <= 'z')
		{
			c1 -= 32;
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (Char)(c2 - 32);
		}
		if (c1 != (UTF8Char)c2)
			return false;
	}
	return true;
}

Bool Text::StrEndsWith(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2)
{
	UnsafeArray<const UTF16Char> ptr1 = str1;
	UnsafeArray<const UTF16Char> ptr2 = str2;
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

Bool Text::StrEndsWith(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2)
{
	UnsafeArray<const UTF32Char> ptr1 = str1;
	UnsafeArray<const UTF32Char> ptr2 = str2;
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

Bool Text::StrEndsWithICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2)
{
	UnsafeArray<const UTF16Char> ptr1 = str1;
	UnsafeArray<const UTF16Char> ptr2 = str2;
	UTF16Char c1;
	UTF16Char c2;
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
			c1 = (UTF16Char)(c1 - 32);
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (UTF16Char)(c2 - 32);
		}
		if (c2 != c1)
			return false;
	}
	return true;
}

Bool Text::StrEndsWithICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2)
{
	UnsafeArray<const UTF32Char> ptr1 = str1;
	UnsafeArray<const UTF32Char> ptr2 = str2;
	UTF32Char c1;
	UTF32Char c2;
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
			c1 -= 32;
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 -= 32;
		}
		if (c2 != c1)
			return false;
	}
	return true;
}

Bool Text::StrIsInt32W(UnsafeArray<const UTF16Char> intStr)
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

Bool Text::StrIsInt32W(UnsafeArray<const UTF32Char> intStr)
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

		retVal = retVal * 10 + (Int32)(*intStr - 48);
		intStr++;
	}
	return true;
}

UIntOS Text::StrReplaceW(UnsafeArray<UTF16Char> str1, UTF16Char oriC, UTF16Char destC)
{
	UTF16Char c;
	UIntOS chrCnt = 0;
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

UIntOS Text::StrReplaceW(UnsafeArray<UTF32Char> str1, UTF32Char oriC, UTF32Char destC)
{
	UTF32Char c;
	UIntOS chrCnt = 0;
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

UIntOS Text::StrReplaceW(UnsafeArray<UTF16Char> str1, UnsafeArray<const UTF16Char> replaceFrom, UnsafeArray<const UTF16Char> replaceTo)
{
	UIntOS cnt;
	UIntOS fromCharCnt;
	UIntOS toCharCnt;
	UIntOS charCnt;
	UnsafeArray<UTF16Char> sptr;
	charCnt = Text::StrCharCnt(UnsafeArray<const UTF16Char>(str1));
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
				MemCopyO(&sptr[toCharCnt], &sptr[fromCharCnt], (charCnt - (UIntOS)(sptr - str1) - fromCharCnt + 1) * sizeof(UTF16Char));
			}
			MemCopyNO(sptr.Ptr(), replaceTo.Ptr(), toCharCnt * sizeof(UTF16Char));
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

UIntOS Text::StrReplaceW(UnsafeArray<UTF32Char> str1, UnsafeArray<const UTF32Char> replaceFrom, UnsafeArray<const UTF32Char> replaceTo)
{
	UIntOS cnt;
	UIntOS fromCharCnt;
	UIntOS toCharCnt;
	UIntOS charCnt;
	UnsafeArray<UTF32Char> sptr;
	charCnt = Text::StrCharCnt(UnsafeArray<const UTF32Char>(str1));
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
				MemCopyO(&sptr[toCharCnt], &sptr[fromCharCnt], (charCnt - (UIntOS)(sptr - str1) - fromCharCnt + 1) * sizeof(UTF32Char));
			}
			MemCopyNO(sptr.Ptr(), replaceTo.Ptr(), toCharCnt * sizeof(UTF32Char));
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

UIntOS Text::StrReplaceICaseW(UnsafeArray<UTF16Char> str1, UnsafeArray<const UTF16Char> replaceFrom, UnsafeArray<const UTF16Char> replaceTo)
{
	UIntOS cnt;
	UIntOS fromCharCnt;
	UIntOS toCharCnt;
	UIntOS charCnt;
	UnsafeArray<UTF16Char> sptr;
	charCnt = Text::StrCharCnt(UnsafeArray<const UTF16Char>(str1));
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
				MemCopyO(&sptr[toCharCnt], &sptr[fromCharCnt], (charCnt - (UIntOS)(sptr - str1) - fromCharCnt + 1) * sizeof(UTF16Char));
			}
			MemCopyNO(sptr.Ptr(), replaceTo.Ptr(), toCharCnt * sizeof(UTF16Char));
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

UIntOS Text::StrReplaceICaseW(UnsafeArray<UTF32Char> str1, UnsafeArray<const UTF32Char> replaceFrom, UnsafeArray<const UTF32Char> replaceTo)
{
	UIntOS cnt;
	UIntOS fromCharCnt;
	UIntOS toCharCnt;
	UIntOS charCnt;
	UnsafeArray<UTF32Char> sptr;
	charCnt = Text::StrCharCnt(UnsafeArray<const UTF32Char>(str1));
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
				MemCopyO(&sptr[toCharCnt], &sptr[fromCharCnt], (charCnt - (UIntOS)(sptr - str1) - fromCharCnt + 1) * sizeof(UTF32Char));
			}
			MemCopyNO(sptr.Ptr(), replaceTo.Ptr(), toCharCnt * sizeof(UTF32Char));
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

UnsafeArray<UTF16Char> Text::StrToCSVRec(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> str1)
{
	UTF16Char c;
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

UnsafeArray<UTF32Char> Text::StrToCSVRec(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> str1)
{
	UTF32Char c;
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

UnsafeArray<const UTF16Char> Text::StrToNewCSVRec(UnsafeArray<const UTF16Char> str1)
{
	UIntOS len = 2;
	UTF16Char c;
	UnsafeArray<const UTF16Char> sptr = str1;
	UnsafeArray<UTF16Char> sptr2;
	UnsafeArray<UTF16Char> outPtr;
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
	outPtr = MemAlloc(UTF16Char, len + 1);
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

UnsafeArray<const UTF32Char> Text::StrToNewCSVRec(UnsafeArray<const UTF32Char> str1)
{
	UIntOS len = 2;
	UTF32Char c;
	UnsafeArray<const UTF32Char> sptr = str1;
	UnsafeArray<UTF32Char> sptr2;
	UnsafeArray<UTF32Char> outPtr;
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
	outPtr = MemAlloc(UTF32Char, len + 1);
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

UIntOS Text::StrCSVSplit(UnsafeArray<UnsafeArray<UTF16Char>> strs, UIntOS maxStrs, UnsafeArray<UTF16Char> strToSplit)
{
	Bool quoted = false;
	Bool first = true;
	UIntOS i = 0;
	UnsafeArray<UTF16Char> strCurr;
	UTF16Char c;
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

UIntOS Text::StrCSVSplit(UnsafeArray<UnsafeArray<UTF32Char>> strs, UIntOS maxStrs, UnsafeArray<UTF32Char> strToSplit)
{
	Bool quoted = false;
	Bool first = true;
	UIntOS i = 0;
	UnsafeArray<UTF32Char> strCurr;
	UTF32Char c;
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

UnsafeArray<UTF16Char> Text::StrCSVJoin(UnsafeArray<UTF16Char> oriStr, UnsafeArray<UnsafeArrayOpt<const UTF16Char>> strs, UIntOS nStrs)
{
	UIntOS i = 0;
	UnsafeArray<const UTF16Char> sptr;
	UTF16Char c;
	while (i < nStrs)
	{
		if (i)
			*oriStr++ = ',';
		*oriStr++ = '"';
		if (strs[i].SetTo(sptr))
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

UnsafeArray<UTF32Char> Text::StrCSVJoin(UnsafeArray<UTF32Char> oriStr, UnsafeArray<UnsafeArrayOpt<const UTF32Char>> strs, UIntOS nStrs)
{
	UIntOS i = 0;
	UnsafeArray<const UTF32Char> sptr;
	UTF32Char c;
	while (i < nStrs)
	{
		if (i)
			*oriStr++ = ',';
		*oriStr++ = '"';
		if (strs[i].SetTo(sptr))
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

UIntOS Text::StrCountChar(UnsafeArray<UTF16Char> str1, UTF16Char c)
{
	UIntOS cnt = 0;
	UTF16Char c2;
	while ((c2 = *str1++) != 0)
		if (c2 == c)
			cnt++;
	return cnt;
}

UIntOS Text::StrCountChar(UnsafeArray<UTF32Char> str1, UTF32Char c)
{
	UIntOS cnt = 0;
	UTF32Char c2;
	while ((c2 = *str1++) != 0)
		if (c2 == c)
			cnt++;
	return cnt;
}

UnsafeArray<UTF16Char> Text::StrUTF8_UTF16C(UnsafeArray<UTF16Char> buff, UnsafeArray<const UTF8Char> bytes, UIntOS byteSize, OptOut<UIntOS> byteConv)
{
	UnsafeArray<const UTF8Char> oriBytes = bytes;
	UTF32Char code;
	UInt8 b;
	while (byteSize > 0)
	{
		b = *bytes++;
		if (b < 0x80)
		{
			*buff++ = b;
			byteSize -= 1;
		}
		else if ((b & 0xe0) == 0xc0)
		{
			if (byteSize < 2)
			{
				bytes--;
				break;
			}
			*buff++ = (UTF16Char)(((UTF32Char)(b & 0x1f) << 6) | (UTF32Char)(*bytes & 0x3f));
			bytes++;
			byteSize -= 2;
		}
		else if ((b & 0xf0) == 0xe0)
		{
			if (byteSize < 3)
			{
				bytes--;
				break;
			}
			*buff++ = (UTF16Char)(((UTF32Char)(b & 0x0f) << 12) | ((UTF32Char)(bytes[0] & 0x3f) << 6) | (UTF32Char)(bytes[1] & 0x3f));
			bytes += 2;
			byteSize -= 3;
		}
		else if ((b & 0xf8) == 0xf0)
		{
			if (byteSize < 4)
			{
				bytes--;
				break;
			}
			code = ((UTF32Char)(b & 0x7) << 18) | ((UTF32Char)(bytes[0] & 0x3f) << 12) | ((UTF32Char)(bytes[1] & 0x3f) << 6) | (UTF32Char)(bytes[2] & 0x3f);
			if (code >= 0x10000)
			{
				*buff++ = (UTF16Char)(((code - 0x10000) >> 10) + 0xd800);
				*buff++ = (UTF16Char)((code & 0x3ff) + 0xdc00);
			}
			else
			{
				*buff++ = (UTF16Char)code;
			}
			bytes += 3;
			byteSize -= 4;
		}
		else if ((b & 0xfc) == 0xf8)
		{
			if (byteSize < 5)
			{
				bytes--;
				break;
			}
			code = ((UTF32Char)(b & 0x3) << 24) | ((UTF32Char)(bytes[0] & 0x3f) << 18) | ((UTF32Char)(bytes[1] & 0x3f) << 12) | ((UTF32Char)(bytes[2] & 0x3f) << 6) | (UTF32Char)(bytes[3] & 0x3f);
			if (code >= 0x10000)
			{
				*buff++ = (UTF16Char)(((code - 0x10000) >> 10) + 0xd800);
				*buff++ = (UTF16Char)((code & 0x3ff) + 0xdc00);
			}
			else
			{
				*buff++ = (UTF16Char)code;
			}
			bytes += 4;
			byteSize -= 5;
		}
		else if ((b & 0xc0) == 0x80)
		{
			*buff++ = b;
			byteSize -= 1;
		}
		else if ((b & 0xfe) == 0xfc)
		{
			if (byteSize < 6)
			{
				bytes--;
				break;
			}
			code = ((UTF32Char)(b & 0x1) << 30) | ((UTF32Char)(bytes[0] & 0x3f) << 24) | ((UTF32Char)(bytes[1] & 0x3f) << 18) | ((UTF32Char)(bytes[2] & 0x3f) << 12) | ((UTF32Char)(bytes[3] & 0x3f) << 6) | (UTF32Char)(bytes[4] & 0x3f);
			if (code >= 0x10000)
			{
				*buff++ = (UTF16Char)(((code - 0x10000) >> 10) + 0xd800);
				*buff++ = (UTF16Char)((code & 0x3ff) + 0xdc00);
			}
			else
			{
				*buff++ = (UTF16Char)code;
			}
			bytes += 5;
			byteSize -= 6;
		}
		else
		{
			break;
		}
	}
	byteConv.Set((UIntOS)(bytes - oriBytes));
	*buff = 0;
	return buff;
}

UIntOS Text::StrUTF8_UTF16CntC(UnsafeArray<const UTF8Char> bytes, UIntOS byteSize)
{
	UIntOS charCnt = 0;
	while (byteSize > 0)
	{
		if ((bytes[0] & 0xfe) == 0xfc && byteSize >= 6)
		{
			charCnt += 2;
			byteSize -= 6;
			bytes += 6;
		}
		else if ((bytes[0] & 0xfc) == 0xf8 && byteSize >= 5)
		{
			charCnt += 2;
			byteSize -= 5;
			bytes += 5;
		}
		else if ((bytes[0] & 0xf8) == 0xf0 && byteSize >= 4)
		{
			charCnt += 2;
			byteSize -= 4;
			bytes += 4;
		}
		else if ((bytes[0] & 0xf0) == 0xe0 && byteSize >= 3)
		{
			charCnt++;
			byteSize -= 3;
			bytes += 3;
		}
		else if ((bytes[0] & 0xe0) == 0xc0 && byteSize >= 2)
		{
			charCnt++;
			byteSize -= 2;
			bytes += 2;
		}
		else
		{
			charCnt++;
			byteSize--;
			bytes++;
		}
	}
	return charCnt;
}

UnsafeArray<UTF32Char> Text::StrUTF8_UTF32C(UnsafeArray<UTF32Char> buff, UnsafeArray<const UTF8Char> bytes, UIntOS byteSize, OptOut<UIntOS> byteConv)
{
	UnsafeArray<const UTF8Char> oriBytes = bytes;
	UTF32Char code;
	UInt8 b;
	while (byteSize > 0)
	{
		b = *bytes++;
		if (b < 0x80)
		{
			*buff++ = b;
			byteSize -= 1;
		}
		else if ((b & 0xe0) == 0xc0)
		{
			if (byteSize < 2)
			{
				bytes--;
				break;
			}
			*buff++ = (UTF32Char)(((UInt32)(b & 0x1f) << 6) | (UInt32)(*bytes & 0x3f));
			bytes++;
			byteSize -= 2;
		}
		else if ((b & 0xf0) == 0xe0)
		{
			if (byteSize < 3)
			{
				bytes--;
				break;
			}
			*buff++ = (UTF32Char)(((UInt32)(b & 0x0f) << 12) | ((UInt32)(bytes[0] & 0x3f) << 6) | (UInt32)(bytes[1] & 0x3f));
			bytes += 2;
			byteSize -= 3;
		}
		else if ((b & 0xf8) == 0xf0)
		{
			if (byteSize < 4)
			{
				bytes--;
				break;
			}
			code = (UTF32Char)(((UInt32)(b & 0x7) << 18) | ((UInt32)(bytes[0] & 0x3f) << 12) | ((UInt32)(bytes[1] & 0x3f) << 6) | (UInt32)(bytes[2] & 0x3f));
			*buff++ = code;
			bytes += 3;
			byteSize -= 4;
		}
		else if ((b & 0xfc) == 0xf8)
		{
			if (byteSize < 5)
			{
				bytes--;
				break;
			}
			code = (UTF32Char)(((UInt32)(b & 0x3) << 24) | ((UInt32)(bytes[0] & 0x3f) << 18) | ((UInt32)(bytes[1] & 0x3f) << 12) | ((UInt32)(bytes[2] & 0x3f) << 6) | (UInt32)(bytes[3] & 0x3f));
			*buff++ = code;
			bytes += 4;
			byteSize -= 5;
		}
		else if ((b & 0xfe) == 0xfc)
		{
			if (byteSize < 6)
			{
				bytes--;
				break;
			}
			code = (UTF32Char)(((UInt32)(b & 0x1) << 30) | ((UInt32)(bytes[0] & 0x3f) << 24) | ((UInt32)(bytes[1] & 0x3f) << 18) | ((UInt32)(bytes[2] & 0x3f) << 12) | ((UInt32)(bytes[3] & 0x3f) << 6) | (UInt32)(bytes[4] & 0x3f));
			*buff++ = code;
			bytes += 5;
			byteSize -= 6;
		}
		else
		{
			break;
		}
	}
	byteConv.Set((UIntOS)(bytes - oriBytes));
	*buff = 0;
	return buff;
}

UIntOS Text::StrUTF8_UTF32CntC(UnsafeArray<const UTF8Char> bytes, UIntOS byteSize)
{
	UIntOS charCnt = 0;
	while (byteSize > 0)
	{
		if ((bytes[0] & 0xfe) == 0xfc && byteSize >= 6)
		{
			charCnt++;
			byteSize -= 6;
			bytes += 6;
		}
		else if ((bytes[0] & 0xfc) == 0xf8 && byteSize >= 5)
		{
			charCnt++;
			byteSize -= 5;
			bytes += 5;
		}
		else if ((bytes[0] & 0xf8) == 0xf0 && byteSize >= 4)
		{
			charCnt++;
			byteSize -= 4;
			bytes += 4;
		}
		else if ((bytes[0] & 0xf0) == 0xe0 && byteSize >= 3)
		{
			charCnt++;
			byteSize -= 3;
			bytes += 3;
		}
		else if ((bytes[0] & 0xe0) == 0xc0 && byteSize >= 2)
		{
			charCnt++;
			byteSize -= 2;
			bytes += 2;
		}
		else
		{
			charCnt++;
			byteSize--;
			bytes++;
		}
	}
	return charCnt;
}


UnsafeArray<UTF16Char> Text::StrUTF8_UTF16(UnsafeArray<UTF16Char> buff, UnsafeArray<const UTF8Char> bytes, OptOut<UIntOS> byteConv)
{
	UnsafeArray<const UTF8Char> oriBytes = bytes;
	UTF32Char code;
	UInt8 b;
	while ((b = *bytes) != 0)
	{
		if (b < 0x80)
		{
			*buff++ = b;
			bytes++;
		}
		else if ((b & 0xe0) == 0xc0)
		{
			*buff++ = (UTF16Char)(((b & 0x1f) << 6) | (bytes[1] & 0x3f));
			bytes += 2;
		}
		else if ((b & 0xf0) == 0xe0)
		{
			*buff++ = (UTF16Char)(((b & 0x0f) << 12) | ((bytes[1] & 0x3f) << 6) | (bytes[2] & 0x3f));
			bytes += 3;
		}
		else if ((b & 0xf8) == 0xf0)
		{
			code = (((UTF32Char)b & 0x7) << 18) | (((UTF32Char)bytes[1] & 0x3f) << 12) | ((UTF32Char)(bytes[2] & 0x3f) << 6) | (UTF32Char)(bytes[3] & 0x3f);
			if (code >= 0x10000)
			{
				buff[0] = (UTF16Char)(((code - 0x10000) >> 10) + 0xd800);
				buff[1] = (UTF16Char)((code & 0x3ff) + 0xdc00);
				buff += 2;
			}
			else
			{
				*buff++ = (UTF16Char)code;
			}
			bytes += 4;
		}
		else if ((b & 0xfc) == 0xf8)
		{
			code = (((UTF32Char)b & 0x3) << 24) | (((UTF32Char)bytes[1] & 0x3f) << 18) | (((UTF32Char)bytes[2] & 0x3f) << 12) | ((UTF32Char)(bytes[3] & 0x3f) << 6) | (bytes[4] & 0x3f);
			if (code >= 0x10000)
			{
				buff[0] = (UTF16Char)(((code - 0x10000) >> 10) + 0xd800);
				buff[1] = (UTF16Char)((code & 0x3ff) + 0xdc00);
				buff += 2;
			}
			else
			{
				*buff++ = (UTF16Char)code;
			}
			bytes += 5;
		}
		else if ((b & 0xc0) == 0x80)
		{
			*buff++ = b;
			bytes += 1;
		}
		else if ((b & 0xfe) == 0xfc)
		{
			code = (((UTF32Char)b & 0x1) << 30) | (((UTF32Char)bytes[1] & 0x3f) << 24) | (((UTF32Char)bytes[2] & 0x3f) << 18) | (((UTF32Char)bytes[3] & 0x3f) << 12) | ((UTF32Char)(bytes[4] & 0x3f) << 6) | (UTF32Char)(bytes[5] & 0x3f);
			if (code >= 0x10000)
			{
				buff[0] = (UTF16Char)(((code - 0x10000) >> 10) + 0xd800);
				buff[1] = (UTF16Char)((code & 0x3ff) + 0xdc00);
				buff += 2;
			}
			else
			{
				*buff++ = (UTF16Char)code;
			}
			bytes += 6;
		}
		else
		{
			*buff++ = b;
			bytes++;
		}
	}
	byteConv.Set((UIntOS)(bytes - oriBytes + 1));
	*buff = 0;
	return buff;
}

UIntOS Text::StrUTF8_UTF16Cnt(UnsafeArray<const UTF8Char> bytes)
{
	UIntOS charCnt = 0;
	while (bytes[0])
	{
		if ((bytes[0] & 0x80) == 0)
		{
			charCnt++;
			bytes++;
		}
		else if ((bytes[0] & 0xe0) == 0xc0)
		{
			charCnt++;
			bytes += 2;
		}
		else if ((bytes[0] & 0xf0) == 0xe0)
		{
			charCnt++;
			bytes += 3;
		}
		else if ((bytes[0] & 0xf8) == 0xf0)
		{
			charCnt += 2;
			bytes += 4;
		}
		else if ((bytes[0] & 0xfc) == 0xf8)
		{
			charCnt += 2;
			bytes += 5;
		}
		else if ((bytes[0] & 0xc0) == 0x80)
		{
			charCnt++;
			bytes += 1;
		}
		else
		{
			charCnt += 2;
			bytes += 6;
		}
	}
	return charCnt;
}

UnsafeArray<UTF32Char> Text::StrUTF8_UTF32(UnsafeArray<UTF32Char> buff, UnsafeArray<const UTF8Char> bytes, OptOut<UIntOS> byteConv)
{
	UnsafeArray<const UTF8Char> oriBytes = bytes;
	UTF32Char code;
	UInt8 b;
	while ((b = *bytes++) != 0)
	{
		if (b < 0x80)
		{
			*buff++ = b;
		}
		else if ((b & 0xe0) == 0xc0)
		{
			*buff++ = (UTF32Char)(((b & 0x1f) << 6) | (*bytes & 0x3f));
			bytes++;
		}
		else if ((b & 0xf0) == 0xe0)
		{
			*buff++ = (((UTF32Char)b & 0x0f) << 12) | ((UTF32Char)(bytes[0] & 0x3f) << 6) | (UTF32Char)(bytes[1] & 0x3f);
			bytes += 2;
		}
		else if ((b & 0xf8) == 0xf0)
		{
			code = (((UTF32Char)b & 0x7) << 18) | (((UTF32Char)bytes[0] & 0x3f) << 12) | ((UTF32Char)(bytes[1] & 0x3f) << 6) | (UTF32Char)(bytes[2] & 0x3f);
			*buff++ = code;
			bytes += 3;
		}
		else if ((b & 0xfc) == 0xf8)
		{
			code = (((UTF32Char)b & 0x3) << 24) | (((UTF32Char)bytes[0] & 0x3f) << 18) | (((UTF32Char)bytes[1] & 0x3f) << 12) | ((UTF32Char)(bytes[2] & 0x3f) << 6) | (UTF32Char)(bytes[3] & 0x3f);
			*buff++ = code;
			bytes += 4;
		}
		else if ((b & 0xfe) == 0xfc)
		{
			code = (((UTF32Char)b & 0x1) << 30) | (((UTF32Char)bytes[0] & 0x3f) << 24) | (((UTF32Char)bytes[1] & 0x3f) << 18) | (((UTF32Char)bytes[2] & 0x3f) << 12) | ((UTF32Char)(bytes[3] & 0x3f) << 6) | (UTF32Char)(bytes[4] & 0x3f);
			*buff++ = code;
			bytes += 5;
		}
		else
		{
			break;
		}
	}
	byteConv.Set((UIntOS)(bytes - oriBytes));
	*buff = 0;
	return buff;
}

UIntOS Text::StrUTF8_UTF32Cnt(UnsafeArray<const UTF8Char> bytes)
{
	UIntOS charCnt = 0;
	while (bytes[0])
	{
		if ((bytes[0] & 0x80) == 0)
		{
			charCnt++;
			bytes++;
		}
		else if ((bytes[0] & 0xe0) == 0xc0)
		{
			charCnt++;
			bytes += 2;
		}
		else if ((bytes[0] & 0xf0) == 0xe0)
		{
			charCnt++;
			bytes += 3;
		}
		else if ((bytes[0] & 0xf8) == 0xf0)
		{
			charCnt++;
			bytes += 4;
		}
		else if ((bytes[0] & 0xfc) == 0xf8)
		{
			charCnt++;
			bytes += 5;
		}
		else if ((bytes[0] & 0xfc) == 0xfc)
		{
			charCnt++;
			bytes += 6;
		}
		else
		{
			break;
		}
	}
	return charCnt;
}

UnsafeArray<UTF8Char> Text::StrUTF16_UTF8(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UTF16Char> wstr)
{
	UTF16Char c;
	UTF32Char code;
	while (true)
	{
		if ((c = *wstr++) == 0)
		{
			*bytes = 0;
			break;
		}
		if (c < 0x80)
		{
			*bytes++ = (UTF8Char)c;
		}
		else if (c < 0x800)
		{
			*bytes++ = (UTF8Char)(0xc0 | (c >> 6));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (c >= 0xd800 && c < 0xdc00 && wstr[0] >= 0xdc00 && wstr[0] < 0xe000)
		{
			code = 0x10000 + ((UTF32Char)(c - 0xd800) << 10) + (UTF32Char)(wstr[0] - 0xdc00);
			wstr++;
			if (code < 0x200000)
			{
				*bytes++ = (UTF8Char)(0xf0 | (code >> 18));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
			}
			else if (code < 0x4000000)
			{
				*bytes++ = (UTF8Char)(0xf8 | (code >> 24));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
			}
			else
			{
				*bytes++ = (UTF8Char)(0xfc | (code >> 30));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 24) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
			}
		}
		else
		{
			*bytes++ = (UTF8Char)(0xe0 | (c >> 12));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
	}
	return bytes;
}

UnsafeArray<UTF8Char> Text::StrUTF16_UTF8C(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UTF16Char> wstr, UIntOS strLen)
{
	UTF16Char c;
	UTF32Char code;
	while (strLen-- > 0)
	{
		c = *wstr++;
		if (c < 0x80)
		{
			*bytes++ = (UTF8Char)c;
		}
		else if (c < 0x800)
		{
			*bytes++ = (UTF8Char)(0xc0 | (c >> 6));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (strLen > 0 && c >= 0xd800 && c < 0xdc00 && wstr[0] >= 0xdc00 && wstr[0] < 0xe000)
		{
			code = (UTF32Char)(0x10000 + ((c - 0xd800) << 10) + (wstr[0] - 0xdc00));
			wstr++;
			strLen--;
			if (code < 0x200000)
			{
				*bytes++ = (UTF8Char)(0xf0 | (code >> 18));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
			}
			else if (code < 0x4000000)
			{
				*bytes++ = (UTF8Char)(0xf8 | (code >> 24));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
			}
			else
			{
				*bytes++ = (UTF8Char)(0xfc | (code >> 30));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 24) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
			}
		}
		else
		{
			*bytes++ = (UTF8Char)(0xe0 | (c >> 12));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
	}
	return bytes;
}

UIntOS Text::StrUTF16_UTF8Cnt(UnsafeArray<const UTF16Char> stri)
{
	UTF16Char c;
	UIntOS byteCnt;
	UTF32Char code;
	byteCnt = 0;
	while (true)
	{
		if ((c = *stri++) == 0)
			break;
		if (c < 0x80)
			byteCnt++;
		else if (c < 0x800)
			byteCnt += 2;
		else if (c >= 0xd800 && c < 0xdc00 && stri[0] >= 0xdc00 && stri[0] < 0xe000)
		{
			code = (UTF32Char)(0x10000 + ((c - 0xd800) << 10) + (stri[0] - 0xdc00));
			stri++;
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
	return byteCnt;
}

UIntOS Text::StrUTF16_UTF8CntC(UnsafeArray<const UTF16Char> stri, UIntOS strLen)
{
	UTF16Char c;
	UIntOS byteCnt;
	UTF32Char code;
	byteCnt = 0;
	while (strLen-- > 0)
	{
		c = *stri++;
		if (c < 0x80)
			byteCnt++;
		else if (c < 0x800)
			byteCnt += 2;
		else if (strLen > 0 && c >= 0xd800 && c < 0xdc00 && stri[0] >= 0xdc00 && stri[0] < 0xe000)
		{
			code = (UTF32Char)(0x10000 + ((c - 0xd800) << 10) + (stri[0] - 0xdc00));
			stri++;
			strLen--;
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
	return byteCnt;
}

UnsafeArray<UTF8Char> Text::StrUTF32_UTF8(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UTF32Char> wstr)
{
	UTF32Char c;
	while (true)
	{
		if ((c = *wstr++) == 0)
		{
			*bytes = 0;
			break;
		}
		if (c < 0x80)
		{
			*bytes++ = (UTF8Char)c;
		}
		else if (c < 0x800)
		{
			*bytes++ = (UTF8Char)(0xc0 | (c >> 6));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (c < 0x10000)
		{
			*bytes++ = (UTF8Char)(0xe0 | (c >> 12));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (c < 0x200000)
		{
			*bytes++ = (UTF8Char)(0xf0 | (c >> 18));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (c < 0x4000000)
		{
			*bytes++ = (UTF8Char)(0xf8 | (c >> 24));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else
		{
			*bytes++ = (UTF8Char)(0xfc | (c >> 30));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 24) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
	}
	return bytes;
}

UnsafeArray<UTF8Char> Text::StrUTF32_UTF8C(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UTF32Char> wstr, UIntOS strLen)
{
	UTF32Char c;
	while (strLen-- > 0)
	{
		c = *wstr++;
		if (c < 0x80)
		{
			*bytes++ = (UTF8Char)c;
		}
		else if (c < 0x800)
		{
			*bytes++ = (UTF8Char)(0xc0 | (c >> 6));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (c < 0x10000)
		{
			*bytes++ = (UTF8Char)(0xe0 | (c >> 12));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (c < 0x200000)
		{
			*bytes++ = (UTF8Char)(0xf0 | (c >> 18));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (c < 0x4000000)
		{
			*bytes++ = (UTF8Char)(0xf8 | (c >> 24));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else
		{
			*bytes++ = (UTF8Char)(0xfc | (c >> 30));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 24) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
	}
	return bytes;
}

UIntOS Text::StrUTF32_UTF8Cnt(UnsafeArray<const UTF32Char> stri)
{
	UTF32Char c;
	UIntOS byteCnt;
	byteCnt = 0;
	while (true)
	{
		if ((c = *stri++) == 0)
			break;
		if (c < 0x80)
			byteCnt++;
		else if (c < 0x800)
			byteCnt += 2;
		else if (c < 0x10000)
			byteCnt += 3;
		else if (c < 0x200000)
			byteCnt += 4;
		else if (c < 0x4000000)
			byteCnt += 5;
		else
			byteCnt += 6;
	}
	return byteCnt;
}

UIntOS Text::StrUTF32_UTF8CntC(UnsafeArray<const UTF32Char> stri, UIntOS strLen)
{
	UTF32Char c;
	UIntOS byteCnt;
	byteCnt = 0;
	while (strLen-- > 0)
	{
		c = *stri++;
		if (c < 0x80)
			byteCnt++;
		else if (c < 0x800)
			byteCnt += 2;
		else if (c < 0x10000)
			byteCnt += 3;
		else if (c < 0x200000)
			byteCnt += 4;
		else if (c < 0x4000000)
			byteCnt += 5;
		else
			byteCnt += 6;
	}
	return byteCnt;
}

UnsafeArray<UTF8Char> Text::StrUTF16BE_UTF8(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UInt8> u16Buff)
{
	UTF16Char c;
	UTF16Char c2;
	UTF32Char code;
	while (true)
	{
		c = ReadMUInt16(&u16Buff[0]);
		u16Buff += 2;
		if (c == 0)
		{
			*bytes = 0;
			break;
		}
		if (c < 0x80)
		{
			*bytes++ = (UTF8Char)c;
		}
		else if (c < 0x800)
		{
			*bytes++ = (UTF8Char)(0xc0 | (c >> 6));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else
		{
			c2 = ReadMUInt16(&u16Buff[0]);
			if (c >= 0xd800 && c < 0xdc00 && c2 >= 0xdc00 && c2 < 0xe000)
			{
				code = 0x10000 + ((UTF32Char)(c - 0xd800) << 10) + (UTF32Char)(c2 - 0xdc00);
				u16Buff += 2;
				if (code < 0x200000)
				{
					*bytes++ = (UTF8Char)(0xf0 | (code >> 18));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
				}
				else if (code < 0x4000000)
				{
					*bytes++ = (UTF8Char)(0xf8 | (code >> 24));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
				}
				else
				{
					*bytes++ = (UTF8Char)(0xfc | (code >> 30));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 24) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
				}
			}
			else
			{
				*bytes++ = (UTF8Char)(0xe0 | (c >> 12));
				*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
			}
		}
	}
	return bytes;
}

UnsafeArray<UTF8Char> Text::StrUTF16BE_UTF8C(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UInt8> u16Buff, UIntOS utf16Cnt)
{
	UTF16Char c;
	UTF16Char c2;
	UTF32Char code;
	while (utf16Cnt-- > 0)
	{
		c = ReadMUInt16(&u16Buff[0]);
		u16Buff += 2;
		if (c < 0x80)
		{
			*bytes++ = (UTF8Char)c;
		}
		else if (c < 0x800)
		{
			*bytes++ = (UTF8Char)(0xc0 | (c >> 6));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (utf16Cnt > 0)
		{
			c2 = ReadMUInt16(&u16Buff[0]);
			if (c >= 0xd800 && c < 0xdc00 && c2 >= 0xdc00 && c2 < 0xe000)
			{
				code = (UTF32Char)(0x10000 + ((c - 0xd800) << 10) + (c2 - 0xdc00));
				u16Buff += 2;
				utf16Cnt--;
				if (code < 0x200000)
				{
					*bytes++ = (UTF8Char)(0xf0 | (code >> 18));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
				}
				else if (code < 0x4000000)
				{
					*bytes++ = (UTF8Char)(0xf8 | (code >> 24));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
				}
				else
				{
					*bytes++ = (UTF8Char)(0xfc | (code >> 30));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 24) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 18) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 12) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | ((code >> 6) & 0x3f));
					*bytes++ = (UTF8Char)(0x80 | (code & 0x3f));
				}
			}
			else
			{
				*bytes++ = (UTF8Char)(0xe0 | (c >> 12));
				*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
				*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
			}
		}
		else
		{
			*bytes++ = (UTF8Char)(0xe0 | (c >> 12));
			*bytes++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*bytes++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
	}
	return bytes;
}

UIntOS Text::StrUTF16BE_UTF8Cnt(UnsafeArray<const UInt8> u16Buff)
{
	UTF16Char c;
	UTF16Char c2;
	UIntOS byteCnt;
	UTF32Char code;
	byteCnt = 0;
	while (true)
	{
		c = ReadMUInt16(&u16Buff[0]);
		u16Buff += 2;
		if (c == 0)
			break;
		if (c < 0x80)
			byteCnt++;
		else if (c < 0x800)
			byteCnt += 2;
		else
		{
			c2 = ReadMUInt16(&u16Buff[0]);
			if (c >= 0xd800 && c < 0xdc00 && c2 >= 0xdc00 && c2 < 0xe000)
			{
				code = (UTF32Char)(0x10000 + ((c - 0xd800) << 10) + (c2 - 0xdc00));
				u16Buff += 2;
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
	}
	return byteCnt;	
}

UIntOS Text::StrUTF16BE_UTF8CntC(UnsafeArray<const UInt8> u16Buff, UIntOS utf16Cnt)
{
	UTF16Char c;
	UTF16Char c2;
	UIntOS byteCnt;
	UTF32Char code;
	byteCnt = 0;
	while (utf16Cnt-- > 0)
	{
		c = ReadMUInt16(&u16Buff[0]);
		u16Buff += 2;
		if (c < 0x80)
			byteCnt++;
		else if (c < 0x800)
			byteCnt += 2;
		else if (utf16Cnt > 0)
		{
			c2 = ReadMUInt16(&u16Buff[0]);
			if (c >= 0xd800 && c < 0xdc00 && c2 >= 0xdc00 && c2 < 0xe000)
			{
				code = (UTF32Char)(0x10000 + ((c - 0xd800) << 10) + (c2 - 0xdc00));
				u16Buff += 2;
				utf16Cnt--;
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

UnsafeArray<UTF32Char> Text::StrUTF16_UTF32(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF16Char> strToJoin)
{
	UTF16Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 0xd800 && c < 0xdc00 && strToJoin[0] >= 0xdc00 && strToJoin[0] < 0xe000)
		{
			*oriStr++ = (UTF32Char)(0x10000 + ((c - 0xd800) << 10) + (strToJoin[0] - 0xdc00));
			strToJoin++;
		}
		else
		{
			*oriStr++ = c;
		}
	}
	*oriStr = c;
	return oriStr;
}

UnsafeArray<UTF32Char> Text::StrUTF16_UTF32(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF16Char> strToJoin, UIntOS charCnt)
{
	UTF16Char c;
	while (charCnt-- > 0)
	{
		c = *strToJoin++;
		if (c >= 0xd800 && c < 0xdc00 && strToJoin[0] >= 0xdc00 && strToJoin[0] < 0xe000)
		{
			*oriStr++ = (UTF32Char)(0x10000 + ((c - 0xd800) << 10) + (strToJoin[0] - 0xdc00));
			strToJoin++;
		}
		else
		{
			*oriStr++ = c;
		}
	}
	*oriStr = 0;
	return oriStr;
}

UIntOS Text::StrUTF16_UTF32Cnt(UnsafeArray<const UTF16Char> strToJoin)
{
	UTF16Char c;
	UIntOS retCnt = 0;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 0xd800 && c < 0xdc00 && strToJoin[0] >= 0xdc00 && strToJoin[0] < 0xe000)
		{
			retCnt++;
			strToJoin++;
		}
		else
		{
			retCnt++;
		}
	}
	return retCnt;
}

UIntOS Text::StrUTF16_UTF32Cnt(UnsafeArray<const UTF16Char> strToJoin, UIntOS charCnt)
{
	UTF16Char c;
	UIntOS retCnt = 0;
	while (charCnt-- > 0)
	{
		c = *strToJoin++;
		if (c >= 0xd800 && c < 0xdc00 && strToJoin[0] >= 0xdc00 && strToJoin[0] < 0xe000)
		{
			retCnt++;
			strToJoin++;
		}
		else
		{
			retCnt++;
		}
	}
	return retCnt;
}

UnsafeArray<UTF16Char> Text::StrUTF32_UTF16(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF32Char> strToJoin)
{
	UTF32Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 0x10000)
		{
			*oriStr++ = (UTF16Char)(0xd800 + ((c - 0x10000) >> 10));
			*oriStr++ = (UTF16Char)((c & 0x3ff) + 0xdc00);
		}
		else
		{
			*oriStr++ = (UTF16Char)c;
		}
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF16Char> Text::StrUTF32_UTF16(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF32Char> strToJoin, UIntOS charCnt)
{
	UTF32Char c;
	while (charCnt-- > 0)
	{
		c = *strToJoin++;
		if (c >= 0x10000)
		{
			*oriStr++ = (UTF16Char)(0xd800 + ((c - 0x10000) >> 10));
			*oriStr++ = (UTF16Char)((c & 0x3ff) + 0xdc00);
		}
		else
		{
			*oriStr++ = (UTF16Char)c;
		}
	}
	*oriStr = 0;
	return oriStr;
}

UIntOS Text::StrUTF32_UTF16Cnt(UnsafeArray<const UTF32Char> strToJoin)
{
	UTF32Char c;
	UIntOS retCnt = 0;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 0x10000)
		{
			retCnt += 2;
		}
		else
		{
			retCnt++;
		}
	}
	return retCnt;
}

UIntOS Text::StrUTF32_UTF16Cnt(UnsafeArray<const UTF32Char> strToJoin, UIntOS charCnt)
{
	UTF32Char c;
	UIntOS retCnt = 0;
	while (charCnt-- > 0)
	{
		c = *strToJoin++;
		if (c >= 0x10000)
		{
			retCnt += 2;
		}
		else
		{
			retCnt++;
		}
	}
	return retCnt;
}

UnsafeArray<const UTF8Char> Text::StrReadChar(UnsafeArray<const UTF8Char> sptr, OutParam<UTF32Char> outChar)
{
	UTF8Char b = *sptr++;
	if (b < 0x80)
	{
		outChar.Set(b);
	}
	else if ((b & 0xe0) == 0xc0)
	{
		outChar.Set((UTF32Char)(((UInt32)(b & 0x1f) << 6) | (UInt32)(*sptr & 0x3f)));
		sptr++;
	}
	else if ((b & 0xf0) == 0xe0)
	{
		outChar.Set((UTF32Char)(((UInt32)(b & 0x0f) << 12) | ((UInt32)(sptr[0] & 0x3f) << 6) | (UInt32)(sptr[1] & 0x3f)));
		sptr += 2;
	}
	else if ((b & 0xf8) == 0xf0)
	{
		outChar.Set((UTF32Char)(((UInt32)(b & 0x7) << 18) | ((UInt32)(sptr[0] & 0x3f) << 12) | ((UInt32)(sptr[1] & 0x3f) << 6) | (UInt32)(sptr[2] & 0x3f)));
		sptr += 3;
	}
	else if ((b & 0xfc) == 0xf8)
	{
		outChar.Set((UTF32Char)(((UInt32)(b & 0x3) << 24) | ((UInt32)(sptr[0] & 0x3f) << 18) | ((UInt32)(sptr[1] & 0x3f) << 12) | ((UInt32)(sptr[2] & 0x3f) << 6) | (UInt32)(sptr[3] & 0x3f)));
		sptr += 4;
	}
	else if ((b & 0xfe) == 0xfc)
	{
		outChar.Set((UTF32Char)(((UInt32)(b & 0x1) << 30) | ((UInt32)(sptr[0] & 0x3f) << 24) | ((UInt32)(sptr[1] & 0x3f) << 18) | ((UInt32)(sptr[2] & 0x3f) << 12) | ((UInt32)(sptr[3] & 0x3f) << 6) | (UInt32)(sptr[4] & 0x3f)));
		sptr += 5;
	}
	else
	{
		outChar.Set(b);
	}
	return sptr;
}

UnsafeArray<const UTF16Char> Text::StrReadChar(UnsafeArray<const UTF16Char> sptr, OutParam<UTF32Char> outChar)
{
	UTF16Char c = *sptr++;
	if (c >= 0xd800 && c < 0xdc00 && sptr[0] >= 0xdc00 && sptr[0] < 0xe000)
	{
		outChar.Set((UTF32Char)(0x10000 + ((c - 0xd800) << 10) + (sptr[0] - 0xdc00)));
		sptr++;
	}
	else
	{
		outChar.Set(c);
	}
	return sptr;
}

UnsafeArray<const UTF32Char> Text::StrReadChar(UnsafeArray<const UTF32Char> sptr, OutParam<UTF32Char> outChar)
{
	outChar.Set(*sptr++);
	return sptr;
}

UnsafeArray<UTF8Char> Text::StrWriteChar(UnsafeArray<UTF8Char> sptr, UTF32Char c)
{
	if (c < 0x80)
	{
		*sptr++ = (UInt8)c;
	}
	else if (c < 0x800)
	{
		*sptr++ = (UTF8Char)(0xc0 | (c >> 6));
		*sptr++ = (UTF8Char)(0x80 | (c & 0x3f));
	}
	else if (c < 0x10000)
	{
		*sptr++ = (UTF8Char)(0xe0 | (c >> 12));
		*sptr++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		*sptr++ = (UTF8Char)(0x80 | (c & 0x3f));
	}
	else if (c < 0x200000)
	{
		*sptr++ = (UTF8Char)(0xf0 | (c >> 18));
		*sptr++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
		*sptr++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		*sptr++ = (UTF8Char)(0x80 | (c & 0x3f));
	}
	else if (c < 0x4000000)
	{
		*sptr++ = (UTF8Char)(0xf8 | (c >> 24));
		*sptr++ = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
		*sptr++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
		*sptr++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		*sptr++ = (UTF8Char)(0x80 | (c & 0x3f));
	}
	else
	{
		*sptr++ = (UTF8Char)(0xfc | (c >> 30));
		*sptr++ = (UTF8Char)(0x80 | ((c >> 24) & 0x3f));
		*sptr++ = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
		*sptr++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
		*sptr++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
		*sptr++ = (UTF8Char)(0x80 | (c & 0x3f));
	}
	return sptr;
}

UnsafeArray<UTF16Char> Text::StrWriteChar(UnsafeArray<UTF16Char> sptr, UTF32Char c)
{
	if (c >= 0x10000)
	{
		*sptr++ = (UTF16Char)(0xd800 + ((c - 0x10000) >> 10));
		*sptr++ = (UTF16Char)((c & 0x3ff) + 0xdc00);
	}
	else
	{
		*sptr++ = (UTF16Char)c;
	}
	return sptr;
}

UnsafeArray<UTF32Char> Text::StrWriteChar(UnsafeArray<UTF32Char> sptr, UTF32Char c)
{
	*sptr++ = c;
	return sptr;
}
