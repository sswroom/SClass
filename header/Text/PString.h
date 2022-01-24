#ifndef _SM_TEXT_PSTRING
#define _SM_TEXT_PSTRING
namespace Text
{
	struct PString
	{
		UTF8Char *v;
		UOSInt leng;

		UTF8Char *ConcatTo(UTF8Char *sbuff);
		UTF8Char *ConcatToS(UTF8Char *sbuff, UOSInt buffSize);
		Bool Equals(const UTF8Char *s, UOSInt len);
		Bool Equals(Text::PString *s);
		Bool EqualsICase(const UTF8Char *s, UOSInt len);
		Bool EqualsICase(Text::PString *s);
		Bool StartsWith(Text::PString *s);
		Bool StartsWith(const UTF8Char *s, UOSInt len);
		Bool StartsWith(UOSInt startIndex, const UTF8Char *s, UOSInt len);
		Bool StartsWithICase(const UTF8Char *s, UOSInt len);
		Bool EndsWith(UTF8Char c);
		Bool EndsWith(const UTF8Char *s, UOSInt len);
		Bool EndsWithICase(const UTF8Char *s, UOSInt len);
		Bool HasUpperCase();
		Bool ContainChars(const UTF8Char *chars);
		UOSInt IndexOf(const UTF8Char *s, UOSInt len);
		UOSInt IndexOf(UTF8Char c);
		UOSInt IndexOfICase(const UTF8Char *s);
		UOSInt LastIndexOf(UTF8Char c);
		OSInt CompareTo(PString *s);
		OSInt CompareTo(const UTF8Char *s);
		OSInt CompareToICase(Text::PString *s);
		OSInt CompareToICase(const UTF8Char *s);
		OSInt CompareToFast(const UTF8Char *s, UOSInt len);
		void RTrim();

		Int32 ToInt32();
		UInt32 ToUInt32();
		Int64 ToInt64();
		UInt64 ToUInt64();
		OSInt ToOSInt();
		UOSInt ToUOSInt();
		Double ToDouble();
		Bool ToUInt8(UInt8 *outVal);
		Bool ToInt16(Int16 *outVal);
		Bool ToUInt16(UInt16 *outVal);
		Bool ToInt32(Int32 *outVal);
		Bool ToUInt32(UInt32 *outVal);
		Bool ToInt64(Int64 *outVal);
		Bool ToUInt64(UInt64 *outVal);
		Bool ToDouble(Double *outVal);
		Bool ToUInt16S(UInt16 *outVal, UInt16 failVal);

		Double MatchRating(Text::PString *s);
		Double MatchRating(const UTF8Char *targetStr, UOSInt strLen);
	};

	UOSInt StrSplitP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen, UTF8Char splitChar); //Optimized
	UOSInt StrSplitTrimP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen, UTF8Char splitChar); //Optimized
	UOSInt StrSplitLineP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen); //Optimized
	UOSInt StrSplitWSP(PString *strs, UOSInt maxStrs, UTF8Char *str, UOSInt strLen); //Optimized
	UOSInt StrCSVSplitP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit);
}
#endif
