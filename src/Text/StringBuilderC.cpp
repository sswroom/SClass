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

Text::StringBuilderC *Text::StringBuilderC::Append(const Char *s)
{
	UOSInt slen = Text::StrCharCntCh(s);
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
		MemCopyNO(buffEnd.Ptr(), s, (slen + 1) * sizeof(Char));
		buffEnd = buffEnd + slen;
	}
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendC(const Char *s, UOSInt charCnt)
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
		MemCopyNO(buffEnd.Ptr(), s, charCnt * sizeof(Char));
		buffEnd = &buffEnd[charCnt];
	}
	*buffEnd = 0;
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendChar(Char c, UOSInt repeatCnt)
{
	if (c == 0)
		return this;
	AllocLeng(repeatCnt);
	while (repeatCnt-- > 0)
	{
		*buffEnd++ = c;
	}
	*buffEnd = 0;
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendCSV(const Char **sarr, UOSInt nStr)
{
	NN<Text::String> s;
	UOSInt i;
	i = 0;
	while (i < nStr)
	{
		s = Text::String::NewCSVRec((const UTF8Char*)sarr[i]);
		if (i > 0)
			this->AppendC(",", 1);
		this->AppendC((const Char*)s->v.Ptr(), s->leng);
		s->Release();
		i++;
	}
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendToUpper(const Char *s)
{
	UOSInt slen = Text::StrCharCntCh(s);
	AllocLeng(slen);
	buffEnd = Text::StrToUpper(buffEnd, s);
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendToLower(const Char *s)
{
	UOSInt slen = Text::StrCharCntCh(s);
	AllocLeng(slen);
	buffEnd = Text::StrToLower(buffEnd, s);
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendASCII(const Char *s)
{
	UOSInt slen = Text::StrCharCntCh(s);
	AllocLeng(slen);
	while ((*buffEnd++ = *s++) != 0);
	buffEnd--;
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendASCII(const Char *s, UOSInt charCnt)
{
	AllocLeng(charCnt);
	while (charCnt-- > 0)
	{
		*buffEnd++ = *s++;
	}
	*buffEnd = 0;
	return this;
}
