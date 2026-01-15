#include "Stdafx.h"
#include "Text/PString.h"

void Text::PString::Trim(UIntOS index)
{
	this->leng = (UIntOS)(Text::StrTrimC(this->v + index, this->leng - index) - this->v);
}

void Text::PString::RTrim()
{
	UIntOS len = this->leng;
	while (len > 0)
	{
		UTF8Char c = this->v[len - 1];
		if (c == ' ' || c == '\t' || c == 0)
		{
			len--;
		}
		else
		{
			break;
		}
	}
	this->v[len] = 0;
	this->leng = len;
}

void Text::PString::TrimWSCRLF()
{
	this->leng = (UIntOS)(Text::StrTrimWSCRLFC(this->v, this->leng) - this->v);
}

void Text::PString::ToUpper()
{
	Text::StrToUpperC(this->v, this->v, this->leng);
}

void Text::PString::ToLower()
{
	Text::StrToLowerC(this->v, this->v, this->leng);
}

void Text::PString::ToCapital()
{
	Text::StrToCapital(this->v, this->v);
}

UIntOS Text::PString::Replace(UTF8Char fromChar, UTF8Char toChar)
{
	return Text::StrReplace(this->v, fromChar, toChar);
}

void Text::PString::RemoveWS()
{
	UnsafeArray<UTF8Char> sptr = this->v;
	UnsafeArray<UTF8Char> dptr = sptr;
	UTF8Char c;
	while ((c = *sptr) != 0)
	{
		switch (c)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;
		default:
			*dptr++ = c;
			break;
		}
		sptr++;
	}
	*dptr = 0;
	this->leng = (UIntOS)(dptr - this->v);
}

void Text::PString::RemoveChar(UTF8Char ch)
{
	UnsafeArray<UTF8Char> sptr = this->v;
	UnsafeArray<UTF8Char> dptr = sptr;
	UTF8Char c;
	while ((c = *sptr) != 0)
	{
		if (c != ch)
		{
			*dptr++ = c;
		}
		sptr++;
	}
	*dptr = 0;
	this->leng = (UIntOS)(dptr - this->v);
}

UIntOS Text::StrSplitP(UnsafeArray<PString> strs, UIntOS maxStrs, PString strToSplit, UTF8Char splitChar)
{
	UIntOS i = 1;
	UTF8Char c;
	UnsafeArray<UTF8Char> sptr = strToSplit.v;
	strs[0] = strToSplit;
	while ((c = *sptr++) != 0)
	{
		if (c == splitChar)
		{
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UIntOS)(sptr - strs[i - 1].v - 1);
			strs[i].v = sptr;
			strs[i].leng -= strs[i - 1].leng + 1;
			sptr[-1] = 0;
			i++;
			if (i >= maxStrs)
			{
				break;
			}
		}
	}
	return i;
}

UIntOS Text::StrSplitTrimP(UnsafeArray<PString> strs, UIntOS maxStrs, PString strToSplit, UTF8Char splitChar)
{
	UIntOS i = 0;
	UTF8Char c;
	UnsafeArray<UTF8Char> lastPtr;
	UnsafeArray<UTF8Char> thisPtr;
	UnsafeArray<UTF8Char> sptr = strToSplit.v;
	while (*sptr == ' ' || *sptr == '\r' || *sptr == '\n' || *sptr == '\t')
	{
		sptr++;
	}
	strs[i].v = lastPtr = sptr;
	strs[i].leng = strToSplit.leng - (UIntOS)(sptr - strToSplit.v);
	i++;
	while (i < maxStrs)
	{
		c = *sptr++;
		if (c == 0)
		{
			thisPtr = sptr - 1;
			while (lastPtr < thisPtr)
			{
				c = thisPtr[-1];
				if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
				{
					*--thisPtr = 0;
				}
				else
				{
					break;
				}
			}
			strs[i - 1].leng = (UIntOS)(thisPtr - strs[i - 1].v);
			break;
		}
		if (c == splitChar)
		{
			sptr[-1] = 0;

			thisPtr = sptr - 1;
			while (lastPtr < thisPtr)
			{
				c = thisPtr[-1];
				if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
					*--thisPtr = 0;
				else
					break;
			}
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UIntOS)(thisPtr - strs[i - 1].v);

			while (*sptr == ' ' || *sptr == '\r' || *sptr == '\n' || *sptr == '\t')
				sptr++;
			strs[i].v = lastPtr = sptr;
			strs[i].leng = strs[i].leng - (UIntOS)(strs[i].v - strs[i - 1].v);
			i++;
		}
	}
	return i;
}

