#include "Stdafx.h"
#include "IO/MemoryReadingStream.h"
#include "Text/CharUtil.h"
#include "Text/CPPText.h"
#include "Text/MyStringW.h"
#include "Text/Cpp/CppReader.h"

void Text::CPPText::ToCPPString(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *str)
{
	UTF32Char c;
	sb->AppendUTF8Char('\"');
	while (true)
	{
		str = Text::StrReadChar(str, &c);
		if (c == 0)
		{
			break;
		}
		if (c == '\r')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('r');
		}
		else if (c == '\n')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('n');
		}
		else if (c == '\f')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('f');
		}
		else if (c == '\t')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('t');
		}
		else if (c == '\'')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('\'');
		}
		else if (c == '\"')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('\"');
		}
		else if (c == '\\')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('\\');
		}
		else if (c == '\?')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('\?');
		}
		else if (c == '\a')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('a');
		}
		else if (c == '\b')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('b');
		}
		else if (c == '\v')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('v');
		}
		else if (c < ' ')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('X');
			sb->AppendHex8((UInt8)c);
		}
		else if (c >= 0x80 && c <= 0xffff)
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('u');
			sb->AppendHex16((UInt16)c);
		}
		else if (c >= 0x10000)
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('U');
			sb->AppendHex32((UInt32)c);
		}
		else
		{
			sb->AppendUTF8Char((UTF8Char)c);
		}
	}
	sb->AppendUTF8Char('\"');
}

void Text::CPPText::ToCPPString(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *str, UOSInt leng)
{
	const UTF8Char *strEnd = str + leng;
	UTF32Char c;
	sb->AppendUTF8Char('\"');
	while (str < strEnd)
	{
		c = *str;
		if (c & 0x80)
		{
			str = Text::StrReadChar(str, &c);
		}
		else
		{
			str++;
		}
		if (c == 0)
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('0');
		}
		else if (c == '\r')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('r');
		}
		else if (c == '\n')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('n');
		}
		else if (c == '\f')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('f');
		}
		else if (c == '\t')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('t');
		}
		else if (c == '\'')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('\'');
		}
		else if (c == '\"')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('\"');
		}
		else if (c == '\\')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('\\');
		}
		else if (c == '\?')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('\?');
		}
		else if (c == '\a')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('a');
		}
		else if (c == '\b')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('b');
		}
		else if (c == '\v')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('v');
		}
		else if (c < ' ')
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('X');
			sb->AppendHex8((UInt8)c);
		}
		else if (c >= 0x80 && c <= 0xffff)
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('u');
			sb->AppendHex16((UInt16)c);
		}
		else if (c >= 0x10000)
		{
			sb->AppendUTF8Char('\\');
			sb->AppendUTF8Char('U');
			sb->AppendHex32((UInt32)c);
		}
		else
		{
			sb->AppendUTF8Char((UTF8Char)c);
		}
	}
	sb->AppendUTF8Char('\"');
}

void Text::CPPText::FromCPPString(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *str)
{
	Bool quoted = false;
	UTF32Char c;
	while (true)
	{
		str = Text::StrReadChar(str, &c);
		if (c == 0)
		{
			break;
		}
		if (c == '\"')
		{
			quoted = !quoted;
		}
		else if (quoted)
		{
			if (c == '\\')
			{
				str = Text::StrReadChar(str, &c);
				if (c == 0)
				{
					break;
				}
				if (c == '\'')
				{
					sb->AppendUTF8Char('\'');
				}
				else if (c == '\"')
				{
					sb->AppendUTF8Char('\"');
				}
				else if (c == '\\')
				{
					sb->AppendUTF8Char('\\');
				}
				else if (c == '?')
				{
					sb->AppendUTF8Char('\?');
				}
				else if (c == 'a')
				{
					sb->AppendUTF8Char('\a');
				}
				else if (c == 'b')
				{
					sb->AppendUTF8Char('\b');
				}
				else if (c == 'f')
				{
					sb->AppendUTF8Char('\f');
				}
				else if (c == 'n')
				{
					sb->AppendUTF8Char('\n');
				}
				else if (c == 'r')
				{
					sb->AppendUTF8Char('\r');
				}
				else if (c == 't')
				{
					sb->AppendUTF8Char('\t');
				}
				else if (c == 'v')
				{
					sb->AppendUTF8Char('\v');
				}
				else if (c == 'X')
				{
					sb->AppendChar(Text::StrHex2UInt8C(str), 1);
					str += 2;
				}
				else if (c == 'u')
				{
					sb->AppendChar((UInt16)Text::StrHex2Int16C(str), 1);
					str += 4;
				}
				else if (c == 'U')
				{
					sb->AppendChar((UTF32Char)Text::StrHex2Int32C(str), 1);
					str += 8;
				}
				else
				{
					sb->AppendChar(c, 1);
				}
			}
			else
			{
				sb->AppendChar(c, 1);
			}
		}
	}
}

