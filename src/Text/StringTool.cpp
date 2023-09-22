#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/CharUtil.h"
#include "Text/JSText.h"
#include "Text/StringTool.h"

void Text::StringTool::BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Text::String *s)
{
	if (s == 0)
	{
		sb->AppendC(UTF8STRC("null"));
	}
	else
	{
		Text::JSText::ToJSTextDQuote(sb, s->v);
	}
}

void Text::StringTool::BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *s)
{
	if (s == 0)
	{
		sb->AppendC(UTF8STRC("null"));
	}
	else
	{
		Text::JSText::ToJSTextDQuote(sb, s);
	}
}

void Text::StringTool::BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::StringMap<Text::String*> *map)
{
	if (map == 0)
	{
		sb->AppendC(UTF8STRC("null"));
		return;
	}
	sb->AppendUTF8Char('{');
	NotNullPtr<Data::ArrayList<Text::String *>> keys = map->GetKeys();
	NotNullPtr<const Data::ArrayList<Text::String *>> vals = map->GetValues();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
			sb->AppendUTF8Char(' ');
		}
		BuildString(sb, keys->GetItem(i));
		sb->AppendUTF8Char(':');
		BuildString(sb, vals->GetItem(i));
		i++;
	}
	sb->AppendUTF8Char('}');
}

void Text::StringTool::BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::StringUTF8Map<Text::String*> *map)
{
	if (map == 0)
	{
		sb->AppendC(UTF8STRC("null"));
		return;
	}
	sb->AppendUTF8Char('{');
	NotNullPtr<Data::ArrayList<const UTF8Char *>> keys = map->GetKeys();
	NotNullPtr<const Data::ArrayList<Text::String *>> vals = map->GetValues();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
			sb->AppendUTF8Char(' ');
		}
		BuildString(sb, keys->GetItem(i));
		sb->AppendUTF8Char(':');
		BuildString(sb, vals->GetItem(i));
		i++;
	}
	sb->AppendUTF8Char('}');
}

void Text::StringTool::BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::StringUTF8Map<const UTF8Char*> *map)
{
	if (map == 0)
	{
		sb->AppendC(UTF8STRC("null"));
		return;
	}
	sb->AppendUTF8Char('{');
	NotNullPtr<Data::ArrayList<const UTF8Char *>> keys = map->GetKeys();
	NotNullPtr<const Data::ArrayList<const UTF8Char *>> vals = map->GetValues();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
			sb->AppendUTF8Char(' ');
		}
		BuildString(sb, keys->GetItem(i));
		sb->AppendUTF8Char(':');
		BuildString(sb, vals->GetItem(i));
		i++;
	}
	sb->AppendUTF8Char('}');
}

void Text::StringTool::BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::ReadingList<const UTF8Char*> *list)
{
	if (list == 0)
	{
		sb->AppendC(UTF8STRC("null"));
		return;
	}
	sb->AppendUTF8Char('[');
	UOSInt i = 0;
	UOSInt j = list->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
			sb->AppendUTF8Char(' ');
		}
		BuildString(sb, list->GetItem(i));
		i++;
	}
	sb->AppendUTF8Char(']');
}

void Text::StringTool::BuildString(NotNullPtr<Text::StringBuilderUTF8> sb, Data::ReadingList<Text::String*> *list)
{
	if (list == 0)
	{
		sb->AppendC(UTF8STRC("null"));
		return;
	}
	sb->AppendUTF8Char('[');
	UOSInt i = 0;
	UOSInt j = list->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
			sb->AppendUTF8Char(' ');
		}
		BuildString(sb, list->GetItem(i));
		i++;
	}
	sb->AppendUTF8Char(']');
}

void Text::StringTool::Int32Join(NotNullPtr<Text::StringBuilderUTF8> sb, Data::List<Int32> *list, Text::CString seperator)
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

Bool Text::StringTool::IsASCIIText(const Data::ByteArrayR &buff)
{
	UInt8 b;
	UOSInt index = 0;
	while (index < buff.GetSize())
	{
		b = buff[index++];
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

Bool Text::StringTool::SplitAsDouble(Text::CStringNN str, UTF8Char splitChar, NotNullPtr<Data::ArrayList<Double>> outArr)
{
	UTF8Char sbuff[128];
	Double v;
	UOSInt i = 0;
	UOSInt j;
	while (true)
	{
		j = str.IndexOf(splitChar, i);
		if (j == INVALID_INDEX)
		{
			if (!Text::StrToDouble(&str.v[i], v))
				return false;
			outArr->Add(v);
			return true;
		}
		if (j - i >= sizeof(sbuff))
		{
			return false;
		}
		Text::StrConcatC(sbuff, &str.v[i], j - i);
		if (!Text::StrToDouble(sbuff, v))
			return false;
		outArr->Add(v);
		i = j + 1;
	}
}

UOSInt Text::StringTool::SplitAsNewString(Text::CStringNN str, UTF8Char splitChar, NotNullPtr<Data::ArrayListNN<Text::String>> outArr)
{
	UOSInt initCnt = outArr->GetCount();
	UOSInt i = 0;
	UOSInt j;
	while (true)
	{
		j = str.IndexOf(splitChar, i);
		if (j == INVALID_INDEX)
		{
			outArr->Add(Text::String::New(str.Substring(i)));
			return outArr->GetCount() - initCnt;
		}
		outArr->Add(Text::String::New(&str.v[i], j - i));
		i = j + 1;
	}
}
