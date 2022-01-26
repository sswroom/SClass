#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Text/CharUtil.h"
#include "Text/CPPText.h"
#include "Text/MyStringW.h"
#include "Text/Cpp/CppReader.h"

void Text::CPPText::ToCPPString(Text::StringBuilderUTF8 *sb, const UTF8Char *str)
{
	UTF32Char c;
	sb->AppendChar('\"', 1);
	while (true)
	{
		str = Text::StrReadChar(str, &c);
		if (c == 0)
		{
			break;
		}
		if (c == '\r')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('r', 1);
		}
		else if (c == '\n')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('n', 1);
		}
		else if (c == '\f')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('f', 1);
		}
		else if (c == '\t')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('t', 1);
		}
		else if (c == '\'')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('\'', 1);
		}
		else if (c == '\"')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('\"', 1);
		}
		else if (c == '\\')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('\\', 1);
		}
		else if (c == '\?')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('\?', 1);
		}
		else if (c == '\a')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('a', 1);
		}
		else if (c == '\b')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('b', 1);
		}
		else if (c == '\v')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('v', 1);
		}
		else if (c < ' ')
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('X', 1);
			sb->AppendHex8((UInt8)c);
		}
		else if (c >= 0x80 && c <= 0xffff)
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('u', 1);
			sb->AppendHex16((UInt16)c);
		}
		else if (c >= 0x10000)
		{
			sb->AppendChar('\\', 1);
			sb->AppendChar('U', 1);
			sb->AppendHex32((UInt32)c);
		}
		else
		{
			sb->AppendChar(c, 1);
		}
	}
	sb->AppendChar('\"', 1);
}

void Text::CPPText::FromCPPString(Text::StringBuilderUTF8 *sb, const UTF8Char *str)
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
					sb->AppendChar('\'', 1);
				}
				else if (c == '\"')
				{
					sb->AppendChar('\"', 1);
				}
				else if (c == '\\')
				{
					sb->AppendChar('\\', 1);
				}
				else if (c == '?')
				{
					sb->AppendChar('\?', 1);
				}
				else if (c == 'a')
				{
					sb->AppendChar('\a', 1);
				}
				else if (c == 'b')
				{
					sb->AppendChar('\b', 1);
				}
				else if (c == 'f')
				{
					sb->AppendChar('\f', 1);
				}
				else if (c == 'n')
				{
					sb->AppendChar('\n', 1);
				}
				else if (c == 'r')
				{
					sb->AppendChar('\r', 1);
				}
				else if (c == 't')
				{
					sb->AppendChar('\t', 1);
				}
				else if (c == 'v')
				{
					sb->AppendChar('\v', 1);
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

Bool Text::CPPText::ParseEnum(Data::ArrayList<const UTF8Char*> *enumEntries, const UTF8Char *cppEnumStr, Text::StringBuilderUTF8 *sbPrefix)
{
	IO::MemoryStream mstm((UInt8*)cppEnumStr, Text::StrCharCnt(cppEnumStr), UTF8STRC("Text.CPPText.ParseEnum"));
	Text::Cpp::CppReader reader(&mstm);
	Text::StringBuilderUTF8 sb;
	if (!reader.NextWord(&sb))
	{
		return false;
	}
	if (sb.Equals(UTF8STRC("typedef")))
	{
		sb.ClearStr();
		if (!reader.NextWord(&sb))
		{
			return false;
		}
		if (!sb.Equals(UTF8STRC("enum")))
		{
			return false;
		}
		sb.ClearStr();
		if (!reader.NextWord(&sb))
		{
			return false;
		}
	}
	else if (sb.Equals(UTF8STRC("enum")))
	{
		sb.ClearStr();
		if (!reader.NextWord(&sb))
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
		if (!reader.NextWord(&sb))
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
		if (!reader.NextWord(&sb))
		{
			return false;
		}
		if (sb.Equals(UTF8STRC("}")))
		{
			return true;
		}
		else if (Text::CharUtil::IsAlphabet(sb.ToString()[0]))
		{
			enumEntries->Add(Text::StrCopyNew(sb.ToString()));
			sb.ClearStr();
			if (!reader.NextWord(&sb))
			{
				return false;
			}
			if (sb.Equals(UTF8STRC("=")))
			{
				sb.ClearStr();
				if (!reader.NextWord(&sb))
				{
					return false;
				}
				if (!Text::CharUtil::IsAlphaNumeric(sb.ToString()[0]))
				{
					return false;
				}
				sb.ClearStr();
				if (!reader.NextWord(&sb))
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
		return {UTF8STRC("Single")};
	case Data::VariItem::ItemType::F64:
		return {UTF8STRC("Double")};
	case Data::VariItem::ItemType::I8:
		return {UTF8STRC("Int8")};
	case Data::VariItem::ItemType::U8:
		return {UTF8STRC("UInt8")};
	case Data::VariItem::ItemType::I16:
		return {UTF8STRC("Int16")};
	case Data::VariItem::ItemType::U16:
		return {UTF8STRC("UInt16")};
	case Data::VariItem::ItemType::I32:
		return {UTF8STRC("Int32")};
	case Data::VariItem::ItemType::U32:
		return {UTF8STRC("UInt32")};
	case Data::VariItem::ItemType::I64:
		return {UTF8STRC("Int64")};
	case Data::VariItem::ItemType::U64:
		return {UTF8STRC("UInt64")};
	case Data::VariItem::ItemType::BOOL:
		return {UTF8STRC("Bool")};
	case Data::VariItem::ItemType::Str:
		return {UTF8STRC("Text::String*")};
	case Data::VariItem::ItemType::Date:
		return {UTF8STRC("Data::DateTime*")};
	case Data::VariItem::ItemType::ByteArr:
		return {UTF8STRC("Data::ReadonlyArr<UInt8>*")};
	case Data::VariItem::ItemType::Vector:
		return {UTF8STRC("Math::Vector2D*")};
	case Data::VariItem::ItemType::UUID:
		return {UTF8STRC("Data::UUID*")};
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Null:
	default:
		return {UTF8STRC("void*")};
	}
}
