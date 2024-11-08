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

		static Optional<String> NewOrNullSlow(UnsafeArrayOpt<const UTF8Char> str);
		static NN<String> NewNotNullSlow(UnsafeArray<const UTF8Char> str);
		static Optional<String> NewOrNull(Text::CString str);
		static NN<String> New(const Text::CStringNN &str)
		{
			if (str.leng == 0) return NewEmpty();
			NN<Text::String> s = NN<Text::String>::FromPtr((Text::String*)MAlloc(str.leng + sizeof(String)));
			s->v = UARR(s->vbuff);
			s->leng = str.leng;
			s->useCnt = 1;
			MemCopyNO(s->v.Ptr(), str.v.Ptr(), str.leng);
			s->v[str.leng] = 0;
			return s;
		}

		static NN<String> New(UnsafeArray<const UTF8Char> str, UOSInt len)
		{
			if (len == 0) return NewEmpty();
			NN<Text::String> s = NN<Text::String>::FromPtr((Text::String*)MAlloc(len + sizeof(String)));
			s->v = UARR(s->vbuff);
			s->leng = len;
			s->useCnt = 1;
			MemCopyNO(s->v.Ptr(), str.Ptr(), len);
			s->v[len] = 0;
			return s;
		}
		
		static NN<String> NewP(UnsafeArray<const UTF8Char> str, UnsafeArrayOpt<const UTF8Char> strEnd);
		static NN<String> NewP(UnsafeArray<const UTF8Char> str, UnsafeArray<const UTF8Char> strEnd);

		static NN<String> New(UOSInt len)
		{
			NN<Text::String> s = NN<Text::String>::FromPtr((Text::String*)MAlloc(len + sizeof(String)));
			s->v = UARR(s->vbuff);
			s->leng = len;
			s->useCnt = 1;
			s->v[0] = 0;
			return s;
		}

		static Optional<String> NewOrNull(UnsafeArrayOpt<const UTF16Char> str);
		static NN<String> NewNotNull(UnsafeArray<const UTF16Char> str);
		static NN<String> NewW(UnsafeArray<const UTF16Char> str, UOSInt len);
		static Optional<String> NewOrNull(UnsafeArrayOpt<const UTF32Char> str);
		static NN<String> NewNotNull(UnsafeArray<const UTF32Char> str);
		static NN<String> NewW(UnsafeArray<const UTF32Char> str, UOSInt len);
		static NN<String> NewCSVRec(UnsafeArray<const UTF8Char> str);
		static NN<String> NewEmpty() { return emptyStr.Clone(); }
		static NN<String> OrEmpty(Optional<Text::String> s) { NN<Text::String> ret; if (s.SetTo(ret)) return ret; return emptyStr.Clone(); }
		static Optional<String> CopyOrNull(Optional<Text::String> s) { NN<Text::String> ret; if (s.SetTo(ret)) return ret->Clone(); return 0; }
		void Release();
		NN<String> Clone() const;

		NN<String> ToNewLower() const;
	private:
		String(UOSInt cnt);
		~String();
	};
}

#define STR_PTR(s) ((s)?UnsafeArrayOpt<const UTF8Char>(s->v):UnsafeArrayOpt<const UTF8Char>(0))
#define STR_PTRC(s) (s)?UnsafeArrayOpt<const UTF8Char>(s->v):UnsafeArrayOpt<const UTF8Char>(0), (s)?s->leng:0
#define STR_CSTR(s) ((s)?(s)->ToCString():Text::CString(0, 0))
#define STR_REL(s) (s->Release())
#define SCOPY_STRING(s) ((s)?(s)->Clone().Ptr():0)
#define SDEL_STRING(s) if (s) { (s)->Release(); s = 0; }
#define OPTSTR_DEL(s) {Optional<Text::String> os = (s); if (!os.IsNull()) {os.OrNull()->Release(); s = 0;}}
#define OPTSTR_CSTR(s) (s.IsNull()?Text::CString(0, 0):s.OrNull()->ToCString())
#endif
