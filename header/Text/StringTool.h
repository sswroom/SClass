#ifndef _SM_TEXT_STRINGTOOL
#define _SM_TEXT_STRINGTOOL
#include "Data/NamedClass.h"
#include "Data/StringMap.h"
#include "Data/StringUTF8Map.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class StringTool
	{
	public:
		static void BuildString(Text::StringBuilderUTF8 *sb, Text::String *s);
		static void BuildString(Text::StringBuilderUTF8 *sb, const UTF8Char *s);
		static void BuildString(Text::StringBuilderUTF8 *sb, Data::StringMap<Text::String*> *map);
		static void BuildString(Text::StringBuilderUTF8 *sb, Data::StringUTF8Map<Text::String*> *map);
		static void BuildString(Text::StringBuilderUTF8 *sb, Data::StringUTF8Map<const UTF8Char*> *map);
		static void BuildString(Text::StringBuilderUTF8 *sb, Data::ReadingList<const UTF8Char*> *list);
		static void BuildString(Text::StringBuilderUTF8 *sb, Data::ReadingList<Text::String*> *list);
		template <class T> static void BuildString(Text::StringBuilderUTF8 *sb, T *obj, Data::NamedClass<T> *cls);
		template <class T> static void BuildString(Text::StringBuilderUTF8 *sb, Data::List<T*> *list, Data::NamedClass<T> *cls, const UTF8Char *clsName);
		static void Int32Join(Text::StringBuilderUTF8 *sb, Data::List<Int32> *list, const UTF8Char *seperator);
		static Bool IsNonASCII(const UTF8Char *s);
		static Bool IsASCIIText(const UInt8 *buff, UOSInt buffLen);
		static Bool IsEmailAddress(const UTF8Char *s);
		static const UTF8Char *Null2Empty(const UTF8Char *s);
	};
}

template <class T> void Text::StringTool::BuildString(Text::StringBuilderUTF8 *sb, T *obj, Data::NamedClass<T> *cls)
{
	UTF8Char sbuff[512];
	sb->AppendChar('{', 1);
	Bool found = false;
	UOSInt i = 0;
	UOSInt j = cls->GetFieldCount();
	while (i < j)
	{
		if (found)
		{
			sb->AppendChar(',', 1);
		}
		found = true;
		Text::JSText::ToJSTextDQuote(sbuff, cls->GetFieldName(i));
		sb->Append(sbuff);
		sb->AppendChar(':', 1);
		Data::VariItem *item = cls->GetNewValue(i, obj);
		if (item)
		{
			item->ToString(sb);
			DEL_CLASS(item);
		}
		else
		{
			sb->AppendC(UTF8STRC("null"));
		}
		i++;
	}
	sb->AppendChar('}', 1);
}

template <class T> void Text::StringTool::BuildString(Text::StringBuilderUTF8 *sb, Data::List<T*> *list, Data::NamedClass<T> *cls, const UTF8Char *clsName)
{
	if (list == 0)
	{
		sb->AppendC(UTF8STRC("null"));
		return;
	}
	UOSInt i;
	UOSInt j;
	sb->AppendChar('[', 1);
	i = 0;
	j = list->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendChar(',', 1);
		}
		sb->Append(clsName);
		BuildString(sb, list->GetItem(i), cls);
		i++;
	}
	sb->AppendChar(']', 1);
}
#endif
