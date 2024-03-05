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
	const UTF8Char *dotPtr = 0;
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
				dotPtr = s;
			}
		}
		else if (c == '@')
		{
			if (atPos != INVALID_INDEX)
			{
				return false;
			}
			atPos = (UOSInt)(s - startPtr - 1);
			dotPtr = 0;

		}
		else
		{
			return false;
		}
	}
	if (atPos == INVALID_INDEX || atPos == 0 || dotPtr == 0 || (s - dotPtr) < 3)
	{
		return false;
	}
	return true;
}

Bool Text::StringTool::IsUInteger(const UTF8Char *s)
{
	UTF8Char c;
	while ((c = *s++) != 0)
	{
		if (!Text::CharUtil::IsDigit(c))
			return false;
	}
	return true;
}

Bool Text::StringTool::IsInteger(const UTF8Char *s)
{
	if (*s == '-')
		s++;
	return IsUInteger(s);
}

Bool Text::StringTool::IsHKID(Text::CStringNN hkid)
{
	UTF8Char sbuff[9];
	UOSInt idLen;
	UTF8Char chk;
	UOSInt ichk;
	if (hkid.EndsWith(')'))
	{
		if (hkid.leng == 10)
		{
			if (hkid.v[7] == '(')
			{
				Text::StrConcatC(sbuff, hkid.v, 7);
				idLen = 7;
				chk = hkid.v[8];
			}
			else
			{
				return false;
			}
		}
		else if (hkid.leng == 11)
		{
			if (hkid.v[8] == '(')
			{
				Text::StrConcatC(sbuff, hkid.v, 8);
				idLen = 8;
				chk = hkid.v[9];
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (hkid.leng == 8)
		{
			Text::StrConcatC(sbuff, hkid.v, 7);
			idLen = 7;
			chk = hkid.v[7];
		}
		else if (hkid.leng == 9)
		{
			Text::StrConcatC(sbuff, hkid.v, 8);
			idLen = 8;
			chk = hkid.v[8];
		}
		else
		{
			return false;
		}
	}

	if (Text::CharUtil::IsDigit(chk))
		ichk = chk - 0x30;
	else if (chk == 'A')
		ichk = 10;
	else
		return false;

	UOSInt thisChk;
	if (idLen == 8)
	{
		if (!Text::CharUtil::IsUpperCase(sbuff[0]) ||
			!Text::CharUtil::IsUpperCase(sbuff[1]) ||
			!Text::CharUtil::IsDigit(sbuff[2]) ||
			!Text::CharUtil::IsDigit(sbuff[3]) ||
			!Text::CharUtil::IsDigit(sbuff[4]) ||
			!Text::CharUtil::IsDigit(sbuff[5]) ||
			!Text::CharUtil::IsDigit(sbuff[6]) ||
			!Text::CharUtil::IsDigit(sbuff[7]))
				return false;
		
		thisChk = 0;
		thisChk += (UOSInt)(sbuff[0] - 'A' + 10) * 9;
		thisChk += (UOSInt)(sbuff[1] - 'A' + 10) * 8;
		thisChk += (UOSInt)(sbuff[2] - '0') * 7;
		thisChk += (UOSInt)(sbuff[3] - '0') * 6;
		thisChk += (UOSInt)(sbuff[4] - '0') * 5;
		thisChk += (UOSInt)(sbuff[5] - '0') * 4;
		thisChk += (UOSInt)(sbuff[6] - '0') * 3;
		thisChk += (UOSInt)(sbuff[7] - '0') * 2;
		if (ichk != (thisChk % 11))
			return false;
		return true;
	}
	else
	{
		if (!Text::CharUtil::IsUpperCase(sbuff[0]) ||
			!Text::CharUtil::IsDigit(sbuff[1]) ||
			!Text::CharUtil::IsDigit(sbuff[2]) ||
			!Text::CharUtil::IsDigit(sbuff[3]) ||
			!Text::CharUtil::IsDigit(sbuff[4]) ||
			!Text::CharUtil::IsDigit(sbuff[5]) ||
			!Text::CharUtil::IsDigit(sbuff[6]))
				return false;

		thisChk = 36 * 9;
		thisChk += (UOSInt)(sbuff[0] - 'A' + 10) * 8;
		thisChk += (UOSInt)(sbuff[1] - '0') * 7;
		thisChk += (UOSInt)(sbuff[2] - '0') * 6;
		thisChk += (UOSInt)(sbuff[3] - '0') * 5;
		thisChk += (UOSInt)(sbuff[4] - '0') * 4;
		thisChk += (UOSInt)(sbuff[5] - '0') * 3;
		thisChk += (UOSInt)(sbuff[6] - '0') * 2;
		if (ichk != (thisChk % 11))
			return false;
		return true;
	}
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

UOSInt Text::StringTool::SplitAsNewString(Text::CStringNN str, UTF8Char splitChar, NotNullPtr<Data::ArrayListStringNN> outArr)
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

Bool Text::StringTool::Equals(Optional<Text::String> s1, Optional<Text::String> s2)
{
	if (s1 == s2)
		return true;
	NotNullPtr<Text::String> str2;
	NotNullPtr<Text::String> str1;
	if (!s1.SetTo(str1))
		return false;
	if (!s2.SetTo(str2))
		return false;
	return str1->Equals(str2);
}

OSInt Text::StringTool::Compare(Text::String *s1, Text::String *s2)
{
	if (s1 == s2)
		return 0;
	NotNullPtr<Text::String> str1;
	NotNullPtr<Text::String> str2;
	if (!str1.Set(s1))
		return -1;
	if (!str2.Set(s2))
		return 1;
	return str1->CompareTo(str2);
}

OSInt Text::StringTool::CompareICase(Text::String *s1, Text::String *s2)
{
	if (s1 == s2)
		return 0;
	NotNullPtr<Text::String> str1;
	NotNullPtr<Text::String> str2;
	if (!str1.Set(s1))
		return -1;
	if (!str2.Set(s2))
		return 1;
	return str1->CompareToICase(str2);
}
