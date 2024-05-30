#ifndef _SM_TEXT_MYSTRINGW
#define _SM_TEXT_MYSTRINGW

namespace Text
{
	UTF16Char *StrConcat(UTF16Char *oriStr, const UTF16Char *strToJoin);
	UTF16Char *StrConcatS(UTF16Char *oriStr, const UTF16Char *strToJoin, UOSInt buffSize);
	UTF16Char *StrConcatC(UTF16Char *oriStr, const UTF16Char *strToJoin, UOSInt charCnt);
	UTF32Char *StrConcat(UTF32Char *oriStr, const UTF32Char *strToJoin);
	UTF32Char *StrConcatS(UTF32Char *oriStr, const UTF32Char *strToJoin, UOSInt buffSize);
	UTF32Char *StrConcatC(UTF32Char *oriStr, const UTF32Char *strToJoin, UOSInt charCnt);
	UTF16Char *StrConcatASCII(UTF16Char *oriStr, const Char *strToJoin);
	UTF32Char *StrConcatASCII(UTF32Char *oriStr, const Char *strToJoin);

	UTF16Char *StrInt16(UTF16Char *oriStr, Int16 val);
	UTF16Char *StrUInt16(UTF16Char *oriStr, UInt16 val);
	UTF32Char *StrInt16(UTF32Char *oriStr, Int16 val);
	UTF32Char *StrUInt16(UTF32Char *oriStr, UInt16 val);

	UTF16Char *StrInt32(UTF16Char *oriStr, Int32 val);
	UTF16Char *StrUInt32(UTF16Char *oriStr, UInt32 val);
	UTF32Char *StrInt32(UTF32Char *oriStr, Int32 val);
	UTF32Char *StrUInt32(UTF32Char *oriStr, UInt32 val);

	UTF16Char *StrInt32S(UTF16Char *oriStr, Int32 val, UTF16Char seperator, OSInt sepCnt);
	UTF32Char *StrInt32S(UTF32Char *oriStr, Int32 val, UTF32Char seperator, OSInt sepCnt);
#ifdef HAS_INT64
	UTF16Char *StrInt64(UTF16Char *oriStr, Int64 val);
	UTF16Char *StrUInt64(UTF16Char *oriStr, UInt64 val);
	UTF32Char *StrInt64(UTF32Char *oriStr, Int64 val);
	UTF32Char *StrUInt64(UTF32Char *oriStr, UInt64 val);

	UTF16Char *StrInt64S(UTF16Char *oriStr, Int64 val, UTF16Char seperator, OSInt sepCnt);
	UTF32Char *StrInt64S(UTF32Char *oriStr, Int64 val, UTF32Char seperator, OSInt sepCnt);
#endif

	UTF16Char *StrToUpper(UTF16Char *oriStr, const UTF16Char *strToProc);
	UTF32Char *StrToUpper(UTF32Char *oriStr, const UTF32Char *strToProc);
	UTF16Char *StrToLower(UTF16Char *oriStr, const UTF16Char *strToProc);
	UTF32Char *StrToLower(UTF32Char *oriStr, const UTF32Char *strToProc);
	UTF16Char *StrToCapital(UTF16Char *oriStr, const UTF16Char *strToProc);
	UTF32Char *StrToCapital(UTF32Char *oriStr, const UTF32Char *strToProc);

	Bool StrEquals(const UTF16Char *str1, const UTF16Char *str2);
	Bool StrEquals(const UTF32Char *str1, const UTF32Char *str2);
	Bool StrEqualsICase(const UTF16Char *str1, const UTF16Char *str2);
	Bool StrEqualsICaseASCII(const UTF16Char *str1, const Char *str2);
	Bool StrEqualsICase(const UTF16Char *str1, const UTF16Char *str2, OSInt str2Len);
	Bool StrEqualsICase(const UTF32Char *str1, const UTF32Char *str2);
	Bool StrEqualsICase(const UTF32Char *str1, const UTF32Char *str2, OSInt str2Len);

