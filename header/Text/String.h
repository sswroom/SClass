#ifndef _SM_TEXT_STRING
#define _SM_TEXT_STRING
#include "Stdafx.h"
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

		static Optional<String> NewOrNullSlow(const UTF8Char *str);
		static NN<String> NewNotNullSlow(const UTF8Char *str);
		static Optional<String> NewOrNull(Text::CString str);
		static NN<String> New(const Text::CString &str)
		{
			if (str.leng == 0) return NewEmpty();
			NN<Text::String> s = NN<Text::String>::FromPtr((Text::String*)MAlloc(str.leng + sizeof(String)));
			s->v = s->vbuff;
			s->leng = str.leng;
			s->useCnt = 1;
			MemCopyNO(s->v, str.v, str.leng);
			s->v[str.leng] = 0;
			return s;
		}

		static NN<String> New(const UTF8Char *str, UOSInt len)
		{
			if (len == 0) return NewEmpty();
			NN<Text::String> s = NN<Text::String>::FromPtr((Text::String*)MAlloc(len + sizeof(String)));
			s->v = s->vbuff;
			s->leng = len;
			s->useCnt = 1;
			MemCopyNO(s->v, str, len);
			s->v[len] = 0;
			return s;
		}
		
		static NN<String> NewP(const UTF8Char *str, const UTF8Char *strEnd);

		static NN<String> New(UOSInt len)
		{
			NN<Text::String> s = NN<Text::String>::FromPtr((Text::String*)MAlloc(len + sizeof(String)));
			s->v = s->vbuff;
			s->leng = len;
			s->useCnt = 1;
			s->v[0] = 0;
			return s;
		}

		static Optional<String> NewOrNull(const UTF16Char *str);
		static NN<String> NewNotNull(const UTF16Char *str);
		static NN<String> New(const UTF16Char *str, UOSInt len);
		static Optional<String> NewOrNull(const UTF32Char *str);
		static NN<String> NewNotNull(const UTF32Char *str);
		static NN<String> New(const UTF32Char *str, UOSInt len);
		static NN<String> NewCSVRec(const UTF8Char *str);
		static NN<String> NewEmpty();
		static NN<String> OrEmpty(Optional<Text::String> s);
		static Optional<String> CopyOrNull(Optional<Text::String> s);
		void Release();
		NN<String> Clone() const;

		NN<String> ToLower();
	private:
		String(UOSInt cnt);
		~String();
	};
}

#define STR_PTR(s) ((s)?s->v:0)
#define STR_PTRC(s) (s)?s->v:0, (s)?s->leng:0
#define STR_CSTR(s) ((s)?(s)->ToCString():Text::CString(0, 0))
#define STR_REL(s) (s->Release())
#define SCOPY_STRING(s) ((s)?(s)->Clone().Ptr():0)
#define SDEL_STRING(s) if (s) { (s)->Release(); s = 0; }
#define OPTSTR_DEL(s) {Optional<Text::String> os = (s); if (!os.IsNull()) os.OrNull()->Release();}
#define OPTSTR_CSTR(s) (s.IsNull()?Text::CString(0, 0):s.OrNull()->ToCString())
#endif
