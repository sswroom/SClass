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
	struct PString
	{
		UTF8Char *v;
		UOSInt len;

		void operator =(UTF8Char *v)
		{
			this->v = v;
			if (v)
			{
				while (*v++)
				this->len = (UOSInt)(v - this->v - 1);
			}
			else
			{
				this->len = 0;
			}
		}
	};

	UTF8Char *StrConcat(UTF8Char *oriStr, const UTF8Char *strToJoin);
	UTF8Char *StrConcatC(UTF8Char *oriStr, const UTF8Char *strToJoin, UOSInt charCnt);
	UTF8Char *StrConcatS(UTF8Char *oriStr, const UTF8Char *strToJoin, UOSInt buffSize);
	UTF8Char *StrConcatASCII(UTF8Char *oriStr, const Char *strToJoin);

	UTF8Char *StrInt16(UTF8Char *oriStr, Int16 val);
	UTF8Char *StrUInt16(UTF8Char *oriStr, UInt16 val);
	UTF8Char *StrInt32(UTF8Char *oriStr, Int32 val);
	UTF8Char *StrUInt32(UTF8Char *oriStr, UInt32 val);
	UTF8Char *StrInt32S(UTF8Char *oriStr, Int32 val, UTF8Char seperator, UOSInt sepCnt);
	UTF8Char *StrUInt32S(UTF8Char *oriStr, UInt32 val, UTF8Char seperator, UOSInt sepCnt);
#ifdef HAS_INT64
	UTF8Char *StrInt64(UTF8Char *oriStr, Int64 val);
	UTF8Char *StrUInt64(UTF8Char *oriStr, UInt64 val);
	UTF8Char *StrInt64S(UTF8Char *oriStr, Int64 val, UTF8Char seperator, UOSInt sepCnt);
	UTF8Char *StrUInt64S(UTF8Char *oriStr, UInt64 val, UTF8Char seperator, UOSInt sepCnt);
#endif

	UTF8Char *StrToUpper(UTF8Char *oriStr, const UTF8Char *strToProc);
	UTF8Char *StrToUpperC(UTF8Char *oriStr, const UTF8Char *strToProc, UOSInt strLen);
	UTF8Char *StrToLower(UTF8Char *oriStr, const UTF8Char *strToProc);
	UTF8Char *StrToLowerC(UTF8Char *oriStr, const UTF8Char *strToProc, UOSInt strLen);
	UTF8Char *StrToCapital(UTF8Char *oriStr, const UTF8Char *strToProc);

	Bool StrEquals(const UTF8Char *str1, const UTF8Char *str2);
	Bool StrEqualsN(const UTF8Char *str1, const UTF8Char *str2);
	Bool StrEqualsC(const UTF8Char *str1, UOSInt len1, const UTF8Char *str2, UOSInt len2);
	Bool StrEqualsICase(const UTF8Char *str1, const UTF8Char *str2);
	Bool StrEqualsICaseC(const UTF8Char *str1, UOSInt str1Len, const UTF8Char *str2, UOSInt str2Len);

	FORCEINLINE OSInt StrCompare(const UTF8Char *str1, const UTF8Char *str2) { return MyString_StrCompare(str1, str2); }
	FORCEINLINE OSInt StrCompareICase(const UTF8Char *str1, const UTF8Char *str2) { return MyString_StrCompareICase(str1, str2); }
	OSInt StrCompareFastC(const UTF8Char *str1, UOSInt len1, const UTF8Char *str2, UOSInt len2);

	FORCEINLINE UOSInt StrCharCnt(const UTF8Char *str) { return MyString_StrCharCnt(str); }
	UOSInt StrCharCntS(const UTF8Char *str, UOSInt maxLen);

	Bool StrHasUpperCase(const UTF8Char *str);

#ifdef HAS_INT64
	UTF8Char *StrHexVal64V(UTF8Char *oriStr, UInt64 val);
	UTF8Char *StrHexVal64(UTF8Char *oriStr, UInt64 val);