	FORCEINLINE OSInt StrCompare(const UTF16Char *str1, const UTF16Char *str2) { return MyString_StrCompareUTF16(str1, str2); }
	FORCEINLINE OSInt StrCompareICase(const UTF16Char *str1, const UTF16Char *str2) { return MyString_StrCompareICaseUTF16(str1, str2); }
	FORCEINLINE OSInt StrCompare(const UTF32Char *str1, const UTF32Char *str2) { return MyString_StrCompareUTF32(str1, str2); }
	FORCEINLINE OSInt StrCompareICase(const UTF32Char *str1, const UTF32Char *str2) { return MyString_StrCompareICaseUTF32(str1, str2); }

	FORCEINLINE UOSInt StrCharCnt(const UTF16Char *str) { return MyString_StrCharCntUTF16(str); }
	FORCEINLINE UOSInt StrCharCnt(const UTF32Char *str) { return MyString_StrCharCntUTF32(str); }
	UOSInt StrCharCntS(const UTF16Char *str, UOSInt maxLen);
	UOSInt StrCharCntS(const UTF32Char *str, UOSInt maxLen);

#ifdef HAS_INT64
	FORCEINLINE UTF16Char *StrHexVal64V(UTF16Char *oriStr, UInt64 val) { return MyString_StrHexVal64VUTF16(oriStr, val); }
	FORCEINLINE UTF16Char *StrHexVal64(UTF16Char *oriStr, UInt64 val) { return MyString_StrHexVal64UTF16(oriStr, val); }
	FORCEINLINE UTF32Char *StrHexVal64V(UTF32Char *oriStr, UInt64 val) { return MyString_StrHexVal64VUTF32(oriStr, val); }
	FORCEINLINE UTF32Char *StrHexVal64(UTF32Char *oriStr, UInt64 val) { return MyString_StrHexVal64UTF32(oriStr, val); }
#endif

	FORCEINLINE UTF16Char *StrHexVal32V(UTF16Char *oriStr, UInt32 val) { return MyString_StrHexVal32VUTF16(oriStr, val); }
	FORCEINLINE UTF16Char *StrHexVal32(UTF16Char *oriStr, UInt32 val) { return MyString_StrHexVal32UTF16(oriStr, val); }
	FORCEINLINE UTF32Char *StrHexVal32V(UTF32Char *oriStr, UInt32 val) { return MyString_StrHexVal32VUTF32(oriStr, val); }
	FORCEINLINE UTF32Char *StrHexVal32(UTF32Char *oriStr, UInt32 val) { return MyString_StrHexVal32UTF32(oriStr, val); }

	FORCEINLINE UTF16Char *StrHexVal24(UTF16Char *oriStr, UInt32 val) { return MyString_StrHexVal24UTF16(oriStr, val); }
	FORCEINLINE UTF32Char *StrHexVal24(UTF32Char *oriStr, UInt32 val) { return MyString_StrHexVal24UTF32(oriStr, val); }

	UTF16Char *StrHexVal16(UTF16Char *oriStr, UInt16 val);
	UTF32Char *StrHexVal16(UTF32Char *oriStr, UInt16 val);

	UTF16Char *StrHexByte(UTF16Char *oriStr, UInt8 val);
	UTF32Char *StrHexByte(UTF32Char *oriStr, UInt8 val);

