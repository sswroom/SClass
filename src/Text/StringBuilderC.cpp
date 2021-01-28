#include "Stdafx.h"
#include "MyMemory.h"
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
	OSInt slen = Text::StrCharCnt(s);
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
		MemCopyNO(buffEnd, s, (slen + 1) * sizeof(Char));
		buffEnd = &buffEnd[slen];
	}
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::Append(const Char *s, OSInt charCnt)
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
		MemCopyNO(buffEnd, s, charCnt * sizeof(Char));
		buffEnd = &buffEnd[charCnt];
	}
	*buffEnd = 0;
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::Append(Char c, OSInt repeatCnt)
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

Text::StringBuilderC *Text::StringBuilderC::AppendCSV(const Char **sarr, OSInt nStr)
{
	const Char *csptr;
	OSInt i;
	i = 0;
	while (i < nStr)
	{
		csptr = StrToNewCSVRec(sarr[i]);
		if (i > 0)
			this->Append(",");
		this->Append(csptr);
		Text::StrDelNew(csptr);
		i++;
	}
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendToUpper(const Char *s)
{
	OSInt slen = Text::StrCharCnt(s);
	AllocLeng(slen);
	buffEnd = Text::StrToUpper(buffEnd, s);
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendToLower(const Char *s)
{
	OSInt slen = Text::StrCharCnt(s);
	AllocLeng(slen);
	buffEnd = Text::StrToLower(buffEnd, s);
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendASCII(Char *s)
{
	OSInt slen = Text::StrCharCnt(s);
	AllocLeng(slen);
	while ((*buffEnd++ = *s++) != 0);
	buffEnd--;
	return this;
}

Text::StringBuilderC *Text::StringBuilderC::AppendASCII(Char *s, OSInt charCnt)
{
	AllocLeng(charCnt);
	while (charCnt-- > 0)
	{
		*buffEnd++ = *s++;
	}
	*buffEnd = 0;
	return this;
}
