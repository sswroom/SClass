#ifndef _SM_TEXT_MYSTRING
#define _SM_TEXT_MYSTRING

#define SCOPY_TEXT(var) (((var) != 0)?Text::StrCopyNew(var):0)
#define SDEL_TEXT(var) if (var != 0) { Text::StrDelNew(var); var = 0;}

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

	OSInt MyString_StrCompare(const Char *str1, const Char *str2);
	OSInt MyString_StrCompareICase(const Char *str1, const Char *str2);
	OSInt MyString_StrCompareUTF16(const UTF16Char *str1, const UTF16Char *str2);
	OSInt MyString_StrCompareICaseUTF16(const UTF16Char *str1, const UTF16Char *str2);
	OSInt MyString_StrCompareUTF32(const UTF32Char *str1, const UTF32Char *str2);
	OSInt MyString_StrCompareICaseUTF32(const UTF32Char *str1, const UTF32Char *str2);

	UOSInt MyString_StrCharCnt(const Char *oriStr);
	UOSInt MyString_StrCharCntUTF16(const UTF16Char *oriStr);
	UOSInt MyString_StrCharCntUTF32(const UTF32Char *oriStr);

	UTF16Char *MyString_StrHexVal64VUTF16(UTF16Char *oriStr, Int64 val);
	UTF16Char *MyString_StrHexVal64UTF16(UTF16Char *oriStr, Int64 val);
	UTF16Char *MyString_StrHexVal32VUTF16(UTF16Char *oriStr, Int32 val);
	UTF16Char *MyString_StrHexVal32UTF16(UTF16Char *oriStr, Int32 val);
	UTF16Char *MyString_StrHexVal24UTF16(UTF16Char *oriStr, Int32 val);
	UTF32Char *MyString_StrHexVal64VUTF32(UTF32Char *oriStr, Int64 val);
	UTF32Char *MyString_StrHexVal64UTF32(UTF32Char *oriStr, Int64 val);
	UTF32Char *MyString_StrHexVal32VUTF32(UTF32Char *oriStr, Int32 val);
	UTF32Char *MyString_StrHexVal32UTF32(UTF32Char *oriStr, Int32 val);
	UTF32Char *MyString_StrHexVal24UTF32(UTF32Char *oriStr, Int32 val);

	Int32 MyString_StrToInt32UTF8(const UTF8Char *intStr);
	Int32 MyString_StrToInt32UTF16(const UTF16Char *intStr);
	Int32 MyString_StrToInt32UTF32(const UTF32Char *intStr);
	Int64 MyString_StrToInt64UTF8(const UTF8Char *str1);
	Int64 MyString_StrToInt64UTF16(const UTF16Char *str1);
	Int64 MyString_StrToInt64UTF32(const UTF32Char *str1);
}

namespace Text
{
	typedef enum
	{
		LBT_NONE,
		LBT_CR,
		LBT_LF,
		LBT_CRLF
	} LineBreakType;

	Char *StrConcat(Char *oriStr, const Char *strToJoin);
	Char *StrConcatC(Char *oriStr, const Char *strToJoin, UOSInt charCnt);
	Char *StrConcatS(Char *oriStr, const Char *strToJoin, UOSInt buffSize);
	UTF8Char *StrConcatASCII(UTF8Char *oriStr, const Char *strToJoin);

	Char *StrInt16(Char *oriStr, Int16 val);
	Char *StrUInt16(Char *oriStr, UInt16 val);
	Char *StrInt32(Char *oriStr, Int32 val);
	Char *StrUInt32(Char *oriStr, UInt32 val);
	Char *StrInt32S(Char *oriStr, Int32 val, Char seperator, OSInt sepCnt);
#ifdef HAS_INT64
	Char *StrInt64(Char *oriStr, Int64 val);
	Char *StrUInt64(Char *oriStr, UInt64 val);
	Char *StrInt64S(Char *oriStr, Int64 val, Char seperator, OSInt sepCnt);
#endif

