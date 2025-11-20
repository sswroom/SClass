#ifndef _SM_TEXT_STRINGTOOL
#define _SM_TEXT_STRINGTOOL
#include "Data/ArrayListStringNN.h"
#include "Data/ByteArray.h"
#include "Data/NamedClass.hpp"
#include "Data/StringMap.hpp"
#include "Data/StringUTF8Map.hpp"
#include "Math/Size2D.h"
#include "Text/CString.h"
#include "Text/JSText.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class StringTool
	{
	public:
		static void BuildJSONString(NN<Text::StringBuilderUTF8> sb, Optional<Text::String> s);
		static void BuildJSONString(NN<Text::StringBuilderUTF8> sb, Data::StringMap<Text::String*> *map);
		static void BuildJSONString(NN<Text::StringBuilderUTF8> sb, Data::ReadingList<Text::String*> *list);
		template <class T> static void BuildJSONString(NN<Text::StringBuilderUTF8> sb, T *obj, NN<Data::NamedClass<T>> cls);
		template <class T> static void BuildJSONString(NN<Text::StringBuilderUTF8> sb, Data::List<T*> *list, NN<Data::NamedClass<T>> cls, UnsafeArray<const UTF8Char> clsName, UOSInt nameLen);
		static void Int32Join(NN<Text::StringBuilderUTF8> sb, NN<const Data::List<Int32>> list, Text::CStringNN seperator);
		static Bool IsNonASCII(UnsafeArray<const UTF8Char> s);
		static Bool IsASCIIText(const Data::ByteArrayR &buff);
		static Bool IsEmailAddress(UnsafeArray<const UTF8Char> s);
		static Bool IsUInteger(UnsafeArray<const UTF8Char> s);
		static Bool IsInteger(UnsafeArray<const UTF8Char> s);
		static Bool IsHKID(Text::CStringNN hkid);
		static UnsafeArray<const UTF8Char> Null2Empty(UnsafeArrayOpt<const UTF8Char> s);
		static Bool SplitAsDouble(Text::CStringNN str, UTF8Char splitChar, NN<Data::ArrayList<Double>> outArr);
		static UOSInt SplitAsNewString(Text::CStringNN str, UTF8Char splitChar, NN<Data::ArrayListStringNN> outArr);
		static Bool Equals(Optional<Text::String> s1, Optional<Text::String> s2);
		static OSInt Compare(Optional<Text::String> s1, Optional<Text::String> s2);
		static OSInt CompareICase(Optional<Text::String> s1, Optional<Text::String> s2);
		static Math::Size2D<UOSInt> GetMonospaceSize(UnsafeArray<const UTF8Char> s);
		static void RemoveEmptyOrWS(NN<Data::ArrayListStringNN> strList);
	};
}

template <class T> void Text::StringTool::BuildJSONString(NN<Text::StringBuilderUTF8> sb, T *obj, NN<Data::NamedClass<T>> cls)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
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
		NN<Data::VariItem> item;
		if (cls->GetNewValue(i, obj).SetTo(item))
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

template <class T> void Text::StringTool::BuildJSONString(NN<Text::StringBuilderUTF8> sb, Data::List<T*> *list, NN<Data::NamedClass<T>> cls, UnsafeArray<const UTF8Char> clsName, UOSInt nameLen)
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
		BuildJSONString(sb, list->GetItem(i), cls);
		i++;
	}
	sb->AppendUTF8Char(']');
}
#endif
