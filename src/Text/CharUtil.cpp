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