	UTF16Char *StrHexBytes(UTF16Char *oriStr, const UInt8 *buff, OSInt buffSize, UTF16Char seperator);
	UTF32Char *StrHexBytes(UTF32Char *oriStr, const UInt8 *buff, OSInt buffSize, UTF32Char seperator);
	Int64 StrHex2Int64C(const UTF16Char *str);
	Int64 StrHex2Int64C(const UTF32Char *str);
	Int32 StrHex2Int32C(const UTF16Char *str);
	Int32 StrHex2Int32C(const UTF32Char *str);
	Int16 StrHex2Int16C(const UTF16Char *str);
	Int16 StrHex2Int16C(const UTF32Char *str);
	FORCEINLINE UInt64 StrHex2UInt64C(const UTF16Char *str) { return (UInt64)StrHex2Int64C(str); }
	FORCEINLINE UInt64 StrHex2UInt64C(const UTF32Char *str) { return (UInt64)StrHex2Int64C(str); }
	FORCEINLINE UInt32 StrHex2UInt32C(const UTF16Char *str) { return (UInt32)StrHex2Int32C(str); }
	FORCEINLINE UInt32 StrHex2UInt32C(const UTF32Char *str) { return (UInt32)StrHex2Int32C(str); }
	FORCEINLINE UInt16 StrHex2UInt16C(const UTF16Char *str) { return (UInt16)StrHex2Int16C(str); }
	FORCEINLINE UInt16 StrHex2UInt16C(const UTF32Char *str) { return (UInt16)StrHex2Int16C(str); }
	UInt8 StrHex2UInt8C(const UTF16Char *str);
	UInt8 StrHex2UInt8C(const UTF32Char *str);
	UOSInt StrHex2Bytes(const UTF16Char *str, UInt8 *buff);
	UOSInt StrHex2Bytes(const UTF32Char *str, UInt8 *buff);
#ifdef HAS_INT64
	Int64 StrOct2Int64(const UTF16Char *str);
	Int64 StrOct2Int64(const UTF32Char *str);
#endif

#if _OSINT_SIZE == 64
	FORCEINLINE UTF16Char *StrOSInt(UTF16Char *oriStr, OSInt val) { return StrInt64(oriStr, (Int64)val); }
	FORCEINLINE UTF32Char *StrOSInt(UTF32Char *oriStr, OSInt val) { return StrInt64(oriStr, (Int64)val); }
	FORCEINLINE UTF16Char *StrOSIntS(UTF16Char *oriStr, OSInt val, UTF16Char seperator, OSInt sepCnt) { return StrInt64S(oriStr, (Int64)val, seperator, sepCnt); }
	FORCEINLINE UTF32Char *StrOSIntS(UTF32Char *oriStr, OSInt val, UTF32Char seperator, OSInt sepCnt) { return StrInt64S(oriStr, (Int64)val, seperator, sepCnt); }
	FORCEINLINE UTF16Char *StrUOSInt(UTF16Char *oriStr, UOSInt val) { return StrUInt64(oriStr, (UInt64)val); }
	FORCEINLINE UTF32Char *StrUOSInt(UTF32Char *oriStr, UOSInt val) { return StrUInt64(oriStr, (UInt64)val); }
	FORCEINLINE UTF16Char *StrHexValOS(UTF16Char *oriStr, UOSInt val) { return StrHexVal64(oriStr, (UInt64)val); }
	FORCEINLINE UTF32Char *StrHexValOS(UTF32Char *oriStr, UOSInt val) { return StrHexVal64(oriStr, (UInt64)val); }
#elif _OSINT_SIZE == 32
	FORCEINLINE UTF16Char *StrOSInt(UTF16Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int32)val); }
	FORCEINLINE UTF32Char *StrOSInt(UTF32Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int32)val); }
	FORCEINLINE UTF16Char *StrOSIntS(UTF16Char *oriStr, OSInt val, UTF16Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int32)val, seperator, sepCnt); }
	FORCEINLINE UTF32Char *StrOSIntS(UTF32Char *oriStr, OSInt val, UTF32Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int32)val, seperator, sepCnt); }
	FORCEINLINE UTF16Char *StrUOSInt(UTF16Char *oriStr, UOSInt val) { return StrUInt32(oriStr, (UInt32)val); }
	FORCEINLINE UTF32Char *StrUOSInt(UTF32Char *oriStr, UOSInt val) { return StrUInt32(oriStr, (UInt32)val); }
	FORCEINLINE UTF16Char *StrHexValOS(UTF16Char *oriStr, UOSInt val) { return StrHexVal32(oriStr, (UInt32)val); }
	FORCEINLINE UTF32Char *StrHexValOS(UTF32Char *oriStr, UOSInt val) { return StrHexVal32(oriStr, (UInt32)val); }
