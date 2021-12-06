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

		static String *New(const UTF8Char *str);
		static String *New(const UTF8Char *str, UOSInt len);
		static String *New(UOSInt len);
		static String *New(const UTF16Char *str);
		static String *New(const UTF16Char *str, UOSInt len);
		static String *New(const UTF32Char *str);
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
		Bool EndsWith(const UTF8Char *s);
		Bool EndsWithICase(const UTF8Char *s);
		Bool HasUpperCase();
		String *ToLower();
		UOSInt IndexOf(const UTF8Char *s);
		UOSInt IndexOf(UTF8Char c);
		UOSInt LastIndexOf(UTF8Char c);
		OSInt CompareTo(String *s);
		OSInt CompareTo(const UTF8Char *s);
		OSInt CompareToICase(Text::String *s);
		OSInt CompareToICase(const UTF8Char *s);

		Int32 ToInt32();
		Int64 ToInt64();
		UInt64 ToUInt64();
		Double ToDouble();
		Bool ToInt32(Int32 *outVal);
		Bool ToDouble(Double *outVal);

		Double MatchRating(Text::String *s);
		Double MatchRating(const UTF8Char *targetStr, UOSInt strLen);

	private:
		~String();
	};
}

#define SCOPY_STRING(s) ((s)?(s)->Clone():0)
#define SDEL_STRING(s) if (s) { s->Release(); s = 0; }
#endif
