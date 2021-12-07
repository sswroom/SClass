#ifndef _SM_TEXT_STRING
#define _SM_TEXT_STRING

namespace Text
{
	struct String
	{
	private:
		static String emptyStr;
	public:
		UOSInt leng;
		UOSInt cnt;
		UTF8Char v[1];

		static String *NewOrNull(const UTF8Char *str);
		static String *NewNotNull(const UTF8Char *str);
		static String *New(const UTF8Char *str, UOSInt len);
		static String *New(UOSInt len);
		static String *NewOrNull(const UTF16Char *str);
		static String *NewNotNull(const UTF16Char *str);
		static String *New(const UTF16Char *str, UOSInt len);
		static String *NewOrNull(const UTF32Char *str);
		static String *NewNotNull(const UTF32Char *str);
		static String *New(const UTF32Char *str, UOSInt len);
		static String *NewCSVRec(const UTF8Char *str);
		static String *NewEmpty();
		static String *OrEmpty(Text::String *s);
		void Release();
		String *Clone();

		UTF8Char *ConcatTo(UTF8Char *sbuff);
		Bool Equals(const UTF8Char *s);
		Bool Equals(Text::String *s);
		Bool EqualsICase(const UTF8Char *s);
		Bool StartsWith(const UTF8Char *s);
		Bool StartsWithICase(const UTF8Char *s);
		Bool EndsWith(UTF8Char c);
		Bool EndsWith(const UTF8Char *s);
		Bool EndsWithICase(const UTF8Char *s);
		Bool HasUpperCase();
		String *ToLower();
		UOSInt IndexOf(const UTF8Char *s);
		UOSInt IndexOf(UTF8Char c);
		UOSInt IndexOfICase(const UTF8Char *s);
		UOSInt LastIndexOf(UTF8Char c);
		OSInt CompareTo(String *s);
		OSInt CompareTo(const UTF8Char *s);
		OSInt CompareToICase(Text::String *s);
		OSInt CompareToICase(const UTF8Char *s);

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

		Double MatchRating(Text::String *s);
		Double MatchRating(const UTF8Char *targetStr, UOSInt strLen);

	private:
		~String();
	};
}

#define STR_PTR(s) ((s)?s->v:0)
#define STR_REL(s) (s->Release())
#define SCOPY_STRING(s) ((s)?(s)->Clone():0)
#define SDEL_STRING(s) if (s) { s->Release(); s = 0; }
#endif
