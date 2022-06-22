#include "Stdafx.h"
#include "Text/PString.h"

void Text::PString::Trim()
{
	this->leng = (UOSInt)(Text::StrTrimC(this->v, this->leng) - this->v);
}

void Text::PString::Trim(UOSInt index)
{
	this->leng = (UOSInt)(Text::StrTrimC(&this->v[index], this->leng - index) - this->v);
}

void Text::PString::RTrim()
{
	UOSInt len = this->leng;
	while (len > 0)
	{
		UTF8Char c = this->v[len - 1];
		if (c == ' ' || c == '\t')
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
	this->leng = (UOSInt)(Text::StrTrimWSCRLFC(this->v, this->leng) - this->v);
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

UOSInt Text::PString::Replace(UTF8Char fromChar, UTF8Char toChar)
{
	return Text::StrReplace(this->v, fromChar, toChar);
}

void Text::PString::RemoveWS()
{
	UTF8Char *sptr = this->v;
	UTF8Char *dptr = sptr;
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
	this->leng = (UOSInt)(dptr - this->v);
}

UOSInt Text::StrSplitP(PString *strs, UOSInt maxStrs, PString strToSplit, UTF8Char splitChar)
{
	UOSInt i = 1;
	UTF8Char c;
	UTF8Char *sptr = strToSplit.v;
	strs[0] = strToSplit;
	while ((c = *sptr++) != 0)
	{
		if (c == splitChar)
		{
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UOSInt)(sptr - strs[i - 1].v - 1);
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

UOSInt Text::StrSplitTrimP(Text::PString *strs, UOSInt maxStrs, PString strToSplit, UTF8Char splitChar)
{
	UOSInt i = 0;
	UTF8Char c;
	UTF8Char *lastPtr;
	UTF8Char *thisPtr;
	UTF8Char *sptr = strToSplit.v;
	while (*sptr == ' ' || *sptr == '\r' || *sptr == '\n' || *sptr == '\t')
	{
		sptr++;
	}
	strs[i].v = lastPtr = sptr;
	strs[i].leng = strToSplit.leng - (UOSInt)(sptr - strToSplit.v);
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
			strs[i - 1].leng = (UOSInt)(thisPtr - strs[i - 1].v);
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
			strs[i - 1].leng = (UOSInt)(thisPtr - strs[i - 1].v);

			while (*sptr == ' ' || *sptr == '\r' || *sptr == '\n' || *sptr == '\t')
				sptr++;
			strs[i].v = lastPtr = sptr;
			strs[i].leng = strs[i].leng - (UOSInt)(strs[i].v - strs[i - 1].v);
			i++;
		}
	}
	return i;
}

UOSInt Text::StrSplitLineP(PString *strs, UOSInt maxStrs, PString strToSplit)
{
	UOSInt i = 0;
	UTF8Char c;
	UTF8Char *sptr = strToSplit.v;
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
			strs[i - 1].leng = (UOSInt)(&sptr[-1] - strs[i - 1].v);
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
			strs[i - 1].leng = (UOSInt)(&sptr[-1] - strs[i - 1].v);
			strs[i].v = sptr;
			strs[i].leng -= strs[i - 1].leng + 1;
			i++;
		}
	}
	return i;
}

UOSInt Text::StrSplitWSP(Text::PString *strs, UOSInt maxStrs, PString strToSplit)
{
	UOSInt i = 0;
	UTF8Char c;
	UOSInt strLen = strToSplit.leng;
	UTF8Char *sptr = strToSplit.v;
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
			strs[0].leng = strLen - (UOSInt)(sptr - strs[0].v - 1);
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
			strs[i - 1].leng = (UOSInt)(sptr - strs[i - 1].v - 1);
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
					strs[i].leng = strs[i].leng - (UOSInt)(sptr - strs[i - 1].v - 1);
					i++;
					break;
				}
			}
		}
	}
	return i;
}

UOSInt Text::StrCSVSplitP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit)
{
	Bool quoted = false;
	Bool first = true;
	UOSInt i = 0;
	UTF8Char *strCurr;
	UTF8Char c;
	strs[i++].v = strCurr = strToSplit;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
		{
			*strCurr = 0;
			strs[i - 1].leng = (UOSInt)(strCurr - strs[i - 1].v);
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
			strs[i - 1].leng = (UOSInt)(strCurr - strs[i - 1].v);
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
