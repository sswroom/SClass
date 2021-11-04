#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/JavaText.h"

void Text::JavaText::ToJavaName(Text::StringBuilderUTF *sb, const UTF8Char *usName, Bool isClass)
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
				sb->AppendChar((UTF32Char)(c - 32), 1);
			}
			else
			{
				sb->AppendChar(c, 1);
			}
			isClass = false;
		}
		else
		{
			sb->AppendChar(c, 1);
		}
	}
}

const UTF8Char *Text::JavaText::GetJavaTypeName(DB::DBUtil::ColType colType, Bool notNull)
{
	switch (colType)
	{
	case DB::DBUtil::CT_Bool:
		if (notNull)
		{
			return (const UTF8Char*)"int";
		}
		else
		{
			return (const UTF8Char*)"Integer";
		}
	case DB::DBUtil::CT_Byte:
		if (notNull)
		{
			return (const UTF8Char*)"int";
		}
		else
		{
			return (const UTF8Char*)"Integer";
		}
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt16:
		if (notNull)
		{
			return (const UTF8Char*)"int";
		}
		else
		{
			return (const UTF8Char*)"Integer";
		}
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
		if (notNull)
		{
			return (const UTF8Char*)"int";
		}
		else
		{
			return (const UTF8Char*)"Integer";
		}
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_UInt64:
		if (notNull)
		{
			return (const UTF8Char*)"long";
		}
		else
		{
			return (const UTF8Char*)"Long";
		}
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		if (notNull)
		{
			return (const UTF8Char*)"double";
		}
		else
		{
			return (const UTF8Char*)"Double";
		}
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_UUID:
		return (const UTF8Char*)"String";
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
		return (const UTF8Char*)"Timestamp";
	case DB::DBUtil::CT_Binary:
		return (const UTF8Char*)"byte[]";
	case DB::DBUtil::CT_Vector:
		return (const UTF8Char*)"Geometry";
	case DB::DBUtil::CT_Unknown:
	default:
		return (const UTF8Char*)"?";
	}
}