UIntOS Text::StrSplitLineP(UnsafeArray<PString> strs, UIntOS maxStrs, PString strToSplit)
{
	UIntOS i = 0;
	UTF8Char c;
	UnsafeArray<UTF8Char> sptr = strToSplit.v;
	strs[i] = strToSplit;
	i++;
	while (i < maxStrs)
	{
		c = *sptr++;
		if (c == 0)
			break;
		if (c == 13)
		{
			sptr[-1] = 0;
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UIntOS)(&sptr[-1] - strs[i - 1].v);
			if (*sptr == 10)
			{
				sptr++;
				strs[i].v = sptr;
				strs[i].leng -= strs[i - 1].leng + 2;
			}
			else
			{
				strs[i].v = sptr;
				strs[i].leng -= strs[i - 1].leng + 1;
			}
			i++;
		}
		else if (c == 10)
		{
			sptr[-1] = 0;
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UIntOS)(&sptr[-1] - strs[i - 1].v);
			strs[i].v = sptr;
			strs[i].leng -= strs[i - 1].leng + 1;
			i++;
		}
	}
	return i;
}

UIntOS Text::StrSplitWSP(UnsafeArray<PString> strs, UIntOS maxStrs, PString strToSplit)
{
	UIntOS i = 0;
	UTF8Char c;
	UIntOS strLen = strToSplit.leng;
	UnsafeArray<UTF8Char> sptr = strToSplit.v;
	strs[0].v = sptr;
	while (true)
	{
		c = *sptr++;
		if (c == 0)
		{
			return 0;
		}
		if (c != 32 && c != '\t')
		{
			strs[0].leng = strLen - (UIntOS)(sptr - strs[0].v - 1);
			strs[0].v = sptr - 1;
			i = 1;
			break;
		}
	}
	while (i < maxStrs)
	{
		c = *sptr++;
		if (c == 0)
			break;
		if (c == 32 || c == '\t')
		{
			sptr[-1] = 0;
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UIntOS)(sptr - strs[i - 1].v - 1);
			while (true)
			{
				c = *sptr++;
				if (c == 0)
				{
					return i;
				}
				if (c != 32 && c != '\t')
				{
					strs[i].v = sptr - 1;
					strs[i].leng = strs[i].leng - (UIntOS)(sptr - strs[i - 1].v - 1);
					i++;
					break;
				}
			}
		}
	}
	return i;
}

UIntOS Text::StrCSVSplitP(UnsafeArray<PString> strs, UIntOS maxStrs, UnsafeArray<UTF8Char> strToSplit)
{
	Bool quoted = false;
	Bool first = true;
	UIntOS i = 0;
	UnsafeArray<UTF8Char> strCurr;
	UTF8Char c;
	strs[i++].v = strCurr = strToSplit;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
		{
			*strCurr = 0;
			strs[i - 1].leng = (UIntOS)(strCurr - strs[i - 1].v);
			break;
		}
		if (c == '"')
		{
			if (!quoted)
			{
				quoted = true;
				first = false;
			}
			else if (*strToSplit == '"')
			{
				strToSplit++;
				*strCurr++ = '"';
				first = false;
			}
			else
			{
				quoted = false;
			}
		}
		else if (c == ',' && !quoted)
		{
			*strCurr = 0;
			strs[i - 1].leng = (UIntOS)(strCurr - strs[i - 1].v);
			strs[i++].v = strCurr = strToSplit;
			first = true;
		}
		else
		{
			if (c == ' ' && first)
			{
			}
			else
			{
				*strCurr++ = c;
				first = false;
			}
		}
	}
	return i;
}
