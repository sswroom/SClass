#ifndef _SM_TEXT_MYSTRING
#define _SM_TEXT_MYSTRING
#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"

#define SCOPY_TEXT(var) (((var) != 0)?Text::StrCopyNew(var).Ptr():0)
#define SDEL_TEXT(var) {UnsafeArray<const UTF8Char> nnvar; if (var.SetTo(nnvar)) { Text::StrDelNew(nnvar); var = 0;}}
#define SDEL_TEXTC(var) {UnsafeArray<const Char> nnvar; if (var.SetTo(nnvar)) { Text::StrDelNewCh(nnvar); var = 0;}}

#if !defined(_WIN32) && !defined(__CYGWIN__) && !defined(__APPLE__)
#define MyString_StrDigit100U8 _MyString_StrDigit100U8
#define MyString_StrDigit100U16 _MyString_StrDigit100U16
#define MyString_StrDigit100U32 _MyString_StrDigit100U32
#define MyString_StrHexArrU16 _MyString_StrHexArrU16
#define MyString_StrHexArrU32 _MyString_StrHexArrU32
#endif

extern "C"
{
	extern Char MyString_STRHEXARR[];
	extern Char MyString_STRhexarr[];
	extern UInt8 MyString_StrUpperArr[];
	extern UInt8 MyString_StrLowerArr[];
	extern UTF8Char MyString_StrDigit100U8[];
	extern UTF16Char MyString_StrDigit100U16[];
	extern UTF32Char MyString_StrDigit100U32[];
	extern UTF16Char MyString_StrHexArrU16[];
	extern UTF32Char MyString_StrHexArrU32[];
#if _WCHAR_SIZE == 4
#define MyString_StrDigit100W MyString_StrDigit100U32
#elif _WCHAR_SIZE == 2
#define MyString_StrDigit100W MyString_StrDigit100U16
#else
#define MyString_StrDigit100W MyString_StrDigit100U8
#endif

	OSInt MyString_StrCompare(const UTF8Char *str1, const UTF8Char *str2);
	OSInt MyString_StrCompareICase(const UTF8Char *str1, const UTF8Char *str2);
	OSInt MyString_StrCompareUTF16(const UTF16Char *str1, const UTF16Char *str2);
	OSInt MyString_StrCompareICaseUTF16(const UTF16Char *str1, const UTF16Char *str2);
	OSInt MyString_StrCompareUTF32(const UTF32Char *str1, const UTF32Char *str2);
	OSInt MyString_StrCompareICaseUTF32(const UTF32Char *str1, const UTF32Char *str2);

	UOSInt MyString_StrCharCnt(const UTF8Char *oriStr);
	UOSInt MyString_StrCharCntUTF16(const UTF16Char *oriStr);
	UOSInt MyString_StrCharCntUTF32(const UTF32Char *oriStr);

	UTF16Char *MyString_StrHexVal64VUTF16(UTF16Char *oriStr, UInt64 val);
	UTF16Char *MyString_StrHexVal64UTF16(UTF16Char *oriStr, UInt64 val);
	UTF16Char *MyString_StrHexVal32VUTF16(UTF16Char *oriStr, UInt32 val);
	UTF16Char *MyString_StrHexVal32UTF16(UTF16Char *oriStr, UInt32 val);
	UTF16Char *MyString_StrHexVal24UTF16(UTF16Char *oriStr, UInt32 val);
	UTF32Char *MyString_StrHexVal64VUTF32(UTF32Char *oriStr, UInt64 val);
	UTF32Char *MyString_StrHexVal64UTF32(UTF32Char *oriStr, UInt64 val);
	UTF32Char *MyString_StrHexVal32VUTF32(UTF32Char *oriStr, UInt32 val);
	UTF32Char *MyString_StrHexVal32UTF32(UTF32Char *oriStr, UInt32 val);
	UTF32Char *MyString_StrHexVal24UTF32(UTF32Char *oriStr, UInt32 val);

	Int32 MyString_StrToInt32UTF8(const UTF8Char *intStr);
	Int32 MyString_StrToInt32UTF16(const UTF16Char *intStr);
	Int32 MyString_StrToInt32UTF32(const UTF32Char *intStr);
	Int64 MyString_StrToInt64UTF8(const UTF8Char *str1);
	Int64 MyString_StrToInt64UTF16(const UTF16Char *str1);
	Int64 MyString_StrToInt64UTF32(const UTF32Char *str1);
}

namespace Text
{
	FORCEINLINE UnsafeArray<UTF8Char> StrConcatC(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin, UOSInt charCnt)
	{
		MemCopyO(oriStr.Ptr(), strToJoin.Ptr(), charCnt);
		oriStr += charCnt;
		oriStr[0] = 0;
		return oriStr;
	}

	FORCEINLINE UnsafeArray<UTF8Char> StrConcatNE(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin, UOSInt charCnt)
	{
		MemCopyO(oriStr.Ptr(), strToJoin.Ptr(), charCnt);
		oriStr += charCnt;
		return oriStr;
	}

	FORCEINLINE UnsafeArray<UTF8Char> StrConcatCS(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin, UOSInt charCnt, UOSInt buffSize)
	{
		if (buffSize > charCnt)
		{
			return StrConcatC(oriStr, strToJoin, charCnt);
		}
		else
		{
			return StrConcatC(oriStr, strToJoin, buffSize - 1);
		}
	}

	FORCEINLINE UnsafeArray<UTF8Char> StrConcat(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin) { while ((*oriStr++ = *strToJoin++) != 0); return oriStr - 1; }
	UnsafeArray<UTF8Char> StrConcatS(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToJoin, UOSInt buffSize);
	UnsafeArray<UTF8Char> StrConcatASCII(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const Char> strToJoin);

	UnsafeArray<UTF8Char> StrInt16(UnsafeArray<UTF8Char> oriStr, Int16 val);
	UnsafeArray<UTF8Char> StrUInt16(UnsafeArray<UTF8Char> oriStr, UInt16 val);
	UnsafeArray<UTF8Char> StrInt32(UnsafeArray<UTF8Char> oriStr, Int32 val);
	UnsafeArray<UTF8Char> StrUInt32(UnsafeArray<UTF8Char> oriStr, UInt32 val);
	UnsafeArray<UTF8Char> StrInt32S(UnsafeArray<UTF8Char> oriStr, Int32 val, UTF8Char seperator, UOSInt sepCnt);
	UnsafeArray<UTF8Char> StrUInt32S(UnsafeArray<UTF8Char> oriStr, UInt32 val, UTF8Char seperator, UOSInt sepCnt);
#ifdef HAS_INT64
	UnsafeArray<UTF8Char> StrInt64(UnsafeArray<UTF8Char> oriStr, Int64 val);
	UnsafeArray<UTF8Char> StrUInt64(UnsafeArray<UTF8Char> oriStr, UInt64 val);
	UnsafeArray<UTF8Char> StrInt64S(UnsafeArray<UTF8Char> oriStr, Int64 val, UTF8Char seperator, UOSInt sepCnt);
	UnsafeArray<UTF8Char> StrUInt64S(UnsafeArray<UTF8Char> oriStr, UInt64 val, UTF8Char seperator, UOSInt sepCnt);
#endif

