#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/String.h"

//#define THREADSAFE
//#define MEMDEBUG

Text::String Text::String::emptyStr(1048576);

Optional<Text::String> Text::String::NewOrNullSlow(UnsafeArrayOpt<const UTF8Char> str)
{
	UnsafeArray<const UTF8Char> nnstr;
	if (!str.SetTo(nnstr)) return 0;
	if (nnstr[0] == 0) return NewEmpty();
	UOSInt len = Text::StrCharCnt(nnstr);
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->v = s->vbuff;
	s->leng = len;
	s->useCnt = 1;
	MemCopyNO(s->v.Ptr(), nnstr.Ptr(), len + 1);
	return s;
}

NN<Text::String> Text::String::NewNotNullSlow(UnsafeArray<const UTF8Char> str)
{
	if (str[0] == 0) return NewEmpty();
	UOSInt len = Text::StrCharCnt(str);
	NN<Text::String> s = NN<Text::String>::FromPtr((Text::String*)MAlloc(len + sizeof(String)));
	s->v = s->vbuff;
	s->leng = len;
	s->useCnt = 1;
	MemCopyNO(s->v.Ptr(), str.Ptr(), len + 1);
	return s;
}

Optional<Text::String> Text::String::NewOrNull(Text::CString str)
{
	Text::CStringNN nnstr;
	if (!str.SetTo(nnstr)) return 0;
	if (nnstr.leng == 0) return NewEmpty();
	Text::String *s = (Text::String*)MAlloc(nnstr.leng + sizeof(String));
	s->v = s->vbuff;
	s->leng = nnstr.leng;
	s->useCnt = 1;
	MemCopyNO(s->v.Ptr(), nnstr.v.Ptr(), nnstr.leng);
	s->v[str.leng] = 0;
	return s;
}

NN<Text::String> Text::String::NewP(UnsafeArray<const UTF8Char> str, UnsafeArrayOpt<const UTF8Char> strEnd)
{
	UnsafeArray<const UTF8Char> strE;
	if (!strEnd.SetTo(strE) || strE == str) return NewEmpty();
	UOSInt len = (UOSInt)(strE - str);
	NN<Text::String> s = NN<Text::String>::FromPtr((Text::String*)MAlloc(len + sizeof(String)));
	s->v = s->vbuff;
	s->leng = len;
	s->useCnt = 1;
	MemCopyNO(s->v.Ptr(), str.Ptr(), len);
	s->v[len] = 0;
	return s;
}

NN<Text::String> Text::String::NewP(UnsafeArray<const UTF8Char> str, UnsafeArray<const UTF8Char> strEnd)
{
	if (strEnd == str) return NewEmpty();
	UOSInt len = (UOSInt)(strEnd - str);
	NN<Text::String> s = NN<Text::String>::FromPtr((Text::String*)MAlloc(len + sizeof(String)));
	s->v = s->vbuff;
	s->leng = len;
	s->useCnt = 1;
	MemCopyNO(s->v.Ptr(), str.Ptr(), len);
	s->v[len] = 0;
	return s;
}

Optional<Text::String> Text::String::NewOrNull(const UTF16Char *str)
{
	if (str == 0) return 0;
	UOSInt charCnt = Text::StrUTF16_UTF8Cnt(str);
	Text::String *s = New(charCnt).Ptr();
	Text::StrUTF16_UTF8(s->v.Ptr(), str);
	return s;
}

NN<Text::String> Text::String::NewNotNull(const UTF16Char *str)
{
	UOSInt charCnt = Text::StrUTF16_UTF8Cnt(str);
	NN<Text::String> s = New(charCnt);
	Text::StrUTF16_UTF8(s->v.Ptr(), str);
	return s;
}

NN<Text::String> Text::String::New(const UTF16Char *str, UOSInt len)
{
	if (len == 0) return NewEmpty();
	UOSInt charCnt = Text::StrUTF16_UTF8CntC(str, len);
	NN<Text::String> s = New(charCnt);
	Text::StrUTF16_UTF8C(s->v.Ptr(), str, len);
	s->v[charCnt] = 0;
	return s;
}

