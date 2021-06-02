#include "Stdafx.h"
#include "MyMemory.h"
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