#endif

	UTF8Char *StrHexVal32V(UTF8Char *oriStr, UInt32 val);
	UTF8Char *StrHexVal32(UTF8Char *oriStr, UInt32 val);
	UTF8Char *StrHexVal24(UTF8Char *oriStr, UInt32 val);
	UTF8Char *StrHexVal16(UTF8Char *oriStr, UInt16 val);
	UTF8Char *StrHexByte(UTF8Char *oriStr, UInt8 val);
	UTF8Char *StrHexBytes(UTF8Char *oriStr, const UInt8 *buff, UOSInt buffSize, UTF8Char seperator);
	Int64 StrHex2Int64C(const UTF8Char *str);
	Bool StrHex2Int64C(const UTF8Char *str, Int64 *outVal);
	Bool StrHex2Int64S(const UTF8Char *str, Int64 *outVal, Int64 failVal);
	Int32 StrHex2Int32C(const UTF8Char *str);
	FORCEINLINE UInt32 StrHex2UInt32C(const UTF8Char *str) { return (UInt32)StrHex2Int32C(str); }
	FORCEINLINE UInt64 StrHex2UInt64C(const UTF8Char *str) { return (UInt64)StrHex2Int64C(str); }
	FORCEINLINE Bool StrHex2UInt64C(const UTF8Char *str, UInt64 *outVal) { return StrHex2Int64C(str, (Int64*)outVal); }
	FORCEINLINE Bool StrHex2UInt64S(const UTF8Char *str, UInt64 *outVal, UInt64 failVal) { return StrHex2Int64S(str, (Int64*)outVal, (Int64)failVal); }
	Int16 StrHex2Int16C(const UTF8Char *str);
	FORCEINLINE UInt16 StrHex2UInt16C(const UTF8Char *str) { return (UInt16)StrHex2Int16C(str); }
	UInt8 StrHex2UInt8C(const UTF8Char *str);
	UOSInt StrHex2Bytes(const UTF8Char *str, UInt8 *buff);
#ifdef HAS_INT64
	Int64 StrOct2Int64(const UTF8Char *str);
#endif

#if _OSINT_SIZE == 64
	FORCEINLINE UTF8Char *StrOSInt(UTF8Char *oriStr, OSInt val) { return StrInt64(oriStr, (Int64)val); }
	FORCEINLINE UTF8Char *StrOSIntS(UTF8Char *oriStr, OSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrInt64S(oriStr, (Int64)val, seperator, sepCnt); }
	FORCEINLINE UTF8Char *StrUOSInt(UTF8Char *oriStr, UOSInt val) { return StrUInt64(oriStr, (UInt64)val); }
	FORCEINLINE UTF8Char *StrUOSIntS(UTF8Char *oriStr, UOSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrUInt64S(oriStr, (UInt64)val, seperator, sepCnt); }
	FORCEINLINE UTF8Char *StrHexValOS(UTF8Char *oriStr, UOSInt val) { return StrHexVal64(oriStr, (UInt64)val); }
#elif _OSINT_SIZE == 32
	FORCEINLINE UTF8Char *StrOSInt(UTF8Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int32)val); }
	FORCEINLINE UTF8Char *StrOSIntS(UTF8Char *oriStr, OSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrInt32S(oriStr, (Int32)val, seperator, sepCnt); }
	FORCEINLINE UTF8Char *StrUOSInt(UTF8Char *oriStr, UOSInt val) { return StrUInt32(oriStr, (UInt32)val); }
	FORCEINLINE UTF8Char *StrUOSIntS(UTF8Char *oriStr, UOSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrUInt32S(oriStr, (UInt32)val, seperator, sepCnt); }
	FORCEINLINE UTF8Char *StrHexValOS(UTF8Char *oriStr, UOSInt val) { return StrHexVal32(oriStr, (UInt32)val); }
#else // _OSINT_SIZE == 16
	FORCEINLINE UTF8Char *StrOSInt(UTF8Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int16)val); }
	FORCEINLINE UTF8Char *StrOSIntS(UTF8Char *oriStr, OSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrInt32S(oriStr, (Int16)val, seperator, sepCnt); }
	FORCEINLINE UTF8Char *StrUOSInt(UTF8Char *oriStr, UOSInt val) { return StrUInt16(oriStr, (UInt16)val); }
	FORCEINLINE UTF8Char *StrUOSIntS(UTF8Char *oriStr, UOSInt val, UTF8Char seperator, UOSInt sepCnt) { return StrUInt32S(oriStr, (UInt16)val, seperator, sepCnt); }
	FORCEINLINE UTF8Char *StrHexValOS(UTF8Char *oriStr, UOSInt val) { return StrHexVal16(oriStr, (UInt16)val); }