Bool Text::CPPText::ParseEnum(Data::ArrayListNN<Text::String> *enumEntries, Text::CString cppEnumStr, NotNullPtr<Text::StringBuilderUTF8> sbPrefix)
{
	IO::MemoryReadingStream mstm(cppEnumStr.v, cppEnumStr.leng);
	Text::Cpp::CppReader reader(mstm);
	Text::StringBuilderUTF8 sb;
	if (!reader.NextWord(sb))
	{
		return false;
	}
	if (sb.Equals(UTF8STRC("typedef")))
	{
		sb.ClearStr();
		if (!reader.NextWord(sb))
		{
			return false;
		}
		if (!sb.Equals(UTF8STRC("enum")))
		{
			return false;
		}
		sb.ClearStr();
		if (!reader.NextWord(sb))
		{
			return false;
		}
	}
	else if (sb.Equals(UTF8STRC("enum")))
	{
		sb.ClearStr();
		if (!reader.NextWord(sb))
		{
			return false;
		}
		if (sb.Equals(UTF8STRC("class")))
		{
			if (!reader.NextWord(sbPrefix))
			{
				return false;
			}
			sbPrefix->AppendC(UTF8STRC("::"));
		}
		sb.ClearStr();
		if (!reader.NextWord(sb))
		{
			return false;
		}
	}

	if (!sb.Equals(UTF8STRC("{")))
	{
		return false;
	}
	while (true)
	{
		sb.ClearStr();
		if (!reader.NextWord(sb))
		{
			return false;
		}
		if (sb.Equals(UTF8STRC("}")))
		{
			return true;
		}
		else if (sb.v[0] == '_' || Text::CharUtil::IsAlphabet(sb.ToString()[0]))
		{
			enumEntries->Add(Text::String::New(sb.ToCString()));
			sb.ClearStr();
			if (!reader.NextWord(sb))
			{
				return false;
			}
			if (sb.Equals(UTF8STRC("=")))
			{
				sb.ClearStr();
				if (!reader.NextWord(sb))
				{
					return false;
				}
				if (!Text::CharUtil::IsAlphaNumeric(sb.ToString()[0]))
				{
					return false;
				}
				sb.ClearStr();
				if (!reader.NextWord(sb))
				{
					return false;
				}
			}
			if (sb.Equals(UTF8STRC("}")))
			{
				return true;
			}
			else if (!sb.Equals(UTF8STRC(",")))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}

Text::CString Text::CPPText::GetCppType(Data::VariItem::ItemType itemType)
{
	switch (itemType)
	{
	case Data::VariItem::ItemType::F32:
		return CSTR("Single");
	case Data::VariItem::ItemType::F64:
		return CSTR("Double");
	case Data::VariItem::ItemType::I8:
		return CSTR("Int8");
	case Data::VariItem::ItemType::U8:
		return CSTR("UInt8");
	case Data::VariItem::ItemType::I16:
		return CSTR("Int16");
	case Data::VariItem::ItemType::U16:
		return CSTR("UInt16");
	case Data::VariItem::ItemType::I32:
		return CSTR("Int32");
	case Data::VariItem::ItemType::U32:
		return CSTR("UInt32");
	case Data::VariItem::ItemType::I64:
		return CSTR("Int64");
	case Data::VariItem::ItemType::U64:
		return CSTR("UInt64");
	case Data::VariItem::ItemType::BOOL:
		return CSTR("Bool");
	case Data::VariItem::ItemType::Str:
		return CSTR("Text::String*");
	case Data::VariItem::ItemType::CStr:
		return CSTR("Text::CString");
	case Data::VariItem::ItemType::Timestamp:
		return CSTR("Data::Timestamp");
	case Data::VariItem::ItemType::Date:
		return CSTR("Data::Date");
	case Data::VariItem::ItemType::ByteArr:
		return CSTR("Data::ReadonlyArr<UInt8>*");
	case Data::VariItem::ItemType::Vector:
		return CSTR("Math::Geometry::Vector2D*");
	case Data::VariItem::ItemType::UUID:
		return CSTR("Data::UUID*");
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Null:
	default:
		return CSTR("void*");
	}
}