	Char *StrToUpper(Char *oriStr, const Char *strToProc);
	Char *StrToLower(Char *oriStr, const Char *strToProc);
	Char *StrToCapital(Char *oriStr, const Char *strToProc);

	Bool StrEquals(const Char *str1, const Char *str2);
	Bool StrEqualsN(const Char *str1, const Char *str2);
	Bool StrEqualsICase(const Char *str1, const Char *str2);
	Bool StrEqualsICase(const Char *str1, const Char *str2, OSInt str2Len);

	FORCEINLINE OSInt StrCompare(const Char *str1, const Char *str2) { return MyString_StrCompare(str1, str2); }
	FORCEINLINE OSInt StrCompareICase(const Char *str1, const Char *str2) { return MyString_StrCompareICase(str1, str2); }

	FORCEINLINE UOSInt StrCharCnt(const Char *str) { return MyString_StrCharCnt(str); }
	OSInt StrCharCntS(const Char *str, OSInt maxLen);

#ifdef HAS_INT64
	Char *StrHexVal64V(Char *oriStr, UInt64 val);
	Char *StrHexVal64(Char *oriStr, UInt64 val);
#endif

	Char *StrHexVal32V(Char *oriStr, UInt32 val);
	Char *StrHexVal32(Char *oriStr, UInt32 val);
	Char *StrHexVal24(Char *oriStr, UInt32 val);
	Char *StrHexVal16(Char *oriStr, UInt16 val);
	Char *StrHexByte(Char *oriStr, UInt8 val);
	Char *StrHexBytes(Char *oriStr, const UInt8 *buff, UOSInt buffSize, Char seperator);
	Int64 StrHex2Int64(const Char *str);
	Int32 StrHex2Int32(const Char *str);
	Int16 StrHex2Int16(const Char *str);
	UInt8 StrHex2Byte(const Char *str);
	UOSInt StrHex2Bytes(const Char *str, UInt8 *buff);
#ifdef HAS_INT64
	Int64 StrOct2Int64(const Char *str);
#endif

#if _OSINT_SIZE == 64
	FORCEINLINE Char *StrOSInt(Char *oriStr, OSInt val) { return StrInt64(oriStr, (Int64)val); }
	FORCEINLINE Char *StrOSIntS(Char *oriStr, OSInt val, Char seperator, OSInt sepCnt) { return StrInt64S(oriStr, (Int64)val, seperator, sepCnt); }
	FORCEINLINE Char *StrUOSInt(Char *oriStr, UOSInt val) { return StrUInt64(oriStr, (UInt64)val); }
	FORCEINLINE Char *StrHexValOS(Char *oriStr, OSInt val) { return StrHexVal64(oriStr, (UInt64)val); }
#elif _OSINT_SIZE == 32
	FORCEINLINE Char *StrOSInt(Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int32)val); }
	FORCEINLINE Char *StrOSIntS(Char *oriStr, OSInt val, Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int32)val, seperator, sepCnt); }
	FORCEINLINE Char *StrUOSInt(Char *oriStr, UOSInt val) { return StrUInt32(oriStr, (UInt32)val); }
	FORCEINLINE Char *StrHexValOS(Char *oriStr, OSInt val) { return StrHexVal32(oriStr, (UInt32)val); }
#else // _OSINT_SIZE == 16
	FORCEINLINE Char *StrOSInt(Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int16)val); }
	FORCEINLINE Char *StrOSIntS(Char *oriStr, OSInt val, Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int16)val, seperator, sepCnt); }
	FORCEINLINE Char *StrUOSInt(Char *oriStr, UOSInt val) { return StrUInt16(oriStr, (UInt16)val); }
	FORCEINLINE Char *StrHexValOS(Char *oriStr, OSInt val) { return StrHexVal16(oriStr, (UInt16)val); }