#endif

	UOSInt StrSplit(UTF8Char **strs, UOSInt maxStrs, UTF8Char *str, UTF8Char splitChar); //Optimized
	UOSInt StrSplitP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen, UTF8Char splitChar); //Optimized
	UOSInt StrSplitTrim(UTF8Char **strs, UOSInt maxStrs, UTF8Char *str, UTF8Char splitChar); //Optimized
	UOSInt StrSplitTrimP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen, UTF8Char splitChar); //Optimized
	UOSInt StrSplitLine(UTF8Char **strs, UOSInt maxStrs, UTF8Char *str); //Optimized
	UOSInt StrSplitLineP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen); //Optimized
	UOSInt StrSplitWS(UTF8Char **strs, UOSInt maxStrs, UTF8Char *str); //Optimized
	UOSInt StrSplitWSP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen); //Optimized
	Bool StrToUInt8(const UTF8Char *intStr, UInt8 *outVal);
	UInt8 StrToUInt8(const UTF8Char *intStr);
	Bool StrToUInt16(const UTF8Char *intStr, UInt16 *outVal);
	Bool StrToUInt16S(const UTF8Char *intStr, UInt16 *outVal, UInt16 failVal);
	Bool StrToInt16(const UTF8Char *str, Int16 *outVal);
	Int16 StrToInt16(const UTF8Char *str);
	Bool StrToUInt32(const UTF8Char *intStr, UInt32 *outVal);
	Bool StrToUInt32S(const UTF8Char *intStr, UInt32 *outVal, UInt32 failVal);
	UInt32 StrToUInt32(const UTF8Char *intStr);
	Bool StrToInt32(const UTF8Char *str, Int32 *outVal);
	Int32 StrToInt32(const UTF8Char *str);
#ifdef HAS_INT64
	Bool StrToInt64(const UTF8Char *intStr, Int64 *outVal);
	FORCEINLINE Int64 StrToInt64(const UTF8Char *str) { return MyString_StrToInt64UTF8(str); }
	Bool StrToUInt64(const UTF8Char *intStr, UInt64 *outVal);
	Bool StrToUInt64S(const UTF8Char *intStr, UInt64 *outVal, UInt64 failVal);
	FORCEINLINE UInt64 StrToUInt64(const UTF8Char *str) { UInt64 v; StrToUInt64S(str, &v, 0); return v; }
