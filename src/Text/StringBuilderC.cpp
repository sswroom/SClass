#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/String.h"
#include "Text/StringBuilderC.h"
#include "Text/MyString.h"

Text::StringBuilderC::StringBuilderC() : Text::StringBuilder<Char>()
{
}

Text::StringBuilderC::~StringBuilderC()
{
}

NN<Text::StringBuilderC> Text::StringBuilderC::Append(UnsafeArray<const Char> s)
{
	UIntOS slen = Text::StrCharCntCh(s);
	AllocLeng(slen);
	if (slen < 8)
	{
		while (slen-- > 0)
		{
			*buffEnd++ = *s++;
		}
		*buffEnd = 0;
	}
	else
	{
		MemCopyNO(buffEnd.Ptr(), s.Ptr(), (slen + 1) * sizeof(Char));
		buffEnd = buffEnd + slen;
	}
	return *this;
}

NN<Text::StringBuilderC> Text::StringBuilderC::AppendC(UnsafeArray<const Char> s, UIntOS charCnt)
{
	AllocLeng(charCnt);
	if (charCnt < 8)
	{
		while (charCnt-- > 0)
		{
			*buffEnd++ = *s++;
		}
	}
	else
	{
		MemCopyNO(buffEnd.Ptr(), s.Ptr(), charCnt * sizeof(Char));
		buffEnd = &buffEnd[charCnt];
	}
	*buffEnd = 0;
	return *this;
}

NN<Text::StringBuilderC> Text::StringBuilderC::AppendChar(Char c, UIntOS repeatCnt)
{
	if (c == 0)
		return *this;
	AllocLeng(repeatCnt);
	while (repeatCnt-- > 0)
	{
		*buffEnd++ = c;
	}
	*buffEnd = 0;
	return *this;
}

NN<Text::StringBuilderC> Text::StringBuilderC::AppendCSV(UnsafeArray<UnsafeArray<const Char>> sarr, UIntOS nStr)
{
	NN<Text::String> s;
	UIntOS i;
	i = 0;
	while (i < nStr)
	{
		s = Text::String::NewCSVRec(UnsafeArray<const UTF8Char>::ConvertFrom(sarr[i]));
		if (i > 0)
			this->AppendC(",", 1);
		this->AppendC((const Char*)s->v.Ptr(), s->leng);
		s->Release();
		i++;
	}
	return *this;
}

NN<Text::StringBuilderC> Text::StringBuilderC::AppendToUpper(UnsafeArray<const Char> s)
{
	UIntOS slen = Text::StrCharCntCh(s);
	AllocLeng(slen);
	buffEnd = Text::StrToUpper(buffEnd, s);
	return *this;
}

NN<Text::StringBuilderC> Text::StringBuilderC::AppendToLower(UnsafeArray<const Char> s)
{
	UIntOS slen = Text::StrCharCntCh(s);
	AllocLeng(slen);
	buffEnd = Text::StrToLower(buffEnd, s);
	return *this;
}

NN<Text::StringBuilderC> Text::StringBuilderC::AppendASCII(UnsafeArray<const Char> s)
{
	UIntOS slen = Text::StrCharCntCh(s);
	AllocLeng(slen);
	while ((*buffEnd++ = *s++) != 0);
	buffEnd--;
	return *this;
}

NN<Text::StringBuilderC> Text::StringBuilderC::AppendASCII(UnsafeArray<const Char> s, UIntOS charCnt)
{
	AllocLeng(charCnt);
	while (charCnt-- > 0)
	{
		*buffEnd++ = *s++;
	}
	*buffEnd = 0;
	return *this;
}
