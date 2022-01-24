#include "Stdafx.h"
#include "Text/PString.h"

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

UOSInt Text::StrSplitP(PString *strs, UOSInt maxStrs, UTF8Char *strToSplit, UOSInt strLen, UTF8Char splitChar)
{
	UOSInt i = 0;
	UTF8Char c;
	strs[i].v = strToSplit;
	strs[i].leng = strLen;
	i++;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
			break;
		if (c == splitChar)
		{
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UOSInt)(strToSplit - strs[i - 1].v - 1);
			strs[i].v = strToSplit;
			strs[i].leng -= strs[i - 1].leng + 1;
			strToSplit[-1] = 0;
			i++;
		}
	}
	return i;
}

UOSInt Text::StrSplitTrimP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit, UOSInt strLen, UTF8Char splitChar)
{
	UOSInt i = 0;
	UTF8Char c;
	UTF8Char *lastPtr;
	UTF8Char *thisPtr;
	while (*strToSplit == ' ' || *strToSplit == '\r' || *strToSplit == '\n' || *strToSplit == '\t')
	{
		strToSplit++;
		strLen--;
	}
	strs[i].v = lastPtr = strToSplit;
	strs[i].leng = strLen;
	i++;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
		{
			thisPtr = strToSplit - 1;
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
			strToSplit[-1] = 0;

			thisPtr = strToSplit - 1;
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

			while (*strToSplit == ' ' || *strToSplit == '\r' || *strToSplit == '\n' || *strToSplit == '\t')
				strToSplit++;
			strs[i].v = lastPtr = strToSplit;
			strs[i].leng = strs[i].leng - (UOSInt)(strs[i].v - strs[i - 1].v);
			i++;
		}
	}
	return i;
}

UOSInt Text::StrSplitLineP(PString *strs, UOSInt maxStrs, UTF8Char *strToSplit, UOSInt strLen)
{
	UOSInt i = 0;
	UTF8Char c;
	strs[i].v = strToSplit;
	strs[i].leng = strLen;
	i++;
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
			break;
		if (c == 13)
		{
			strToSplit[-1] = 0;
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UOSInt)(&strToSplit[-1] - strs[i - 1].v);
			if (*strToSplit == 10)
			{
				strToSplit++;
				strs[i].v = strToSplit;
				strs[i].leng -= strs[i - 1].leng - 2;
			}
			else
			{
				strs[i].v = strToSplit;
				strs[i].leng -= strs[i - 1].leng - 1;
			}
			i++;
		}
		else if (c == 10)
		{
			strToSplit[-1] = 0;
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UOSInt)(&strToSplit[-1] - strs[i - 1].v);
			strs[i].v = strToSplit;
			strs[i].leng -= strs[i - 1].leng - 1;
			i++;
		}
	}
	return i;
}

UOSInt Text::StrSplitWSP(Text::PString *strs, UOSInt maxStrs, UTF8Char *strToSplit, UOSInt strLen)
{
	UOSInt i = 0;
	UTF8Char c;
	strs[0].v = strToSplit;
	while (true)
	{
		c = *strToSplit++;
		if (c == 0)
		{
			return 0;
		}
		if (c != 32 && c != '\t')
		{
			strs[0].leng = strLen - (UOSInt)(strToSplit - strs[0].v - 1);
			strs[0].v = strToSplit;
			i = 1;
			break;
		}
	}
	while (i < maxStrs)
	{
		c = *strToSplit++;
		if (c == 0)
			break;
		if (c == 32 || c == '\t')
		{
			strToSplit[-1] = 0;
			strs[i].leng = strs[i - 1].leng;
			strs[i - 1].leng = (UOSInt)(strToSplit - strs[i - 1].v - 1);
			while (true)
			{
				c = *strToSplit++;
				if (c == 0)
				{
					return i;
				}
				if (c != 32 && c != '\t')
				{
					strs[i].v = strToSplit - 1;
					strs[i].leng = strs[i].leng - (UOSInt)(strToSplit - strs[i - 1].v - 1);
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