#endif

	OSInt StrToOSInt(const UTF8Char *str);
	Bool StrToOSInt(const UTF8Char *intStr, OSInt *outVal);
	UOSInt StrToUOSInt(const UTF8Char *str);
	Bool StrToUOSInt(const UTF8Char *intStr, UOSInt *outVal);
	Bool StrToBool(const UTF8Char *str);
	UOSInt StrIndexOf(const UTF8Char *str1, const UTF8Char *str2);
	UOSInt StrIndexOfChar(const UTF8Char *str1, UTF8Char c);
	UOSInt StrIndexOfCharC(const UTF8Char *str1, UOSInt len1, UTF8Char c);
	UOSInt StrIndexOfC(const UTF8Char *str1, UOSInt len1, const UTF8Char *str2, UOSInt len2);
	UOSInt StrIndexOfICase(const UTF8Char *str1, const UTF8Char *str2);
	UOSInt StrLastIndexOfChar(const UTF8Char *str1, UTF8Char c);
	UOSInt StrLastIndexOfCharC(const UTF8Char *str1, UOSInt len1, UTF8Char c);
	UOSInt StrLastIndexOfC(const UTF8Char *str1, UOSInt len1, UTF8Char c);
	UOSInt StrLastIndexOf(const UTF8Char *str1, const UTF8Char *str2);
	Bool StrContainChars(const UTF8Char *str, const UTF8Char *chars);
	UTF8Char *StrRTrim(UTF8Char* str1);
	UTF8Char *StrLTrim(UTF8Char* str1);
	UTF8Char *StrTrim(UTF8Char* str1);
	UTF8Char *StrTrimC(UTF8Char* str1, UOSInt len);
	UTF8Char *StrTrimWSCRLF(UTF8Char* str1);
	UTF8Char *StrRemoveChar(UTF8Char *str1, UTF8Char c);

	const UTF8Char *StrCopyNew(const UTF8Char *str1);
	const UTF8Char *StrCopyNewC(const UTF8Char *str1, UOSInt strLen);
	void StrDelNew(const UTF8Char *newStr);
	Bool StrStartsWith(const UTF8Char *str1, const UTF8Char *str2);
	Bool StrStartsWithC(const UTF8Char *str1, UOSInt len1, const UTF8Char *str2, UOSInt len2);
	Bool StrStartsWithICase(const UTF8Char *str1, const UTF8Char *str2);
	Bool StrStartsWithICaseC(const UTF8Char *str1, UOSInt len1, const UTF8Char *str2, UOSInt len2);
	Bool StrEndsWith(const UTF8Char *str1, const UTF8Char *str2);
	Bool StrEndsWithC(const UTF8Char *str1, UOSInt len1, const UTF8Char *str2, UOSInt len2);
	Bool StrEndsWithICase(const UTF8Char *str1, const UTF8Char *str2);
	Bool StrEndsWithICaseC(const UTF8Char *str1, UOSInt len1, const UTF8Char *str2, UOSInt len2);
	Bool StrIsInt32(const UTF8Char *str1);
	UOSInt StrReplace(UTF8Char *str1, UTF8Char oriC, UTF8Char destC);
	UOSInt StrReplace(UTF8Char *str1, const UTF8Char *replaceFrom, const UTF8Char *replaceTo);
	UOSInt StrReplaceICase(UTF8Char *str1, const UTF8Char *replaceFrom, const UTF8Char *replaceTo);
	UTF8Char *StrToCSVRec(UTF8Char *oriStr, const UTF8Char *str1);
	const UTF8Char *StrToNewCSVRec(const UTF8Char *str1);
	UOSInt StrCSVSplit(UTF8Char **strs, UOSInt maxStrs, UTF8Char *strToSplit);
	UOSInt StrCSVSplitP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit);
	UTF8Char *StrCSVJoin(UTF8Char *oriStr, const UTF8Char **strs, UOSInt nStrs);
	UOSInt StrCountChar(const UTF8Char *str1, UTF8Char c);
	UTF8Char *StrRemoveANSIEscapes(UTF8Char *str1);

	FORCEINLINE Char *StrConcat(Char *oriStr, const Char *strToJoin) { return (Char*)StrConcat((UTF8Char*)oriStr, (const UTF8Char*)strToJoin); }
	FORCEINLINE Char *StrConcatS(Char *oriStr, const Char *strToJoin, UOSInt buffSize) { return (Char*)StrConcatS((UTF8Char*)oriStr, (const UTF8Char*)strToJoin, buffSize); }
	FORCEINLINE Char *StrConcatC(Char *oriStr, const Char *strToJoin, UOSInt charCnt) { return (Char*)StrConcatC((UTF8Char*)oriStr, (const UTF8Char*)strToJoin, charCnt); }
	FORCEINLINE Char *StrInt16(Char *oriStr, Int16 val) { return (Char*)StrInt16((UTF8Char*)oriStr, val); }; //Optimized
	FORCEINLINE Char *StrUInt16(Char *oriStr, UInt16 val) { return (Char*)StrUInt16((UTF8Char*)oriStr, val); }; //Optimized
	FORCEINLINE Char *StrInt32(Char *oriStr, Int32 val) { return (Char*)StrInt32((UTF8Char*)oriStr, val); }; //Optimized
	FORCEINLINE Char *StrUInt32(Char *oriStr, UInt32 val) { return (Char*)StrUInt32((UTF8Char*)oriStr, val); }; //Optimized
	FORCEINLINE Char *StrInt32S(Char *oriStr, Int32 val, Char seperator, UOSInt sepCnt) { return (Char*)StrInt32S((UTF8Char*)oriStr, val, (UTF8Char)seperator, sepCnt); };
	FORCEINLINE Char *StrUInt32S(Char *oriStr, UInt32 val, Char seperator, UOSInt sepCnt) { return (Char*)StrUInt32S((UTF8Char*)oriStr, val, (UTF8Char)seperator, sepCnt); };
