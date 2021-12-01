#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/String.h"

Text::String *Text::String::New(const UTF8Char *str)
{
	UOSInt len = Text::StrCharCnt(str);
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->leng = len;
	s->cnt = 1;
	MemCopyNO(s->v, str, len + 1);
	return s;
}

Text::String *Text::String::New(const UTF8Char *str, UOSInt len)
{
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->leng = len;
	s->cnt = 1;
	MemCopyNO(s->v, str, len);
	s->v[len] = 0;
	return s;
}

Text::String *Text::String::New(UOSInt len)
{
	Text::String *s = (Text::String*)MAlloc(len + sizeof(String));
	s->leng = len;
	s->cnt = 1;
	s->v[0] = 0;
	return s;
}

Text::String *Text::String::New(const UTF16Char *str)
{
	UOSInt charCnt = Text::StrUTF16_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF16_UTF8(s->v, str);
	return s;
}

Text::String *Text::String::New(const UTF32Char *str)
{
	UOSInt charCnt = Text::StrUTF32_UTF8Cnt(str);
	Text::String *s = New(charCnt);
	Text::StrUTF32_UTF8(s->v, str);
	return s;
}

void Text::String::Release()
{
	this->cnt--;
	if (this->cnt == 0)
	{
		MemFree(this);
	}
}

Text::String *Text::String::Clone()
{
	this->cnt++;
	return this;
}

Text::String::~String()
{
}