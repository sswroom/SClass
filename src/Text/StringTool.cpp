#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/CharUtil.h"
#include "Text/JSText.h"
#include "Text/StringTool.h"

//#define VERBOSE

void Text::StringTool::BuildJSONString(NN<Text::StringBuilderUTF8> sb, Optional<Text::String> s)
{
	NN<Text::String> nns;
	if (!s.SetTo(nns))
	{
		sb->AppendC(UTF8STRC("null"));
	}
	else
	{
		Text::JSText::ToJSTextDQuote(sb, nns->v);
	}
}

void Text::StringTool::BuildJSONString(NN<Text::StringBuilderUTF8> sb, Data::StringMap<Text::String*> *map)
{
	if (map == 0)
	{
		sb->AppendC(UTF8STRC("null"));
		return;
	}
	sb->AppendUTF8Char('{');
	NN<Data::ArrayList<Text::String *>> keys = map->GetKeys();
	NN<const Data::ArrayList<Text::String *>> vals = map->GetValues();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
			sb->AppendUTF8Char(' ');
		}
		BuildJSONString(sb, keys->GetItem(i));
		sb->AppendUTF8Char(':');
		BuildJSONString(sb, vals->GetItem(i));
		i++;
	}
	sb->AppendUTF8Char('}');
}

void Text::StringTool::BuildJSONString(NN<Text::StringBuilderUTF8> sb, Data::ReadingList<Text::String*> *list)
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
		BuildJSONString(sb, list->GetItem(i));
		i++;
	}
	sb->AppendUTF8Char(']');
}

void Text::StringTool::Int32Join(NN<Text::StringBuilderUTF8> sb, Data::List<Int32> *list, Text::CStringNN seperator)
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

Bool Text::StringTool::IsNonASCII(UnsafeArray<const UTF8Char> s)
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

Bool Text::StringTool::IsEmailAddress(UnsafeArray<const UTF8Char> s)
{
	UOSInt atPos = INVALID_INDEX;
	UnsafeArrayOpt<const UTF8Char> dotPtr = 0;
	UnsafeArray<const UTF8Char> nndotPtr;
	UnsafeArray<const UTF8Char> startPtr = s;
	UTF8Char c;
	while ((c = *s++) != 0)
	{
		if (Text::CharUtil::IsAlphaNumeric(c) || c == '-' || c == '_')
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
	if (atPos == INVALID_INDEX || atPos == 0 || !dotPtr.SetTo(nndotPtr) || (s - nndotPtr) < 3)
	{
		return false;
	}
	return true;
}

Bool Text::StringTool::IsUInteger(UnsafeArray<const UTF8Char> s)
{
	UTF8Char c;
	while ((c = *s++) != 0)
	{
		if (!Text::CharUtil::IsDigit(c))
			return false;
	}
	return true;
}

Bool Text::StringTool::IsInteger(UnsafeArray<const UTF8Char> s)
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
		thisChk += ichk;
		if ((thisChk % 11) != 0)
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
#if defined(VERBOSE)
		printf("%c: %d * 9 = %d\r\n", ' ', 36, 36 * 9);
		printf("%c: %d * 8 = %d\r\n", sbuff[0], sbuff[0] - 'A' + 10, (sbuff[0] - 'A' + 10) * 8);
		printf("%c: %d * 7 = %d\r\n", sbuff[1], sbuff[1] - '0', (sbuff[1] - '0') * 7);
		printf("%c: %d * 6 = %d\r\n", sbuff[2], sbuff[2] - '0', (sbuff[2] - '0') * 6);
		printf("%c: %d * 5 = %d\r\n", sbuff[3], sbuff[3] - '0', (sbuff[3] - '0') * 5);
		printf("%c: %d * 4 = %d\r\n", sbuff[4], sbuff[4] - '0', (sbuff[4] - '0') * 4);
		printf("%c: %d * 3 = %d\r\n", sbuff[5], sbuff[5] - '0', (sbuff[5] - '0') * 3);
		printf("%c: %d * 2 = %d\r\n", sbuff[6], sbuff[6] - '0', (sbuff[6] - '0') * 2);
		printf("Total = %d, Mod = %d, Check = %d\r\n", (UInt32)thisChk, (UInt32)(thisChk % 11), (UInt32)ichk);
#endif
		thisChk += ichk;
		if ((thisChk % 11) != 0)
			return false;
		return true;
	}
}

UnsafeArray<const UTF8Char> Text::StringTool::Null2Empty(UnsafeArrayOpt<const UTF8Char> s)
{
	UnsafeArray<const UTF8Char> nns;
	if (s.SetTo(nns)) return nns;
	return U8STR("");
}

Bool Text::StringTool::SplitAsDouble(Text::CStringNN str, UTF8Char splitChar, NN<Data::ArrayList<Double>> outArr)
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

UOSInt Text::StringTool::SplitAsNewString(Text::CStringNN str, UTF8Char splitChar, NN<Data::ArrayListStringNN> outArr)
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
	NN<Text::String> str2;
	NN<Text::String> str1;
	if (!s1.SetTo(str1))
		return false;
	if (!s2.SetTo(str2))
		return false;
	return str1->Equals(str2);
}

OSInt Text::StringTool::Compare(Optional<Text::String> s1, Optional<Text::String> s2)
{
	if (s1 == s2)
		return 0;
	NN<Text::String> str1;
	NN<Text::String> str2;
	if (!s1.SetTo(str1))
		return -1;
	if (!s2.SetTo(str2))
		return 1;
	return str1->CompareTo(str2);
}

OSInt Text::StringTool::CompareICase(Optional<Text::String> s1, Optional<Text::String> s2)
{
	if (s1 == s2)
		return 0;
	NN<Text::String> str1;
	NN<Text::String> str2;
	if (!s1.SetTo(str1))
		return -1;
	if (!s2.SetTo(str2))
		return 1;
	return str1->CompareToICase(str2);
}