#else // _OSINT_SIZE == 16
	FORCEINLINE UTF16Char *StrOSInt(UTF16Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int16)val); }
	FORCEINLINE UTF32Char *StrOSInt(UTF32Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int16)val); }
	FORCEINLINE UTF16Char *StrOSIntS(UTF16Char *oriStr, OSInt val, UTF16Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int16)val, seperator, sepCnt); }
	FORCEINLINE UTF32Char *StrOSIntS(UTF32Char *oriStr, OSInt val, UTF32Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int16)val, seperator, sepCnt); }
	FORCEINLINE UTF16Char *StrUOSInt(UTF16Char *oriStr, UOSInt val) { return StrUInt16(oriStr, (UInt16)val); }
	FORCEINLINE UTF32Char *StrUOSInt(UTF32Char *oriStr, UOSInt val) { return StrUInt16(oriStr, (UInt16)val); }
	FORCEINLINE UTF16Char *StrHexValOS(UTF16Char *oriStr, UOSInt val) { return StrHexVal16(oriStr, (UInt16)val); }
	FORCEINLINE UTF32Char *StrHexValOS(UTF32Char *oriStr, UOSInt val) { return StrHexVal16(oriStr, (UInt16)val); }
#endif

	UOSInt StrSplit(UTF16Char **strs, UOSInt maxStrs, UTF16Char *str, UTF16Char splitChar); //Optimized
	UOSInt StrSplit(UTF32Char **strs, UOSInt maxStrs, UTF32Char *str, UTF32Char splitChar); //Optimized
	UOSInt StrSplitTrim(UTF16Char **strs, UOSInt maxStrs, UTF16Char *str, UTF16Char splitChar); //Optimized
	UOSInt StrSplitTrim(UTF32Char **strs, UOSInt maxStrs, UTF32Char *str, UTF32Char splitChar); //Optimized
	UOSInt StrSplitLine(UTF16Char **strs, UOSInt maxStrs, UTF16Char *str); //Optimized
	UOSInt StrSplitLine(UTF32Char **strs, UOSInt maxStrs, UTF32Char *str); //Optimized
	UOSInt StrSplitWS(UTF16Char **strs, UOSInt maxStrs, UTF16Char *str); //Optimized
	UOSInt StrSplitWS(UTF32Char **strs, UOSInt maxStrs, UTF32Char *str); //Optimized
	Bool StrToUInt8(const UTF16Char *intStr, UInt8 *outVal);
	Bool StrToUInt8(const UTF32Char *intStr, UInt8 *outVal);
	UInt8 StrToUInt8(const UTF16Char *intStr);
	UInt8 StrToUInt8(const UTF32Char *intStr);
	Bool StrToUInt16(const UTF16Char *intStr, UInt16 *outVal);
	Bool StrToUInt16(const UTF32Char *intStr, UInt16 *outVal);
	Bool StrToInt16(const UTF16Char *intStr, Int16 *outVal);
	Bool StrToInt16(const UTF32Char *intStr, Int16 *outVal);
	Int16 StrToInt16(const UTF16Char *str);
	Int16 StrToInt16(const UTF32Char *str);
	Bool StrToUInt32(const UTF16Char *intStr, UInt32 *outVal);
	Bool StrToUInt32(const UTF32Char *intStr, UInt32 *outVal);
	UInt32 StrToUInt32(const UTF16Char *intStr);
	UInt32 StrToUInt32(const UTF32Char *intStr);
	Bool StrToInt32(const UTF16Char *intStr, Int32 *outVal);
	Bool StrToInt32(const UTF32Char *intStr, Int32 *outVal);
	Int32 StrToInt32(const UTF16Char *str);
	Int32 StrToInt32(const UTF32Char *str);

