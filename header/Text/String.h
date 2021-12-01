#ifndef _SM_TEXT_STRING
#define _SM_TEXT_STRING

namespace Text
{
	struct String
	{
		UOSInt leng;
		UOSInt cnt;
		UTF8Char v[1];

		static String *New(const UTF8Char *str);
		static String *New(const UTF8Char *str, UOSInt len);
		static String *New(UOSInt len);
		static String *New(const UTF16Char *str);
		static String *New(const UTF32Char *str);
		void Release();
		String *Clone();

	private:
		~String();
	};
}

#define SDEL_STRING(s) if (s) { s->Release(); s = 0; }
#endif
