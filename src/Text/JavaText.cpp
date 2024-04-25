#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/JavaText.h"

void Text::JavaText::ToJavaName(NN<Text::StringBuilderUTF8> sb, const UTF8Char *usName, Bool isClass)
{
	UTF8Char c;
	while ((c = *usName++) != 0)
	{
		if (c == '_' || c == ' ' || c == '/')
		{
			isClass = true;
		}
		else if (isClass)
		{
			if (c >= 'a' && c <= 'z')
			{
				sb->AppendUTF8Char((UTF8Char)(c - 32));
			}
			else
			{
				sb->AppendUTF8Char(c);
			}
			isClass = false;
		}
		else
		{
			sb->AppendUTF8Char(c);
		}
	}
}

void Text::JavaText::ToDBName(NN<Text::StringBuilderUTF8> sb, const UTF8Char *name)
{
	UTF8Char c;
	Bool notFirst = false;
	while ((c = *name++) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			if (notFirst)
				sb->AppendUTF8Char('_');
			sb->AppendUTF8Char((UTF8Char)(c + 32));
		}
		else
		{
			sb->AppendUTF8Char(c);
		}
		notFirst = true;
	}
}

Text::CStringNN Text::JavaText::GetJavaTypeName(DB::DBUtil::ColType colType, Bool notNull)
{
	switch (colType)
	{
	case DB::DBUtil::CT_Bool:
		if (notNull)
		{
			return CSTR("boolean");
		}
		else
		{
			return CSTR("Boolean");
		}
	case DB::DBUtil::CT_Byte:
		if (notNull)
		{
			return CSTR("int");
		}
		else
		{
			return CSTR("Integer");
		}
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt16:
		if (notNull)
		{
			return CSTR("int");
		}
		else
		{
			return CSTR("Integer");
		}
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
		if (notNull)
		{
			return CSTR("int");
		}
		else
		{
			return CSTR("Integer");
		}
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_UInt64:
		if (notNull)
		{
			return CSTR("long");
		}
		else
		{
			return CSTR("Long");
		}
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
		if (notNull)
		{
			return CSTR("double");
		}
		else
		{
			return CSTR("Double");
		}
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		return CSTR("String");
	case DB::DBUtil::CT_Date:
		return CSTR("Date");
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTimeTZ:
		return CSTR("Timestamp");
	case DB::DBUtil::CT_Binary:
		return CSTR("byte[]");
	case DB::DBUtil::CT_Vector:
		return CSTR("Geometry");
	case DB::DBUtil::CT_Unknown:
	default:
		return CSTR("?");
	}
}

Text::CStringNN Text::JavaText::GetJavaTypeName(Data::VariItem::ItemType itemType, Bool notNull)
{
	switch (itemType)
	{
	case Data::VariItem::ItemType::BOOL:
		if (notNull)
		{
			return CSTR("boolean");
		}
		else
		{
			return CSTR("Boolean");
		}
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I8:
		if (notNull)
		{
			return CSTR("int");
		}
		else
		{
			return CSTR("Integer");
		}
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::I16:
		if (notNull)
		{
			return CSTR("int");
		}
		else
		{
			return CSTR("Integer");
		}
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::NI32:
		if (notNull)
		{
			return CSTR("int");
		}
		else
		{
			return CSTR("Integer");
		}
	case Data::VariItem::ItemType::U64:
	case Data::VariItem::ItemType::I64:
		if (notNull)
		{
			return CSTR("long");
		}
		else
		{
			return CSTR("Long");
		}
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
		if (notNull)
		{
			return CSTR("double");
		}
		else
		{
			return CSTR("Double");
		}
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::UUID:
		return CSTR("String");
	case Data::VariItem::ItemType::Date:
		return CSTR("Date");
	case Data::VariItem::ItemType::Timestamp:
		return CSTR("Timestamp");
	case Data::VariItem::ItemType::ByteArr:
		return CSTR("byte[]");
	case Data::VariItem::ItemType::Vector:
		return CSTR("Geometry");
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Unknown:
	default:
		return CSTR("?");
	}
}
