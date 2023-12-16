#ifndef _SM_TEXT_STRINGTOOL
#define _SM_TEXT_STRINGTOOL
#include "Data/ArrayListStringNN.h"
#include "Data/ByteArray.h"
#include "Data/NamedClass.h"
#include "Data/StringMap.h"
#include "Data/StringUTF8Map.h"
#include "Text/CString.h"
#include "Text/JSText.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class StringTool
	{
	public:
		static void BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Text::String *s);
		static void BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *s);
		static void BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::StringMap<Text::String*> *map);
		static void BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::StringUTF8Map<Text::String*> *map);
		static void BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::StringUTF8Map<const UTF8Char*> *map);
		static void BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::ReadingList<const UTF8Char*> *list);
		static void BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::ReadingList<Text::String*> *list);
		template <class T> static void BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, T *obj, Data::NamedClass<T> *cls);
		template <class T> static void BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::List<T*> *list, Data::NamedClass<T> *cls, const UTF8Char *clsName, UOSInt nameLen);
		static void Int32Join(NotNullPtr<Text::StringBuilderUTF8> sb, Data::List<Int32> *list, Text::CString seperator);
		static Bool IsNonASCII(const UTF8Char *s);
		static Bool IsASCIIText(const Data::ByteArrayR &buff);
		static Bool IsEmailAddress(const UTF8Char *s);
		static const UTF8Char *Null2Empty(const UTF8Char *s);
		static Bool SplitAsDouble(Text::CStringNN str, UTF8Char splitChar, NotNullPtr<Data::ArrayList<Double>> outArr);
		static UOSInt SplitAsNewString(Text::CStringNN str, UTF8Char splitChar, NotNullPtr<Data::ArrayListStringNN> outArr);
		static Bool Equals(Text::String *s1, Text::String *s2);
		static OSInt Compare(Text::String *s1, Text::String *s2);
		static OSInt CompareICase(Text::String *s1, Text::String *s2);
	};
}

template <class T> void Text::StringTool::BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, T *obj, Data::NamedClass<T> *cls)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sb->AppendUTF8Char('{');
	Bool found = false;
	UOSInt i = 0;
	UOSInt j = cls->GetFieldCount();
	while (i < j)
	{
		if (found)
		{
			sb->AppendUTF8Char(',');
		}
		found = true;
		sptr = Text::JSText::ToJSTextDQuote(sbuff, cls->GetFieldName(i));
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendUTF8Char(':');
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
	sb->AppendUTF8Char('}');
}

template <class T> void Text::StringTool::BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::List<T*> *list, Data::NamedClass<T> *cls, const UTF8Char *clsName, UOSInt nameLen)
{
	if (list == 0)
	{
		sb->AppendC(UTF8STRC("null"));
		return;
	}
	UOSInt i;
	UOSInt j;
	sb->AppendUTF8Char('[');
	i = 0;
	j = list->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
		}
		sb->AppendC(clsName, nameLen);
		BuildString(sb, list->GetItem(i), cls);
		i++;
	}
	sb->AppendUTF8Char(']');
}
#endif