#endif

	UOSInt StrSplit(Char **strs, UOSInt maxStrs, Char *str, Char splitChar); //Optimized
	UOSInt StrSplitTrim(Char **strs, UOSInt maxStrs, Char *str, Char splitChar); //Optimized
	UOSInt StrSplitLine(Char **strs, UOSInt maxStrs, Char *str); //Optimized
	UOSInt StrSplitWS(Char **strs, UOSInt maxStrs, Char *str); //Optimized
	Bool StrToUInt8(const Char *intStr, UInt8 *outVal);
	UInt8 StrToUInt8(const Char *intStr);
	Bool StrToUInt16(const Char *intStr, UInt16 *outVal);
	Bool StrToInt16(const Char *str, Int16 *outVal);
	Int16 StrToInt16(const Char *str);
	Bool StrToUInt32(const Char *intStr, UInt32 *outVal);
	UInt32 StrToUInt32(const Char *intStr);
	Bool StrToInt32(const Char *str, Int32 *outVal);
	Int32 StrToInt32(const Char *str);
#ifdef HAS_INT64
	Bool StrToInt64(const Char *intStr, Int64 *outVal);
	FORCEINLINE Int64 StrToInt64(const Char *str) { return MyString_StrToInt64UTF8((const UTF8Char*)str); }
#endif

	OSInt StrToOSInt(const Char *str);
	Bool StrToOSInt(const Char *intStr, OSInt *outVal);
	Bool StrToBool(const Char *str);
	OSInt StrIndexOf(const Char *str1, const Char *str2);
	OSInt StrIndexOf(const Char *str1, Char c);
	OSInt StrIndexOfICase(const Char *str1, const Char *str2);
	OSInt StrLastIndexOf(const Char *str1, Char c);
	OSInt StrLastIndexOf(const Char *str1, const Char *str2);
	Char *StrRTrim(Char* str1);
	Char *StrLTrim(Char* str1);
	Char *StrTrim(Char* str1);
	Char *StrTrimWSCRLF(Char* str1);
	Char *StrRemoveChar(Char *str1, Char c);

	const Char *StrCopyNew(const Char *str1);
	const Char *StrCopyNewC(const Char *str1, UOSInt strLen);
	void StrDelNew(const Char *newStr);
	Bool StrStartsWith(const Char *str1, const Char *str2);
	Bool StrStartsWithICase(const Char *str1, const Char *str2);
	Bool StrEndsWith(const Char *str1, const Char *str2);
	Bool StrEndsWithICase(const Char *str1, const Char *str2);
	Bool StrIsInt32(const Char *str1);
	UOSInt StrReplace(Char *str1, Char oriC, Char destC);
	UOSInt StrReplace(Char *str1, const Char *replaceFrom, const Char *replaceTo);
	UOSInt StrReplaceICase(Char *str1, const Char *replaceFrom, const Char *replaceTo);
	Char *StrToCSVRec(Char *oriStr, const Char *str1);
	const Char *StrToNewCSVRec(const Char *str1);
	UOSInt StrCSVSplit(Char **strs, UOSInt maxStrs, Char *strToSplit);
	Char *StrCSVJoin(Char *oriStr, const Char **strs, UOSInt nStrs);
	UOSInt StrCountChar(Char *str1, Char c);


	FORCEINLINE UTF8Char *StrConcat(UTF8Char *oriStr, const UTF8Char *strToJoin) { return (UTF8Char*)StrConcat((Char*)oriStr, (const Char*)strToJoin); }
	FORCEINLINE UTF8Char *StrConcatS(UTF8Char *oriStr, const UTF8Char *strToJoin, UOSInt buffSize) { return (UTF8Char*)StrConcatS((Char*)oriStr, (const Char*)strToJoin, buffSize); }
	FORCEINLINE UTF8Char *StrConcatC(UTF8Char *oriStr, const UTF8Char *strToJoin, UOSInt charCnt) { return (UTF8Char*)StrConcatC((Char*)oriStr, (const Char*)strToJoin, charCnt); }
	FORCEINLINE UTF8Char *StrInt16(UTF8Char *oriStr, Int16 val) { return (UTF8Char*)StrInt16((Char*)oriStr, val); }; //Optimized
	FORCEINLINE UTF8Char *StrUInt16(UTF8Char *oriStr, UInt16 val) { return (UTF8Char*)StrUInt16((Char*)oriStr, val); }; //Optimized
	FORCEINLINE UTF8Char *StrInt32(UTF8Char *oriStr, Int32 val) { return (UTF8Char*)StrInt32((Char*)oriStr, val); }; //Optimized
	FORCEINLINE UTF8Char *StrUInt32(UTF8Char *oriStr, UInt32 val) { return (UTF8Char*)StrUInt32((Char*)oriStr, val); }; //Optimized
	FORCEINLINE UTF8Char *StrInt32S(UTF8Char *oriStr, Int32 val, UTF8Char seperator, OSInt sepCnt) { return (UTF8Char*)StrInt32S((Char*)oriStr, val, (Char)seperator, sepCnt); };
