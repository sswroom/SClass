#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/String.h"

//#define THREADSAFE
//#define MEMDEBUG

Text::String Text::String::emptyStr(1048576);

Text::String *Text::String::NewOrNull(const UTF8Char *str)
{
	if (str == 0) return 0;
	UOSInt len = Text::StrCharCnt(str);
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->v = s->vbuff;
	s->leng = len;
	s->useCnt = 1;
	MemCopyNO(s->v, str, len + 1);
	return s;
}

Text::String *Text::String::NewNotNull(const UTF8Char *str)
{
	UOSInt len = Text::StrCharCnt(str);
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->v = s->vbuff;
	s->leng = len;
	s->useCnt = 1;
	MemCopyNO(s->v, str, len + 1);
	return s;
}

Text::String *Text::String::New(const UTF8Char *str, UOSInt len)
{
	if (len == 0) return NewEmpty();
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->v = s->vbuff;
	s->leng = len;
	s->useCnt = 1;
	MemCopyNO(s->v, str, len);
	s->v[len] = 0;
	return s;
}

Text::String *Text::String::New(UOSInt len)
{
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->v = s->vbuff;
	s->leng = len;
	s->useCnt = 1;
	s->v[0] = 0;
	return s;
}

Text::String *Text::String::NewOrNull(const UTF16Char *str)
{
	if (str == 0) return 0;
	UOSInt charCnt = Text::StrUTF16_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF16_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::NewNotNull(const UTF16Char *str)
{
	UOSInt charCnt = Text::StrUTF16_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF16_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::New(const UTF16Char *str, UOSInt len)
{
	if (len == 0) return NewEmpty();
	UOSInt charCnt = Text::StrUTF16_UTF8CntC(str, len);
	Text::String *s = New(charCnt);
	Text::StrUTF16_UTF8C(s->v, str, len);
	s->v[charCnt] = 0;
	return s;
}

Text::String *Text::String::NewOrNull(const UTF32Char *str)
{
	if (str == 0) return 0;
	UOSInt charCnt = Text::StrUTF32_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF32_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::NewNotNull(const UTF32Char *str)
{
	UOSInt charCnt = Text::StrUTF32_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF32_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::New(const UTF32Char *str, UOSInt len)
{
	if (len == 0) return NewEmpty();
	UOSInt charCnt = Text::StrUTF32_UTF8CntC(str, len);
	Text::String *s = New(charCnt);
	Text::StrUTF32_UTF8C(s->v, str, len);
	s->v[charCnt] = 0;
	return s;
}

Text::String *Text::String::NewCSVRec(const UTF8Char *str)
{
	UOSInt len = 2;
	UTF8Char c;
	const UTF8Char *sptr = str;
	UTF8Char *sptr2;
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
	Text::String *s = New(len);
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

Text::String *Text::String::NewEmpty()
{
	return emptyStr.Clone();
}

Text::String *Text::String::OrEmpty(Text::String *s)
{
	if (s) return s;
	return emptyStr.Clone();
}

void Text::String::Release()
{
#if defined(THREADSAFE)
#if _OSINT_SIZE == 64
	Interlocked_DecrementU64(&this->cnt);
#else
	Interlocked_DecrementU32(&this->cnt);
#endif
	if (this->cnt == 0)
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

Text::String *Text::String::Clone()
{
#if defined(MEMDEBUG)
	return New(this->v, this->leng);
#elif defined(THREADSAFE)
	#if _OSINT_SIZE == 64
	Interlocked_IncrementU64(&this->cnt);
	#else
	Interlocked_IncrementU32(&this->cnt);
	#endif
	return this;
#else
	this->useCnt++;
	return this;
#endif
}

Text::String *Text::String::ToLower()
{
	if (this->HasUpperCase())
	{
		UOSInt len = this->leng;
		Text::String *s = Text::String::New(len);
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