#ifdef HAS_INT64
	Bool StrToInt64(const UTF16Char *intStr, Int64 *outVal);
	Bool StrToInt64(const UTF32Char *intStr, Int64 *outVal);
	FORCEINLINE Int64 StrToInt64(const UTF16Char *str) { return MyString_StrToInt64UTF16(str); }
	FORCEINLINE Int64 StrToInt64(const UTF32Char *str) { return MyString_StrToInt64UTF32(str); }
	Bool StrToUInt64(const UTF16Char *intStr, UInt64 *outVal);
	Bool StrToUInt64(const UTF32Char *intStr, UInt64 *outVal);
	Bool StrToUInt64S(const UTF16Char *intStr, UInt64 *outVal, UInt64 failVal);
	Bool StrToUInt64S(const UTF32Char *intStr, UInt64 *outVal, UInt64 failVal);
	UInt64 StrToUInt64(const UTF16Char *str);
	UInt64 StrToUInt64(const UTF32Char *str);
#endif

	OSInt StrToOSInt(const UTF16Char *str);
	OSInt StrToOSInt(const UTF32Char *str);
	Bool StrToOSInt(const UTF16Char *intStr, OSInt *outVal);
	Bool StrToOSInt(const UTF32Char *intStr, OSInt *outVal);
	UOSInt StrToUOSInt(const UTF16Char *str);
	UOSInt StrToUOSInt(const UTF32Char *str);
	Bool StrToUOSInt(const UTF16Char *intStr, UOSInt *outVal);
	Bool StrToUOSInt(const UTF32Char *intStr, UOSInt *outVal);

	Bool StrToBool(const UTF16Char *str);
	Bool StrToBool(const UTF32Char *str);
	UOSInt StrIndexOf(const UTF16Char *str1, const UTF16Char *str2);
	UOSInt StrIndexOfChar(const UTF16Char *str1, UTF16Char c);
	UOSInt StrIndexOf(const UTF32Char *str1, const UTF32Char *str2);
	UOSInt StrIndexOfChar(const UTF32Char *str1, UTF32Char c);
	UOSInt StrIndexOfICase(const UTF16Char *str1, const UTF16Char *str2);
	UOSInt StrIndexOfICase(const UTF32Char *str1, const UTF32Char *str2);
	UOSInt StrLastIndexOfChar(const UTF16Char *str1, UTF16Char c);
	UOSInt StrLastIndexOfChar(const UTF32Char *str1, UTF32Char c);
	UOSInt StrLastIndexOf(const UTF16Char *str1, const UTF16Char *str2);
	UOSInt StrLastIndexOf(const UTF32Char *str1, const UTF32Char *str2);
	UTF16Char *StrRTrim(UTF16Char* str1);
	UTF32Char *StrRTrim(UTF32Char* str1);
	UTF16Char *StrLTrim(UTF16Char* str1);
	UTF32Char *StrLTrim(UTF32Char* str1);
	UTF16Char *StrTrim(UTF16Char* str1);
	UTF32Char *StrTrim(UTF32Char* str1);
	UTF16Char *StrTrimWSCRLF(UTF16Char* str1);
	UTF32Char *StrTrimWSCRLF(UTF32Char* str1);
	UTF16Char *StrRemoveChar(UTF16Char *str1, UTF16Char c);
	UTF32Char *StrRemoveChar(UTF32Char *str1, UTF32Char c);

	const UTF16Char *StrCopyNew(const UTF16Char *str1);
	const UTF32Char *StrCopyNew(const UTF32Char *str1);
	const UTF16Char *StrCopyNewC(const UTF16Char *str1, UOSInt strLen);
	const UTF32Char *StrCopyNewC(const UTF32Char *str1, UOSInt strLen);
#if _WCHAR_SIZE == 4
	const WChar *StrCopyNewUTF16_W(const UTF16Char *str1);