#ifdef HAS_INT64
	FORCEINLINE Char *StrInt64(Char *oriStr, Int64 val) { return (Char*)StrInt64((UTF8Char*)oriStr, val); }; //Optimized
	FORCEINLINE Char *StrUInt64(Char *oriStr, UInt64 val) { return (Char*)StrUInt64((UTF8Char*)oriStr, val); }; //Optimized
	FORCEINLINE Char *StrInt64S(Char *oriStr, Int64 val, Char seperator, UOSInt sepCnt) { return (Char*)StrInt64S((UTF8Char*)oriStr, val, (UTF8Char)seperator, sepCnt); };
	FORCEINLINE Char *StrUInt64S(Char *oriStr, UInt64 val, Char seperator, UOSInt sepCnt) { return (Char*)StrUInt64S((UTF8Char*)oriStr, val, (UTF8Char)seperator, sepCnt); };
#endif
	FORCEINLINE Char *StrToUpper(Char *oriStr, const Char *strToProc) { return (Char*)StrToUpper((UTF8Char*)oriStr, (const UTF8Char*)strToProc); }
	FORCEINLINE Char *StrToLower(Char *oriStr, const Char *strToProc) { return (Char*)StrToLower((UTF8Char*)oriStr, (const UTF8Char*)strToProc); }
	FORCEINLINE Char *StrToCapital(Char *oriStr, const Char *strToProc) { return (Char*)StrToCapital((UTF8Char*)oriStr, (const UTF8Char*)strToProc); }
	FORCEINLINE Bool StrEquals(const Char *str1, const Char *str2) { return StrEquals((const UTF8Char*)str1, (const UTF8Char*)str2); }
	FORCEINLINE Bool StrEqualsN(const Char *str1, const Char *str2) { return StrEqualsN((const UTF8Char*)str1, (const UTF8Char*)str2); }
	FORCEINLINE Bool StrEqualsICase(const Char *str1, const Char *str2) { return StrEqualsICase((const UTF8Char*)str1, (const UTF8Char*)str2); }
	FORCEINLINE Bool StrEqualsICaseC(const Char *str1, UOSInt str1Len, const Char *str2, UOSInt str2Len) { return StrEqualsICaseC((const UTF8Char*)str1, str1Len, (const UTF8Char*)str2, str2Len); }
	FORCEINLINE OSInt StrCompare(const Char *str1, const Char *str2) { return MyString_StrCompare((const UTF8Char*)str1, (const UTF8Char*)str2); }
	FORCEINLINE OSInt StrCompareICase(const Char *str1, const Char *str2) { return MyString_StrCompareICase((const UTF8Char*)str1, (const UTF8Char*)str2); }
	FORCEINLINE UOSInt StrCharCnt(const Char *str) { return MyString_StrCharCnt((const UTF8Char*)str); }
	FORCEINLINE UOSInt StrCharCntS(const Char *str, UOSInt maxLen) { return StrCharCntS((const UTF8Char*)str, maxLen); }
	FORCEINLINE Bool StrHasUpperCase(const Char *str) { return StrHasUpperCase((const UTF8Char*)str); }

#ifdef HAS_INT64
	FORCEINLINE Char *StrHexVal64V(Char *oriStr, UInt64 val) { return (Char*)StrHexVal64V((UTF8Char*)oriStr, val); }
	FORCEINLINE Char *StrHexVal64(Char *oriStr, UInt64 val) { return (Char*)StrHexVal64((UTF8Char*)oriStr, val); }
#endif
	FORCEINLINE Char *StrHexVal32V(Char *oriStr, UInt32 val) { return (Char*)StrHexVal32V((UTF8Char*)oriStr, val); }
	FORCEINLINE Char *StrHexVal32(Char *oriStr, UInt32 val) { return (Char*)StrHexVal32((UTF8Char*)oriStr, val); }
	FORCEINLINE Char *StrHexVal24(Char *oriStr, UInt32 val) { return (Char*)StrHexVal24((UTF8Char*)oriStr, val); }
	FORCEINLINE Char *StrHexVal16(Char *oriStr, UInt16 val) { return (Char*)StrHexVal16((UTF8Char*)oriStr, val); }
	FORCEINLINE Char *StrHexByte(Char *oriStr, UInt8 val) { return (Char*)StrHexByte((UTF8Char*)oriStr, val); }
	FORCEINLINE Char *StrHexBytes(Char *oriStr, const UInt8 *buff, UOSInt buffSize, Char seperator) { return (Char*)StrHexBytes((UTF8Char*)oriStr, buff, buffSize, (UTF8Char)seperator); };
	FORCEINLINE Int64 StrHex2Int64C(const Char *str) { return StrHex2Int64C((const UTF8Char*)str); };
	FORCEINLINE Int32 StrHex2Int32C(const Char *str) { return StrHex2Int32C((const UTF8Char*)str); }
	FORCEINLINE UInt32 StrHex2UInt32C(const Char *str) { return (UInt32)StrHex2Int32C((const UTF8Char*)str); }
	FORCEINLINE UInt64 StrHex2UInt64C(const Char *str) { return (UInt64)StrHex2Int64C((const UTF8Char*)str); }
	FORCEINLINE Int16 StrHex2Int16C(const Char *str) { return StrHex2Int16C((const UTF8Char*)str); }
	FORCEINLINE UInt16 StrHex2UInt16C(const Char *str) { return (UInt16)StrHex2Int16C((const UTF8Char*)str); }
	FORCEINLINE UInt8 StrHex2UInt8C(const Char *str) { return StrHex2UInt8C((const UTF8Char*)str); }
	FORCEINLINE UOSInt StrHex2Bytes(const Char *str, UInt8 *buff) { return StrHex2Bytes((const UTF8Char*)str, buff); };
