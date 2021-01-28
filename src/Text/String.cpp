#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/String.h"
#include "Text/MyString.h"
#include <memory.h>

Text::String::String(const UTF8Char *str)
{
	this->leng = Text::StrCharCnt(str);
	this->strVal = (const UTF8Char*)MemAlloc(UTF8Char, this->leng + 1);
	MemCopyNO((void*)this->strVal, str, sizeof(UTF8Char) * (this->leng + 1));
}

Text::String::String(const UTF8Char *str, UTF8Char *strEnd)
{
	this->leng = strEnd - str;
	this->strVal = (const UTF8Char*)MemAlloc(UTF8Char, this->leng + 1);
	MemCopyNO((void*)this->strVal, str, sizeof(UTF8Char) * (this->leng + 1));
}

Text::String::String(OSInt leng)
{
	this->leng = leng;
	this->strVal = (const UTF8Char*)MemAlloc(UTF8Char, this->leng + 1);
}

Text::String::~String()
{
	MemFree((void*)this->strVal);
}

OSInt Text::String::CompareTo(Data::IComparable *obj)
{
	return Text::StrCompare(this->strVal, ((Text::String*)obj)->strVal);
}
