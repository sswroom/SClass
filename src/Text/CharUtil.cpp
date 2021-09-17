#include "Stdafx.h"
#include "Text/CharUtil.h"

Bool Text::CharUtil::PtrIsWS(const UTF8Char **sptr)
{
	UTF8Char c = **sptr;
	switch (c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		++*sptr;
		return true;
	}
	return false;
}

Bool Text::CharUtil::IsWS(const UTF8Char *sptr)
{
	UTF8Char c = *sptr;
	switch (c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		return true;
	}
	return false;
}

Bool Text::CharUtil::IsPunctuation(UTF8Char c)
{
	switch (c)
	{
	case '[':
	case ']':
	case '-':
	case '\'':
	case ',':
	case '(':
	case ')':
		return true;
	default:
		return false;
	}
}

Bool Text::CharUtil::IsAlphaNumeric(UTF8Char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

Bool Text::CharUtil::IsAlphabet(UTF8Char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

Bool Text::CharUtil::UTF8CharValid(const UTF8Char *sptr)
{
	UTF8Char c = *sptr;
	if (c < 0x80)
	{
		return true;
	}
	else if ((c & 0xE0) == 0xC0)
	{
		return ((sptr[1] & 0xC0) == 0x80);
	}
	else if ((c & 0xF0) == 0xE0)
	{
		return ((sptr[1] & 0xC0) == 0x80) && ((sptr[2] & 0xC0) == 0x80);
	}
	else if ((c & 0xF8) == 0xF0)
	{
		return ((sptr[1] & 0xC0) == 0x80) && ((sptr[2] & 0xC0) == 0x80) && ((sptr[3] & 0xC0) == 0x80);
	}
	else if ((c & 0xFC) == 0xF8)
	{
		return ((sptr[1] & 0xC0) == 0x80) && ((sptr[2] & 0xC0) == 0x80) && ((sptr[3] & 0xC0) == 0x80) && ((sptr[4] & 0xC0) == 0x80);
	}
	else if ((c & 0xFE) == 0xFC)
	{
		return ((sptr[1] & 0xC0) == 0x80) && ((sptr[2] & 0xC0) == 0x80) && ((sptr[3] & 0xC0) == 0x80) && ((sptr[4] & 0xC0) == 0x80) && ((sptr[5] & 0xC0) == 0x80);
	}
	return false;
}