#ifdef HAS_INT64
	FORCEINLINE Int64 StrOct2Int64(const Char *str) { return StrOct2Int64((const UTF8Char*)str); };
#endif

#if _OSINT_SIZE == 64
	FORCEINLINE Char *StrOSInt(Char *oriStr, OSInt val) { return StrInt64(oriStr, (Int64)val); }
	FORCEINLINE Char *StrOSIntS(Char *oriStr, OSInt val, Char seperator, UOSInt sepCnt) { return StrInt64S(oriStr, (Int64)val, seperator, sepCnt); }
	FORCEINLINE Char *StrUOSInt(Char *oriStr, UOSInt val) { return StrUInt64(oriStr, (UInt64)val); }
	FORCEINLINE Char *StrUOSIntS(Char *oriStr, UOSInt val, Char seperator, UOSInt sepCnt) { return StrUInt64S(oriStr, (UInt64)val, seperator, sepCnt); }
	FORCEINLINE Char *StrHexValOS(Char *oriStr, UOSInt val) { return StrHexVal64(oriStr, (UInt64)val); }
#elif _OSINT_SIZE == 32
	FORCEINLINE Char *StrOSInt(Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int32)val); }
	FORCEINLINE Char *StrOSIntS(Char *oriStr, OSInt val, Char seperator, UOSInt sepCnt) { return StrInt32S(oriStr, (Int32)val, seperator, sepCnt); }
	FORCEINLINE Char *StrUOSInt(Char *oriStr, UOSInt val) { return StrUInt32(oriStr, (UInt32)val); }
	FORCEINLINE Char *StrUOSIntS(Char *oriStr, UOSInt val, Char seperator, UOSInt sepCnt) { return StrUInt32S(oriStr, (UInt32)val, seperator, sepCnt); }
	FORCEINLINE Char *StrHexValOS(Char *oriStr, UOSInt val) { return StrHexVal32(oriStr, (UInt32)val); }
#else // _OSINT_SIZE == 16
	FORCEINLINE Char *StrOSInt(Char *oriStr, OSInt val) { return StrInt32(oriStr, (Int16)val); }
	FORCEINLINE Char *StrOSIntS(Char *oriStr, OSInt val, Char seperator, UOSInt sepCnt) { return StrInt32S(oriStr, (Int16)val, seperator, sepCnt); }
	FORCEINLINE Char *StrUOSInt(Char *oriStr, UOSInt val) { return StrUInt16(oriStr, (UInt16)val); }
	FORCEINLINE Char *StrUOSIntS(Char *oriStr, UOSInt val, Char seperator, UOSInt sepCnt) { return StrUInt32S(oriStr, (UInt16)val, seperator, sepCnt); }
	FORCEINLINE Char *StrHexValOS(Char *oriStr, UOSInt val) { return StrHexVal16(oriStr, (UInt16)val); }
