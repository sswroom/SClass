#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/CharUtil.h"
#include "Text/JSText.h"
#include "Text/StringTool.h"

void Text::StringTool::BuildString(Text::StringBuilderUTF *sb, const UTF8Char *s)
{
	if (s == 0)
	{
		sb->Append((const UTF8Char*)"null");
	}
	else
	{
		Text::JSText::ToJSTextDQuote(sb, s);
	}
}

void Text::StringTool::BuildString(Text::StringBuilderUTF *sb, Data::StringUTF8Map<const UTF8Char*> *map)
{
	if (map == 0)
	{
		sb->Append((const UTF8Char*)"null");
		return;
	}
	sb->AppendChar('{', 1);
	Data::ArrayList<const UTF8Char *> *keys = map->GetKeys();
	Data::ArrayList<const UTF8Char *> *vals = map->GetValues();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendChar(',', 1);
			sb->AppendChar(' ', 1);
		}
		BuildString(sb, keys->GetItem(i));
		sb->AppendChar('=', 1);
		BuildString(sb, vals->GetItem(i));
		i++;
	}
	sb->AppendChar('}', 1);
}

void Text::StringTool::BuildString(Text::StringBuilderUTF *sb, Data::List<const UTF8Char*> *list)
{
	if (list == 0)
	{
		sb->Append((const UTF8Char*)"null");
		return;
	}
	sb->AppendChar('[', 1);
	UOSInt i = 0;
	UOSInt j = list->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendChar(',', 1);
			sb->AppendChar(' ', 1);
		}
		BuildString(sb, list->GetItem(i));
		i++;
	}
	sb->AppendChar('}', 1);
}

void Text::StringTool::Int32Join(Text::StringBuilderUTF *sb, Data::List<Int32> *list, const UTF8Char *seperator)
{
	UOSInt i = 0;
	UOSInt j = list->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->Append(seperator);
		}
		sb->AppendI32(list->GetItem(i));
		i++;
	}
}

Bool Text::StringTool::IsNonASCII(const UTF8Char *s)
{
	UTF8Char c;
	while ((c = *s++) != 0)
	{
		if (c >= 0x80)
		{
			return true;
		}
	}
	return false;
}

Bool Text::StringTool::IsASCIIText(const UInt8 *buff, UOSInt buffLen)
{
	UInt8 b;
	while (buffLen-- > 0)
	{
		b = *buff++;
		if ((b >= 0x20 && b < 0x7F) || b == 13 || b == 10)
		{

		}
		else
		{
			return false;
		}
	}
	return true;
}

Bool Text::StringTool::IsEmailAddress(const UTF8Char *s)
{
	UOSInt atPos = INVALID_INDEX;
	Bool dotFound = false;
	const UTF8Char *startPtr = s;
	UTF8Char c;
	while ((c = *s++) != 0)
	{
		if (Text::CharUtil::IsAlphaNumeric(c) || c == '-')
		{

		}
		else if (c == '.')
		{
			if (atPos != INVALID_INDEX)
			{
				dotFound = true;
			}
		}
		else if (c == '@')
		{
			if (atPos != INVALID_INDEX)
			{
				return false;
			}
			atPos = (UOSInt)(s - startPtr - 1);
			dotFound = false;

		}
		else
		{
			return false;
		}
	}
	if (atPos == INVALID_INDEX || atPos == 0 || !dotFound)
	{
		return false;
	}
	return true;
}

const UTF8Char *Text::StringTool::Null2Empty(const UTF8Char *s)
{
	if (s) return s;
	return (const UTF8Char*)"";
}