#endif
	UnsafeArray<const UTF8Char> StrToUTF8New(const UTF16Char *str1);
	UnsafeArray<const UTF8Char> StrToUTF8New(const UTF32Char *str1);
	const WChar *StrToWCharNew(UnsafeArray<const UTF8Char> str1);
	void StrDelNew(const UTF16Char *newStr);
	void StrDelNew(const UTF32Char *newStr);

	Bool StrStartsWith(const UTF16Char *str1, const UTF16Char *str2);
	Bool StrStartsWith(const UTF32Char *str1, const UTF32Char *str2);
	Bool StrStartsWithICase(const UTF16Char *str1, const UTF16Char *str2);
	Bool StrStartsWithICase(const UTF32Char *str1, const UTF32Char *str2);
	Bool StrStartsWithICase(const UTF16Char *str1, const Char *str2);
	Bool StrStartsWithICase(const UTF32Char *str1, const Char *str2);
	Bool StrEndsWith(const UTF16Char *str1, const UTF16Char *str2);
	Bool StrEndsWith(const UTF32Char *str1, const UTF32Char *str2);
	Bool StrEndsWithICase(const UTF16Char *str1, const UTF16Char *str2);
	Bool StrEndsWithICase(const UTF32Char *str1, const UTF32Char *str2);
	Bool StrIsInt32(const UTF16Char *str1);
	Bool StrIsInt32(const UTF32Char *str1);
	UOSInt StrReplaceW(UTF16Char *str1, UTF16Char oriC, UTF16Char destC);
	UOSInt StrReplaceW(UTF32Char *str1, UTF32Char oriC, UTF32Char destC);
	UOSInt StrReplaceW(UTF16Char *str1, const UTF16Char *replaceFrom, const UTF16Char *replaceTo);
	UOSInt StrReplaceW(UTF32Char *str1, const UTF32Char *replaceFrom, const UTF32Char *replaceTo);
	UOSInt StrReplaceICaseW(UTF16Char *str1, const UTF16Char *replaceFrom, const UTF16Char *replaceTo);
	UOSInt StrReplaceICaseW(UTF32Char *str1, const UTF32Char *replaceFrom, const UTF32Char *replaceTo);
	UTF16Char *StrToCSVRec(UTF16Char *oriStr, const UTF16Char *str1);
	UTF32Char *StrToCSVRec(UTF32Char *oriStr, const UTF32Char *str1);
	const UTF16Char *StrToNewCSVRec(const UTF16Char *str1);
	const UTF32Char *StrToNewCSVRec(const UTF32Char *str1);
	UOSInt StrCSVSplit(UTF16Char **strs, UOSInt maxStrs, UTF16Char *strToSplit);
	UOSInt StrCSVSplit(UTF32Char **strs, UOSInt maxStrs, UTF32Char *strToSplit);
	UTF16Char *StrCSVJoin(UTF16Char *oriStr, const UTF16Char **strs, UOSInt nStrs);
	UTF32Char *StrCSVJoin(UTF32Char *oriStr, const UTF32Char **strs, UOSInt nStrs);
	UOSInt StrCountChar(UTF16Char *str1, UTF16Char c);
	UOSInt StrCountChar(UTF32Char *str1, UTF32Char c);

	UTF16Char *StrUTF8_UTF16C(UTF16Char *buff, UnsafeArray<const UTF8Char> bytes, UOSInt byteSize, OptOut<UOSInt> byteConv);
	UOSInt StrUTF8_UTF16CntC(UnsafeArray<const UTF8Char> bytes, UOSInt byteSize);
	UTF32Char *StrUTF8_UTF32C(UTF32Char *buff, UnsafeArray<const UTF8Char> bytes, UOSInt byteSize, OptOut<UOSInt>  byteConv);
	UOSInt StrUTF8_UTF32CntC(UnsafeArray<const UTF8Char> bytes, UOSInt byteSize);

	//byteConv includes NULL
	UTF16Char *StrUTF8_UTF16(UTF16Char *buff, UnsafeArray<const UTF8Char> bytes, OptOut<UOSInt> byteConv);
	UOSInt StrUTF8_UTF16Cnt(UnsafeArray<const UTF8Char> bytes);
	UTF32Char *StrUTF8_UTF32(UTF32Char *buff, UnsafeArray<const UTF8Char> bytes, OptOut<UOSInt> byteConv);
	UOSInt StrUTF8_UTF32Cnt(UnsafeArray<const UTF8Char> bytes);

	UnsafeArray<UTF8Char> StrUTF16_UTF8(UnsafeArray<UTF8Char> bytes, const UTF16Char *wstr);
	UnsafeArray<UTF8Char> StrUTF16_UTF8C(UnsafeArray<UTF8Char> bytes, const UTF16Char *wstr, UOSInt strLen);
	UOSInt StrUTF16_UTF8Cnt(const UTF16Char *stri);
	UOSInt StrUTF16_UTF8CntC(const UTF16Char *stri, UOSInt strLen);
	UnsafeArray<UTF8Char> StrUTF32_UTF8(UnsafeArray<UTF8Char> bytes, const UTF32Char *wstr);
	UnsafeArray<UTF8Char> StrUTF32_UTF8C(UnsafeArray<UTF8Char> bytes, const UTF32Char *wstr, UOSInt strLen);
	UOSInt StrUTF32_UTF8Cnt(const UTF32Char *stri);
	UOSInt StrUTF32_UTF8CntC(const UTF32Char *stri, UOSInt strLen);

	UnsafeArray<UTF8Char> StrUTF16BE_UTF8(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UInt8> u16Buff);
	UnsafeArray<UTF8Char> StrUTF16BE_UTF8C(UnsafeArray<UTF8Char> bytes, UnsafeArray<const UInt8> u16Buff, UOSInt utf16Cnt);
	UOSInt StrUTF16BE_UTF8Cnt(UnsafeArray<const UInt8> u16Buff);
	UOSInt StrUTF16BE_UTF8CntC(UnsafeArray<const UInt8> u16Buff, UOSInt utf16Cnt);

	FORCEINLINE UTF16Char *StrUTF8_UTF16(UTF16Char *oriStr, UnsafeArray<const UTF8Char> strToJoin) { return StrUTF8_UTF16(oriStr, strToJoin, 0); }
	FORCEINLINE UTF32Char *StrUTF8_UTF32(UTF32Char *oriStr, UnsafeArray<const UTF8Char> strToJoin) { return StrUTF8_UTF32(oriStr, strToJoin, 0); }
	UTF32Char *StrUTF16_UTF32(UTF32Char *oriStr, const UTF16Char *strToJoin);
	UTF32Char *StrUTF16_UTF32(UTF32Char *oriStr, const UTF16Char *strToJoin, UOSInt charCnt);
	UOSInt StrUTF16_UTF32Cnt(const UTF16Char *strToJoin);
	UOSInt StrUTF16_UTF32Cnt(const UTF16Char *strToJoin, UOSInt charCnt);
	UTF16Char *StrUTF32_UTF16(UTF16Char *oriStr, const UTF32Char *strToJoin);
	UTF16Char *StrUTF32_UTF16(UTF16Char *oriStr, const UTF32Char *strToJoin, UOSInt charCnt);
	UOSInt StrUTF32_UTF16Cnt(const UTF32Char *strToJoin);
	UOSInt StrUTF32_UTF16Cnt(const UTF32Char *strToJoin, UOSInt charCnt);