#endif
	FORCEINLINE UOSInt StrSplit(Char **strs, UOSInt maxStrs, Char *str, Char splitChar) { return StrSplit((UTF8Char **)strs, maxStrs, (UTF8Char*)str, (UTF8Char)splitChar); }
	FORCEINLINE UOSInt StrSplitTrim(Char **strs, UOSInt maxStrs, Char *str, Char splitChar) { return StrSplitTrim((UTF8Char **)strs, maxStrs, (UTF8Char*)str, (UTF8Char)splitChar); }
	FORCEINLINE UOSInt StrSplitLine(Char **strs, UOSInt maxStrs, Char *str) { return StrSplitLine((UTF8Char**)strs, maxStrs, (UTF8Char*)str); };
	FORCEINLINE UOSInt StrSplitWS(Char **strs, UOSInt maxStrs, Char *str) { return StrSplitWS((UTF8Char**)strs, maxStrs, (UTF8Char*)str); };
	FORCEINLINE Bool StrToUInt8(const Char *intStr, UInt8 *outVal) { return StrToUInt8((const UTF8Char*)intStr, outVal); };
	FORCEINLINE UInt8 StrToUInt8(const Char *intStr) { return StrToUInt8((const UTF8Char*)intStr); };
	FORCEINLINE Bool StrToUInt16(const Char *intStr, UInt16 *outVal) { return StrToUInt16((const UTF8Char*)intStr, outVal); };
	FORCEINLINE Bool StrToUInt16S(const Char *intStr, UInt16 *outVal, UInt16 failVal) { return StrToUInt16S((const UTF8Char*)intStr, outVal, failVal); };
	FORCEINLINE Bool StrToInt16(const Char *str, Int16 *outVal) { return StrToInt16((const UTF8Char*)str, outVal); }
	FORCEINLINE Int16 StrToInt16(const Char *str) { return StrToInt16((const UTF8Char*)str); }
	FORCEINLINE Bool StrToUInt32(const Char *intStr, UInt32 *outVal) { return StrToUInt32((const UTF8Char*)intStr, outVal); };
	FORCEINLINE Bool StrToUInt32S(const Char *intStr, UInt32 *outVal, UInt32 failVal) { return StrToUInt32S((const UTF8Char*)intStr, outVal, failVal); };
	FORCEINLINE UInt32 StrToUInt32(const Char *intStr) { return StrToUInt32((const UTF8Char*)intStr); };
	FORCEINLINE Bool StrToInt32(const Char *str, Int32 *outVal) { return StrToInt32((const UTF8Char*)str, outVal); }
	FORCEINLINE Int32 StrToInt32(const Char *str) { return StrToInt32((const UTF8Char*)str); }
#ifdef HAS_INT64
	FORCEINLINE Bool StrToInt64(const Char *intStr, Int64 *outVal) { return StrToInt64((const UTF8Char*)intStr, outVal); }
	FORCEINLINE Int64 StrToInt64(const Char *str) { return MyString_StrToInt64UTF8((const UTF8Char*)str); }
	FORCEINLINE Bool StrToUInt64(const Char *intStr, UInt64 *outVal) { return StrToUInt64((const UTF8Char*)intStr, outVal); }
	FORCEINLINE UInt64 StrToUInt64(const Char *str) { return StrToUInt64((const UTF8Char*)str); }