	UnsafeArray<UTF8Char> StrToUpper(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToProc);
	UnsafeArray<UTF8Char> StrToUpperC(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToProc, UOSInt strLen);
	UnsafeArray<UTF8Char> StrToLower(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToProc);
	UnsafeArray<UTF8Char> StrToLowerC(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToProc, UOSInt strLen);
	UnsafeArray<UTF8Char> StrToCapital(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> strToProc);

	Bool StrEquals(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2);
	Bool StrEqualsN(UnsafeArrayOpt<const UTF8Char> str1, UnsafeArrayOpt<const UTF8Char> str2);
	FORCEINLINE Bool StrEqualsC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2);
	Bool StrEqualsICase(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2);
	Bool StrEqualsICaseC(UnsafeArray<const UTF8Char> str1, UOSInt str1Len, UnsafeArray<const UTF8Char> str2, UOSInt str2Len);

	FORCEINLINE OSInt StrCompare(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2) { return MyString_StrCompare(str1.Ptr(), str2.Ptr()); }
	FORCEINLINE OSInt StrCompareICase(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2) { return MyString_StrCompareICase(str1.Ptr(), str2.Ptr()); }
	OSInt StrCompareFastC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2);

	FORCEINLINE UOSInt StrCharCnt(UnsafeArray<const UTF8Char> str)
	{
#if 0 //defined(__GNUC__)
		return (UOSInt)strlen(UnsafeArray<const Char>::Convert(str).Ptr());
#else
		return MyString_StrCharCnt(str.Ptr());
#endif
	}
	UOSInt StrCharCntS(UnsafeArray<const UTF8Char> str, UOSInt maxLen);

	Bool StrHasUpperCase(UnsafeArray<const UTF8Char> str);

#ifdef HAS_INT64
	UnsafeArray<UTF8Char> StrHexVal64V(UnsafeArray<UTF8Char> oriStr, UInt64 val);
	UnsafeArray<UTF8Char> StrHexVal64(UnsafeArray<UTF8Char> oriStr, UInt64 val);
	UnsafeArray<UTF8Char> StrHexVal64VLC(UnsafeArray<UTF8Char> oriStr, UInt64 val);
	UnsafeArray<UTF8Char> StrHexVal64LC(UnsafeArray<UTF8Char> oriStr, UInt64 val);
#endif

	UnsafeArray<UTF8Char> StrHexVal32V(UnsafeArray<UTF8Char> oriStr, UInt32 val);
	UnsafeArray<UTF8Char> StrHexVal32(UnsafeArray<UTF8Char> oriStr, UInt32 val);
	UnsafeArray<UTF8Char> StrHexVal24(UnsafeArray<UTF8Char> oriStr, UInt32 val);
	UnsafeArray<UTF8Char> StrHexVal16(UnsafeArray<UTF8Char> oriStr, UInt16 val);
	UnsafeArray<UTF8Char> StrHexByte(UnsafeArray<UTF8Char> oriStr, UInt8 val);
	UnsafeArray<UTF8Char> StrHexBytes(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UInt8> buff, UOSInt buffSize, UTF8Char seperator);
	UnsafeArray<UTF8Char> StrHexVal32VLC(UnsafeArray<UTF8Char> oriStr, UInt32 val);
	UnsafeArray<UTF8Char> StrHexVal32LC(UnsafeArray<UTF8Char> oriStr, UInt32 val);
	UnsafeArray<UTF8Char> StrHexVal24LC(UnsafeArray<UTF8Char> oriStr, UInt32 val);
	UnsafeArray<UTF8Char> StrHexVal16LC(UnsafeArray<UTF8Char> oriStr, UInt16 val);
	UnsafeArray<UTF8Char> StrHexByteLC(UnsafeArray<UTF8Char> oriStr, UInt8 val);
	UnsafeArray<UTF8Char> StrHexBytesLC(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UInt8> buff, UOSInt buffSize, UTF8Char seperator);
	Int64 StrHex2Int64C(UnsafeArray<const UTF8Char> str);
	Bool StrHex2Int64V(UnsafeArray<const UTF8Char> str, OutParam<Int64> outVal);
	Bool StrHex2Int64S(UnsafeArray<const UTF8Char> str, OutParam<Int64> outVal, Int64 failVal);
	Int32 StrHex2Int32C(UnsafeArray<const UTF8Char> str);
	Bool StrHex2Int32V(UnsafeArray<const UTF8Char> str, OutParam<Int32> outVal);
	Int16 StrHex2Int16C(UnsafeArray<const UTF8Char> str);
	Bool StrHex2Int16V(UnsafeArray<const UTF8Char> str, OutParam<Int16> outVal);
	UInt8 StrHex2UInt8C(UnsafeArray<const UTF8Char> str);
	FORCEINLINE UInt64 StrHex2UInt64C(UnsafeArray<const UTF8Char> str) { return (UInt64)StrHex2Int64C(str); }
	FORCEINLINE Bool StrHex2UInt64V(UnsafeArray<const UTF8Char> str, OutParam<UInt64> outVal) { return StrHex2Int64V(str, OutParam<Int64>::ConvertFrom(outVal)); }
	FORCEINLINE Bool StrHex2UInt64S(UnsafeArray<const UTF8Char> str, OutParam<UInt64> outVal, UInt64 failVal) { return StrHex2Int64S(str, OutParam<Int64>::ConvertFrom(outVal), (Int64)failVal); }
	FORCEINLINE UInt32 StrHex2UInt32C(UnsafeArray<const UTF8Char> str) { return (UInt32)StrHex2Int32C(str); }
	FORCEINLINE Bool StrHex2UInt32V(UnsafeArray<const UTF8Char> str, OutParam<UInt32> outVal) { return StrHex2Int32V(str, OutParam<Int32>::ConvertFrom(outVal)); }
	FORCEINLINE UInt16 StrHex2UInt16C(UnsafeArray<const UTF8Char> str) { return (UInt16)StrHex2Int16C(str); }
	FORCEINLINE Bool StrHex2UInt16V(UnsafeArray<const UTF8Char> str, OutParam<UInt16> outVal) { return StrHex2Int16V(str, OutParam<Int16>::ConvertFrom(outVal)); }
	UOSInt StrHex2Bytes(UnsafeArray<const UTF8Char> str, UnsafeArray<UInt8> buff);
	UOSInt StrHex2BytesS(UnsafeArray<const UTF8Char> str, UnsafeArray<UInt8> buff, UTF8Char seperator);