#ifdef HAS_INT64
	FORCEINLINE UTF8Char *StrInt64(UTF8Char *oriStr, Int64 val) { return (UTF8Char*)StrInt64((Char*)oriStr, val); }; //Optimized
	FORCEINLINE UTF8Char *StrUInt64(UTF8Char *oriStr, UInt64 val) { return (UTF8Char*)StrUInt64((Char*)oriStr, val); }; //Optimized
	FORCEINLINE UTF8Char *StrInt64S(UTF8Char *oriStr, Int64 val, UTF8Char seperator, OSInt sepCnt) { return (UTF8Char*)StrInt64S((Char*)oriStr, val, (Char)seperator, sepCnt); };
#endif
	FORCEINLINE UTF8Char *StrToUpper(UTF8Char *oriStr, const UTF8Char *strToProc) { return (UTF8Char*)StrToUpper((Char*)oriStr, (const Char*)strToProc); }
	FORCEINLINE UTF8Char *StrToLower(UTF8Char *oriStr, const UTF8Char *strToProc) { return (UTF8Char*)StrToLower((Char*)oriStr, (const Char*)strToProc); }
	FORCEINLINE UTF8Char *StrToCapital(UTF8Char *oriStr, const UTF8Char *strToProc) { return (UTF8Char*)StrToCapital((Char*)oriStr, (const Char*)strToProc); }
	FORCEINLINE Bool StrEquals(const UTF8Char *str1, const UTF8Char *str2) { return StrEquals((const Char*)str1, (const Char*)str2); }
	FORCEINLINE Bool StrEqualsN(const UTF8Char *str1, const UTF8Char *str2) { return StrEqualsN((const Char*)str1, (const Char*)str2); }
	FORCEINLINE Bool StrEqualsICase(const UTF8Char *str1, const UTF8Char *str2) { return StrEqualsICase((const Char*)str1, (const Char*)str2); }
	FORCEINLINE Bool StrEqualsICase(const UTF8Char *str1, const UTF8Char *str2, OSInt str2Len) { return StrEqualsICase((const Char*)str1, (const Char*)str2, str2Len); }
	FORCEINLINE OSInt StrCompare(const UTF8Char *str1, const UTF8Char *str2) { return MyString_StrCompare((const Char*)str1, (const Char*)str2); }
	FORCEINLINE OSInt StrCompareICase(const UTF8Char *str1, const UTF8Char *str2) { return MyString_StrCompareICase((const Char*)str1, (const Char*)str2); }
	FORCEINLINE UOSInt StrCharCnt(const UTF8Char *str) { return MyString_StrCharCnt((const Char*)str); }
	FORCEINLINE OSInt StrCharCntS(const UTF8Char *str, OSInt maxLen) { return StrCharCntS((const Char*)str, maxLen); }

#ifdef HAS_INT64
	FORCEINLINE UTF8Char *StrHexVal64V(UTF8Char *oriStr, UInt64 val) { return (UTF8Char*)StrHexVal64V((Char*)oriStr, val); }
	FORCEINLINE UTF8Char *StrHexVal64(UTF8Char *oriStr, UInt64 val) { return (UTF8Char*)StrHexVal64((Char*)oriStr, val); }