#endif
	FORCEINLINE OSInt StrToOSInt(const Char *str) { return StrToOSInt((const UTF8Char*)str); };
	FORCEINLINE Bool StrToOSInt(const Char *intStr, OSInt *outVal) { return StrToOSInt((const UTF8Char*)intStr, outVal); };
	FORCEINLINE UOSInt StrToUOSInt(const Char *str) { return StrToUOSInt((const UTF8Char*)str); };
	FORCEINLINE Bool StrToUOSInt(const Char *intStr, UOSInt *outVal) { return StrToUOSInt((const UTF8Char*)intStr, outVal); };
	FORCEINLINE Bool StrToBool(const Char *str) { return StrToBool((const UTF8Char*)str); };

	FORCEINLINE UOSInt StrIndexOf(const Char *str1, const Char *str2) { return StrIndexOf((const UTF8Char*)str1, (const UTF8Char*)str2); }
	FORCEINLINE UOSInt StrIndexOfChar(const Char *str1, Char c) { return StrIndexOfChar((const UTF8Char*)str1, (UTF8Char)c); }
	FORCEINLINE UOSInt StrIndexOfICase(const Char *str1, const Char *str2) { return StrIndexOfICase((const UTF8Char*)str1, (const UTF8Char*)str2); };
	FORCEINLINE UOSInt StrLastIndexOfChar(const Char *str1, Char c) { return StrLastIndexOfChar((const UTF8Char*)str1, (UTF8Char)c); }
	FORCEINLINE UOSInt StrLastIndexOf(const Char *str1, const Char *str2) { return StrLastIndexOf((const UTF8Char*)str1, (const UTF8Char *)str2); }
	FORCEINLINE Bool StrContainChars(const Char *str, const Char *chars) { return StrContainChars((const UTF8Char*)str, (const UTF8Char*)chars); }
	FORCEINLINE Char *StrRTrim(Char* str1) { return (Char*)StrRTrim((UTF8Char*)str1); };
	FORCEINLINE Char *StrLTrim(Char* str1) { return (Char*)StrLTrim((UTF8Char*)str1); };
	FORCEINLINE Char *StrTrim(Char* str1) { return (Char*)StrTrim((UTF8Char*)str1); };
	FORCEINLINE Char *StrTrimWSCRLF(Char* str1) { return (Char*)StrTrimWSCRLF((UTF8Char*)str1); };
	FORCEINLINE Char *StrRemoveChar(Char *str1, Char c) { return (Char*)StrRemoveChar((UTF8Char*)str1, (UTF8Char)c); };

	const Char *StrCopyNew(const Char *str1);
	const Char *StrCopyNewC(const Char *str1, UOSInt strLen);
	void StrDelNew(const Char *newStr);
	FORCEINLINE Bool StrStartsWith(const Char *str1, const Char *str2) { return StrStartsWith((const UTF8Char*)str1, (const UTF8Char*)str2); }
	FORCEINLINE Bool StrStartsWithICase(const Char *str1, const Char *str2) { return StrStartsWithICase((const UTF8Char*)str1, (const UTF8Char*)str2); }
	FORCEINLINE Bool StrEndsWith(const Char *str1, const Char *str2) { return StrEndsWith((const UTF8Char*)str1, (const UTF8Char*)str2); }
	FORCEINLINE Bool StrEndsWithICase(const Char *str1, const Char *str2) { return StrEndsWithICase((const UTF8Char*)str1, (const UTF8Char*)str2); }
	FORCEINLINE Bool StrIsInt32(const Char *str1) { return StrIsInt32((const UTF8Char*)str1); };
	FORCEINLINE UOSInt StrReplace(Char *str1, Char oriC, Char destC) { return StrReplace((UTF8Char*)str1, (UTF8Char)oriC, (UTF8Char)destC); };
	FORCEINLINE UOSInt StrReplace(Char *str1, const Char *replaceFrom, const Char *replaceTo) { return StrReplace((UTF8Char*)str1, (const UTF8Char*)replaceFrom, (const UTF8Char*)replaceTo); };
	FORCEINLINE UOSInt StrReplaceICase(Char *str1, const Char *replaceFrom, const Char *replaceTo) { return StrReplaceICase((UTF8Char*)str1, (const UTF8Char*)replaceFrom, (const UTF8Char*)replaceTo); };
	FORCEINLINE Char *StrToCSVRec(Char *oriStr, const Char *str1) { return (Char*)StrToCSVRec((UTF8Char*)oriStr, (const UTF8Char*)str1); };
	FORCEINLINE const Char *StrToNewCSVRec(const Char *str1) { return (const Char*)StrToNewCSVRec((const UTF8Char*)str1); }
	FORCEINLINE UOSInt StrCSVSplit(Char **strs, UOSInt maxStrs, Char *strToSplit) { return StrCSVSplit((UTF8Char**)strs, maxStrs, (UTF8Char*)strToSplit); };
	FORCEINLINE Char *StrCSVJoin(Char *oriStr, const Char **strs, UOSInt nStrs) { return (Char*)StrCSVJoin((UTF8Char*)oriStr, (const UTF8Char**)strs, nStrs); };
	FORCEINLINE UOSInt StrCountChar(const Char *str1, Char c) { return StrCountChar((const UTF8Char*)str1, (UTF8Char)c); };
	FORCEINLINE Char *StrRemoveANSIEscapes(Char *str1) { return (Char*)StrRemoveANSIEscapes((UTF8Char*)str1); };


	struct CString
	{
		const UTF8Char *v;
		UOSInt len;

		FORCEINLINE Bool Equals(const UTF8Char *s, UOSInt len)
		{
			return Text::StrEqualsC(v, this->len, s, len);
		}

		FORCEINLINE Bool StartsWith(const UTF8Char *s, UOSInt len)
		{
			return Text::StrStartsWithC(v, this->len, s, len);
		}
	};
}
#endif