#if _WCHAR_SIZE == 4
	FORCEINLINE WChar *StrUTF8_WCharC(WChar *buff, UnsafeArray<const UTF8Char> bytes, UOSInt byteSize, OptOut<UOSInt> byteConv) { return (WChar*)StrUTF8_UTF32C((UTF32Char*)buff, bytes, byteSize, byteConv); }
	FORCEINLINE UOSInt StrUTF8_WCharCntC(UnsafeArray<const UTF8Char> bytes, UOSInt byteSize) { return StrUTF8_UTF32CntC(bytes, byteSize); }
	FORCEINLINE WChar *StrUTF8_WChar(WChar *buff, UnsafeArray<const UTF8Char> bytes, OptOut<UOSInt> byteConv) { return (WChar*)StrUTF8_UTF32((UTF32Char*)buff, bytes, byteConv); }
	FORCEINLINE UOSInt StrUTF8_WCharCnt(UnsafeArray<const UTF8Char> bytes) { return StrUTF8_UTF32Cnt(bytes); }
	FORCEINLINE UnsafeArray<UTF8Char> StrWChar_UTF8C(UnsafeArray<UTF8Char> bytes, const WChar *wstr, UOSInt strLen) { return StrUTF32_UTF8C(bytes, (const UTF32Char*)wstr, strLen); }
	FORCEINLINE UOSInt StrWChar_UTF8CntC(const WChar *stri, UOSInt strLen) { return StrUTF32_UTF8CntC((const UTF32Char*)stri, strLen); }
	FORCEINLINE UnsafeArray<UTF8Char> StrWChar_UTF8(UnsafeArray<UTF8Char> bytes, const WChar *wstr) { return StrUTF32_UTF8(bytes, (const UTF32Char*)wstr); }
	FORCEINLINE UOSInt StrWChar_UTF8Cnt(const WChar *stri) { return StrUTF32_UTF8Cnt((const UTF32Char*)stri); }