#ifdef HAS_INT64
	Int64 StrOct2Int64(UnsafeArray<const UTF8Char> str);
#endif

#if _OSINT_SIZE == 64
	FORCEINLINE UnsafeArray<UTF8Char> StrOSInt(UnsafeArray<UTF8Char> oriStr, OSInt val) { return StrInt64(oriStr, (Int64)val); }
	FORCEINLINE UnsafeArray<UTF8Char> StrOSIntS(UnsafeArray<UTF8Char> oriStr, OSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrInt64S(oriStr, (Int64)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF8Char> StrUOSInt(UnsafeArray<UTF8Char> oriStr, UOSInt val) { return StrUInt64(oriStr, (UInt64)val); }
	FORCEINLINE UnsafeArray<UTF8Char> StrUOSIntS(UnsafeArray<UTF8Char> oriStr, UOSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrUInt64S(oriStr, (UInt64)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF8Char> StrHexValOS(UnsafeArray<UTF8Char> oriStr, UOSInt val) { return StrHexVal64(oriStr, (UInt64)val); }
#elif _OSINT_SIZE == 32
	FORCEINLINE UnsafeArray<UTF8Char> StrOSInt(UnsafeArray<UTF8Char> oriStr, OSInt val) { return StrInt32(oriStr, (Int32)val); }
	FORCEINLINE UnsafeArray<UTF8Char> StrOSIntS(UnsafeArray<UTF8Char> oriStr, OSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrInt32S(oriStr, (Int32)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF8Char> StrUOSInt(UnsafeArray<UTF8Char> oriStr, UOSInt val) { return StrUInt32(oriStr, (UInt32)val); }
	FORCEINLINE UnsafeArray<UTF8Char> StrUOSIntS(UnsafeArray<UTF8Char> oriStr, UOSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrUInt32S(oriStr, (UInt32)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF8Char> StrHexValOS(UnsafeArray<UTF8Char> oriStr, UOSInt val) { return StrHexVal32(oriStr, (UInt32)val); }
#else // _OSINT_SIZE == 16
	FORCEINLINE UnsafeArray<UTF8Char> StrOSInt(UnsafeArray<UTF8Char> oriStr, OSInt val) { return StrInt32(oriStr, (Int16)val); }
	FORCEINLINE UnsafeArray<UTF8Char> StrOSIntS(UnsafeArray<UTF8Char> oriStr, OSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrInt32S(oriStr, (Int16)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF8Char> StrUOSInt(UnsafeArray<UTF8Char> oriStr, UOSInt val) { return StrUInt16(oriStr, (UInt16)val); }
	FORCEINLINE UnsafeArray<UTF8Char> StrUOSIntS(UnsafeArray<UTF8Char> oriStr, UOSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrUInt32S(oriStr, (UInt16)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF8Char> StrHexValOS(UnsafeArray<UTF8Char> oriStr, UOSInt val) { return StrHexVal16(oriStr, (UInt16)val); }
#endif

	UOSInt StrSplit(UnsafeArray<UnsafeArray<UTF8Char>> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> str, UTF8Char splitChar); //Optimized
	UOSInt StrSplitTrim(UnsafeArray<UnsafeArray<UTF8Char>> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> str, UTF8Char splitChar); //Optimized
	UOSInt StrSplitLine(UnsafeArray<UnsafeArray<UTF8Char>> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> str); //Optimized
	UOSInt StrSplitWS(UnsafeArray<UnsafeArray<UTF8Char>> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> str); //Optimized
	Bool StrToUInt8(UnsafeArray<const UTF8Char> intStr, OutParam<UInt8> outVal);
	UInt8 StrToUInt8(UnsafeArray<const UTF8Char> intStr);
	Bool StrToUInt16(UnsafeArray<const UTF8Char> intStr, OutParam<UInt16> outVal);
	Bool StrToUInt16S(UnsafeArray<const UTF8Char> intStr, OutParam<UInt16> outVal, UInt16 failVal);
	Bool StrToInt16(UnsafeArray<const UTF8Char> str, OutParam<Int16> outVal);
	Int16 StrToInt16(UnsafeArray<const UTF8Char> str);
	Bool StrToUInt32(UnsafeArray<const UTF8Char> intStr, OutParam<UInt32> outVal);
	Bool StrToUInt32S(UnsafeArray<const UTF8Char> intStr, OutParam<UInt32> outVal, UInt32 failVal);
	UInt32 StrToUInt32(UnsafeArray<const UTF8Char> intStr);
	Bool StrToInt32(UnsafeArray<const UTF8Char> str, OutParam<Int32> outVal);
	Int32 StrToInt32(UnsafeArray<const UTF8Char> str);
#ifdef HAS_INT64
	Bool StrToInt64(UnsafeArray<const UTF8Char> intStr, OutParam<Int64> outVal);
	FORCEINLINE Int64 StrToInt64(UnsafeArray<const UTF8Char> str) { return MyString_StrToInt64UTF8(str.Ptr()); }
	Bool StrToUInt64(UnsafeArray<const UTF8Char> intStr, OutParam<UInt64> outVal);
	Bool StrToUInt64S(UnsafeArray<const UTF8Char> intStr, OutParam<UInt64> outVal, UInt64 failVal);
	FORCEINLINE UInt64 StrToUInt64(UnsafeArray<const UTF8Char> str) { UInt64 v; StrToUInt64S(str, v, 0); return v; }
#endif

	OSInt StrToOSInt(UnsafeArray<const UTF8Char> str);
	Bool StrToOSInt(UnsafeArray<const UTF8Char> intStr, OutParam<OSInt> outVal);
	UOSInt StrToUOSInt(UnsafeArray<const UTF8Char> str);
	Bool StrToUOSInt(UnsafeArray<const UTF8Char> intStr, OutParam<UOSInt> outVal);
	Bool StrToBool(UnsafeArrayOpt<const UTF8Char> str);
	UOSInt StrIndexOf(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2);
	UOSInt StrIndexOfChar(UnsafeArray<const UTF8Char> str1, UTF8Char c);
	UOSInt StrIndexOfCharC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UTF8Char c);
	UOSInt StrIndexOfC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2);
	UOSInt StrIndexOfICase(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2);
	UOSInt StrLastIndexOfChar(UnsafeArray<const UTF8Char> str1, UTF8Char c);
	UOSInt StrLastIndexOfCharC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UTF8Char c);
	UOSInt StrLastIndexOfC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UTF8Char c);
	UOSInt StrLastIndexOf(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2);
	Bool StrContainChars(UnsafeArray<const UTF8Char> str, UnsafeArray<const UTF8Char> chars);
	UnsafeArray<UTF8Char> StrRTrim(UnsafeArray<UTF8Char> str1);
	UnsafeArray<UTF8Char> StrLTrim(UnsafeArray<UTF8Char> str1);
	UnsafeArray<UTF8Char> StrTrim(UnsafeArray<UTF8Char> str1);
	UnsafeArray<UTF8Char> StrTrimC(UnsafeArray<UTF8Char> str1, UOSInt len);
	UnsafeArray<UTF8Char> StrTrimWSCRLF(UnsafeArray<UTF8Char> str1);
	UnsafeArray<UTF8Char> StrTrimWSCRLFC(UnsafeArray<UTF8Char> str1, UOSInt len);
	UnsafeArray<UTF8Char> StrRemoveChar(UnsafeArray<UTF8Char> str1, UTF8Char c);

	UnsafeArray<const UTF8Char> StrCopyNew(UnsafeArray<const UTF8Char> str1);
	UnsafeArrayOpt<const UTF8Char> StrSCopyNew(UnsafeArrayOpt<const UTF8Char> str1);
	UnsafeArray<const UTF8Char> StrCopyNewC(UnsafeArray<const UTF8Char> str1, UOSInt strLen);
	void StrDelNew(UnsafeArray<const UTF8Char> newStr);
	Bool StrStartsWith(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2);
	FORCEINLINE Bool StrStartsWithC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2);
	Bool StrStartsWithICase(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2);
	Bool StrStartsWithICaseC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2);
	Bool StrEndsWith(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2);
	Bool StrEndsWithC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2);
	Bool StrEndsWithICase(UnsafeArray<const UTF8Char> str1, UnsafeArray<const UTF8Char> str2);
	Bool StrEndsWithICaseC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2);
	Bool StrIsInt32(UnsafeArray<const UTF8Char> str1);
	UOSInt StrReplace(UnsafeArray<UTF8Char> str1, UTF8Char oriC, UTF8Char destC);
	UOSInt StrReplace(UnsafeArray<UTF8Char> str1, UnsafeArray<const UTF8Char> replaceFrom, UnsafeArray<const UTF8Char> replaceTo);
	UnsafeArray<UTF8Char> StrReplaceC(UnsafeArray<UTF8Char> str1, UnsafeArray<UTF8Char> sptrEnd, UnsafeArray<const UTF8Char> replaceFrom, UOSInt replaceFromLen, UnsafeArray<const UTF8Char> replaceTo, UOSInt replaceToLen);
	UOSInt StrReplaceICase(UnsafeArray<UTF8Char> str1, UnsafeArray<const UTF8Char> replaceFrom, UnsafeArray<const UTF8Char> replaceTo);
	UnsafeArray<UTF8Char> StrToCSVRec(UnsafeArray<UTF8Char> oriStr, UnsafeArray<const UTF8Char> str1);
	UnsafeArray<const UTF8Char> StrToNewCSVRec(UnsafeArray<const UTF8Char> str1);
	UOSInt StrCSVSplit(UnsafeArray<UnsafeArray<UTF8Char>> strs, UOSInt maxStrs, UnsafeArray<UTF8Char> strToSplit);
	UnsafeArray<UTF8Char> StrCSVJoin(UnsafeArray<UTF8Char> oriStr, UnsafeArray<UnsafeArrayOpt<const UTF8Char>> strs, UOSInt nStrs);
	UOSInt StrCountChar(UnsafeArray<const UTF8Char> str1, UTF8Char c);
	UOSInt StrCountStr(UnsafeArray<const UTF8Char> str1, UOSInt strLen1, UnsafeArray<const UTF8Char> str2, UOSInt strLen2);
	UnsafeArray<UTF8Char> StrRemoveANSIEscapes(UnsafeArray<UTF8Char> str1);

	FORCEINLINE UnsafeArray<Char> StrConcat(UnsafeArray<Char> oriStr, UnsafeArray<const Char> strToJoin) { return UnsafeArray<Char>::ConvertFrom(StrConcat(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), UnsafeArray<const UTF8Char>::ConvertFrom(strToJoin))); }
	FORCEINLINE UnsafeArray<Char> StrConcatS(UnsafeArray<Char> oriStr, UnsafeArray<const Char> strToJoin, UOSInt buffSize) { return UnsafeArray<Char>::ConvertFrom(StrConcatS(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), UnsafeArray<const UTF8Char>::ConvertFrom(strToJoin), buffSize)); }
	FORCEINLINE UnsafeArray<Char> StrConcatC(UnsafeArray<Char> oriStr, UnsafeArray<const Char> strToJoin, UOSInt charCnt) { return UnsafeArray<Char>::ConvertFrom(StrConcatC(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), UnsafeArray<const UTF8Char>::ConvertFrom(strToJoin), charCnt)); }
	FORCEINLINE UnsafeArray<Char> StrInt16(UnsafeArray<Char> oriStr, Int16 val) { return UnsafeArray<Char>::ConvertFrom(StrInt16(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }; //Optimized
	FORCEINLINE UnsafeArray<Char> StrUInt16(UnsafeArray<Char> oriStr, UInt16 val) { return UnsafeArray<Char>::ConvertFrom(StrUInt16(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }; //Optimized
	FORCEINLINE UnsafeArray<Char> StrInt32(UnsafeArray<Char> oriStr, Int32 val) { return UnsafeArray<Char>::ConvertFrom(StrInt32(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }; //Optimized
	FORCEINLINE UnsafeArray<Char> StrUInt32(UnsafeArray<Char> oriStr, UInt32 val) { return UnsafeArray<Char>::ConvertFrom(StrUInt32(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }; //Optimized
	FORCEINLINE UnsafeArray<Char> StrInt32S(UnsafeArray<Char> oriStr, Int32 val, Char seperator, UOSInt sepCnt) { return UnsafeArray<Char>::ConvertFrom(StrInt32S(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val, (UTF8Char)seperator, sepCnt)); };
	FORCEINLINE UnsafeArray<Char> StrUInt32S(UnsafeArray<Char> oriStr, UInt32 val, Char seperator, UOSInt sepCnt) { return UnsafeArray<Char>::ConvertFrom(StrUInt32S(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val, (UTF8Char)seperator, sepCnt)); };
#ifdef HAS_INT64
	FORCEINLINE UnsafeArray<Char> StrInt64(UnsafeArray<Char> oriStr, Int64 val) { return UnsafeArray<Char>::ConvertFrom(StrInt64(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }; //Optimized
	FORCEINLINE UnsafeArray<Char> StrUInt64(UnsafeArray<Char> oriStr, UInt64 val) { return UnsafeArray<Char>::ConvertFrom(StrUInt64(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }; //Optimized
	FORCEINLINE UnsafeArray<Char> StrInt64S(UnsafeArray<Char> oriStr, Int64 val, Char seperator, UOSInt sepCnt) { return UnsafeArray<Char>::ConvertFrom(StrInt64S(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val, (UTF8Char)seperator, sepCnt)); };
	FORCEINLINE UnsafeArray<Char> StrUInt64S(UnsafeArray<Char> oriStr, UInt64 val, Char seperator, UOSInt sepCnt) { return UnsafeArray<Char>::ConvertFrom(StrUInt64S(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val, (UTF8Char)seperator, sepCnt)); };
#endif
	FORCEINLINE UnsafeArray<Char> StrToUpper(UnsafeArray<Char> oriStr, UnsafeArray<const Char> strToProc) { return UnsafeArray<Char>::ConvertFrom(StrToUpper(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), UnsafeArray<const UTF8Char>::ConvertFrom(strToProc))); }
	FORCEINLINE UnsafeArray<Char> StrToLower(UnsafeArray<Char> oriStr, UnsafeArray<const Char> strToProc) { return UnsafeArray<Char>::ConvertFrom(StrToLower(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), UnsafeArray<const UTF8Char>::ConvertFrom(strToProc))); }
	FORCEINLINE UnsafeArray<Char> StrToCapital(UnsafeArray<Char> oriStr, UnsafeArray<const Char> strToProc) { return UnsafeArray<Char>::ConvertFrom(StrToCapital(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), UnsafeArray<const UTF8Char>::ConvertFrom(strToProc))); }
	FORCEINLINE Bool StrEqualsCh(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return StrEquals(UnsafeArray<const UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(str2)); }
	FORCEINLINE Bool StrEqualsChN(UnsafeArrayOpt<const Char> str1, UnsafeArrayOpt<const Char> str2) { return StrEqualsN(UnsafeArrayOpt<const UTF8Char>::ConvertFrom(str1), UnsafeArrayOpt<const UTF8Char>::ConvertFrom(str2)); }
	FORCEINLINE Bool StrEqualsICaseCh(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return StrEqualsICase(UnsafeArray<const UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(str2)); }
	FORCEINLINE Bool StrEqualsICaseChC(UnsafeArray<const Char> str1, UOSInt str1Len, UnsafeArray<const Char> str2, UOSInt str2Len) { return StrEqualsICaseC(UnsafeArray<const UTF8Char>::ConvertFrom(str1), str1Len, UnsafeArray<const UTF8Char>::ConvertFrom(str2), str2Len); }
	FORCEINLINE OSInt StrCompareCh(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return MyString_StrCompare(UnsafeArray<const UTF8Char>::ConvertFrom(str1).Ptr(), UnsafeArray<const UTF8Char>::ConvertFrom(str2).Ptr()); }
	FORCEINLINE OSInt StrCompareICaseCh(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return MyString_StrCompareICase(UnsafeArray<const UTF8Char>::ConvertFrom(str1).Ptr(), UnsafeArray<const UTF8Char>::ConvertFrom(str2).Ptr()); }
	FORCEINLINE UOSInt StrCharCntCh(UnsafeArray<const Char> str) { return StrCharCnt(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	FORCEINLINE UOSInt StrCharCntSCh(UnsafeArray<const Char> str, UOSInt maxLen) { return StrCharCntS(UnsafeArray<const UTF8Char>::ConvertFrom(str), maxLen); }
	FORCEINLINE Bool StrHasUpperCaseCh(UnsafeArray<const Char> str) { return StrHasUpperCase(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }

#ifdef HAS_INT64
	FORCEINLINE UnsafeArray<Char> StrHexVal64VCh(UnsafeArray<Char> oriStr, UInt64 val) { return UnsafeArray<Char>::ConvertFrom(StrHexVal64V(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }
	FORCEINLINE UnsafeArray<Char> StrHexVal64Ch(UnsafeArray<Char> oriStr, UInt64 val) { return UnsafeArray<Char>::ConvertFrom(StrHexVal64(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }
#endif
	FORCEINLINE UnsafeArray<Char> StrHexVal32VCh(UnsafeArray<Char> oriStr, UInt32 val) { return UnsafeArray<Char>::ConvertFrom(StrHexVal32V(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }
	FORCEINLINE UnsafeArray<Char> StrHexVal32Ch(UnsafeArray<Char> oriStr, UInt32 val) { return UnsafeArray<Char>::ConvertFrom(StrHexVal32(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }
	FORCEINLINE UnsafeArray<Char> StrHexVal24Ch(UnsafeArray<Char> oriStr, UInt32 val) { return UnsafeArray<Char>::ConvertFrom(StrHexVal24(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }
	FORCEINLINE UnsafeArray<Char> StrHexVal16Ch(UnsafeArray<Char> oriStr, UInt16 val) { return UnsafeArray<Char>::ConvertFrom(StrHexVal16(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }
	FORCEINLINE UnsafeArray<Char> StrHexByteCh(UnsafeArray<Char> oriStr, UInt8 val) { return UnsafeArray<Char>::ConvertFrom(StrHexByte(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), val)); }
	FORCEINLINE UnsafeArray<Char> StrHexBytesCh(UnsafeArray<Char> oriStr, UnsafeArray<const UInt8> buff, UOSInt buffSize, Char seperator) { return UnsafeArray<Char>::ConvertFrom(StrHexBytes(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), buff, buffSize, (UTF8Char)seperator)); };
	FORCEINLINE Int64 StrHex2Int64ChC(UnsafeArray<const Char> str) { return StrHex2Int64C(UnsafeArray<const UTF8Char>::ConvertFrom(str)); };
	FORCEINLINE Int32 StrHex2Int32ChC(UnsafeArray<const Char> str) { return StrHex2Int32C(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	FORCEINLINE UInt32 StrHex2UInt32ChC(UnsafeArray<const Char> str) { return (UInt32)StrHex2Int32C(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	FORCEINLINE UInt64 StrHex2UInt64ChC(UnsafeArray<const Char> str) { return (UInt64)StrHex2Int64C(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	FORCEINLINE Int16 StrHex2Int16ChC(UnsafeArray<const Char> str) { return StrHex2Int16C(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	FORCEINLINE UInt16 StrHex2UInt16ChC(UnsafeArray<const Char> str) { return (UInt16)StrHex2Int16C(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	FORCEINLINE UInt8 StrHex2UInt8ChC(UnsafeArray<const Char> str) { return StrHex2UInt8C(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	FORCEINLINE UOSInt StrHex2BytesCh(UnsafeArray<const Char> str, UnsafeArray<UInt8> buff) { return StrHex2Bytes(UnsafeArray<const UTF8Char>::ConvertFrom(str), buff); };
#ifdef HAS_INT64
	FORCEINLINE Int64 StrOct2Int64Ch(UnsafeArray<const Char> str) { return StrOct2Int64(UnsafeArray<const UTF8Char>::ConvertFrom(str)); };
#endif

#if _OSINT_SIZE == 64
	FORCEINLINE UnsafeArray<Char> StrOSInt(UnsafeArray<Char> oriStr, OSInt val) { return StrInt64(oriStr, (Int64)val); }
	FORCEINLINE UnsafeArray<Char> StrOSIntS(UnsafeArray<Char> oriStr, OSInt val, Char seperator, UOSInt sepCnt) { return StrInt64S(oriStr, (Int64)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<Char> StrUOSInt(UnsafeArray<Char> oriStr, UOSInt val) { return StrUInt64(oriStr, (UInt64)val); }
	FORCEINLINE UnsafeArray<Char> StrUOSIntS(UnsafeArray<Char> oriStr, UOSInt val, Char seperator, UOSInt sepCnt) { return StrUInt64S(oriStr, (UInt64)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<Char> StrHexValOS(UnsafeArray<Char> oriStr, UOSInt val) { return StrHexVal64Ch(oriStr, (UInt64)val); }
#elif _OSINT_SIZE == 32
	FORCEINLINE UnsafeArray<Char> StrOSInt(UnsafeArray<Char> oriStr, OSInt val) { return StrInt32(oriStr, (Int32)val); }
	FORCEINLINE UnsafeArray<Char> StrOSIntS(UnsafeArray<Char> oriStr, OSInt val, Char seperator, UOSInt sepCnt) { return StrInt32S(oriStr, (Int32)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<Char> StrUOSInt(UnsafeArray<Char> oriStr, UOSInt val) { return StrUInt32(oriStr, (UInt32)val); }
	FORCEINLINE UnsafeArray<Char> StrUOSIntS(UnsafeArray<Char> oriStr, UOSInt val, Char seperator, UOSInt sepCnt) { return StrUInt32S(oriStr, (UInt32)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<Char> StrHexValOS(UnsafeArray<Char> oriStr, UOSInt val) { return StrHexVal32Ch(oriStr, (UInt32)val); }
#else // _OSINT_SIZE == 16
	FORCEINLINE UnsafeArray<Char> StrOSInt(UnsafeArray<Char> oriStr, OSInt val) { return StrInt32(oriStr, (Int16)val); }
	FORCEINLINE UnsafeArray<Char> StrOSIntS(UnsafeArray<Char> oriStr, OSInt val, Char seperator, UOSInt sepCnt) { return StrInt32S(oriStr, (Int16)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<Char> StrUOSInt(UnsafeArray<Char> oriStr, UOSInt val) { return StrUInt16(oriStr, (UInt16)val); }
	FORCEINLINE UnsafeArray<Char> StrUOSIntS(UnsafeArray<Char> oriStr, UOSInt val, Char seperator, UOSInt sepCnt) { return StrUInt32S(oriStr, (UInt16)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<Char> StrHexValOS(UnsafeArray<Char> oriStr, UOSInt val) { return StrHexVal16Ch(oriStr, (UInt16)val); }
#endif
	FORCEINLINE UOSInt StrSplitCh(UnsafeArray<UnsafeArray<Char>> strs, UOSInt maxStrs, UnsafeArray<Char> str, Char splitChar) { return StrSplit(UnsafeArray<UnsafeArray<UTF8Char>>::ConvertFrom(strs), maxStrs, UnsafeArray<UTF8Char>::ConvertFrom(str), (UTF8Char)splitChar); }
	FORCEINLINE UOSInt StrSplitTrimCh(UnsafeArray<UnsafeArray<Char>> strs, UOSInt maxStrs, UnsafeArray<Char> str, Char splitChar) { return StrSplitTrim(UnsafeArray<UnsafeArray<UTF8Char>>::ConvertFrom(strs), maxStrs, UnsafeArray<UTF8Char>::ConvertFrom(str), (UTF8Char)splitChar); }
	FORCEINLINE UOSInt StrSplitLineCh(UnsafeArray<UnsafeArray<Char>> strs, UOSInt maxStrs, UnsafeArray<Char> str) { return StrSplitLine(UnsafeArray<UnsafeArray<UTF8Char>>::ConvertFrom(strs), maxStrs, UnsafeArray<UTF8Char>::ConvertFrom(str)); };
	FORCEINLINE UOSInt StrSplitWSCh(UnsafeArray<UnsafeArray<Char>> strs, UOSInt maxStrs, UnsafeArray<Char> str) { return StrSplitWS(UnsafeArray<UnsafeArray<UTF8Char>>::ConvertFrom(strs), maxStrs, UnsafeArray<UTF8Char>::ConvertFrom(str)); };
	FORCEINLINE Bool StrToUInt8Ch(UnsafeArray<const Char> intStr, OutParam<UInt8> outVal) { return StrToUInt8(UnsafeArray<const UTF8Char>::ConvertFrom(intStr), outVal); };
	FORCEINLINE UInt8 StrToUInt8Ch(UnsafeArray<const Char> intStr) { return StrToUInt8(UnsafeArray<const UTF8Char>::ConvertFrom(intStr)); };
	FORCEINLINE Bool StrToUInt16Ch(UnsafeArray<const Char> intStr, OutParam<UInt16> outVal) { return StrToUInt16(UnsafeArray<const UTF8Char>::ConvertFrom(intStr), outVal); };
	FORCEINLINE Bool StrToUInt16SCh(UnsafeArray<const Char> intStr, OutParam<UInt16> outVal, UInt16 failVal) { return StrToUInt16S(UnsafeArray<const UTF8Char>::ConvertFrom(intStr), outVal, failVal); };
	FORCEINLINE Bool StrToInt16Ch(UnsafeArray<const Char> str, OutParam<Int16> outVal) { return StrToInt16(UnsafeArray<const UTF8Char>::ConvertFrom(str), outVal); }
	FORCEINLINE Int16 StrToInt16Ch(UnsafeArray<const Char> str) { return StrToInt16(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
	FORCEINLINE Bool StrToUInt32Ch(UnsafeArray<const Char> intStr, OutParam<UInt32> outVal) { return StrToUInt32(UnsafeArray<const UTF8Char>::ConvertFrom(intStr), outVal); };
	FORCEINLINE Bool StrToUInt32SCh(UnsafeArray<const Char> intStr, OutParam<UInt32> outVal, UInt32 failVal) { return StrToUInt32S(UnsafeArray<const UTF8Char>::ConvertFrom(intStr), outVal, failVal); };
	FORCEINLINE UInt32 StrToUInt32Ch(UnsafeArray<const Char> intStr) { return StrToUInt32(UnsafeArray<const UTF8Char>::ConvertFrom(intStr)); };
	FORCEINLINE Bool StrToInt32Ch(UnsafeArray<const Char> str, OutParam<Int32> outVal) { return StrToInt32(UnsafeArray<const UTF8Char>::ConvertFrom(str), outVal); }
	FORCEINLINE Int32 StrToInt32Ch(UnsafeArray<const Char> str) { return StrToInt32(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
#ifdef HAS_INT64
	FORCEINLINE Bool StrToInt64Ch(UnsafeArray<const Char> intStr, OutParam<Int64> outVal) { return StrToInt64(UnsafeArray<const UTF8Char>::ConvertFrom(intStr), outVal); }
	FORCEINLINE Int64 StrToInt64Ch(UnsafeArray<const Char> str) { return MyString_StrToInt64UTF8(UnsafeArray<const UTF8Char>::ConvertFrom(str).Ptr()); }
	FORCEINLINE Bool StrToUInt64Ch(UnsafeArray<const Char> intStr, OutParam<UInt64> outVal) { return StrToUInt64(UnsafeArray<const UTF8Char>::ConvertFrom(intStr), outVal); }
	FORCEINLINE UInt64 StrToUInt64Ch(UnsafeArray<const Char> str) { return StrToUInt64(UnsafeArray<const UTF8Char>::ConvertFrom(str)); }
#endif
	FORCEINLINE OSInt StrToOSIntCh(UnsafeArray<const Char> str) { return StrToOSInt(UnsafeArray<const UTF8Char>::ConvertFrom(str)); };
	FORCEINLINE Bool StrToOSIntCh(UnsafeArray<const Char> intStr, OutParam<OSInt> outVal) { return StrToOSInt(UnsafeArray<const UTF8Char>::ConvertFrom(intStr), outVal); };
	FORCEINLINE UOSInt StrToUOSIntCh(UnsafeArray<const Char> str) { return StrToUOSInt(UnsafeArray<const UTF8Char>::ConvertFrom(str)); };
	FORCEINLINE Bool StrToUOSIntCh(UnsafeArray<const Char> intStr, OutParam<UOSInt> outVal) { return StrToUOSInt(UnsafeArray<const UTF8Char>::ConvertFrom(intStr), outVal); };
	FORCEINLINE Bool StrToBoolCh(UnsafeArrayOpt<const Char> str) { return StrToBool(UnsafeArrayOpt<const UTF8Char>::ConvertFrom(str)); };

	FORCEINLINE UOSInt StrIndexOfCh(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return StrIndexOf(UnsafeArray<const UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(str2)); }
	FORCEINLINE UOSInt StrIndexOfCharCh(UnsafeArray<const Char> str1, Char c) { return StrIndexOfChar(UnsafeArray<const UTF8Char>::ConvertFrom(str1), (UTF8Char)c); }
	FORCEINLINE UOSInt StrIndexOfICase(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return StrIndexOfICase(UnsafeArray<const UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(str2)); };
	FORCEINLINE UOSInt StrLastIndexOfCharCh(UnsafeArray<const Char> str1, Char c) { return StrLastIndexOfChar(UnsafeArray<const UTF8Char>::ConvertFrom(str1), (UTF8Char)c); }
	FORCEINLINE UOSInt StrLastIndexOf(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return StrLastIndexOf(UnsafeArray<const UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(str2)); }
	FORCEINLINE Bool StrContainChars(UnsafeArray<const Char> str, UnsafeArray<const Char> chars) { return StrContainChars(UnsafeArray<const UTF8Char>::ConvertFrom(str), UnsafeArray<const UTF8Char>::ConvertFrom(chars)); }
	FORCEINLINE UnsafeArray<Char> StrRTrim(UnsafeArray<Char> str1) { return UnsafeArray<Char>::ConvertFrom(StrRTrim(UnsafeArray<UTF8Char>::ConvertFrom(str1))); };
	FORCEINLINE UnsafeArray<Char> StrLTrim(UnsafeArray<Char> str1) { return UnsafeArray<Char>::ConvertFrom(StrLTrim(UnsafeArray<UTF8Char>::ConvertFrom(str1))); };
	FORCEINLINE UnsafeArray<Char> StrTrim(UnsafeArray<Char> str1) { return UnsafeArray<Char>::ConvertFrom(StrTrim(UnsafeArray<UTF8Char>::ConvertFrom(str1))); };
	FORCEINLINE UnsafeArray<Char> StrTrimWSCRLF(UnsafeArray<Char> str1) { return UnsafeArray<Char>::ConvertFrom(StrTrimWSCRLF(UnsafeArray<UTF8Char>::ConvertFrom(str1))); };
	FORCEINLINE UnsafeArray<Char> StrRemoveChar(UnsafeArray<Char> str1, Char c) { return UnsafeArray<Char>::ConvertFrom(StrRemoveChar(UnsafeArray<UTF8Char>::ConvertFrom(str1), (UTF8Char)c)); };

	UnsafeArray<const Char> StrCopyNewCh(UnsafeArray<const Char> str1);
	UnsafeArray<const Char> StrCopyNewChC(UnsafeArray<const Char> str1, UOSInt strLen);
	void StrDelNewCh(UnsafeArray<const Char> newStr);
	FORCEINLINE Bool StrStartsWith(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return StrStartsWith(UnsafeArray<const UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(str2)); }
	FORCEINLINE Bool StrStartsWithICase(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return StrStartsWithICase(UnsafeArray<const UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(str2)); }
	FORCEINLINE Bool StrEndsWith(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return StrEndsWith(UnsafeArray<const UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(str2)); }
	FORCEINLINE Bool StrEndsWithICase(UnsafeArray<const Char> str1, UnsafeArray<const Char> str2) { return StrEndsWithICase(UnsafeArray<const UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(str2)); }
	FORCEINLINE Bool StrIsInt32Ch(UnsafeArray<const Char> str1) { return StrIsInt32(UnsafeArray<const UTF8Char>::ConvertFrom(str1)); };
	FORCEINLINE UOSInt StrReplaceCh(UnsafeArray<Char> str1, Char oriC, Char destC) { return StrReplace(UnsafeArray<UTF8Char>::ConvertFrom(str1), (UTF8Char)oriC, (UTF8Char)destC); };
	FORCEINLINE UOSInt StrReplaceCh(UnsafeArray<Char> str1, UnsafeArray<const Char> replaceFrom, UnsafeArray<const Char> replaceTo) { return StrReplace(UnsafeArray<UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(replaceFrom), UnsafeArray<const UTF8Char>::ConvertFrom(replaceTo)); };
	FORCEINLINE UOSInt StrReplaceICaseCh(UnsafeArray<Char> str1, UnsafeArray<const Char> replaceFrom, UnsafeArray<const Char> replaceTo) { return StrReplaceICase(UnsafeArray<UTF8Char>::ConvertFrom(str1), UnsafeArray<const UTF8Char>::ConvertFrom(replaceFrom), UnsafeArray<const UTF8Char>::ConvertFrom(replaceTo)); };
	FORCEINLINE UnsafeArray<Char> StrToCSVRec(UnsafeArray<Char> oriStr, UnsafeArray<const Char> str1) { return UnsafeArray<Char>::ConvertFrom(StrToCSVRec(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), UnsafeArray<const UTF8Char>::ConvertFrom(str1))); };
	FORCEINLINE UnsafeArray<const Char> StrToNewCSVRec(UnsafeArray<const Char> str1) { return UnsafeArray<const Char>::ConvertFrom(StrToNewCSVRec(UnsafeArray<const UTF8Char>::ConvertFrom(str1))); }
	FORCEINLINE UOSInt StrCSVSplit(UnsafeArray<UnsafeArray<Char>> strs, UOSInt maxStrs, UnsafeArray<Char> strToSplit) { return StrCSVSplit(UnsafeArray<UnsafeArray<UTF8Char>>::ConvertFrom(strs), maxStrs, UnsafeArray<UTF8Char>::ConvertFrom(strToSplit)); };
	FORCEINLINE UnsafeArray<Char> StrCSVJoin(UnsafeArray<Char> oriStr, UnsafeArray<UnsafeArrayOpt<const Char>> strs, UOSInt nStrs) { return UnsafeArray<Char>::ConvertFrom(StrCSVJoin(UnsafeArray<UTF8Char>::ConvertFrom(oriStr), UnsafeArray<UnsafeArrayOpt<const UTF8Char>>::ConvertFrom(strs), nStrs)); };
	FORCEINLINE UOSInt StrCountChar(UnsafeArray<const Char> str1, Char c) { return StrCountChar(UnsafeArray<const UTF8Char>::ConvertFrom(str1), (UTF8Char)c); };
	FORCEINLINE UnsafeArray<Char> StrRemoveANSIEscapes(UnsafeArray<Char> str1) { return UnsafeArray<Char>::ConvertFrom(StrRemoveANSIEscapes(UnsafeArray<UTF8Char>::ConvertFrom(str1))); };
}

Bool Text::StrEqualsC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2)
{
	if (len1 != len2)
	{
		return false;
	}
#if _OSINT_SIZE == 64
	while (len2 >= 8)
	{
		UInt64 v = ReadNUInt64(str1.Ptr());
		if (v != ReadNUInt64(str2.Ptr()))
		{
			return false;
		}
		str1 += 8;
		str2 += 8;
		len2 -= 8;
	}
#else
	while (len2 >= 8)
	{
		UInt32 v = ReadNUInt32(str1.Ptr());
		if (v != ReadNUInt32(str2.Ptr()))
		{
			return false;
		}
		v = ReadNUInt32(&str1[4]);
		if (v != ReadNUInt32(&str2[4]))
		{
			return false;
		}
		str1 += 8;
		str2 += 8;
		len2 -= 8;
	}
#endif
	switch (len2)
	{
	case 7:
		return ReadNUInt32(str1.Ptr()) == ReadNUInt32(str2.Ptr()) && ReadNUInt16(&str1[4]) == ReadNUInt16(&str2[4]) && str1[6] == str2[6];
	case 6:
		return ReadNUInt32(str1.Ptr()) == ReadNUInt32(str2.Ptr()) && ReadNUInt16(&str1[4]) == ReadNUInt16(&str2[4]);
	case 5:
		return ReadNUInt32(str1.Ptr()) == ReadNUInt32(str2.Ptr()) && str1[4] == str2[4];
	case 4:
		return ReadNUInt32(str1.Ptr()) == ReadNUInt32(str2.Ptr());
	case 3:
		return ReadNUInt16(str1.Ptr()) == ReadNUInt16(str2.Ptr()) && str1[2] == str2[2];
	case 2:
		return ReadNUInt16(str1.Ptr()) == ReadNUInt16(str2.Ptr());
	case 1:
		return str1[0] == str2[0];
	default:
		return true;
	}
}

Bool Text::StrStartsWithC(UnsafeArray<const UTF8Char> str1, UOSInt len1, UnsafeArray<const UTF8Char> str2, UOSInt len2)
{
	if (len1 < len2)
	{
		return false;
	}
#if _OSINT_SIZE == 64
	while (len2 >= 8)
	{
		if (ReadNUInt64(str1.Ptr()) != ReadNUInt64(str2.Ptr()))
			return false;
		str1 += 8;
		str2 += 8;
		len2 -= 8;
	}
#else
	while (len2 >= 8)
	{
		if (ReadNUInt32(str1) != ReadNUInt32(str2))
			return false;
		if (ReadNUInt32(&str1[4]) != ReadNUInt32(&str2[4]))
			return false;
		str1 += 8;
		str2 += 8;
		len2 -= 8;
	}
#endif
	switch (len2)
	{
	case 7:
		return ReadNUInt32(str1.Ptr()) == ReadNUInt32(str2.Ptr()) && ReadNUInt16(&str1[4]) == ReadNUInt16(&str2[4]) && str1[6] == str2[6];
	case 6:
		return ReadNUInt32(str1.Ptr()) == ReadNUInt32(str2.Ptr()) && ReadNUInt16(&str1[4]) == ReadNUInt16(&str2[4]);
	case 5:
		return ReadNUInt32(str1.Ptr()) == ReadNUInt32(str2.Ptr()) && str1[4] == str2[4];
	case 4:
		return ReadNUInt32(str1.Ptr()) == ReadNUInt32(str2.Ptr());
	case 3:
		return ReadNUInt16(str1.Ptr()) == ReadNUInt16(str2.Ptr()) && str1[2] == str2[2];
	case 2:
		return ReadNUInt16(str1.Ptr()) == ReadNUInt16(str2.Ptr());
	case 1:
		return str1[0] == str2[0];
	default:
		return true;
	}
}
#endif