#endif
	FORCEINLINE UTF8Char *StrHexVal32V(UTF8Char *oriStr, UInt32 val) { return (UTF8Char*)StrHexVal32V((Char*)oriStr, val); }
	FORCEINLINE UTF8Char *StrHexVal32(UTF8Char *oriStr, UInt32 val) { return (UTF8Char*)StrHexVal32((Char*)oriStr, val); }
	FORCEINLINE UTF8Char *StrHexVal24(UTF8Char *oriStr, UInt32 val) { return (UTF8Char*)StrHexVal24((Char*)oriStr, val); }
	FORCEINLINE UTF8Char *StrHexVal16(UTF8Char *oriStr, UInt16 val) { return (UTF8Char*)StrHexVal16((Char*)oriStr, val); }
	FORCEINLINE UTF8Char *StrHexByte(UTF8Char *oriStr, UInt8 val) { return (UTF8Char*)StrHexByte((Char*)oriStr, val); }
	FORCEINLINE UTF8Char *StrHexBytes(UTF8Char *oriStr, const UInt8 *buff, UOSInt buffSize, UTF8Char seperator) { return (UTF8Char*)StrHexBytes((Char*)oriStr, buff, buffSize, (Char)seperator); };
	FORCEINLINE Int64 StrHex2Int64(const UTF8Char *str) { return StrHex2Int64((const Char*)str); };
	FORCEINLINE Int32 StrHex2Int32(const UTF8Char *str) { return StrHex2Int32((const Char*)str); }
	FORCEINLINE Int16 StrHex2Int16(const UTF8Char *str) { return StrHex2Int16((const Char*)str); }
	FORCEINLINE UInt8 StrHex2Byte(const UTF8Char *str) { return StrHex2Byte((const Char*)str); }
	FORCEINLINE UOSInt StrHex2Bytes(const UTF8Char *str, UInt8 *buff) { return StrHex2Bytes((const Char*)str, buff); };
#ifdef HAS_INT64
	FORCEINLINE Int64 StrOct2Int64(const UTF8Char *str) { return StrOct2Int64((const Char*)str); };
#endif

#if _OSINT_SIZE == 64
	FORCEINLINE UTF8Char *StrOSInt(UTF8Char *oriStr, OSInt val) { return StrInt64(oriStr, (Int64)val); }
	FORCEINLINE UTF8Char *StrOSIntS(UTF8Char *oriStr, OSInt val, UTF8Char seperator, OSInt sepCnt) { return StrInt64S(oriStr, (Int64)val, seperator, sepCnt); }
	FORCEINLINE UTF8Char *StrUOSInt(UTF8Char *oriStr, UOSInt val) { return StrUInt64(oriStr, (UInt64)val); }
	FORCEINLINE UTF8Char *StrHexValOS(UTF8Char *oriStr, UOSInt val) { return StrHexVal64(oriStr, (UInt64)val); }
#elif _OSINT_SIZE == 32
	FORCEINLINE UTF8Char *StrOSInt(UTF8Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int32)val); }
	FORCEINLINE UTF8Char *StrOSIntS(UTF8Char *oriStr, OSInt val, UTF8Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int32)val, seperator, sepCnt); }
	FORCEINLINE UTF8Char *StrUOSInt(UTF8Char *oriStr, UOSInt val) { return StrUInt32(oriStr, (UInt32)val); }
	FORCEINLINE UTF8Char *StrHexValOS(UTF8Char *oriStr, UOSInt val) { return StrHexVal32(oriStr, (UInt32)val); }
#else // _OSINT_SIZE == 16
	FORCEINLINE UTF8Char *StrOSInt(UTF8Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int16)val); }
	FORCEINLINE UTF8Char *StrOSIntS(UTF8Char *oriStr, OSInt val, UTF8Char seperator, OSInt sepCnt) { return StrInt32S(oriStr, (Int16)val, seperator, sepCnt); }
	FORCEINLINE UTF8Char *StrUOSInt(UTF8Char *oriStr, UOSInt val) { return StrUInt16(oriStr, (UInt16)val); }
	FORCEINLINE UTF8Char *StrHexValOS(UTF8Char *oriStr, UOSInt val) { return StrHexVal16(oriStr, (UInt16)val); }
