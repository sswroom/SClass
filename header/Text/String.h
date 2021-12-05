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
		static String *New(const UTF32Char *str);
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
		Bool HasUpperCase();
		String *ToLower();
		UOSInt IndexOf(const UTF8Char *s);

	private:
		~String();
	};
}

#define SDEL_STRING(s) if (s) { s->Release(); s = 0; }
#endif
