#ifndef _SM_TEXT_MYSTRINGW
#define _SM_TEXT_MYSTRINGW

namespace Text
{
	UnsafeArray<UTF16Char> StrConcat(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToJoin);
	UnsafeArray<UTF16Char> StrConcatS(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToJoin, UOSInt buffSize);
	UnsafeArray<UTF16Char> StrConcatC(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToJoin, UOSInt charCnt);
	UnsafeArray<UTF32Char> StrConcat(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToJoin);
	UnsafeArray<UTF32Char> StrConcatS(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToJoin, UOSInt buffSize);
	UnsafeArray<UTF32Char> StrConcatC(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToJoin, UOSInt charCnt);
	UnsafeArray<UTF16Char> StrConcatASCII(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const Char> strToJoin);
	UnsafeArray<UTF32Char> StrConcatASCII(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const Char> strToJoin);

	UnsafeArray<UTF16Char> StrInt16(UnsafeArray<UTF16Char> oriStr, Int16 val);
	UnsafeArray<UTF16Char> StrUInt16(UnsafeArray<UTF16Char> oriStr, UInt16 val);
	UnsafeArray<UTF32Char> StrInt16(UnsafeArray<UTF32Char> oriStr, Int16 val);
	UnsafeArray<UTF32Char> StrUInt16(UnsafeArray<UTF32Char> oriStr, UInt16 val);

	UnsafeArray<UTF16Char> StrInt32(UnsafeArray<UTF16Char> oriStr, Int32 val);
	UnsafeArray<UTF16Char> StrUInt32(UnsafeArray<UTF16Char> oriStr, UInt32 val);
	UnsafeArray<UTF32Char> StrInt32(UnsafeArray<UTF32Char> oriStr, Int32 val);
	UnsafeArray<UTF32Char> StrUInt32(UnsafeArray<UTF32Char> oriStr, UInt32 val);

	UnsafeArray<UTF16Char> StrInt32S(UnsafeArray<UTF16Char> oriStr, Int32 val, UTF16Char seperator, OSInt sepCnt);
	UnsafeArray<UTF32Char> StrInt32S(UnsafeArray<UTF32Char> oriStr, Int32 val, UTF32Char seperator, OSInt sepCnt);
#ifdef HAS_INT64
	UnsafeArray<UTF16Char> StrInt64(UnsafeArray<UTF16Char> oriStr, Int64 val);
	UnsafeArray<UTF16Char> StrUInt64(UnsafeArray<UTF16Char> oriStr, UInt64 val);
	UnsafeArray<UTF32Char> StrInt64(UnsafeArray<UTF32Char> oriStr, Int64 val);
	UnsafeArray<UTF32Char> StrUInt64(UnsafeArray<UTF32Char> oriStr, UInt64 val);

	UnsafeArray<UTF16Char> StrInt64S(UnsafeArray<UTF16Char> oriStr, Int64 val, UTF16Char seperator, OSInt sepCnt);
	UnsafeArray<UTF32Char> StrInt64S(UnsafeArray<UTF32Char> oriStr, Int64 val, UTF32Char seperator, OSInt sepCnt);
#endif

	UnsafeArray<UTF16Char> StrToUpper(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToProc);
	UnsafeArray<UTF32Char> StrToUpper(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToProc);
	UnsafeArray<UTF16Char> StrToLower(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToProc);
	UnsafeArray<UTF32Char> StrToLower(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToProc);
	UnsafeArray<UTF16Char> StrToCapital(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> strToProc);
	UnsafeArray<UTF32Char> StrToCapital(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> strToProc);

	Bool StrEquals(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2);
	Bool StrEquals(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2);
	Bool StrEqualsICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2);
	Bool StrEqualsICaseASCII(UnsafeArray<const UTF16Char> str1, UnsafeArray<const Char> str2);
	Bool StrEqualsICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2, OSInt str2Len);
	Bool StrEqualsICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2);
	Bool StrEqualsICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2, OSInt str2Len);

	FORCEINLINE OSInt StrCompare(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2) { return MyString_StrCompareUTF16(str1.Ptr(), str2.Ptr()); }
	FORCEINLINE OSInt StrCompareICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2) { return MyString_StrCompareICaseUTF16(str1.Ptr(), str2.Ptr()); }
	FORCEINLINE OSInt StrCompare(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2) { return MyString_StrCompareUTF32(str1.Ptr(), str2.Ptr()); }
	FORCEINLINE OSInt StrCompareICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2) { return MyString_StrCompareICaseUTF32(str1.Ptr(), str2.Ptr()); }

	FORCEINLINE UOSInt StrCharCnt(UnsafeArray<const UTF16Char> str) { return MyString_StrCharCntUTF16(str.Ptr()); }
	FORCEINLINE UOSInt StrCharCnt(UnsafeArray<const UTF32Char> str) { return MyString_StrCharCntUTF32(str.Ptr()); }
	UOSInt StrCharCntS(UnsafeArray<const UTF16Char> str, UOSInt maxLen);
	UOSInt StrCharCntS(UnsafeArray<const UTF32Char> str, UOSInt maxLen);