#elif _WCHAR_SIZE == 2
	FORCEINLINE WChar *StrUTF8_WCharC(WChar *buff, UnsafeArray<const UTF8Char> bytes, UOSInt byteSize, OptOut<UOSInt> byteConv) { return StrUTF8_UTF16C(buff, bytes, byteSize, byteConv); }
	FORCEINLINE UOSInt StrUTF8_WCharCntC(UnsafeArray<const UTF8Char> bytes, UOSInt byteSize) { return StrUTF8_UTF16CntC(bytes, byteSize); }
	FORCEINLINE WChar *StrUTF8_WChar(WChar *buff, UnsafeArray<const UTF8Char> bytes, OptOut<UOSInt> byteConv) { return StrUTF8_UTF16(buff, bytes, byteConv); }
	FORCEINLINE UOSInt StrUTF8_WCharCnt(UnsafeArray<const UTF8Char> bytes) { return StrUTF8_UTF16Cnt(bytes); }
	FORCEINLINE UnsafeArray<UTF8Char> StrWChar_UTF8C(UnsafeArray<UTF8Char> bytes, const WChar *wstr, UOSInt strLen) { return StrUTF16_UTF8C(bytes, wstr, strLen); }
	FORCEINLINE UOSInt StrWChar_UTF8CntC(const WChar *stri, UOSInt strLen) { return StrUTF16_UTF8CntC(stri, strLen); }
	FORCEINLINE UnsafeArray<UTF8Char> StrWChar_UTF8(UnsafeArray<UTF8Char> bytes, const WChar *wstr) { return StrUTF16_UTF8(bytes, wstr); }
	FORCEINLINE UOSInt StrWChar_UTF8Cnt(const WChar *stri) { return StrUTF16_UTF8Cnt(stri); }
#endif
	UnsafeArray<const UTF8Char> StrReadChar(UnsafeArray<const UTF8Char> sptr, OutParam<UTF32Char> outChar);
	const UTF16Char *StrReadChar(const UTF16Char *sptr, OutParam<UTF32Char> outChar);
	const UTF32Char *StrReadChar(const UTF32Char *sptr, OutParam<UTF32Char> outChar);
	UnsafeArray<UTF8Char> StrWriteChar(UnsafeArray<UTF8Char> sptr, UTF32Char c);
	UTF16Char *StrWriteChar(UTF16Char *sptr, UTF32Char c);
	UTF32Char *StrWriteChar(UTF32Char *sptr, UTF32Char c);
}
#endif