Optional<Text::String> Text::String::NewOrNull(const UTF32Char *str)
{
	if (str == 0) return 0;
	UOSInt charCnt = Text::StrUTF32_UTF8Cnt(str);
	Text::String *s = New(charCnt).Ptr();
	Text::StrUTF32_UTF8(s->v.Ptr(), str);
	return s;
}

NN<Text::String> Text::String::NewNotNull(const UTF32Char *str)
{
	UOSInt charCnt = Text::StrUTF32_UTF8Cnt(str);
	NN<Text::String> s = New(charCnt);
	Text::StrUTF32_UTF8(s->v.Ptr(), str);
	return s;
}

NN<Text::String> Text::String::New(const UTF32Char *str, UOSInt len)
{
	if (len == 0) return NewEmpty();
	UOSInt charCnt = Text::StrUTF32_UTF8CntC(str, len);
	NN<Text::String> s = New(charCnt);
	Text::StrUTF32_UTF8C(s->v.Ptr(), str, len);
	s->v[charCnt] = 0;
	return s;
}

NN<Text::String> Text::String::NewCSVRec(UnsafeArray<const UTF8Char> str)
{
	UOSInt len = 2;
	UTF8Char c;
	UnsafeArray<const UTF8Char> sptr = str;
	UnsafeArray<UTF8Char> sptr2;
	while ((c = *sptr++) != 0)
	{
		if (c == '"')
		{
			len += 2;
		}
		else
		{
			len += 1;
		}
	}
	NN<Text::String> s = New(len);
	sptr2 = s->v;
	*sptr2++ = '"';
	while ((c = *str++) != 0)
	{
		if (c == '"')
		{
			*sptr2++ = '"';
			*sptr2++ = '"';
		}
		else
		{
			*sptr2++ = c;
		}
	}
	*sptr2++ = '"';
	*sptr2 = 0;
	return s;
}

NN<Text::String> Text::String::NewEmpty()
{
	return emptyStr.Clone();
}

NN<Text::String> Text::String::OrEmpty(Optional<Text::String> s)
{
	NN<Text::String> ret;
	if (s.SetTo(ret)) return ret;
	return emptyStr.Clone();
}

Optional<Text::String> Text::String::CopyOrNull(Optional<Text::String> s)
{
	NN<Text::String> ret;
	if (s.SetTo(ret)) return ret->Clone();
	return 0;
}

void Text::String::Release()
{
#if defined(THREADSAFE)
#if _OSINT_SIZE == 64
	UOSInt cnt = Interlocked_DecrementU64(&this->useCnt);
#else
	UOSInt cnt = Interlocked_DecrementU32(&this->useCnt);
#endif
	if (cnt == 0)
	{
		MemFree(this);
	}
#else
	this->useCnt--;
	if (this->useCnt == 0)
	{
		MemFree(this);
	}
#endif
}

NN<Text::String> Text::String::Clone() const
{
#if defined(MEMDEBUG)
	return New(this->v, this->leng);
#elif defined(THREADSAFE)
	#if _OSINT_SIZE == 64
	Interlocked_IncrementU64(&((Text::String*)this)->useCnt);
	#else
	Interlocked_IncrementU32(&((Text::String*)this)->useCnt);
	#endif
	return NN<Text::String>(*(Text::String*)this);
#else
	((Text::String*)this)->useCnt++;
	return NN<Text::String>(*(Text::String*)this);
#endif
}

NN<Text::String> Text::String::ToLower()
{
	if (this->HasUpperCase())
	{
		UOSInt len = this->leng;
		NN<Text::String> s = Text::String::New(len);
		Text::StrToLowerC(s->v, this->v, len);
		return s;
	}
	else
	{
		return this->Clone();
	}
}

Text::String::String(UOSInt cnt)
{
	this->v = this->vbuff;
	this->useCnt = cnt;
	this->leng = 0;
	this->vbuff[0] = 0;
}

Text::String::~String()
{
}