#ifdef HAS_INT64
	FORCEINLINE UnsafeArray<UTF16Char> StrHexVal64V(UnsafeArray<UTF16Char> oriStr, UInt64 val) { return MyString_StrHexVal64VUTF16(oriStr.Ptr(), val); }
	FORCEINLINE UnsafeArray<UTF16Char> StrHexVal64(UnsafeArray<UTF16Char> oriStr, UInt64 val) { return MyString_StrHexVal64UTF16(oriStr.Ptr(), val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrHexVal64V(UnsafeArray<UTF32Char> oriStr, UInt64 val) { return MyString_StrHexVal64VUTF32(oriStr.Ptr(), val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrHexVal64(UnsafeArray<UTF32Char> oriStr, UInt64 val) { return MyString_StrHexVal64UTF32(oriStr.Ptr(), val); }
#endif

	FORCEINLINE UnsafeArray<UTF16Char> StrHexVal32V(UnsafeArray<UTF16Char> oriStr, UInt32 val) { return MyString_StrHexVal32VUTF16(oriStr.Ptr(), val); }
	FORCEINLINE UnsafeArray<UTF16Char> StrHexVal32(UnsafeArray<UTF16Char> oriStr, UInt32 val) { return MyString_StrHexVal32UTF16(oriStr.Ptr(), val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrHexVal32V(UnsafeArray<UTF32Char> oriStr, UInt32 val) { return MyString_StrHexVal32VUTF32(oriStr.Ptr(), val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrHexVal32(UnsafeArray<UTF32Char> oriStr, UInt32 val) { return MyString_StrHexVal32UTF32(oriStr.Ptr(), val); }

	FORCEINLINE UnsafeArray<UTF16Char> StrHexVal24(UnsafeArray<UTF16Char> oriStr, UInt32 val) { return MyString_StrHexVal24UTF16(oriStr.Ptr(), val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrHexVal24(UnsafeArray<UTF32Char> oriStr, UInt32 val) { return MyString_StrHexVal24UTF32(oriStr.Ptr(), val); }

	UnsafeArray<UTF16Char> StrHexVal16(UnsafeArray<UTF16Char> oriStr, UInt16 val);
	UnsafeArray<UTF32Char> StrHexVal16(UnsafeArray<UTF32Char> oriStr, UInt16 val);

	UnsafeArray<UTF16Char> StrHexByte(UnsafeArray<UTF16Char> oriStr, UInt8 val);
	UnsafeArray<UTF32Char> StrHexByte(UnsafeArray<UTF32Char> oriStr, UInt8 val);

	UnsafeArray<UTF16Char> StrHexBytes(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UInt8> buff, OSInt buffSize, UTF16Char seperator);
	UnsafeArray<UTF32Char> StrHexBytes(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UInt8> buff, OSInt buffSize, UTF32Char seperator);
	Int64 StrHex2Int64WC(UnsafeArray<const UTF16Char> str);
	Int64 StrHex2Int64WC(UnsafeArray<const UTF32Char> str);
	Int32 StrHex2Int32WC(UnsafeArray<const UTF16Char> str);
	Int32 StrHex2Int32WC(UnsafeArray<const UTF32Char> str);
	Int16 StrHex2Int16WC(UnsafeArray<const UTF16Char> str);
	Int16 StrHex2Int16WC(UnsafeArray<const UTF32Char> str);
	FORCEINLINE UInt64 StrHex2UInt64WC(UnsafeArray<const UTF16Char> str) { return (UInt64)StrHex2Int64WC(str); }
	FORCEINLINE UInt64 StrHex2UInt64WC(UnsafeArray<const UTF32Char> str) { return (UInt64)StrHex2Int64WC(str); }
	FORCEINLINE UInt32 StrHex2UInt32WC(UnsafeArray<const UTF16Char> str) { return (UInt32)StrHex2Int32WC(str); }
	FORCEINLINE UInt32 StrHex2UInt32WC(UnsafeArray<const UTF32Char> str) { return (UInt32)StrHex2Int32WC(str); }
	FORCEINLINE UInt16 StrHex2UInt16WC(UnsafeArray<const UTF16Char> str) { return (UInt16)StrHex2Int16WC(str); }
	FORCEINLINE UInt16 StrHex2UInt16WC(UnsafeArray<const UTF32Char> str) { return (UInt16)StrHex2Int16WC(str); }
	UInt8 StrHex2UInt8WC(UnsafeArray<const UTF16Char> str);
	UInt8 StrHex2UInt8WC(UnsafeArray<const UTF32Char> str);
	UOSInt StrHex2BytesW(UnsafeArray<const UTF16Char> str, UnsafeArray<UInt8> buff);
	UOSInt StrHex2BytesW(UnsafeArray<const UTF32Char> str, UnsafeArray<UInt8> buff);
#ifdef HAS_INT64
	Int64 StrOct2Int64W(UnsafeArray<const UTF16Char> str);
	Int64 StrOct2Int64W(UnsafeArray<const UTF32Char> str);
#endif

#if _OSINT_SIZE == 64
	FORCEINLINE UnsafeArray<UTF16Char> StrOSInt(UnsafeArray<UTF16Char> oriStr, OSInt val) { return StrInt64(oriStr, (Int64)val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrOSInt(UnsafeArray<UTF32Char> oriStr, OSInt val) { return StrInt64(oriStr, (Int64)val); }
	FORCEINLINE UnsafeArray<UTF16Char> StrOSIntS(UnsafeArray<UTF16Char> oriStr, OSInt val, UTF16Char seperator, OSInt sepCnt) { return StrInt64S(oriStr, (Int64)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF32Char> StrOSIntS(UnsafeArray<UTF32Char> oriStr, OSInt val, UTF32Char seperator, OSInt sepCnt) { return StrInt64S(oriStr, (Int64)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF16Char> StrUOSInt(UnsafeArray<UTF16Char> oriStr, UOSInt val) { return StrUInt64(oriStr, (UInt64)val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrUOSInt(UnsafeArray<UTF32Char> oriStr, UOSInt val) { return StrUInt64(oriStr, (UInt64)val); }
	FORCEINLINE UnsafeArray<UTF16Char> StrHexValOS(UnsafeArray<UTF16Char> oriStr, UOSInt val) { return StrHexVal64(oriStr, (UInt64)val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrHexValOS(UnsafeArray<UTF32Char> oriStr, UOSInt val) { return StrHexVal64(oriStr, (UInt64)val); }
#elif _OSINT_SIZE == 32
	FORCEINLINE UnsafeArray<UTF16Char> StrOSInt(UnsafeArray<UTF16Char> oriStr, OSInt val) { return StrInt32(oriStr, (Int32)val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrOSInt(UnsafeArray<UTF32Char> oriStr, OSInt val) { return StrInt32(oriStr, (Int32)val); }
	FORCEINLINE UnsafeArray<UTF16Char> StrOSIntS(UnsafeArray<UTF16Char> oriStr, OSInt val, UTF16Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int32)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF32Char> StrOSIntS(UnsafeArray<UTF32Char> oriStr, OSInt val, UTF32Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int32)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF16Char> StrUOSInt(UnsafeArray<UTF16Char> oriStr, UOSInt val) { return StrUInt32(oriStr, (UInt32)val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrUOSInt(UnsafeArray<UTF32Char> oriStr, UOSInt val) { return StrUInt32(oriStr, (UInt32)val); }
	FORCEINLINE UnsafeArray<UTF16Char> StrHexValOS(UnsafeArray<UTF16Char> oriStr, UOSInt val) { return StrHexVal32(oriStr, (UInt32)val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrHexValOS(UnsafeArray<UTF32Char> oriStr, UOSInt val) { return StrHexVal32(oriStr, (UInt32)val); }
#else // _OSINT_SIZE == 16
	FORCEINLINE UnsafeArray<UTF16Char> StrOSInt(UnsafeArray<UTF16Char> oriStr, OSInt val) { return StrInt32(oriStr, (Int16)val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrOSInt(UnsafeArray<UTF32Char> oriStr, OSInt val) { return StrInt32(oriStr, (Int16)val); }
	FORCEINLINE UnsafeArray<UTF16Char> StrOSIntS(UnsafeArray<UTF16Char> oriStr, OSInt val, UTF16Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int16)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF32Char> StrOSIntS(UnsafeArray<UTF32Char> oriStr, OSInt val, UTF32Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int16)val, seperator, sepCnt); }
	FORCEINLINE UnsafeArray<UTF16Char> StrUOSInt(UnsafeArray<UTF16Char> oriStr, UOSInt val) { return StrUInt16(oriStr, (UInt16)val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrUOSInt(UnsafeArray<UTF32Char> oriStr, UOSInt val) { return StrUInt16(oriStr, (UInt16)val); }
	FORCEINLINE UnsafeArray<UTF16Char> StrHexValOS(UnsafeArray<UTF16Char> oriStr, UOSInt val) { return StrHexVal16(oriStr, (UInt16)val); }
	FORCEINLINE UnsafeArray<UTF32Char> StrHexValOS(UnsafeArray<UTF32Char> oriStr, UOSInt val) { return StrHexVal16(oriStr, (UInt16)val); }
#endif

	UOSInt StrSplit(UnsafeArray<UnsafeArray<UTF16Char>> strs, UOSInt maxStrs, UnsafeArray<UTF16Char> str, UTF16Char splitChar); //Optimized
	UOSInt StrSplit(UnsafeArray<UnsafeArray<UTF32Char>> strs, UOSInt maxStrs, UnsafeArray<UTF32Char> str, UTF32Char splitChar); //Optimized
	UOSInt StrSplitTrim(UnsafeArray<UnsafeArray<UTF16Char>> strs, UOSInt maxStrs, UnsafeArray<UTF16Char> str, UTF16Char splitChar); //Optimized
	UOSInt StrSplitTrim(UnsafeArray<UnsafeArray<UTF32Char>> strs, UOSInt maxStrs, UnsafeArray<UTF32Char> str, UTF32Char splitChar); //Optimized
	UOSInt StrSplitLine(UnsafeArray<UnsafeArray<UTF16Char>> strs, UOSInt maxStrs, UnsafeArray<UTF16Char> str); //Optimized
	UOSInt StrSplitLine(UnsafeArray<UnsafeArray<UTF32Char>> strs, UOSInt maxStrs, UnsafeArray<UTF32Char> str); //Optimized
	UOSInt StrSplitWS(UnsafeArray<UnsafeArray<UTF16Char>> strs, UOSInt maxStrs, UnsafeArray<UTF16Char> str); //Optimized
	UOSInt StrSplitWS(UnsafeArray<UnsafeArray<UTF32Char>> strs, UOSInt maxStrs, UnsafeArray<UTF32Char> str); //Optimized
	Bool StrToUInt8W(UnsafeArray<const UTF16Char> intStr, OutParam<UInt8> outVal);
	Bool StrToUInt8W(UnsafeArray<const UTF32Char> intStr, OutParam<UInt8> outVal);
	FORCEINLINE Bool StrToUInt8W(UnsafeArray<WChar> intStr, OutParam<UInt8> outVal) { return StrToUInt8W(UnsafeArray<const WChar>(intStr), outVal); }
	UInt8 StrToUInt8W(UnsafeArray<const UTF16Char> intStr);
	UInt8 StrToUInt8W(UnsafeArray<const UTF32Char> intStr);
	Bool StrToUInt16W(UnsafeArray<const UTF16Char> intStr, OutParam<UInt16> outVal);
	Bool StrToUInt16W(UnsafeArray<const UTF32Char> intStr, OutParam<UInt16> outVal);
	Bool StrToInt16W(UnsafeArray<const UTF16Char> intStr, OutParam<Int16> outVal);
	Bool StrToInt16W(UnsafeArray<const UTF32Char> intStr, OutParam<Int16> outVal);
	Int16 StrToInt16W(UnsafeArray<const UTF16Char> str);
	Int16 StrToInt16W(UnsafeArray<const UTF32Char> str);
	Bool StrToUInt32W(UnsafeArray<const UTF16Char> intStr, OutParam<UInt32> outVal);
	Bool StrToUInt32W(UnsafeArray<const UTF32Char> intStr, OutParam<UInt32> outVal);
	UInt32 StrToUInt32W(UnsafeArray<const UTF16Char> intStr);
	UInt32 StrToUInt32W(UnsafeArray<const UTF32Char> intStr);
	Bool StrToInt32W(UnsafeArray<const UTF16Char> intStr, OutParam<Int32> outVal);
	Bool StrToInt32W(UnsafeArray<const UTF32Char> intStr, OutParam<Int32> outVal);
	Int32 StrToInt32W(UnsafeArray<const UTF16Char> str);
	Int32 StrToInt32W(UnsafeArray<const UTF32Char> str);
	FORCEINLINE Int32 StrToInt32W(UnsafeArray<WChar> intStr) { return StrToInt32W(UnsafeArray<const WChar>(intStr)); }

#ifdef HAS_INT64
	Bool StrToInt64W(UnsafeArray<const UTF16Char> intStr, OutParam<Int64> outVal);
	Bool StrToInt64W(UnsafeArray<const UTF32Char> intStr, OutParam<Int64> outVal);
	FORCEINLINE Int64 StrToInt64W(UnsafeArray<const UTF16Char> str) { return MyString_StrToInt64UTF16(str.Ptr()); }
	FORCEINLINE Int64 StrToInt64W(UnsafeArray<const UTF32Char> str) { return MyString_StrToInt64UTF32(str.Ptr()); }
	Bool StrToUInt64W(UnsafeArray<const UTF16Char> intStr, OutParam<UInt64> outVal);
	Bool StrToUInt64W(UnsafeArray<const UTF32Char> intStr, OutParam<UInt64> outVal);
	Bool StrToUInt64SW(UnsafeArray<const UTF16Char> intStr, OutParam<UInt64> outVal, UInt64 failVal);
	Bool StrToUInt64SW(UnsafeArray<const UTF32Char> intStr, OutParam<UInt64> outVal, UInt64 failVal);
	UInt64 StrToUInt64W(UnsafeArray<const UTF16Char> str);
	UInt64 StrToUInt64W(UnsafeArray<const UTF32Char> str);
#endif

	OSInt StrToOSIntW(UnsafeArray<const UTF16Char> str);
	OSInt StrToOSIntW(UnsafeArray<const UTF32Char> str);
	Bool StrToOSIntW(UnsafeArray<const UTF16Char> intStr, OutParam<OSInt> outVal);
	Bool StrToOSIntW(UnsafeArray<const UTF32Char> intStr, OutParam<OSInt> outVal);
	UOSInt StrToUOSIntW(UnsafeArray<const UTF16Char> str);
	UOSInt StrToUOSIntW(UnsafeArray<const UTF32Char> str);
	Bool StrToUOSIntW(UnsafeArray<const UTF16Char> intStr, OutParam<UOSInt> outVal);
	Bool StrToUOSIntW(UnsafeArray<const UTF32Char> intStr, OutParam<UOSInt> outVal);

	Bool StrToBoolW(UnsafeArrayOpt<const UTF16Char> str);
	Bool StrToBoolW(UnsafeArrayOpt<const UTF32Char> str);
	UOSInt StrIndexOfW(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2);
	UOSInt StrIndexOfCharW(UnsafeArray<const UTF16Char> str1, UTF16Char c);
	UOSInt StrIndexOfW(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2);
	UOSInt StrIndexOfCharW(UnsafeArray<const UTF32Char> str1, UTF32Char c);
	UOSInt StrIndexOfICaseW(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2);
	UOSInt StrIndexOfICaseW(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2);
	UOSInt StrLastIndexOfCharW(UnsafeArray<const UTF16Char> str1, UTF16Char c);
	UOSInt StrLastIndexOfCharW(UnsafeArray<const UTF32Char> str1, UTF32Char c);
	UOSInt StrLastIndexOfW(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2);
	UOSInt StrLastIndexOfW(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2);
	UnsafeArray<UTF16Char> StrRTrim(UnsafeArray<UTF16Char> str1);
	UnsafeArray<UTF32Char> StrRTrim(UnsafeArray<UTF32Char> str1);
	UnsafeArray<UTF16Char> StrLTrim(UnsafeArray<UTF16Char> str1);
	UnsafeArray<UTF32Char> StrLTrim(UnsafeArray<UTF32Char> str1);
	UnsafeArray<UTF16Char> StrTrim(UnsafeArray<UTF16Char> str1);
	UnsafeArray<UTF32Char> StrTrim(UnsafeArray<UTF32Char> str1);
	UnsafeArray<UTF16Char> StrTrimWSCRLF(UnsafeArray<UTF16Char> str1);
	UnsafeArray<UTF32Char> StrTrimWSCRLF(UnsafeArray<UTF32Char> str1);
	UnsafeArray<UTF16Char> StrRemoveChar(UnsafeArray<UTF16Char> str1, UTF16Char c);
	UnsafeArray<UTF32Char> StrRemoveChar(UnsafeArray<UTF32Char> str1, UTF32Char c);

	UnsafeArray<const UTF16Char> StrCopyNew(UnsafeArray<const UTF16Char> str1);
	UnsafeArray<const UTF32Char> StrCopyNew(UnsafeArray<const UTF32Char> str1);
	UnsafeArray<const UTF16Char> StrCopyNewC(UnsafeArray<const UTF16Char> str1, UOSInt strLen);
	UnsafeArray<const UTF32Char> StrCopyNewC(UnsafeArray<const UTF32Char> str1, UOSInt strLen);
#if _WCHAR_SIZE == 4
	UnsafeArray<const WChar> StrCopyNewUTF16_W(UnsafeArray<const UTF16Char> str1);
#endif
	UnsafeArray<const UTF8Char> StrToUTF8New(UnsafeArray<const UTF16Char> str1);
	UnsafeArray<const UTF8Char> StrToUTF8New(UnsafeArray<const UTF32Char> str1);
	UnsafeArray<const WChar> StrToWCharNew(UnsafeArray<const UTF8Char> str1);
	void StrDelNew(UnsafeArray<const UTF16Char> newStr);
	void StrDelNew(UnsafeArray<const UTF32Char> newStr);

	Bool StrStartsWith(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2);
	Bool StrStartsWith(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2);
	Bool StrStartsWithICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2);
	Bool StrStartsWithICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2);
	Bool StrStartsWithICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const Char> str2);
	Bool StrStartsWithICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const Char> str2);
	Bool StrEndsWith(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2);
	Bool StrEndsWith(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2);
	Bool StrEndsWithICase(UnsafeArray<const UTF16Char> str1, UnsafeArray<const UTF16Char> str2);
	Bool StrEndsWithICase(UnsafeArray<const UTF32Char> str1, UnsafeArray<const UTF32Char> str2);
	Bool StrIsInt32W(UnsafeArray<const UTF16Char> str1);
	Bool StrIsInt32W(UnsafeArray<const UTF32Char> str1);
	UOSInt StrReplaceW(UnsafeArray<UTF16Char> str1, UTF16Char oriC, UTF16Char destC);
	UOSInt StrReplaceW(UnsafeArray<UTF32Char> str1, UTF32Char oriC, UTF32Char destC);
	UOSInt StrReplaceW(UnsafeArray<UTF16Char> str1, UnsafeArray<const UTF16Char> replaceFrom, UnsafeArray<const UTF16Char> replaceTo);
	UOSInt StrReplaceW(UnsafeArray<UTF32Char> str1, UnsafeArray<const UTF32Char> replaceFrom, UnsafeArray<const UTF32Char> replaceTo);
	UOSInt StrReplaceICaseW(UnsafeArray<UTF16Char> str1, UnsafeArray<const UTF16Char> replaceFrom, UnsafeArray<const UTF16Char> replaceTo);
	UOSInt StrReplaceICaseW(UnsafeArray<UTF32Char> str1, UnsafeArray<const UTF32Char> replaceFrom, UnsafeArray<const UTF32Char> replaceTo);
	UnsafeArray<UTF16Char> StrToCSVRec(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF16Char> str1);
	UnsafeArray<UTF32Char> StrToCSVRec(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF32Char> str1);
	UnsafeArray<const UTF16Char> StrToNewCSVRec(UnsafeArray<const UTF16Char> str1);
	UnsafeArray<const UTF32Char> StrToNewCSVRec(UnsafeArray<const UTF32Char> str1);
	UOSInt StrCSVSplit(UnsafeArray<UnsafeArray<UTF16Char>> strs, UOSInt maxStrs, UnsafeArray<UTF16Char> strToSplit);
	UOSInt StrCSVSplit(UnsafeArray<UnsafeArray<UTF32Char>> strs, UOSInt maxStrs, UnsafeArray<UTF32Char> strToSplit);
	UnsafeArray<UTF16Char> StrCSVJoin(UnsafeArray<UTF16Char> oriStr, UnsafeArray<UnsafeArrayOpt<const UTF16Char>> strs, UOSInt nStrs);
	UnsafeArray<UTF32Char> StrCSVJoin(UnsafeArray<UTF32Char> oriStr, UnsafeArray<UnsafeArrayOpt<const UTF32Char>> strs, UOSInt nStrs);
	UOSInt StrCountChar(UnsafeArray<UTF16Char> str1, UTF16Char c);
	UOSInt StrCountChar(UnsafeArray<UTF32Char> str1, UTF32Char c);

	UnsafeArray<UTF16Char> StrUTF8_UTF16C(UnsafeArray<UTF16Char> buff, UnsafeArray<const UTF8Char> bytes, UOSInt byteSize, OptOut<UOSInt> byteConv);
	UOSInt StrUTF8_UTF16CntC(UnsafeArray<const UTF8Char> bytes, UOSInt byteSize);
	UnsafeArray<UTF32Char> StrUTF8_UTF32C(UnsafeArray<UTF32Char> buff, UnsafeArray<const UTF8Char> bytes, UOSInt byteSize, OptOut<UOSInt>  byteConv);
	UOSInt StrUTF8_UTF32CntC(UnsafeArray<const UTF8Char> bytes, UOSInt byteSize);

	//byteConv includes NULL
	UnsafeArray<UTF16Char> StrUTF8_UTF16(UnsafeArray<UTF16Char> buff, UnsafeArray<const UTF8Char> bytes, OptOut<UOSInt> byteConv);
	UOSInt StrUTF8_UTF16Cnt(UnsafeArray<const UTF8Char> bytes);
	UnsafeArray<UTF32Char> StrUTF8_UTF32(UnsafeArray<UTF32Char> buff, UnsafeArray<const UTF8Char> bytes, OptOut<UOSInt> byteConv);
	UOSInt StrUTF8_UTF32Cnt(UnsafeArray<const UTF8Char> bytes);

	UnsafeArray<UTF8Char> StrUTF16_UTF8(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UTF16Char> wstr);
	UnsafeArray<UTF8Char> StrUTF16_UTF8C(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UTF16Char> wstr, UOSInt strLen);
	UOSInt StrUTF16_UTF8Cnt(UnsafeArray<const UTF16Char> stri);
	UOSInt StrUTF16_UTF8CntC(UnsafeArray<const UTF16Char> stri, UOSInt strLen);
	UnsafeArray<UTF8Char> StrUTF32_UTF8(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UTF32Char> wstr);
	UnsafeArray<UTF8Char> StrUTF32_UTF8C(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UTF32Char> wstr, UOSInt strLen);
	UOSInt StrUTF32_UTF8Cnt(UnsafeArray<const UTF32Char> stri);
	UOSInt StrUTF32_UTF8CntC(UnsafeArray<const UTF32Char> stri, UOSInt strLen);

	UnsafeArray<UTF8Char> StrUTF16BE_UTF8(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UInt8> u16Buff);
	UnsafeArray<UTF8Char> StrUTF16BE_UTF8C(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UInt8> u16Buff, UOSInt utf16Cnt);
	UOSInt StrUTF16BE_UTF8Cnt(UnsafeArray<const UInt8> u16Buff);
	UOSInt StrUTF16BE_UTF8CntC(UnsafeArray<const UInt8> u16Buff, UOSInt utf16Cnt);

	FORCEINLINE UnsafeArray<UTF16Char> StrUTF8_UTF16(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF8Char> strToJoin) { return StrUTF8_UTF16(oriStr, strToJoin, 0); }
	FORCEINLINE UnsafeArray<UTF32Char> StrUTF8_UTF32(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF8Char> strToJoin) { return StrUTF8_UTF32(oriStr, strToJoin, 0); }
	UnsafeArray<UTF32Char> StrUTF16_UTF32(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF16Char> strToJoin);
	UnsafeArray<UTF32Char> StrUTF16_UTF32(UnsafeArray<UTF32Char> oriStr, UnsafeArray<const UTF16Char> strToJoin, UOSInt charCnt);
	UOSInt StrUTF16_UTF32Cnt(UnsafeArray<const UTF16Char> strToJoin);
	UOSInt StrUTF16_UTF32Cnt(UnsafeArray<const UTF16Char> strToJoin, UOSInt charCnt);
	UnsafeArray<UTF16Char> StrUTF32_UTF16(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF32Char> strToJoin);
	UnsafeArray<UTF16Char> StrUTF32_UTF16(UnsafeArray<UTF16Char> oriStr, UnsafeArray<const UTF32Char> strToJoin, UOSInt charCnt);
	UOSInt StrUTF32_UTF16Cnt(UnsafeArray<const UTF32Char> strToJoin);
	UOSInt StrUTF32_UTF16Cnt(UnsafeArray<const UTF32Char> strToJoin, UOSInt charCnt);

#if _WCHAR_SIZE == 4
	FORCEINLINE UnsafeArray<WChar> StrUTF8_WCharC(UnsafeArray<WChar> buff, UnsafeArray<const UTF8Char> bytes, UOSInt byteSize, OptOut<UOSInt> byteConv) { return UnsafeArray<WChar>::ConvertFrom(StrUTF8_UTF32C(UnsafeArray<UTF32Char>::ConvertFrom(buff), bytes, byteSize, byteConv)); }
	FORCEINLINE UOSInt StrUTF8_WCharCntC(UnsafeArray<const UTF8Char> bytes, UOSInt byteSize) { return StrUTF8_UTF32CntC(bytes, byteSize); }
	FORCEINLINE UnsafeArray<WChar> StrUTF8_WChar(UnsafeArray<WChar> buff, UnsafeArray<const UTF8Char> bytes, OptOut<UOSInt> byteConv) { return UnsafeArray<WChar>::ConvertFrom(StrUTF8_UTF32(UnsafeArray<UTF32Char>::ConvertFrom(buff), bytes, byteConv)); }
	FORCEINLINE UOSInt StrUTF8_WCharCnt(UnsafeArray<const UTF8Char> bytes) { return StrUTF8_UTF32Cnt(bytes); }
	FORCEINLINE UnsafeArray<UTF8Char> StrWChar_UTF8C(UnsafeArray<UTF8Char> bytes, UnsafeArray<const WChar> wstr, UOSInt strLen) { return StrUTF32_UTF8C(bytes, UnsafeArray<const UTF32Char>::ConvertFrom(wstr), strLen); }
	FORCEINLINE UOSInt StrWChar_UTF8CntC(UnsafeArray<const WChar> stri, UOSInt strLen) { return StrUTF32_UTF8CntC(UnsafeArray<const UTF32Char>::ConvertFrom(stri), strLen); }
	FORCEINLINE UnsafeArray<UTF8Char> StrWChar_UTF8(UnsafeArray<UTF8Char> bytes, UnsafeArray<const WChar> wstr) { return StrUTF32_UTF8(bytes, UnsafeArray<const UTF32Char>::ConvertFrom(wstr)); }
	FORCEINLINE UOSInt StrWChar_UTF8Cnt(UnsafeArray<const WChar> stri) { return StrUTF32_UTF8Cnt(UnsafeArray<const UTF32Char>::ConvertFrom(stri)); }
#elif _WCHAR_SIZE == 2
	FORCEINLINE UnsafeArray<WChar> StrUTF8_WCharC(UnsafeArray<WChar> buff, UnsafeArray<const UTF8Char> bytes, UOSInt byteSize, OptOut<UOSInt> byteConv) { return StrUTF8_UTF16C(buff, bytes, byteSize, byteConv); }
	FORCEINLINE UOSInt StrUTF8_WCharCntC(UnsafeArray<const UTF8Char> bytes, UOSInt byteSize) { return StrUTF8_UTF16CntC(bytes, byteSize); }
	FORCEINLINE UnsafeArray<WChar> StrUTF8_WChar(UnsafeArray<WChar> buff, UnsafeArray<const UTF8Char> bytes, OptOut<UOSInt> byteConv) { return StrUTF8_UTF16(buff, bytes, byteConv); }
	FORCEINLINE UOSInt StrUTF8_WCharCnt(UnsafeArray<const UTF8Char> bytes) { return StrUTF8_UTF16Cnt(bytes); }
	FORCEINLINE UnsafeArray<UTF8Char> StrWChar_UTF8C(UnsafeArray<UTF8Char> bytes, UnsafeArray<const WChar> wstr, UOSInt strLen) { return StrUTF16_UTF8C(bytes, wstr, strLen); }
	FORCEINLINE UOSInt StrWChar_UTF8CntC(UnsafeArray<const WChar> stri, UOSInt strLen) { return StrUTF16_UTF8CntC(stri, strLen); }
	FORCEINLINE UnsafeArray<UTF8Char> StrWChar_UTF8(UnsafeArray<UTF8Char> bytes, UnsafeArray<const WChar> wstr) { return StrUTF16_UTF8(bytes, wstr); }
	FORCEINLINE UOSInt StrWChar_UTF8Cnt(UnsafeArray<const WChar> stri) { return StrUTF16_UTF8Cnt(stri); }
#endif
	UnsafeArray<const UTF8Char> StrReadChar(UnsafeArray<const UTF8Char> sptr, OutParam<UTF32Char> outChar);
	UnsafeArray<const UTF16Char> StrReadChar(UnsafeArray<const UTF16Char> sptr, OutParam<UTF32Char> outChar);
	UnsafeArray<const UTF32Char> StrReadChar(UnsafeArray<const UTF32Char> sptr, OutParam<UTF32Char> outChar);
	UnsafeArray<UTF8Char> StrWriteChar(UnsafeArray<UTF8Char> sptr, UTF32Char c);
	UnsafeArray<UTF16Char> StrWriteChar(UnsafeArray<UTF16Char> sptr, UTF32Char c);
	UnsafeArray<UTF32Char> StrWriteChar(UnsafeArray<UTF32Char> sptr, UTF32Char c);
}
#endif