#endif
	FORCEINLINE UOSInt StrSplit(UTF8Char **strs, UOSInt maxStrs, UTF8Char *str, UTF8Char splitChar) { return StrSplit((Char **)strs, maxStrs, (Char*)str, (Char)splitChar); }
	FORCEINLINE UOSInt StrSplitTrim(UTF8Char **strs, UOSInt maxStrs, UTF8Char *str, UTF8Char splitChar) { return StrSplitTrim((Char **)strs, maxStrs, (Char*)str, (Char)splitChar); }
	FORCEINLINE UOSInt StrSplitLine(UTF8Char **strs, UOSInt maxStrs, UTF8Char *str) { return StrSplitLine((Char**)strs, maxStrs, (Char*)str); };
	FORCEINLINE UOSInt StrSplitWS(UTF8Char **strs, UOSInt maxStrs, UTF8Char *str) { return StrSplitWS((Char**)strs, maxStrs, (Char*)str); };
	FORCEINLINE Bool StrToUInt8(const UTF8Char *intStr, UInt8 *outVal) { return StrToUInt8((const Char*)intStr, outVal); };
	FORCEINLINE UInt8 StrToUInt8(const UTF8Char *intStr) { return StrToUInt8((const Char*)intStr); };
	FORCEINLINE Bool StrToUInt16(const UTF8Char *intStr, UInt16 *outVal) { return StrToUInt16((const Char*)intStr, outVal); };
	FORCEINLINE Bool StrToInt16(const UTF8Char *str, Int16 *outVal) { return StrToInt16((const Char*)str, outVal); }
	FORCEINLINE Int16 StrToInt16(const UTF8Char *str) { return StrToInt16((const Char*)str); }
	FORCEINLINE Bool StrToUInt32(const UTF8Char *intStr, UInt32 *outVal) { return StrToUInt32((const Char*)intStr, outVal); };
	FORCEINLINE UInt32 StrToUInt32(const UTF8Char *intStr) { return StrToUInt32((const Char*)intStr); };
	FORCEINLINE Bool StrToInt32(const UTF8Char *str, Int32 *outVal) { return StrToInt32((const Char*)str, outVal); }
	FORCEINLINE Int32 StrToInt32(const UTF8Char *str) { return StrToInt32((const Char*)str); }
#ifdef HAS_INT64
	FORCEINLINE Bool StrToInt64(const UTF8Char *intStr, Int64 *outVal) { return StrToInt64((const Char*)intStr, outVal); }
	FORCEINLINE Int64 StrToInt64(const UTF8Char *str) { return MyString_StrToInt64UTF8(str); }
