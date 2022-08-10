#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/JavaText.h"

void Text::JavaText::ToJavaName(Text::StringBuilderUTF8 *sb, const UTF8Char *usName, Bool isClass)
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

Text::CString Text::JavaText::GetJavaTypeName(DB::DBUtil::ColType colType, Bool notNull)
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
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_Date:
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
