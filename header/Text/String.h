#ifndef _SM_TEXT_STRING
#define _SM_TEXT_STRING
#include "Text/CString.h"
#include "Text/PString.h"

namespace Text
{
	struct String : public PString
	{
	private:
		static String emptyStr;
	public:
		UOSInt useCnt;
		UTF8Char vbuff[1];

		static String *NewOrNullSlow(const UTF8Char *str);
		static String *NewNotNullSlow(const UTF8Char *str);
		static String *NewOrNull(Text::CString str);
		static String *New(Text::CString str);
		static String *New(const UTF8Char *str, UOSInt len);
		static String *NewP(const UTF8Char *str, const UTF8Char *strEnd);
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
		String *Clone() const;

		String *ToLower();
	private:
		String(UOSInt cnt);
		~String();
	};
}

#define STR_PTR(s) ((s)?s->v:0)
#define STR_PTRC(s) (s)?s->v:0, (s)?s->leng:0
#define STR_CSTR(s) ((s)?(s)->ToCString():Text::CString(0, 0))
#define STR_REL(s) (s->Release())
#define SCOPY_STRING(s) ((s)?(s)->Clone():0)
#define SDEL_STRING(s) if (s) { (s)->Release(); s = 0; }
#endif