#endif
	FORCEINLINE OSInt StrToOSInt(const UTF8Char *str) { return StrToOSInt((const Char*)str); };
	FORCEINLINE Bool StrToOSInt(const UTF8Char *intStr, OSInt *outVal) { return StrToOSInt((const Char*)intStr, outVal); };
	FORCEINLINE Bool StrToBool(const UTF8Char *str) { return StrToBool((const Char*)str); };

	FORCEINLINE OSInt StrIndexOf(const UTF8Char *str1, const UTF8Char *str2) { return StrIndexOf((const Char*)str1, (const Char*)str2); }
	FORCEINLINE OSInt StrIndexOf(const UTF8Char *str1, UTF8Char c) { return StrIndexOf((const Char*)str1, (Char)c); }
	FORCEINLINE OSInt StrIndexOfICase(const UTF8Char *str1, const UTF8Char *str2) { return StrIndexOfICase((const Char*)str1, (const Char*)str2); };
	FORCEINLINE OSInt StrLastIndexOf(const UTF8Char *str1, UTF8Char c) { return StrLastIndexOf((const Char*)str1, (Char)c); }
	FORCEINLINE OSInt StrLastIndexOf(const UTF8Char *str1, const UTF8Char *str2) { return StrLastIndexOf((const Char*)str1, (const Char *)str2); }
	FORCEINLINE UTF8Char *StrRTrim(UTF8Char* str1) { return (UTF8Char*)StrRTrim((Char*)str1); };
	FORCEINLINE UTF8Char *StrLTrim(UTF8Char* str1) { return (UTF8Char*)StrLTrim((Char*)str1); };
	FORCEINLINE UTF8Char *StrTrim(UTF8Char* str1) { return (UTF8Char*)StrTrim((Char*)str1); };
	FORCEINLINE UTF8Char *StrTrimWSCRLF(UTF8Char* str1) { return (UTF8Char*)StrTrimWSCRLF((Char*)str1); };
	FORCEINLINE UTF8Char *StrRemoveChar(UTF8Char *str1, UTF8Char c) { return (UTF8Char*)StrRemoveChar((Char*)str1, (Char)c); };

	const UTF8Char *StrCopyNew(const UTF8Char *str1);
	const UTF8Char *StrCopyNewC(const UTF8Char *str1, UOSInt strLen);
	void StrDelNew(const UTF8Char *newStr);
	FORCEINLINE Bool StrStartsWith(const UTF8Char *str1, const UTF8Char *str2) { return StrStartsWith((const Char*)str1, (const Char*)str2); }
	FORCEINLINE Bool StrStartsWithICase(const UTF8Char *str1, const UTF8Char *str2) { return StrStartsWithICase((const Char*)str1, (const Char*)str2); }
	FORCEINLINE Bool StrEndsWith(const UTF8Char *str1, const UTF8Char *str2) { return StrEndsWith((const Char*)str1, (const Char*)str2); }
	FORCEINLINE Bool StrEndsWithICase(const UTF8Char *str1, const UTF8Char *str2) { return StrEndsWithICase((const Char*)str1, (const Char*)str2); }
	FORCEINLINE Bool StrIsInt32(const UTF8Char *str1) { return StrIsInt32((const Char*)str1); };
	FORCEINLINE UOSInt StrReplace(UTF8Char *str1, UTF8Char oriC, UTF8Char destC) { return StrReplace((Char*)str1, (Char)oriC, (Char)destC); };
	FORCEINLINE UOSInt StrReplace(UTF8Char *str1, const UTF8Char *replaceFrom, const UTF8Char *replaceTo) { return StrReplace((Char*)str1, (const Char*)replaceFrom, (const Char*)replaceTo); };
	FORCEINLINE UOSInt StrReplaceICase(UTF8Char *str1, const UTF8Char *replaceFrom, const UTF8Char *replaceTo) { return StrReplaceICase((Char*)str1, (const Char*)replaceFrom, (const Char*)replaceTo); };
	FORCEINLINE UTF8Char *StrToCSVRec(UTF8Char *oriStr, const UTF8Char *str1) { return (UTF8Char*)StrToCSVRec((Char*)oriStr, (const Char*)str1); };
	FORCEINLINE const UTF8Char *StrToNewCSVRec(const UTF8Char *str1) { return (const UTF8Char*)StrToNewCSVRec((const Char*)str1); }
	FORCEINLINE UOSInt StrCSVSplit(UTF8Char **strs, UOSInt maxStrs, UTF8Char *strToSplit) { return StrCSVSplit((Char**)strs, maxStrs, (Char*)strToSplit); };
	FORCEINLINE UTF8Char *StrCSVJoin(UTF8Char *oriStr, const UTF8Char **strs, UOSInt nStrs) { return (UTF8Char*)StrCSVJoin((Char*)oriStr, (const Char**)strs, nStrs); };
	FORCEINLINE UOSInt StrCountChar(UTF8Char *str1, UTF8Char c) { return StrCountChar((Char*)str1, (Char)c); };
}
#endif
