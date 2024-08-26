#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

#if defined(ENABLE_SSE) && (defined(CPU_X86_64) || defined(CPU_X86_32))
#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#endif

Char MyString_STRHEXARR[] = "0123456789ABCDEF";
Char MyString_STRhexarr[] = "0123456789abcdef";

UInt8 MyString_StrUpperArr[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

UInt8 MyString_StrLowerArr[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

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

UnsafeArray<UTF8Char> Text::StrConcat(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin)
{
	while ((*oriStr++ = *strToJoin++) != 0);
	return oriStr - 1;
}

UnsafeArray<UTF8Char> Text::StrConcatS(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin, UOSInt buffSize)
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

UnsafeArray<UTF8Char> Text::StrConcatASCII(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const Char> strToJoin)
{
	while ((*oriStr++ = (UTF8Char)*strToJoin++) != 0);
	return oriStr - 1;
}

UnsafeArray<UTF8Char> Text::StrInt16(UnsafeArray<UTF8Char> oriStr, Int16 val)
{
	if (val < 0)
	{
		val = (Int16)-val;
		*oriStr++ = '-';
	}
	if (val < 10)
	{
		*oriStr++ = MyString_StrDigit100U8[val * 2 + 1];
	}
	else if (val < 100)
	{
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16(&MyString_StrDigit100U8[val * 2]));
		oriStr += 2;
	}
	else if (val < 1000)
	{
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		*oriStr = MyString_StrDigit100U8[(val / 100) * 2 + 1];
		oriStr += 3;
	}
	else if (val < 10000)
	{
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16(&MyString_StrDigit100U8[(val / 100) * 2]));
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		oriStr += 4;
	}
	else
	{
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		val = val / 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		*oriStr = MyString_StrDigit100U8[(val / 100) * 2 + 1];
		oriStr += 5;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF8Char> Text::StrUInt16(UnsafeArray<UTF8Char> oriStr, UInt16 val)
{
	if (val < 10)
	{
		*oriStr++ = MyString_StrDigit100U8[val * 2 + 1];
	}
	else if (val < 100)
	{
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16(&MyString_StrDigit100U8[val * 2]));
		oriStr += 2;
	}
	else if (val < 1000)
	{
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		*oriStr = MyString_StrDigit100U8[(val / 100) * 2 + 1];
		oriStr += 3;
	}
	else if (val < 10000)
	{
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16(&MyString_StrDigit100U8[(val / 100) * 2]));
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		oriStr += 4;
	}
	else
	{
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		val = val / 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16(&MyString_StrDigit100U8[(val % 100) * 2]));
		*oriStr = MyString_StrDigit100U8[(val / 100) * 2 + 1];
		oriStr += 5;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF8Char> Text::StrInt32(UnsafeArray<UTF8Char> oriStr, Int32 val)
{
	if (val < 0)
	{
		val = -val;
		*oriStr++ = '-';
	}
#if 1
	if (val < 10)
	{
		*oriStr = MyString_StrDigit100U8[val * 2 + 1];
		oriStr++;
	}
	else if (val < 100)
	{
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 2;
	}
	else if (val < 1000)
	{
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 3;
	}
	else if (val < 10000)
	{
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 4;
	}
	else if (val < 100000)
	{
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 5;
	}
	else if (val < 1000000)
	{
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 6;
	}
	else if (val < 10000000)
	{
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 7;
	}
	else if (val < 100000000)
	{
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 8;
	}
	else if (val < 1000000000)
	{
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 9;
	}
	else
	{
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 10;
	}
	*oriStr = 0;
	return oriStr;
#else
	UTF8Char buff[10];
	UnsafeArray<UTF8Char> str;
	str = &buff[10];
	if (val == 0)
	{
		*--str = 0x30;
	}
	else
	{
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
	}
	UOSInt len = (UOSInt)(&buff[10] - str);
	while (len >= 4)
	{
		WriteNUInt32(oriStr, ReadNUInt32(str));
		oriStr += 4;
		str += 4;
		len -= 4;
	}
	if (len >= 2)
	{
		WriteNUInt16(oriStr, ReadNUInt16(str));
		oriStr += 2;
		str += 2;
		len -= 2;
	}
	if (len)
	{
		*oriStr++ = *str;
	}
	*oriStr = 0;
	return oriStr;
#endif
}

UnsafeArray<UTF8Char> Text::StrUInt32(UnsafeArray<UTF8Char> oriStr, UInt32 val)
{
#if 1
	if (val < 10)
	{
		*oriStr = MyString_StrDigit100U8[val * 2 + 1];
		oriStr++;
	}
	else if (val < 100)
	{
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 2;
	}
	else if (val < 1000)
	{
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 3;
	}
	else if (val < 10000)
	{
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 4;
	}
	else if (val < 100000)
	{
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 5;
	}
	else if (val < 1000000)
	{
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 6;
	}
	else if (val < 10000000)
	{
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 7;
	}
	else if (val < 100000000)
	{
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 8;
	}
	else if (val < 1000000000)
	{
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 9;
	}
	else
	{
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 10;
	}
	*oriStr = 0;
	return oriStr;
#else
	UTF8Char buff[10];
	UnsafeArray<UTF8Char> str;
	str = &buff[10];
	if (val == 0)
	{
		*--str = 0x30;
	}
	else
	{
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
	}
	while (str < &buff[10])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
#endif
}

UnsafeArray<UTF8Char> Text::StrInt32S(UnsafeArray<UTF8Char> oriStr, Int32 val, UTF8Char seperator, UOSInt sepCnt)
{
	UTF8Char buff[20];
	UnsafeArray<UTF8Char> str;
	UOSInt i;
	if (val < 0)
	{
		val = -val;
		*oriStr++ = '-';
	}
	str = UnsafeArray<UTF8Char>::FromPtrNoCheck(buff + 20);
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
		*--str = (UTF8Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str.Ptr() < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}


UnsafeArray<UTF8Char> Text::StrUInt32S(UnsafeArray<UTF8Char> oriStr, UInt32 val, UTF8Char seperator, UOSInt sepCnt)
{
	UTF8Char buff[20];
	UnsafeArray<UTF8Char> str;
	UOSInt i;
	str = UnsafeArray<UTF8Char>::FromPtrNoCheck(&buff[20]);
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
		*--str = (UTF8Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str.Ptr() < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

#ifdef HAS_INT64
#if _OSINT_SIZE == 64
UnsafeArray<UTF8Char> Text::StrInt64(UnsafeArray<UTF8Char> oriStr, Int64 val)
{
	if (val < 0)
	{
		val = -val;
		*oriStr++ = '-';
	}
#if 1
	if (val < 0x100000000)
	{
		return Text::StrUInt32(oriStr, (UInt32)val);
	}
	if (val < 10000000000)
	{
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 10;
	}
	else if (val < 100000000000)
	{
		WriteNInt16((UInt8*)&oriStr[9], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 11;
	}
	else if (val < 1000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[10], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 12;
	}
	else if (val < 10000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[11], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[9], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 13;
	}
	else if (val < 100000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[12], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[10], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 14;
	}
	else if (val < 1000000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[13], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[11], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[9], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 15;
	}
	else if (val < 10000000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[14], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[12], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[10], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 16;
	}
	else if (val < 100000000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[15], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[13], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[11], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[9], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 17;
	}
	else if (val < 1000000000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[16], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[14], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[12], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[10], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 18;
	}
	else
	{
		WriteNInt16((UInt8*)&oriStr[17], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[15], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[13], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[11], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[9], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 19;
	}
	*oriStr = 0;
	return oriStr;
#else
	UTF8Char buff[20];
	UnsafeArray<UTF8Char> str;
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
			*str = MyString_StrDigit100U8[val * 2 + 1];
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
#endif
}

UnsafeArray<UTF8Char> Text::StrUInt64(UnsafeArray<UTF8Char> oriStr, UInt64 val)
{
#if 1
	if (val < 0x100000000)
	{
		return Text::StrUInt32(oriStr, (UInt32)val);
	}
	if (val < 10000000000)
	{
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 10;
	}
	else if (val < 100000000000)
	{
		WriteNInt16((UInt8*)&oriStr[9], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 11;
	}
	else if (val < 1000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[10], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 12;
	}
	else if (val < 10000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[11], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[9], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 13;
	}
	else if (val < 100000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[12], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[10], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 14;
	}
	else if (val < 1000000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[13], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[11], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[9], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 15;
	}
	else if (val < 10000000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[14], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[12], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[10], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 16;
	}
	else if (val < 100000000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[15], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[13], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[11], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[9], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 17;
	}
	else if (val < 1000000000000000000)
	{
		WriteNInt16((UInt8*)&oriStr[16], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[14], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[12], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[10], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 18;
	}
	else if (val < 10000000000000000000ULL)
	{
		WriteNInt16((UInt8*)&oriStr[17], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[15], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[13], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[11], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[9], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[7], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[5], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[3], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[1], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		oriStr[0] = MyString_StrDigit100U8[val * 2 + 1];
		oriStr += 19;
	}
	else
	{
		WriteNInt16((UInt8*)&oriStr[18], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[16], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[14], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[12], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[10], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[8], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[6], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[4], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[2], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[(val % 100) * 2]));
		val /= 100;
		WriteNInt16((UInt8*)&oriStr[0], ReadNInt16((const UInt8*)&MyString_StrDigit100U8[val * 2]));
		oriStr += 18;
	}
	*oriStr = 0;
	return oriStr;
#else
	UTF8Char buff[20];
	UnsafeArray<UTF8Char> str;
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
#endif
}

UnsafeArray<UTF8Char> Text::StrInt64S(UnsafeArray<UTF8Char> oriStr, Int64 val, UTF8Char seperator, UOSInt sepCnt)
{
	UTF8Char buff[40];
	UnsafeArray<UTF8Char> str;
	UOSInt i;
	if (val < 0)
	{
		val = -val;
		*oriStr++ = '-';
	}
	str = UnsafeArray<UTF8Char>::FromPtrNoCheck(&buff[40]);
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
		*--str = (UTF8Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str.Ptr() < &buff[40])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF8Char> Text::StrUInt64S(UnsafeArray<UTF8Char> oriStr, UInt64 val, UTF8Char seperator, UOSInt sepCnt)
{
	UTF8Char buff[40];
	UnsafeArray<UTF8Char> str;
	UOSInt i;
	str = UnsafeArray<UTF8Char>::FromPtrNoCheck(&buff[40]);
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
		*--str = (UTF8Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str.Ptr() < &buff[40])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr;
}
#else
UnsafeArray<UTF8Char> Text::StrInt64(UnsafeArray<UTF8Char> oriStr, Int64 val)
{
	UTF8Char buff[20];
	UnsafeArray<UTF8Char> str;
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
			*str = MyString_StrDigit100U8[i32Val * 2 + 1];
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr.Ptr();
}

UnsafeArray<UTF8Char> Text::StrUInt64(UnsafeArray<UTF8Char> oriStr, UInt64 val)
{
	UTF8Char buff[20];
	UnsafeArray<UTF8Char> str;
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
			*str = MyString_StrDigit100U8[i32Val * 2 + 1];
		}
	}
	while (str < &buff[20])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr.Ptr();
}

UnsafeArray<UTF8Char> Text::StrInt64S(UnsafeArray<UTF8Char> oriStr, Int64 val, UTF8Char seperator, UOSInt sepCnt)
{
	UTF8Char buff[40];
	UnsafeArray<UTF8Char> str;
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
		*--str = (UTF8Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str < &buff[40])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr.Ptr();
}

UnsafeArray<UTF8Char> Text::StrUInt64S(UnsafeArray<UTF8Char> oriStr, UInt64 val, UTF8Char seperator, UOSInt sepCnt)
{
	UTF8Char buff[40];
	UnsafeArray<UTF8Char> str;
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
		*--str = (UTF8Char)(0x30 + val % 10);
		val = val / 10;
		i--;
	}
	while (str < &buff[40])
	{
		*oriStr++ = *str++;
	}
	*oriStr = 0;
	return oriStr.Ptr();
}
#endif
#endif

UnsafeArray<UTF8Char> Text::StrToUpper(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin)
{
	UOSInt c;
	while ((c = *strToJoin++) != 0)
	{
/*		if (c >= 'a' && c <= 'z')
		{
			*oriStr++ = (UTF8Char)(c - 32);
		}
		else
		{
			*oriStr++ = c;
		}*/
		*oriStr++ = MyString_StrUpperArr[c];
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF8Char> Text::StrToUpperC(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin, UOSInt strLen)
{
#if _OSINT_SIZE == 64
	while (strLen >= 8)
	{
		UInt64 v = ReadUInt64(&strToJoin[0]);
		oriStr[0] = MyString_StrUpperArr[v & 0xff];
		oriStr[1] = MyString_StrUpperArr[(v >> 8) & 0xff];
		oriStr[2] = MyString_StrUpperArr[(v >> 16) & 0xff];
		oriStr[3] = MyString_StrUpperArr[(v >> 24) & 0xff];
		oriStr[4] = MyString_StrUpperArr[(v >> 32) & 0xff];
		oriStr[5] = MyString_StrUpperArr[(v >> 40) & 0xff];
		oriStr[6] = MyString_StrUpperArr[(v >> 48) & 0xff];
		oriStr[7] = MyString_StrUpperArr[(v >> 56)];
		strToJoin += 8;
		oriStr += 8;
		strLen -= 8;
	}
	if (strLen >= 4)
	{
		UInt32 v = ReadUInt32(&strToJoin[0]);
		oriStr[0] = MyString_StrUpperArr[v & 0xff];
		oriStr[1] = MyString_StrUpperArr[(v >> 8) & 0xff];
		oriStr[2] = MyString_StrUpperArr[(v >> 16) & 0xff];
		oriStr[3] = MyString_StrUpperArr[(v >> 24)];
		strToJoin += 4;
		oriStr += 4;
		strLen -= 4;
	}
#else
	while (strLen >= 4)
	{
		UInt32 v = ReadUInt32(&strToJoin[0]);
		oriStr[0] = MyString_StrUpperArr[v & 0xff];
		oriStr[1] = MyString_StrUpperArr[(v >> 8) & 0xff];
		oriStr[2] = MyString_StrUpperArr[(v >> 16) & 0xff];
		oriStr[3] = MyString_StrUpperArr[(v >> 24)];
		strToJoin += 4;
		oriStr += 4;
		strLen -= 4;

	}
#endif
	if (strLen >= 2)
	{
		UInt16 v = ReadUInt16(&strToJoin[0]);
		oriStr[0] = MyString_StrUpperArr[v & 0xff];
		oriStr[1] = MyString_StrUpperArr[(v >> 8)];
		strToJoin += 2;
		oriStr += 2;
		strLen -= 2;
	}
	if (strLen > 0)
	{
		oriStr[0] = MyString_StrUpperArr[*strToJoin++];
		oriStr++;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF8Char> Text::StrToLower(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin)
{
	UOSInt c;
	while ((c = *strToJoin++) != 0)
	{
		*oriStr++ = MyString_StrLowerArr[c];
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF8Char> Text::StrToLowerC(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin, UOSInt strLen)
{
#if _OSINT_SIZE == 64
	while (strLen >= 8)
	{
		UInt64 v = ReadUInt64(&strToJoin[0]);
		oriStr[0] = MyString_StrLowerArr[v & 0xff];
		oriStr[1] = MyString_StrLowerArr[(v >> 8) & 0xff];
		oriStr[2] = MyString_StrLowerArr[(v >> 16) & 0xff];
		oriStr[3] = MyString_StrLowerArr[(v >> 24) & 0xff];
		oriStr[4] = MyString_StrLowerArr[(v >> 32) & 0xff];
		oriStr[5] = MyString_StrLowerArr[(v >> 40) & 0xff];
		oriStr[6] = MyString_StrLowerArr[(v >> 48) & 0xff];
		oriStr[7] = MyString_StrLowerArr[(v >> 56)];
		strToJoin += 8;
		oriStr += 8;
		strLen -= 8;
	}
	if (strLen >= 4)
	{
		UInt32 v = ReadUInt32(&strToJoin[0]);
		oriStr[0] = MyString_StrLowerArr[v & 0xff];
		oriStr[1] = MyString_StrLowerArr[(v >> 8) & 0xff];
		oriStr[2] = MyString_StrLowerArr[(v >> 16) & 0xff];
		oriStr[3] = MyString_StrLowerArr[(v >> 24)];
		strToJoin += 4;
		oriStr += 4;
		strLen -= 4;
	}
#else
	while (strLen >= 4)
	{
		UInt32 v = ReadUInt32(&strToJoin[0]);
		oriStr[0] = MyString_StrLowerArr[v & 0xff];
		oriStr[1] = MyString_StrLowerArr[(v >> 8) & 0xff];
		oriStr[2] = MyString_StrLowerArr[(v >> 16) & 0xff];
		oriStr[3] = MyString_StrLowerArr[(v >> 24)];
		strToJoin += 4;
		oriStr += 4;
		strLen -= 4;

	}
#endif
	if (strLen >= 2)
	{
		UInt16 v = ReadUInt16(&strToJoin[0]);
		oriStr[0] = MyString_StrLowerArr[v & 0xff];
		oriStr[1] = MyString_StrLowerArr[(v >> 8)];
		strToJoin += 2;
		oriStr += 2;
		strLen -= 2;
	}
	if (strLen > 0)
	{
		oriStr[0] = MyString_StrLowerArr[*strToJoin++];
		oriStr++;
	}
	*oriStr = 0;
	return oriStr;
}

UnsafeArray<UTF8Char> Text::StrToCapital(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin)
{
	Bool lastLetter = false;
	UTF8Char c;
	while ((c = *strToJoin++) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			if (lastLetter)
			{
				*oriStr++ = (UTF8Char)(c + 32);
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
				*oriStr++ = (UTF8Char)(c - 32);
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

Bool Text::StrEquals(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2)
{
	UTF8Char c;
	while ((c = *str1++) != 0)
	{
		if (c != *str2++)
			return false;
	}
	return *str2 == 0;
}

Bool Text::StrEqualsN(UnsafeArrayOpt<const UTF8Char> str1, UnsafeArrayOpt<const UTF8Char> str2)
{
	UnsafeArray<const UTF8Char> nnstr1;
	UnsafeArray<const UTF8Char> nnstr2;
	if (str1.Ptr() == str2.Ptr())
		return true;
	if (!str1.SetTo(nnstr1) || !str2.SetTo(nnstr2))
		return false;
	return StrEquals(nnstr1, nnstr2);
}

Bool Text::StrEqualsICase(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2)
{
	UOSInt c1;
	while ((c1 = *str1++) != 0)
	{
		if (MyString_StrUpperArr[c1] != MyString_StrUpperArr[*str2])
		{
			return false;
		}
		str2++;
	}
	return *str2 == 0;
}

Bool Text::StrEqualsICaseC(UnsafeArray<const UTF8Char> str1, UOSInt str1Len, UnsafeArray<const UTF8Char> str2, UOSInt str2Len)
{
	if (str1Len != str2Len)
	{
		return false;
	}
	while (str2Len >= 4)
	{
		UInt32 v1 = ReadNUInt32(str1.Ptr());
		UInt32 v2 = ReadNUInt32(str2.Ptr());
		if (MyString_StrUpperArr[v1 & 0xff] != MyString_StrUpperArr[v2 & 0xff])
		{
			return false;
		}
		if (MyString_StrUpperArr[(v1 >> 8) & 0xff] != MyString_StrUpperArr[(v2 >> 8) & 0xff])
		{
			return false;
		}
		if (MyString_StrUpperArr[(v1 >> 16) & 0xff] != MyString_StrUpperArr[(v2 >> 16) & 0xff])
		{
			return false;
		}
		if (MyString_StrUpperArr[(v1 >> 24)] != MyString_StrUpperArr[(v2 >> 24)])
		{
			return false;
		}

		str2Len -= 4;
		str1 += 4;
		str2 += 4;
	}
	while (str2Len-- > 0)
	{
		if (MyString_StrUpperArr[*str1] != MyString_StrUpperArr[*str2])
		{
			return false;
		}
		str1++;
		str2++;
	}
	return true;
}

OSInt Text::StrCompareFastC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2)
{
	OSInt defRet;
	if (len1 > len2)
	{
		defRet = 1;
	}
	else if (len1 == len2)
	{
		defRet = 0;
	}
	else
	{
		defRet = -1;
		len2 = len1;
	}
	while (len2 >= 4)
	{
		REGVAR UInt32 v1 = ReadMUInt32(&str1[0]);
		REGVAR UInt32 v2 = ReadMUInt32(&str2[0]);
		if (v1 > v2)
		{
			return 1;
		}
		else if (v1 < v2)
		{
			return -1;
		}
		len2 -= 4;
		str1 += 4;
		str2 += 4;
	}
	while (len2 > 0)
	{
		REGVAR UTF8Char c1 = *str1;
		REGVAR UTF8Char c2 = *str2;
		if (c1 > c2)
		{
			return 1;
		}
		else if (c1 < c2)
		{
			return -1;
		}
		len2--;
		str1++;
		str2++;
	}
	return defRet;
}

UOSInt Text::StrCharCntS(UnsafeArray<const UTF8Char> str, UOSInt maxLen)
{
	UnsafeArray<const UTF8Char> currPtr = str;
	UnsafeArray<const UTF8Char> endPtr = str + maxLen;
	while (currPtr < endPtr && *currPtr++);
	return (UOSInt)(currPtr - str);
}

Bool Text::StrHasUpperCase(UnsafeArray<const UTF8Char> str)
{
	UTF8Char c;
	while ((c = *str++) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			return true;
		}
	}
	return false;
}

UnsafeArray<UTF8Char> Text::StrHexVal64V(UnsafeArray<UTF8Char> oriStr, UInt64 val)
{
	UInt32 v1 = (UInt32)((UInt64)val >> 32);
	UInt32 v2 = (UInt32)(val & 0xffffffff);
	if (v1 == 0)
	{
		return StrHexVal32V(oriStr, v2);
	}
	if ((v1 & 0xfffffff0) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(oriStr + 1, v2);
	}
	else if ((v1 & 0xffffff00) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(oriStr + 2, v2);
	}
	else if ((v1 & 0xfffff000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(oriStr + 3, v2);
	}
	else if ((v1 & 0xffff0000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(v1 >> 12) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(oriStr + 4, v2);
	}
	else if ((v1 & 0xfff00000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(v1 >> 16) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(v1 >> 12) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(oriStr + 5, v2);
	}
	else if ((v1 & 0xff000000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(v1 >> 20) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(v1 >> 16) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[(v1 >> 12) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(oriStr + 6, v2);
	}
	else if ((v1 & 0xf0000000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(v1 >> 24) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(v1 >> 20) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[(v1 >> 16) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRHEXARR[(v1 >> 12) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[6] = (UTF8Char)MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(oriStr + 7, v2);
	}
	else
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(v1 >> 28) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(v1 >> 24) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[(v1 >> 20) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRHEXARR[(v1 >> 16) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRHEXARR[(v1 >> 12) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRHEXARR[(v1 >> 8) & 0xf];
		oriStr[6] = (UTF8Char)MyString_STRHEXARR[(v1 >> 4) & 0xf];
		oriStr[7] = (UTF8Char)MyString_STRHEXARR[v1 & 0xf];
		return StrHexVal32(oriStr + 8, v2);
	}
}

UnsafeArray<UTF8Char> Text::StrHexVal64(UnsafeArray<UTF8Char> oriStr, UInt64 val)
{
	UnsafeArray<UTF8Char> tmp = oriStr + 16;
	OSInt i = 16;
	while (i-- > 0)
	{
		*--tmp = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[16] = 0;
	return oriStr + 16;
}

UnsafeArray<UTF8Char> Text::StrHexVal32V(UnsafeArray<UTF8Char> oriStr, UInt32 val)
{
	if ((val & 0xfffffff0) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		oriStr[1] = 0;
		return oriStr + 1;
	}
	else if ((val & 0xffffff00) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		oriStr[2] = 0;
		return oriStr + 2;
	}
	else if ((val & 0xfffff000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		oriStr[3] = 0;
		return oriStr + 3;
	}
	else if ((val & 0xffff0000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(val >> 12) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		oriStr[4] = 0;
		return oriStr + 4;
	}
	else if ((val & 0xfff00000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(val >> 16) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(val >> 12) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		oriStr[5] = 0;
		return oriStr + 5;
	}
	else if ((val & 0xff000000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(val >> 20) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(val >> 16) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[(val >> 12) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		oriStr[6] = 0;
		return oriStr + 6;
	}
	else if ((val & 0xf0000000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(val >> 24) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(val >> 20) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[(val >> 16) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRHEXARR[(val >> 12) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[6] = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		oriStr[7] = 0;
		return oriStr + 7;
	}
	else
	{
		oriStr[0] = (UTF8Char)MyString_STRHEXARR[(val >> 28) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRHEXARR[(val >> 24) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRHEXARR[(val >> 20) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRHEXARR[(val >> 16) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRHEXARR[(val >> 12) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRHEXARR[(val >> 8) & 0xf];
		oriStr[6] = (UTF8Char)MyString_STRHEXARR[(val >> 4) & 0xf];
		oriStr[7] = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		oriStr[8] = 0;
		return oriStr + 8;
	}
}

UnsafeArray<UTF8Char> Text::StrHexVal32(UnsafeArray<UTF8Char> oriStr, UInt32 val)
{
	UnsafeArray<UTF8Char> tmp = oriStr + 8;
	UOSInt i = 8;
	while (i-- > 0)
	{
		*--tmp = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[8] = 0;
	return oriStr + 8;
}

UnsafeArray<UTF8Char> Text::StrHexVal24(UnsafeArray<UTF8Char> oriStr, UInt32 val)
{
	UnsafeArray<UTF8Char> tmp = oriStr + 6;
	UInt32 i = 6;
	while (i-- > 0)
	{
		*--tmp = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		val >>= 4;
	}
	oriStr[6] = 0;
	return oriStr + 6;
}

UnsafeArray<UTF8Char> Text::StrHexVal16(UnsafeArray<UTF8Char> oriStr, UInt16 val)
{
	UnsafeArray<UTF8Char> tmp = oriStr + 4;
	Int32 i = 4;
	while (i-- > 0)
	{
		*--tmp = (UTF8Char)MyString_STRHEXARR[val & 0xf];
		val = (UInt16)(val >> 4);
	}
	oriStr[4] = 0;
	return oriStr + 4;
}

UnsafeArray<UTF8Char> Text::StrHexByte(UnsafeArray<UTF8Char> oriStr, UInt8 val)
{
	oriStr[0] = (UTF8Char)MyString_STRHEXARR[val >> 4];
	oriStr[1] = (UTF8Char)MyString_STRHEXARR[val & 15];
	oriStr[2] = 0;
	return oriStr + 2;
}

UnsafeArray<UTF8Char> Text::StrHexBytes(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UInt8> buff, UOSInt buffSize, UTF8Char seperator)
{
	UOSInt val;
	if (seperator == 0)
	{
		while (buffSize-- > 0)
		{
			val = *buff++;
			oriStr[0] = (UTF8Char)MyString_STRHEXARR[val >> 4];
			oriStr[1] = (UTF8Char)MyString_STRHEXARR[val & 15];
			oriStr += 2;
		}
		oriStr[0] = 0;
	}
	else
	{
		while (buffSize-- > 0)
		{
			val = *buff++;
			oriStr[0] = (UTF8Char)MyString_STRHEXARR[val >> 4];
			oriStr[1] = (UTF8Char)MyString_STRHEXARR[val & 15];
			oriStr[2] = seperator;
			oriStr += 3;
		}
		*--oriStr = 0;
	}
	return oriStr;
}

UnsafeArray<UTF8Char> Text::StrHexVal64VLC(UnsafeArray<UTF8Char> oriStr, UInt64 val)
{
	UInt32 v1 = (UInt32)((UInt64)val >> 32);
	UInt32 v2 = (UInt32)(val & 0xffffffff);
	if (v1 == 0)
	{
		return StrHexVal32VLC(oriStr, v2);
	}
	if ((v1 & 0xfffffff0) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[v1 & 0xf];
		return StrHexVal32LC(oriStr + 1, v2);
	}
	else if ((v1 & 0xffffff00) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(v1 >> 4) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[v1 & 0xf];
		return StrHexVal32LC(oriStr + 2, v2);
	}
	else if ((v1 & 0xfffff000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(v1 >> 8) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(v1 >> 4) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[v1 & 0xf];
		return StrHexVal32LC(oriStr + 3, v2);
	}
	else if ((v1 & 0xffff0000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(v1 >> 12) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(v1 >> 8) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[(v1 >> 4) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRhexarr[v1 & 0xf];
		return StrHexVal32LC(oriStr + 4, v2);
	}
	else if ((v1 & 0xfff00000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(v1 >> 16) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(v1 >> 12) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[(v1 >> 8) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRhexarr[(v1 >> 4) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRhexarr[v1 & 0xf];
		return StrHexVal32LC(oriStr + 5, v2);
	}
	else if ((v1 & 0xff000000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(v1 >> 20) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(v1 >> 16) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[(v1 >> 12) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRhexarr[(v1 >> 8) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRhexarr[(v1 >> 4) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRhexarr[v1 & 0xf];
		return StrHexVal32LC(oriStr + 6, v2);
	}
	else if ((v1 & 0xf0000000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(v1 >> 24) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(v1 >> 20) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[(v1 >> 16) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRhexarr[(v1 >> 12) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRhexarr[(v1 >> 8) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRhexarr[(v1 >> 4) & 0xf];
		oriStr[6] = (UTF8Char)MyString_STRhexarr[v1 & 0xf];
		return StrHexVal32LC(oriStr + 7, v2);
	}
	else
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(v1 >> 28) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(v1 >> 24) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[(v1 >> 20) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRhexarr[(v1 >> 16) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRhexarr[(v1 >> 12) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRhexarr[(v1 >> 8) & 0xf];
		oriStr[6] = (UTF8Char)MyString_STRhexarr[(v1 >> 4) & 0xf];
		oriStr[7] = (UTF8Char)MyString_STRhexarr[v1 & 0xf];
		return StrHexVal32LC(oriStr + 8, v2);
	}
}

UnsafeArray<UTF8Char> Text::StrHexVal64LC(UnsafeArray<UTF8Char> oriStr, UInt64 val)
{
	UnsafeArray<UTF8Char> tmp = oriStr + 16;
	OSInt i = 16;
	while (i-- > 0)
	{
		*--tmp = (UTF8Char)MyString_STRhexarr[val & 0xf];
		val >>= 4;
	}
	oriStr[16] = 0;
	return oriStr + 16;
}

UnsafeArray<UTF8Char> Text::StrHexVal32VLC(UnsafeArray<UTF8Char> oriStr, UInt32 val)
{
	if ((val & 0xfffffff0) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[val & 0xf];
		oriStr[1] = 0;
		return oriStr + 1;
	}
	else if ((val & 0xffffff00) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(val >> 4) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[val & 0xf];
		oriStr[2] = 0;
		return oriStr + 2;
	}
	else if ((val & 0xfffff000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(val >> 8) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(val >> 4) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[val & 0xf];
		oriStr[3] = 0;
		return oriStr + 3;
	}
	else if ((val & 0xffff0000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(val >> 12) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(val >> 8) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[(val >> 4) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRhexarr[val & 0xf];
		oriStr[4] = 0;
		return oriStr + 4;
	}
	else if ((val & 0xfff00000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(val >> 16) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(val >> 12) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[(val >> 8) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRhexarr[(val >> 4) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRhexarr[val & 0xf];
		oriStr[5] = 0;
		return oriStr + 5;
	}
	else if ((val & 0xff000000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(val >> 20) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(val >> 16) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[(val >> 12) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRhexarr[(val >> 8) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRhexarr[(val >> 4) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRhexarr[val & 0xf];
		oriStr[6] = 0;
		return oriStr + 6;
	}
	else if ((val & 0xf0000000) == 0)
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(val >> 24) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(val >> 20) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[(val >> 16) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRhexarr[(val >> 12) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRhexarr[(val >> 8) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRhexarr[(val >> 4) & 0xf];
		oriStr[6] = (UTF8Char)MyString_STRhexarr[val & 0xf];
		oriStr[7] = 0;
		return oriStr + 7;
	}
	else
	{
		oriStr[0] = (UTF8Char)MyString_STRhexarr[(val >> 28) & 0xf];
		oriStr[1] = (UTF8Char)MyString_STRhexarr[(val >> 24) & 0xf];
		oriStr[2] = (UTF8Char)MyString_STRhexarr[(val >> 20) & 0xf];
		oriStr[3] = (UTF8Char)MyString_STRhexarr[(val >> 16) & 0xf];
		oriStr[4] = (UTF8Char)MyString_STRhexarr[(val >> 12) & 0xf];
		oriStr[5] = (UTF8Char)MyString_STRhexarr[(val >> 8) & 0xf];
		oriStr[6] = (UTF8Char)MyString_STRhexarr[(val >> 4) & 0xf];
		oriStr[7] = (UTF8Char)MyString_STRhexarr[val & 0xf];
		oriStr[8] = 0;
		return oriStr + 8;
	}
}

UnsafeArray<UTF8Char> Text::StrHexVal32LC(UnsafeArray<UTF8Char> oriStr, UInt32 val)
{
	UnsafeArray<UTF8Char> tmp = oriStr + 8;
	UOSInt i = 8;
	while (i-- > 0)
	{
		*--tmp = (UTF8Char)MyString_STRhexarr[val & 0xf];
		val >>= 4;
	}
	oriStr[8] = 0;
	return oriStr + 8;
}

UnsafeArray<UTF8Char> Text::StrHexVal24LC(UnsafeArray<UTF8Char> oriStr, UInt32 val)
{
	UnsafeArray<UTF8Char> tmp = oriStr + 6;
	UInt32 i = 6;
	while (i-- > 0)
	{
		*--tmp = (UTF8Char)MyString_STRhexarr[val & 0xf];
		val >>= 4;
	}
	oriStr[6] = 0;
	return oriStr + 6;
}

UnsafeArray<UTF8Char> Text::StrHexVal16LC(UnsafeArray<UTF8Char> oriStr, UInt16 val)
{
	UnsafeArray<UTF8Char> tmp = oriStr + 4;
	Int32 i = 4;
	while (i-- > 0)
	{
		*--tmp = (UTF8Char)MyString_STRhexarr[val & 0xf];
		val = (UInt16)(val >> 4);
	}
	oriStr[4] = 0;
	return oriStr + 4;
}

UnsafeArray<UTF8Char> Text::StrHexByteLC(UnsafeArray<UTF8Char> oriStr, UInt8 val)
{
	oriStr[0] = (UTF8Char)MyString_STRhexarr[val >> 4];
	oriStr[1] = (UTF8Char)MyString_STRhexarr[val & 15];
	oriStr[2] = 0;
	return oriStr + 2;
}

UnsafeArray<UTF8Char> Text::StrHexBytesLC(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UInt8> buff, UOSInt buffSize, UTF8Char seperator)
{
	UOSInt val;
	if (seperator == 0)
	{
		while (buffSize-- > 0)
		{
			val = *buff++;
			oriStr[0] = (UTF8Char)MyString_STRhexarr[val >> 4];
			oriStr[1] = (UTF8Char)MyString_STRhexarr[val & 15];
			oriStr += 2;
		}
		oriStr[0] = 0;
	}
	else
	{
		while (buffSize-- > 0)
		{
			val = *buff++;
			oriStr[0] = (UTF8Char)MyString_STRhexarr[val >> 4];
			oriStr[1] = (UTF8Char)MyString_STRhexarr[val & 15];
			oriStr[2] = seperator;
			oriStr += 3;
		}
		*--oriStr = 0;
	}
	return oriStr;
}

Int64 Text::StrHex2Int64C(UnsafeArray<const UTF8Char> str)
{
	OSInt i = 16;
	Int64 outVal = 0;
	UTF8Char c;
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

Bool Text::StrHex2Int64V(UnsafeArray<const UTF8Char> str, OutParam<Int64> outVal)
{
	UOSInt i = 0;
	Int64 currVal = 0;
	UTF8Char c;
	while (true)
	{
		c = *str++;
		if (c == 0)
		{
			outVal.Set(currVal);
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

Bool Text::StrHex2Int64S(UnsafeArray<const UTF8Char> str, OutParam<Int64> outVal, Int64 failVal)
{
	UOSInt i = 0;
	Int64 currVal = 0;
	UTF8Char c;
	while (true)
	{
		c = *str++;
		if (c == 0)
		{
			outVal.Set(currVal);
			return true;
		}
		i++;
		if (i >= 17)
		{
			outVal.Set(failVal);
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
			outVal.Set(failVal);
			return false;
		}
	}
}

Int32 Text::StrHex2Int32C(UnsafeArray<const UTF8Char> str)
{
	OSInt i = 8;
	Int32 outVal = 0;
	UTF8Char c;
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

Bool Text::StrHex2Int32V(UnsafeArray<const UTF8Char> str, OutParam<Int32> outVal)
{
	UOSInt i = 0;
	Int32 currVal = 0;
	UTF8Char c;
	if (*str == 0)
		return false;
	while (true)
	{
		c = *str++;
		if (c == 0)
		{
			outVal.Set(currVal);
			return true;
		}
		i++;
		if (i >= 9)
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

Int16 Text::StrHex2Int16C(UnsafeArray<const UTF8Char> str)
{
	UOSInt i = 4;
	Int32 outVal = 0;
	UTF8Char c;
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

Bool Text::StrHex2Int16V(UnsafeArray<const UTF8Char> str, OutParam<Int16> outVal)
{
	UOSInt i = 0;
	Int16 currVal = 0;
	UTF8Char c;
	while (true)
	{
		c = *str++;
		if (c == 0)
		{
			outVal.Set(currVal);
			return true;
		}
		i++;
		if (i >= 5)
		{
			return false;
		}
		if (c >= '0' && c <= '9')
		{
			currVal = (Int16)((currVal << 4) | (c - 48));
		}
		else if (c >= 'A' && c <= 'F')
		{
			currVal = (Int16)((currVal << 4) | (c - 0x37));
		}
		else if (c >= 'a' && c <= 'f')
		{
			currVal = (Int16)((currVal << 4) | (c - 0x57));
		}
		else
		{
			return false;
		}
	}
}

UInt8 Text::StrHex2UInt8C(UnsafeArray<const UTF8Char> str)
{
	UInt8 outVal = 0;
	UTF8Char c;
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

UOSInt Text::StrHex2Bytes(UnsafeArray<const UTF8Char> str, UnsafeArray<UInt8> buff)
{
	return StrHex2BytesS(str, buff, ' ');
}

UOSInt Text::StrHex2BytesS(UnsafeArray<const UTF8Char> str, UnsafeArray<UInt8> buff, UTF8Char seperator)
{
	UOSInt outVal = 0;
	UInt8 tmpVal;
	UTF8Char c;
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
		else if (c == seperator)
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

Int64 Text::StrOct2Int64(UnsafeArray<const UTF8Char> str)
{
	Int32 i = 22;
	Int32 outVal = 0;
	UTF8Char c;
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

UOSInt Text::StrSplit(UnsafeArray<UnsafeArray<UTF8Char>> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> strToSplit, UTF8Char splitChar)
{
	UOSInt i = 1;
	UTF8Char c;
	strs[0] = strToSplit;
	while ((c = *strToSplit++) != 0)
	{
		if (c == splitChar)
		{
			strToSplit[-1] = 0;
			strs[i++] = strToSplit;
			if (i >= maxStrs)
			{
				break;
			}
		}
	}
	return i;
}

UOSInt Text::StrSplitTrim(UnsafeArray<UnsafeArray<UTF8Char>> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> strToSplit, UTF8Char splitChar)
{
	UOSInt i = 0;
	UTF8Char c;
	UnsafeArray<UTF8Char> lastPtr;
	UnsafeArray<UTF8Char> thisPtr;
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

UOSInt Text::StrSplitLine(UnsafeArray<UnsafeArray<UTF8Char>> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> strToSplit)
{
	UOSInt i = 0;
	UTF8Char c;
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

UOSInt Text::StrSplitWS(UnsafeArray<UnsafeArray<UTF8Char>> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> strToSplit)
{
	UOSInt i = 0;
	UTF8Char c;
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

Bool Text::StrToUInt8(UnsafeArray<const UTF8Char> intStr, OutParam<UInt8> outVal)
{
	UInt32 retVal = 0;
	UInt32 c;
	while ((c = *intStr) != 0)
	{
		if (c < '0' || c > '9')
			return false;
		retVal = retVal * 10 + c - 48;
		if (retVal & 0xffffff00)
			return false;
		intStr++;
	}
	outVal.Set((UInt8)retVal);
	return true;
}

UInt8 Text::StrToUInt8(UnsafeArray<const UTF8Char> intStr)
{
	UInt32 retVal = 0;
	UInt32 c;
	while ((c = *intStr) != 0)
	{
		if (c < '0' || c > '9')
			return 0;
		retVal = retVal * 10 + c - 48;
		if (retVal & 0xffffff00)
			return 0;
		intStr++;
	}
	return (UInt8)retVal;
}

Bool Text::StrToUInt16(UnsafeArray<const UTF8Char> intStr, OutParam<UInt16> outVal)
{
	UInt32 retVal = 0;
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		retVal = (UInt16)StrHex2Int16C(intStr + 2);
	}
	else
	{
		UInt32 c;
		while ((c = *intStr) != 0)
		{
			if (c < '0' || c > '9')
				return false;
			retVal = retVal * 10 + c - 48;
			if (retVal & 0xffff0000)
				return false;
			intStr++;
		}
	}
	outVal.Set((UInt16)retVal);
	return true;
}

Bool Text::StrToUInt16S(UnsafeArray<const UTF8Char> intStr, OutParam<UInt16> outVal, UInt16 failVal)
{
	UInt32 retVal = 0;
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		retVal = (UInt16)StrHex2Int16C(intStr + 2);
	}
	else
	{
		UInt32 c;
		while ((c = *intStr) != 0)
		{
			if (c < '0' || c > '9')
			{
				outVal.Set(failVal);
				return false;
			}
			retVal = retVal * 10 + c - 48;
			if (retVal & 0xffff0000)
			{
				outVal.Set(failVal);
				return false;
			}
			intStr++;
		}
	}
	outVal.Set((UInt16)retVal);
	return true;
}

Bool Text::StrToInt16(UnsafeArray<const UTF8Char> intStr, OutParam<Int16> outVal)
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
		retVal = StrHex2Int16C(intStr + 2);
	}
	else
	{
		UInt32 c;
		while ((c = *intStr) != 0)
		{
			if (c < '0' || c > '9')
				return false;
			retVal = retVal * 10 + (Int32)c - 48;
			intStr++;
		}
	}
	if (sign)
		outVal.Set((Int16)-retVal);
	else
		outVal.Set((Int16)retVal);
	return true;
}

Int16 Text::StrToInt16(UnsafeArray<const UTF8Char> intStr)
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
		retVal = StrHex2Int16C(intStr + 2);
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

Bool Text::StrToUInt32(UnsafeArray<const UTF8Char> intStr, OutParam<UInt32> outVal)
{
	UInt32 retVal = 0;
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		retVal = StrHex2UInt32C(intStr + 2);
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
	outVal.Set(retVal);
	return true;
}

Bool Text::StrToUInt32S(UnsafeArray<const UTF8Char> intStr, OutParam<UInt32> outVal, UInt32 failVal)
{
	UInt32 retVal = 0;
	while (*intStr)
	{
		if (*intStr < '0' || *intStr > '9')
		{
			outVal.Set(failVal);
			return false;
		}
		retVal = retVal * 10 + (UInt32)*intStr - 48;
		intStr++;
	}
	outVal.Set(retVal);
	return true;
}

UInt32 Text::StrToUInt32(UnsafeArray<const UTF8Char> intStr)
{
	UInt32 retVal = 0;
	UInt32 newVal;
	UInt32 c;
	while ((c = *intStr) != 0)
	{
		if (c < '0' || c > '9')
			return 0;
		newVal = retVal * 10 + c - 48;
		if (newVal < retVal)
			return 0;
		retVal = newVal;
		intStr++;
	}
	return retVal;
}

Bool Text::StrToInt32(UnsafeArray<const UTF8Char> intStr, OutParam<Int32> outVal)
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
		if (!StrHex2Int32V(intStr + 2, retVal))
			return false;
	}
	else
	{
		UInt32 c;
		if (*intStr == 0)
			return false;
		while ((c = *intStr) != 0)
		{
			if (c < '0' || c > '9')
				return false;
			retVal = retVal * 10 + (Int32)c - 48;
			intStr++;
		}
	}
	if (sign)
		outVal.Set(-retVal);
	else
		outVal.Set(retVal);
	return true;
}

Int32 Text::StrToInt32(UnsafeArray<const UTF8Char> intStr)
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
		retVal = StrHex2Int32C(intStr + 2);
	}
	else
	{
		UInt32 c;
		while ((c = *intStr) != 0)
		{
			if (c < '0' || c > '9')
				return 0;
			retVal = retVal * 10 + (Int32)c - 48;
			intStr++;
		}
	}
	if (sign)
		return -retVal;
	else
		return retVal;
}

Bool Text::StrToInt64(UnsafeArray<const UTF8Char> intStr, OutParam<Int64> outVal)
{
	Int64 ret = Text::StrToInt64(intStr);
	if (ret == 0)
	{
		if (intStr[0] != '0' || intStr[1] != 0)
			return false;
	}
	outVal.Set(ret);
	return true;
}

Bool Text::StrToUInt64(UnsafeArray<const UTF8Char> intStr, OutParam<UInt64> outVal)
{
	if (intStr[0] == 0)
		return false;
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		return Text::StrHex2UInt64V(intStr + 2, outVal);
	}
	UInt64 c;
	UInt64 v = 0;
	while ((c = *intStr++) != 0)
	{
		if (c < '0' || c > '9')
			return false;
		v = v * 10 + c - '0';
	}
	outVal.Set(v);
	return true;
}

Bool Text::StrToUInt64S(UnsafeArray<const UTF8Char> intStr, OutParam<UInt64> outVal, UInt64 failVal)
{
	if (intStr[0] == 0)
	{
		outVal.Set(failVal);
		return false;
	}
	if (intStr[0] == '0' && intStr[1] == 'x')
	{
		return Text::StrHex2UInt64S(intStr + 2, outVal, failVal);
	}
	UInt64 c;
	UInt64 v = 0;
	while ((c = *intStr++) != 0)
	{
		if (c < '0' || c > '9')
		{
			outVal.Set(failVal);
			return false;
		}
		v = v * 10 + c - '0';
	}
	outVal.Set(v);
	return true;
}

OSInt Text::StrToOSInt(UnsafeArray<const UTF8Char> str)
{
#if _OSINT_SIZE == 64
	return Text::StrToInt64(str);
#elif _OSINT_SIZE == 32
	return Text::StrToInt32(str);
#else
	return Text::StrToInt16(str);
#endif
}

Bool Text::StrToOSInt(UnsafeArray<const UTF8Char> intStr, OutParam<OSInt> outVal)
{
#if _OSINT_SIZE == 64
	return Text::StrToInt64(intStr, outVal);
#elif _OSINT_SIZE == 32
	return Text::StrToInt32(intStr, outVal);
#else
	return Text::StrToInt16(intStr, outVal);
#endif
}


UOSInt Text::StrToUOSInt(UnsafeArray<const UTF8Char> str)
{
#if _OSINT_SIZE == 64
	return Text::StrToUInt64(str);
#elif _OSINT_SIZE == 32
	return Text::StrToUInt32(str);
#else
	return Text::StrToUInt16(str);
#endif
}

Bool Text::StrToUOSInt(UnsafeArray<const UTF8Char> intStr, OutParam<UOSInt> outVal)
{
#if _OSINT_SIZE == 64
	return Text::StrToUInt64(intStr, outVal);
#elif _OSINT_SIZE == 32
	return Text::StrToUInt32(intStr, outVal);
#else
	return Text::StrToUInt16(intStr, outVal);
#endif
}

Bool Text::StrToBool(UnsafeArrayOpt<const UTF8Char> str)
{
	UnsafeArray<const UTF8Char> nnstr;
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
		return Text::StrToInt32(nnstr) != 0;
	}
}

UOSInt Text::StrIndexOf(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2)
{
	UnsafeArray<const UTF8Char> ptr = str1;
	UnsafeArray<const UTF8Char> ptr2;
	UnsafeArray<const UTF8Char> ptr3;
	UTF8Char c;
	while (*ptr)
	{
		ptr2 = ptr;
		ptr3 = str2;
		while (true)
		{
			if ((c = *ptr3) == 0)
			{
				return (UOSInt)(ptr - str1);
			}
			else if (*ptr2++ != c)
			{
				break;
			}
			ptr3++;
		}
		ptr++;
	}
	return INVALID_INDEX;
}

UOSInt Text::StrIndexOfChar(UnsafeArray<const UTF8Char> str1, UTF8Char c)
{
	REGVAR UnsafeArray<const UTF8Char> ptr = str1;
	REGVAR UTF8Char c2;
	while ((c2 = *ptr) != 0)
		if (c2 == c)
			return (UOSInt)(ptr - str1);
		else
			ptr++;
	return INVALID_INDEX;
}

UOSInt Text::StrIndexOfCharC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UTF8Char c)
{
	REGVAR UnsafeArray<const UTF8Char> ptr = str1;
	REGVAR UInt16 c2;

	while (len1 >= 4)
	{
		if (ptr[0] == c)
			return (UOSInt)(ptr - str1);
		if (ptr[1] == c)
			return (UOSInt)(ptr - str1 + 1);
		if (ptr[2] == c)
			return (UOSInt)(ptr - str1 + 2);
		if (ptr[3] == c)
			return (UOSInt)(ptr - str1 + 3);
		ptr += 4;
		len1 -= 4;
	}
	if (len1 >= 2)
	{
		c2 = ReadUInt16(&ptr[0]);
		if ((UTF8Char)(c2 & 0xff) == c)
			return (UOSInt)(ptr - str1);
		if ((UTF8Char)(c2 >> 8) == c)
			return (UOSInt)(ptr - str1 + 1);
		ptr += 2;
		len1 -= 2;
	}
	if (len1 && (*ptr == c))
	{
		return (UOSInt)(ptr - str1);
	}
	return INVALID_INDEX;
}

UOSInt Text::StrIndexOfC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2)
{
	if (len1 < len2)
	{
		return INVALID_INDEX;
	}
	if (len1 == len2)
	{

	}
	UnsafeArray<const UTF8Char> ptr = str1;
	UnsafeArray<const UTF8Char> ptr2;
	UnsafeArray<const UTF8Char> ptr3;
	UTF8Char c;
	UInt8 v1;
	UInt16 v2;
	UInt32 v3;
	switch (len2)
	{
	case 0:
		return INVALID_INDEX;
	case 1:
		v1 = *str2;
		while (len1-- > 0)
		{
			if (*ptr == v1)
				return (UOSInt)(ptr - str1);
			ptr++;
		}
		return INVALID_INDEX;
	case 2:
		v2 = ReadNUInt16(&str2[0]);
		while (len1-- > 1)
		{
			if (ReadNUInt16(&ptr[0]) == v2)
				return (UOSInt)(ptr - str1);
			ptr++;
		}
		return INVALID_INDEX;
	case 3:
		v2 = ReadNUInt16(&str2[0]);
		v1 = str2[2];
		while (len1-- > 2)
		{
			if (ReadNUInt16(&ptr[0]) == v2 && ptr[2] == v1)
				return (UOSInt)(ptr - str1);
			ptr++;
		}
		return INVALID_INDEX;
	case 4:
		v3 = ReadNUInt32(&str2[0]);
		while (len1-- > 3)
		{
			if (ReadNUInt32(&ptr[0]) == v3)
				return (UOSInt)(ptr - str1);
			ptr++;
		}
		return INVALID_INDEX;
	default:
		while (*ptr)
		{
			ptr2 = ptr;
			ptr3 = str2;
			while (true)
			{
				if ((c = *ptr3) == 0)
				{
					return (UOSInt)(ptr - str1);
				}
				else if (*ptr2++ != c)
				{
					break;
				}
				ptr3++;
			}
			ptr++;
		}
		return INVALID_INDEX;
	}
}

UOSInt Text::StrIndexOfICase(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2)
{
	UnsafeArray<const UTF8Char> ptr = str1;
	UnsafeArray<const UTF8Char> ptr2;
	UnsafeArray<const UTF8Char> ptr3;
	UTF8Char c2;
	UTF8Char c3;
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
					c2 = (UTF8Char)(c2 - 32);
				if (c3 >= 'a' && c3 <= 'z')
					c3 = (UTF8Char)(c3 - 32);
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

UOSInt Text::StrLastIndexOfChar(UnsafeArray<const UTF8Char> str1, UTF8Char c)
{
	UnsafeArray<const UTF8Char> sptr;
	UnsafeArray<const UTF8Char> cpos = str1 - 1;
	UTF8Char ch;
	sptr = str1;
	while ((ch = *sptr++) != 0)
	{
		if (ch == c)
			cpos = sptr - 1;
	}
	return (UOSInt)(cpos - str1);
}

UOSInt Text::StrLastIndexOfCharC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UTF8Char c)
{
	while (len1 >= 4)
	{
		UInt16 c2;
		len1 -= 4;
		c2 = ReadUInt16(&str1[len1 + 2]);
		if ((UTF8Char)(c2 >> 8) == c)
			return len1 + 3;
		if ((UTF8Char)(c2 & 0xff) == c)
			return len1 + 2;
		c2 = ReadUInt16(&str1[len1]);
		if ((UTF8Char)(c2 >> 8) == c)
			return len1 + 1;
		if ((UTF8Char)(c2 & 0xff) == c)
			return len1;
	}
	if (len1 >= 2)
	{
		UInt16 c2;
		len1 -= 2;
		c2 = ReadUInt16(&str1[len1]);
		if ((UTF8Char)(c2 >> 8) == c)
			return len1 + 1;
		if ((UTF8Char)(c2 & 0xff) == c)
			return len1;
	}
	if (len1 && *str1 == c)
		return 0;
	return INVALID_INDEX;
}

UOSInt Text::StrLastIndexOfC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UTF8Char c)
{
	while (len1-- > 0)
	{
		if (str1[len1] == c)
			return len1;
	}
	return INVALID_INDEX;
}

UOSInt Text::StrLastIndexOf(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2)
{
	UOSInt leng1 = Text::StrCharCnt(str1);
	UOSInt leng2 = Text::StrCharCnt(str2);
	if (leng2 > leng1)
		return INVALID_INDEX;
	UnsafeArray<const UTF8Char> ptr = str1 + (leng1 - leng2);
	UnsafeArray<const UTF8Char> ptr2;
	UnsafeArray<const UTF8Char> ptr3;
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

Bool Text::StrContainChars(UnsafeArray<const UTF8Char> str, UnsafeArray<const UTF8Char> chars)
{
	UnsafeArray<const UTF8Char> cptr;
	UTF8Char c;
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

UnsafeArray<UTF8Char> Text::StrRTrim(UnsafeArray<UTF8Char> str1)
{
	UnsafeArray<UTF8Char> sp = str1;
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

UnsafeArray<UTF8Char> Text::StrLTrim(UnsafeArray<UTF8Char> str1)
{
	UnsafeArray<UTF8Char> sptr;
	if (str1[0] != ' ' && str1[0] != '\t')
	{
		return str1 + Text::StrCharCnt(UnsafeArray<const UTF8Char>(str1));
	}
	else
	{
		sptr = str1;
		while (*sptr == ' ' || *sptr == '\t')
			sptr++;
		return Text::StrConcat(str1, sptr);
	}
}

UnsafeArray<UTF8Char> Text::StrTrim(UnsafeArray<UTF8Char> str1)
{
	UnsafeArray<UTF8Char> sptr = str1;
	UnsafeArray<UTF8Char> sptr2;
	UTF8Char c;
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

UnsafeArray<UTF8Char> Text::StrTrimC(UnsafeArray<UTF8Char> str1, UOSInt len)
{
	while (len > 0 && (str1[len - 1] == ' ' || str1[len - 1] == '\t'))
	{
		str1[--len] = 0;
	}
	UnsafeArray<UTF8Char> sptr;
	UTF8Char c;
	if (str1[0] == ' ' || str1[0] == '\t')
	{
		sptr = str1 + 1;
		while ((c = *sptr) == ' ' || c == '\t')
			sptr++;
		len -= (UOSInt)(sptr - str1);
		MemCopyO(str1.Ptr(), sptr.Ptr(), len + 1);
	}
	return str1 + len;
}

UnsafeArray<UTF8Char> Text::StrTrimWSCRLF(UnsafeArray<UTF8Char> str1)
{
	UnsafeArray<UTF8Char> sptr = str1;
	UnsafeArray<UTF8Char> sptr2;
	UTF8Char c;
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

UnsafeArray<UTF8Char> Text::StrTrimWSCRLFC(UnsafeArray<UTF8Char> str1, UOSInt len1)
{
	while (len1 > 0 && (str1[len1 - 1] == ' ' || str1[len1 - 1] == '\t' || str1[len1 - 1] == '\r' || str1[len1 - 1] == '\n'))
	{
		str1[--len1] = 0;
	}
	UnsafeArray<UTF8Char> sptr;
	UTF8Char c;
	if (len1 > 0 && (str1[0] == ' ' || str1[0] == '\t' || str1[0] == '\r' || str1[0] == '\n'))
	{
		sptr = str1 + 1;
		while ((c = *sptr) == ' ' || c == '\t' || c == '\r' || c == '\n')
			sptr++;
		len1 -= (UOSInt)(sptr - str1);
		MemCopyO(str1.Ptr(), sptr.Ptr(), len1 + 1);
	}
	return str1 + len1;
}

UnsafeArray<UTF8Char> Text::StrRemoveChar(UnsafeArray<UTF8Char> str1, UTF8Char c)
{
	UnsafeArray<UTF8Char> sp = str1;
	UTF8Char c2;
	while (*str1)
		if ((c2 = *str1++) != c)
			*sp++ = c2;
	*sp = 0;
	return sp;
}

UnsafeArray<const UTF8Char> Text::StrCopyNew(UnsafeArray<const UTF8Char> str1)
{
	UOSInt len = Text::StrCharCnt(str1);
	UnsafeArray<UTF8Char> s = MemAllocArr(UTF8Char, len + 1);
	Text::StrConcatC(s, str1, len);
	return s;
}

UnsafeArrayOpt<const UTF8Char> Text::StrSCopyNew(UnsafeArrayOpt<const UTF8Char> str1)
{
	UnsafeArray<const UTF8Char> nns;
	if (str1.SetTo(nns))
		return StrCopyNew(nns);
	return 0;
}

UnsafeArray<const UTF8Char> Text::StrCopyNewC(UnsafeArray<const UTF8Char> str1, UOSInt strLen)
{
	UnsafeArray<UTF8Char> s = MemAllocArr(UTF8Char, strLen + 1);
	Text::StrConcatC(s, str1, strLen);
	return s;
}

void Text::StrDelNew(UnsafeArray<const UTF8Char> newStr)
{
	MemFree((void*)newStr.Ptr());
}

Bool Text::StrStartsWith(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2)
{
	while (*str2)
	{
		if (*str1++ != *str2++)
			return false;
	}
	return true;
}

Bool Text::StrStartsWithICase(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2)
{
	UTF8Char c1;
	UTF8Char c2;
	UTF8Char uc1;
	UTF8Char uc2;
	while ((c2 = *str2) != 0)
	{
		c1 = *str1;
		if (c1 == c2)
		{
			str1++;
			str2++;
		}
		else
		{
			uc1 = c1 & 0xdf;
			uc2 = c2 & 0xdf;
			if (uc1 == uc2 && uc1 >= 'A' && uc1 <= 'Z')
			{
				str1++;
				str2++;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

Bool Text::StrStartsWithICaseC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2)
{
	if (len1 < len2)
	{
		return false;
	}
	REGVAR UInt8 *upperArr = MyString_StrUpperArr;
	while (len2 >= 4)
	{
		REGVAR UInt32 v1 = ReadNUInt32(str1.Ptr());
		REGVAR UInt32 v2 = ReadNUInt32(str2.Ptr());
		if (v1 != v2)
		{
			if (upperArr[v1 & 0xff] != upperArr[v2 & 0xff])
				return false;
			if (upperArr[(v1 >> 8) & 0xff] != upperArr[(v2 >> 8) & 0xff])
				return false;
			if (upperArr[(v1 >> 16) & 0xff] != upperArr[(v2 >> 16) & 0xff])
				return false;
			if (upperArr[(v1 >> 24)] != upperArr[(v2 >> 24)])
				return false;
		}
		str1 += 4;
		str2 += 4;
		len2 -= 4;
	}
	if (len2 >= 2)
	{
		REGVAR UInt16 v1 = ReadNUInt16(str1.Ptr());
		REGVAR UInt16 v2 = ReadNUInt16(str2.Ptr());
		if (v1 != v2)
		{
			if (upperArr[v1 & 0xff] != upperArr[v2 & 0xff] ||
				upperArr[(v1 >> 8)] != upperArr[(v2 >> 8)])
				return false;
		}
		str1 += 2;
		str2 += 2;
		len2 -= 2;
	}
	if (len2 > 0)
	{
		return upperArr[*str1] == upperArr[*str2];
	}
	return true;
}

Bool Text::StrEndsWith(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2)
{
	UnsafeArray<const UTF8Char> ptr1 = str1;
	UnsafeArray<const UTF8Char> ptr2 = str2;
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

Bool Text::StrEndsWithC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2)
{
	if (len1 < len2)
	{
		return false;
	}
	while (len2 >= 4)
	{
		len2 -= 4;
		len1 -= 4;
		if (ReadNInt32(&str1[len1]) != ReadNInt32(&str2[len2]))
		{
			return false;
		}
	}
	if (len2 >= 2)
	{
		len2 -= 2;
		len1 -= 2;
		if (ReadNInt16(&str1[len1]) != ReadNInt16(&str2[len2]))
		{
			return false;
		}
	}
	if (len2 >= 1)
	{
		return str1[len1 - 1] == str2[len2 - 1];
	}
	else
	{
		return true;
	}
}

Bool Text::StrEndsWithICase(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2)
{
	UnsafeArray<const UTF8Char> ptr1 = str1;
	UnsafeArray<const UTF8Char> ptr2 = str2;
	UTF8Char c1;
	UTF8Char c2;
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
			c1 = (UTF8Char)(c1 - 32);
		}
		if (c2 >= 'a' && c2 <= 'z')
		{
			c2 = (UTF8Char)(c2 - 32);
		}
		if (c2 != c1)
			return false;
	}
	return true;
}

Bool Text::StrEndsWithICaseC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2)
{
	if (len1 < len2)
	{
		return false;
	}
	return Text::StrEqualsICaseC(str1 + (len1 - len2), len2, str2, len2);
}

Bool Text::StrIsInt32(UnsafeArray<const UTF8Char> intStr)
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

UOSInt Text::StrReplace(UnsafeArray<UTF8Char> str1, UTF8Char oriC, UTF8Char destC)
{
	UTF8Char c;
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

UOSInt Text::StrReplace(UnsafeArray<UTF8Char> str1, UnsafeArray<const UTF8Char> replaceFrom, UnsafeArray<const UTF8Char> replaceTo)
{
	UOSInt cnt;
	UOSInt fromCharCnt;
	UOSInt toCharCnt;
	UOSInt charCnt;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> strEnd;
	charCnt = Text::StrCharCnt(UnsafeArray<const UTF8Char>(str1));
	sptr = str1 + charCnt;
	strEnd = sptr;
	if ((fromCharCnt = Text::StrCharCnt(replaceFrom)) == 0)
		return 0;
	toCharCnt = Text::StrCharCnt(replaceTo);

	cnt = 0;
	sptr -= fromCharCnt;
	while (sptr >= str1)
	{
		if (Text::StrStartsWithC(sptr, (UOSInt)(strEnd - sptr), replaceFrom, fromCharCnt))
		{
			if (fromCharCnt != toCharCnt)
			{
				MemCopyO(&sptr[toCharCnt], &sptr[fromCharCnt], (charCnt - (UOSInt)(sptr - str1) - fromCharCnt + 1) * sizeof(UTF8Char));
				strEnd = strEnd + toCharCnt - fromCharCnt;
			}
			MemCopyNO(sptr.Ptr(), replaceTo.Ptr(), toCharCnt * sizeof(UTF8Char));
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

UnsafeArray<UTF8Char> Text::StrReplaceC(UnsafeArray<UTF8Char> str1, UnsafeArray<UTF8Char> strEnd, UnsafeArray<const UTF8Char> replaceFrom, UOSInt replaceFromLen, UnsafeArray<const UTF8Char> replaceTo, UOSInt replaceToLen)
{
//	UOSInt cnt;
	UOSInt charLen = (UOSInt)(strEnd - str1);
	UnsafeArray<UTF8Char> sptr;
	sptr = strEnd;
	if (replaceFromLen == 0)
		return str1;

//	cnt = 0;
	sptr -= replaceFromLen;
	while (sptr >= str1)
	{
		if (Text::StrStartsWithC(sptr, (UOSInt)(strEnd - sptr), replaceFrom, replaceFromLen))
		{
			if (replaceFromLen != replaceToLen)
			{
				MemCopyO(&sptr[replaceToLen], &sptr[replaceFromLen], (charLen - (UOSInt)(sptr - str1) - replaceFromLen + 1) * sizeof(UTF8Char));
				strEnd = strEnd + replaceToLen - replaceFromLen;
			}
			MemCopyNO(sptr.Ptr(), replaceTo.Ptr(), replaceToLen * sizeof(UTF8Char));
			sptr -= replaceFromLen;
//			cnt++;
			charLen = charLen + replaceToLen - replaceFromLen;
		}
		else
		{
			sptr -= 1;
		}
	}
	return strEnd;
}

UOSInt Text::StrReplaceICase(UnsafeArray<UTF8Char> str1, UnsafeArray<const UTF8Char> replaceFrom, UnsafeArray<const UTF8Char> replaceTo)
{
	UOSInt cnt;
	UOSInt fromCharCnt;
	UOSInt toCharCnt;
	UOSInt charCnt;
	UnsafeArray<UTF8Char> sptr;
	charCnt = Text::StrCharCnt(UnsafeArray<const UTF8Char>(str1));
	sptr = str1 + charCnt;
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
			MemCopyNO(sptr.Ptr(), replaceTo.Ptr(), toCharCnt * sizeof(Char));
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

UnsafeArray<UTF8Char> Text::StrToCSVRec(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> str1)
{
	UTF8Char c;
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

UnsafeArray<const UTF8Char> Text::StrToNewCSVRec(UnsafeArray<const UTF8Char> str1)
{
	UOSInt len = 2;
	UTF8Char c;
	UnsafeArray<const UTF8Char> sptr = str1;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> outPtr;
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
	outPtr = MemAllocArr(UTF8Char, len + 1);
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

UOSInt Text::StrCSVSplit(UnsafeArray<UnsafeArray<UTF8Char>> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> strToSplit)
{
	Bool quoted = false;
	Bool first = true;
	UOSInt i = 0;
	UnsafeArray<UTF8Char> strCurr;
	UTF8Char c;
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

UnsafeArray<UTF8Char> Text::StrCSVJoin(UnsafeArray<UTF8Char> oriStr, UnsafeArray<UnsafeArrayOpt<const UTF8Char>> strs, UOSInt nStrs)
{
	UOSInt i = 0;
	UnsafeArray<const UTF8Char> sptr;
	UTF8Char c;
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

UOSInt Text::StrCountChar(UnsafeArray<const UTF8Char> str1, UTF8Char c)
{
	UOSInt cnt = 0;
	UTF8Char c2;
	while ((c2 = *str1++) != 0)
		if (c2 == c)
			cnt++;
	return cnt;
}

UOSInt Text::StrCountStr(UnsafeArray<const UTF8Char> str1, UOSInt strLen1, UnsafeArray<const UTF8Char> str2, UOSInt strLen2)
{
	if (strLen1 < strLen2)
	{
		return 0;
	}
	UOSInt i = 0;
	if (strLen2 == 1)
	{
		UOSInt cnt = 0;
		while (i < strLen1)
		{
			if (str1[i++] == *str2)
				cnt++;
		}
		return cnt;
	}
	else if (strLen2 == 2)
	{
		UOSInt cnt = 0;
		UInt16 val = ReadNUInt16(&str2[0]);
		while (i < strLen1 - 1)
		{
			if (ReadNUInt16(&str1[i++]) == val)
			{
				cnt++;
				i++;
			}
		}
		return cnt;
	}
	else
	{
		UOSInt cnt = 0;
		UOSInt j;
		while ((j = Text::StrIndexOfC(str1 + i, strLen1 - i, str2, strLen2)) != INVALID_INDEX)
		{
			cnt++;
			i += j + strLen2;
		}
		return cnt;
	}
}

UnsafeArray<UTF8Char> Text::StrRemoveANSIEscapes(UnsafeArray<UTF8Char> str1)
{
	UTF8Char c;
	UnsafeArray<UTF8Char> dest = str1;
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

UnsafeArray<const Char> Text::StrCopyNewCh(UnsafeArray<const Char> str1)
{
	UOSInt len = Text::StrCharCntCh(str1);
	UnsafeArray<Char> s = MemAllocArr(Char, len + 1);
	Text::StrConcatC(s, str1, len);
	return s;
}

UnsafeArray<const Char> Text::StrCopyNewChC(UnsafeArray<const Char> str1, UOSInt strLen)
{
	UnsafeArray<Char> s = MemAllocArr(Char, strLen + 1);
	Text::StrConcatC(s, str1, strLen);
	return s;
}

void Text::StrDelNewCh(UnsafeArray<const Char> newStr)
{
	MemFree((void*)newStr.Ptr());
}
