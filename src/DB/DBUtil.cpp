#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "DB/DBUtil.h"
#include "Math/Math.h"
#include "Math/WKTWriter.h"
#include "Math/Geometry/Point.h"
#include "Text/CharUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

UTF8Char *DB::DBUtil::SDBStrUTF8(UTF8Char *sqlstr, const UTF8Char *val, SQLType sqlType)
{
	UTF8Char *sptr;
	UTF8Char c;
	if (val == 0)
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));

	if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		sptr = sqlstr;
		*sptr++ = '\'';
		while ((c = *val++) != 0)
		{
			if (c == '\\')
			{
				*sptr++ = '\\';
				*sptr++ = '\\';
			}
			else if (c == '\'')
			{
				*sptr++ = '\\';
				*sptr++ = '\'';
			}
			else if (c == '\"')
			{
				*sptr++ = '\\';
				*sptr++ = '"';
			}
			else if (c == '\r')
			{
				*sptr++ = '\\';
				*sptr++ = 'r';
			}
			else if (c == '\n')
			{
				*sptr++ = '\\';
				*sptr++ = 'n';
			}
			else if (c == '\t')
			{
				*sptr++ = '\\';
				*sptr++ = 't';
			}
			else if (c == '\b')
			{
				*sptr++ = '\\';
				*sptr++ = 'b';
			}
			else if (c == 26)
			{
				*sptr++ = '\\';
				*sptr++ = 'Z';
			}
			else if ((c & 0x80) == 0)
			{
				*sptr++ = c;
			}
			else if ((c & 0xe0) == 0xc0)
			{
				*sptr++ = c;
				*sptr++ = *val++;
			}
			else if ((c & 0xf0) == 0xe0)
			{
				*sptr++ = c;
				*sptr++ = *val++;
				*sptr++ = *val++;
			}
			else if ((c & 0xf8) == 0xf0)
			{
				*sptr++ = '?';
				val += 3;
			}
			else if ((c & 0xfc) == 0xf8)
			{
				*sptr++ = '?';
				val += 4;
			}
			else if ((c & 0xfe) == 0xfc)
			{
				*sptr++ = '?';
				val += 5;
			}
			else
			{
				*sptr++ = '?';
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::SQLite)
	{
		sptr = sqlstr;
		*sptr++ = '\'';
		while ((c = *val++) != 0)
		{
			if (c == '\\')
			{
				*sptr++ = '\\';
				*sptr++ = '\\';
			}
			else if (c == '\'')
			{
				*sptr++ = '\\';
				*sptr++ = '\'';
			}
			else if (c == '\"')
			{
				*sptr++ = '\\';
				*sptr++ = '"';
			}
			else if (c == '\r')
			{
				*sptr++ = '\\';
				*sptr++ = 'r';
			}
			else if (c == '\n')
			{
				*sptr++ = '\\';
				*sptr++ = 'n';
			}
			else if (c == '\t')
			{
				*sptr++ = '\\';
				*sptr++ = 't';
			}
			else if ((c & 0x80) == 0)
			{
				*sptr++ = c;
			}
			else
			{
				if ((c & 0xe0) == 0xc0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
				}
				else if ((c & 0xf0) == 0xe0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xf8) == 0xf0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xfc) == 0xf8)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xfe) == 0xfc)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else
				{
					*sptr++ = '?';
				}
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		sptr = sqlstr;
		*sptr++ = 'N';
		*sptr++ = '\'';
		while ((c = *val++) != 0)
		{
			if (c == '\'')
			{
				*sptr++ = '\\';
				*sptr++ = '\'';
			}
			else if ((c & 0x80) == 0)
			{
				*sptr++ = c;
			}
			else
			{
				if ((c & 0xe0) == 0xc0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
				}
				else if ((c & 0xf0) == 0xe0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xf8) == 0xf0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xfc) == 0xf8)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xfe) == 0xfc)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else
				{
					*sptr++ = '?';
				}
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::Access)
	{
		sptr = sqlstr;
		*sptr++ = '\'';
		while ((c = *val++) != 0)
		{
			if (c == '\'')
			{
				*sptr++ = '\'';
				*sptr++ = '\'';
			}
			else if ((c & 0x80) == 0)
			{
				*sptr++ = c;
			}
			else
			{
				if ((c & 0xe0) == 0xc0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
				}
				else if ((c & 0xf0) == 0xe0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xf8) == 0xf0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xfc) == 0xf8)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xfe) == 0xfc)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else
				{
					*sptr++ = '?';
				}
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::PostgreSQL)
	{
		sptr = sqlstr;
		*sptr++ = '\'';
		while ((c = *val++) != 0)
		{
			if (c == '\'')
			{
				*sptr++ = '\'';
				*sptr++ = '\'';
			}
			else if ((c & 0x80) == 0)
			{
				*sptr++ = c;
			}
			else
			{
				if ((c & 0xe0) == 0xc0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
				}
				else if ((c & 0xf0) == 0xe0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xf8) == 0xf0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xfc) == 0xf8)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xfe) == 0xfc)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else
				{
					*sptr++ = '?';
				}
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;		
	}
	else
	{
		sptr = sqlstr;
		*sptr++ = '\'';
		while ((c = *val++) != 0)
		{
			if (c == '\'')
			{
				*sptr++ = '\\';
				*sptr++ = '\'';
			}
			else if ((c & 0x80) == 0)
			{
				*sptr++ = c;
			}
			else
			{
				if ((c & 0xe0) == 0xc0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
				}
				else if ((c & 0xf0) == 0xe0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xf8) == 0xf0)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xfc) == 0xf8)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else if ((c & 0xfe) == 0xfc)
				{
					*sptr++ = c;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
					*sptr++ = *val++;
				}
				else
				{
					*sptr++ = '?';
				}
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
}

UOSInt DB::DBUtil::SDBStrUTF8Leng(const UTF8Char *val, DB::DBUtil::SQLType sqlType)
{
	UOSInt leng = 0;
	UTF8Char c;
	if (val == 0)
		return 4;

	if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		while ((c = *val++) != 0)
		{
			if (c == '\\')
			{
				leng += 2;
			}
			else if (c == '\'')
			{
				leng += 2;
			}
			else if (c == '\"')
			{
				leng += 2;
			}
			else if (c == '\r')
			{
				leng += 2;
			}
			else if (c == '\n')
			{
				leng += 2;
			}
			else if (c == '\t')
			{
				leng += 2;
			}
			else if (c == '\b')
			{
				leng += 2;
			}
			else if (c == 26)
			{
				leng += 2;
			}
			else if ((c & 0x80) == 0)
			{
				leng += 1;
			}
			else if ((c & 0xe0) == 0xc0)
			{
				leng += 2;
				val += 1;
			}
			else if ((c & 0xf0) == 0xe0)
			{
				leng += 3;
				val += 2;
			}
			else if ((c & 0xf8) == 0xf0)
			{
				leng += 1;
				val += 3;
			}
			else if ((c & 0xfc) == 0xf8)
			{
				leng += 1;
				val += 4;
			}
			else if ((c & 0xfe) == 0xfc)
			{
				leng += 1;
				val += 5;
			}
			else
			{
				leng += 1;
			}
		}
		leng += 2;
		return leng;
	}
	else if (sqlType == DB::DBUtil::SQLType::SQLite)
	{
		while ((c = *val++) != 0)
		{
			if (c == '\\')
			{
				leng += 2;
			}
			else if (c == '\'')
			{
				leng += 2;
			}
			else if (c == '\"')
			{
				leng += 2;
			}
			else if (c == '\r')
			{
				leng += 2;
			}
			else if (c == '\n')
			{
				leng += 2;
			}
			else if (c == '\t')
			{
				leng += 2;
			}
			else if ((c & 0x80) == 0)
			{
				leng += 1;
			}
			else
			{
				if ((c & 0xe0) == 0xc0)
				{
					val += 1;
					leng += 2;
				}
				else if ((c & 0xf0) == 0xe0)
				{
					val += 2;
					leng += 3;
				}
				else if ((c & 0xf8) == 0xf0)
				{
					val += 3;
					leng += 4;
				}
				else if ((c & 0xfc) == 0xf8)
				{
					val += 4;
					leng += 5;
				}
				else if ((c & 0xfe) == 0xfc)
				{
					val += 5;
					leng += 6;
				}
				else
				{
					leng += 1;
				}
			}
		}
		leng += 2;
		return leng;
	}
	else if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		while ((c = *val++) != 0)
		{
			if (c == '\'')
			{
				leng += 2;
			}
			else if ((c & 0x80) == 0)
			{
				leng += 1;
			}
			else
			{
				if ((c & 0xe0) == 0xc0)
				{
					val += 1;
					leng += 2;
				}
				else if ((c & 0xf0) == 0xe0)
				{
					val += 2;
					leng += 3;
				}
				else if ((c & 0xf8) == 0xf0)
				{
					val += 3;
					leng += 4;
				}
				else if ((c & 0xfc) == 0xf8)
				{
					val += 4;
					leng += 5;
				}
				else if ((c & 0xfe) == 0xfc)
				{
					val += 5;
					leng += 6;
				}
				else
				{
					leng += 1;
				}
			}
		}
		leng += 3;
		return leng;
	}
	else if (sqlType == DB::DBUtil::SQLType::Access)
	{
		while ((c = *val++) != 0)
		{
			if (c == '\'')
			{
				leng += 2;
			}
			else if ((c & 0x80) == 0)
			{
				leng += 1;
			}
			else
			{
				if ((c & 0xe0) == 0xc0)
				{
					val += 1;
					leng += 2;
				}
				else if ((c & 0xf0) == 0xe0)
				{
					val += 2;
					leng += 3;
				}
				else if ((c & 0xf8) == 0xf0)
				{
					val += 3;
					leng += 4;
				}
				else if ((c & 0xfc) == 0xf8)
				{
					val += 4;
					leng += 5;
				}
				else if ((c & 0xfe) == 0xfc)
				{
					val += 5;
					leng += 6;
				}
				else
				{
					leng += 1;
				}
			}
		}
		leng += 3;
		return leng;
	}
	else if (sqlType == DB::DBUtil::SQLType::PostgreSQL)
	{
		while ((c = *val++) != 0)
		{
			if (c == '\'')
			{
				leng += 2;
			}
			else if ((c & 0x80) == 0)
			{
				leng += 1;
			}
			else
			{
				if ((c & 0xe0) == 0xc0)
				{
					val += 1;
					leng += 2;
				}
				else if ((c & 0xf0) == 0xe0)
				{
					val += 2;
					leng += 3;
				}
				else if ((c & 0xf8) == 0xf0)
				{
					val += 3;
					leng += 4;
				}
				else if ((c & 0xfc) == 0xf8)
				{
					val += 4;
					leng += 5;
				}
				else if ((c & 0xfe) == 0xfc)
				{
					val += 5;
					leng += 6;
				}
				else
				{
					leng += 1;
				}
			}
		}
		leng += 2;
		return leng;
	}
	else
	{
		while ((c = *val++) != 0)
		{
			if (c == '\'')
			{
				leng += 2;
			}
			else if ((c & 0x80) == 0)
			{
				leng += 1;
			}
			else
			{
				if ((c & 0xe0) == 0xc0)
				{
					val += 1;
					leng += 2;
				}
				else if ((c & 0xf0) == 0xe0)
				{
					val += 2;
					leng += 3;
				}
				else if ((c & 0xf8) == 0xf0)
				{
					val += 3;
					leng += 4;
				}
				else if ((c & 0xfc) == 0xf8)
				{
					val += 4;
					leng += 5;
				}
				else if ((c & 0xfe) == 0xfc)
				{
					val += 5;
					leng += 6;
				}
				else
				{
					leng += 1;
				}
			}
		}
		leng += 2;
		return leng;
	}
}

UTF8Char *DB::DBUtil::SDBStrW(UTF8Char *sqlstr, const WChar *val, DB::DBUtil::SQLType sqlType)
{
	UTF8Char *sptr;
	UTF32Char c;
	if (val == 0)
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));

	if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		sptr = sqlstr;
		*sptr++ = '\'';
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\\')
			{
				*sptr++ = '\\';
				*sptr++ = '\\';
			}
			else if (c == '\'')
			{
				*sptr++ = '\\';
				*sptr++ = '\'';
			}
			else if (c == '\"')
			{
				*sptr++ = '\\';
				*sptr++ = '"';
			}
			else if (c == '\r')
			{
				*sptr++ = '\\';
				*sptr++ = 'r';
			}
			else if (c == '\n')
			{
				*sptr++ = '\\';
				*sptr++ = 'n';
			}
			else if (c == '\t')
			{
				*sptr++ = '\\';
				*sptr++ = 't';
			}
			else if (c == '\b')
			{
				*sptr++ = '\\';
				*sptr++ = 'b';
			}
			else if (c == 26)
			{
				*sptr++ = '\\';
				*sptr++ = 'Z';
			}
			else
			{
				if (c >= 0x200000)
				{
					*sptr++ = '?';
				}
				else
				{
					sptr = Text::StrWriteChar(sptr, c);
				}
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::SQLite)
	{
		sptr = sqlstr;
		*sptr++ = '\'';
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\'')
			{
				*sptr++ = '\'';
				*sptr++ = '\'';
			}
			else
			{
				sptr = Text::StrWriteChar(sptr, c);
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		sptr = sqlstr;
		*sptr++ = 'N';
		*sptr++ = '\'';
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\'')
			{
				*sptr++ = '\\';
				*sptr++ = '\'';
			}
			else
			{
				sptr = Text::StrWriteChar(sptr, c);
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::Access)
	{
		sptr = sqlstr;
		*sptr++ = '\'';
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\'')
			{
				*sptr++ = '\'';
				*sptr++ = '\'';
			}
			else
			{
				sptr = Text::StrWriteChar(sptr, c);
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::PostgreSQL)
	{
		sptr = sqlstr;
		*sptr++ = '\'';
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\'')
			{
				*sptr++ = '\'';
				*sptr++ = '\'';
			}
			else
			{
				sptr = Text::StrWriteChar(sptr, c);
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else
	{
		sptr = sqlstr;
		*sptr++ = '\'';
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\'')
			{
				*sptr++ = '\\';
				*sptr++ = '\'';
			}
			else
			{
				sptr = Text::StrWriteChar(sptr, c);
			}
		}
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
}

UOSInt DB::DBUtil::SDBStrWLeng(const WChar *val, DB::DBUtil::SQLType sqlType)
{
	UOSInt leng = 0;
	UTF32Char c;
	if (val == 0)
		return 4;

	if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\\')
			{
				leng += 2;
			}
			else if (c == '\'')
			{
				leng += 2;
			}
			else if (c == '\"')
			{
				leng += 2;
			}
			else if (c == '\r')
			{
				leng += 2;
			}
			else if (c == '\n')
			{
				leng += 2;
			}
			else if (c == '\t')
			{
				leng += 2;
			}
			else if (c == '\b')
			{
				leng += 2;
			}
			else if (c == 26)
			{
				leng += 2;
			}
			else if (c < 0x80)
			{
				leng += 1;
			}
			else if (c < 0x800)
			{
				leng += 2;
			}
			else if (c < 0x10000)
			{
				leng += 3;
			}
			else if (c < 0x200000)
			{
//				leng += 4;
				leng += 1;
			}
			else if (c < 0x4000000)
			{
//				leng += 5;
				leng += 1;
			}
			else
			{
//				leng += 6;
				leng += 1;
			}
		}
		leng += 2;
		return leng;
	}
	else if (sqlType == DB::DBUtil::SQLType::SQLite)
	{
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\'')
			{
				leng += 2;
			}
			else if (c < 0x80)
			{
				leng += 1;
			}
			else if (c < 0x800)
			{
				leng += 2;
			}
			else if (c < 0x10000)
			{
				leng += 3;
			}
			else if (c < 0x200000)
			{
				leng += 4;
			}
			else if (c < 0x4000000)
			{
				leng += 5;
			}
			else
			{
				leng += 6;
			}
		}
		leng += 2;
		return leng;
	}
	else if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\'')
			{
				leng += 2;
			}
			else if (c < 0x80)
			{
				leng += 1;
			}
			else if (c < 0x800)
			{
				leng += 2;
			}
			else if (c < 0x10000)
			{
				leng += 3;
			}
			else if (c < 0x200000)
			{
				leng += 4;
			}
			else if (c < 0x4000000)
			{
				leng += 5;
			}
			else
			{
				leng += 6;
			}
		}
		leng += 3;
		return leng;
	}
	else if (sqlType == DB::DBUtil::SQLType::Access)
	{
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\'')
			{
				leng += 2;
			}
			else if (c < 0x80)
			{
				leng += 1;
			}
			else if (c < 0x800)
			{
				leng += 2;
			}
			else if (c < 0x10000)
			{
				leng += 3;
			}
			else if (c < 0x200000)
			{
				leng += 4;
			}
			else if (c < 0x4000000)
			{
				leng += 5;
			}
			else
			{
				leng += 6;
			}
		}
		leng += 3;
		return leng;
	}
	else
	{
		while (true)
		{
			val = Text::StrReadChar(val, &c);
			if (c == 0)
			{
				break;
			}
			if (c == '\'')
			{
				leng += 2;
			}
			else if (c < 0x80)
			{
				leng += 1;
			}
			else if (c < 0x800)
			{
				leng += 2;
			}
			else if (c < 0x10000)
			{
				leng += 3;
			}
			else if (c < 0x200000)
			{
				leng += 4;
			}
			else if (c < 0x4000000)
			{
				leng += 5;
			}
			else
			{
				leng += 6;
			}
		}
		leng += 2;
		return leng;
	}
}

UTF8Char *DB::DBUtil::SDBInt32(UTF8Char *sqlstr, Int32 val, DB::DBUtil::SQLType sqlType)
{
	return Text::StrInt32(sqlstr, val);
}

UOSInt DB::DBUtil::SDBInt32Leng(Int32 val, DB::DBUtil::SQLType sqlType)
{
	UTF8Char buff[12];
	return (UOSInt)(Text::StrInt32(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBInt64(UTF8Char *sqlstr, Int64 val, DB::DBUtil::SQLType sqlType)
{
	return Text::StrInt64(sqlstr, val);
}

UOSInt DB::DBUtil::SDBInt64Leng(Int64 val, DB::DBUtil::SQLType sqlType)
{
	UTF8Char buff[22];
	return (UOSInt)(Text::StrInt64(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBUInt32(UTF8Char *sqlstr, UInt32 val, DB::DBUtil::SQLType sqlType)
{
	return Text::StrUInt32(sqlstr, val);
}

UOSInt DB::DBUtil::SDBUInt32Leng(UInt32 val, DB::DBUtil::SQLType sqlType)
{
	UTF8Char buff[12];
	return (UOSInt)(Text::StrUInt32(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBUInt64(UTF8Char *sqlstr, UInt64 val, DB::DBUtil::SQLType sqlType)
{
	return Text::StrUInt64(sqlstr, val);
}

UOSInt DB::DBUtil::SDBUInt64Leng(UInt64 val, DB::DBUtil::SQLType sqlType)
{
	UTF8Char buff[22];
	return (UOSInt)(Text::StrUInt64(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBDate(UTF8Char *sqlstr, Data::DateTime *dat, DB::DBUtil::SQLType sqlType, Int8 tzQhr)
{
	UTF8Char *sptr;
	if (dat == 0)
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));
	Data::DateTime dt(dat);
	if (sqlType == DB::DBUtil::SQLType::Access)
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		*sptr++ = '#';
		sptr = dt.ToLocalStr(sptr);
		*sptr++ = '#';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		dt.ConvertTimeZoneQHR(tzQhr);
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::SQLite)
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::Oracle)
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		sptr = Text::StrConcatC(sptr, UTF8STRC("TIMESTAMP '"));
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
}

UOSInt DB::DBUtil::SDBDateLeng(Data::DateTime *dat, DB::DBUtil::SQLType sqlType)
{
	if (dat == 0)
		return 4;

	if (sqlType == DB::DBUtil::SQLType::Access)
	{
		UTF8Char buff[100];
		return (UOSInt)(dat->ToLocalStr(buff) - buff + 2);
	}
	else if (sqlType == DB::DBUtil::SQLType::MSSQL || sqlType == DB::DBUtil::SQLType::SQLite)
	{
		return 25;
	}
	else if (sqlType == DB::DBUtil::SQLType::Oracle)
	{
		return 31;
	}
	else if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		return 25;
	}
	else
	{
		return 21;
	}
}

UTF8Char *DB::DBUtil::SDBTS(UTF8Char *sqlstr, Data::Timestamp ts, SQLType sqlType, Int8 tzQhr)
{
	UTF8Char *sptr;
	if (ts.IsZero())
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));
	if (sqlType == DB::DBUtil::SQLType::Access)
	{
		ts.tzQhr = 0;
		sptr = sqlstr;
		*sptr++ = '#';
		sptr = ts.ToString(sptr, "dd/MM/yyyy HH:mm:ss");
		*sptr++ = '#';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		ts.tzQhr = tzQhr;
		sptr = sqlstr;
		sptr = Text::StrConcatC(sptr, UTF8STRC("CAST('"));
		sptr = ts.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fffffff");
		sptr = Text::StrConcatC(sptr, UTF8STRC("' as datetime2(7))"));
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::SQLite)
	{
		ts.tzQhr = 0;
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = ts.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::Oracle)
	{
		ts.tzQhr = 0;
		sptr = sqlstr;
		sptr = Text::StrConcatC(sptr, UTF8STRC("TIMESTAMP '"));
		sptr = ts.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fffffffff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		ts.tzQhr = 0;
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = ts.ToString(sptr, "yyyy-MM-dd HH:mm:ss.ffffff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::PostgreSQL)
	{
		ts.tzQhr = 0;
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = ts.ToString(sptr, "yyyy-MM-dd HH:mm:ss.ffffff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else
	{
		ts.tzQhr = 0;
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = ts.ToString(sptr, "yyyy-MM-dd HH:mm:ss");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
}

UOSInt DB::DBUtil::SDBTSLeng(Data::Timestamp ts, SQLType sqlType)
{
	if (ts.IsZero())
		return 4;
	switch (sqlType)
	{
	case DB::DBUtil::SQLType::Access:
	case DB::DBUtil::SQLType::MDBTools:
		return 21;
	case DB::DBUtil::SQLType::MSSQL:
		return 51;
	case DB::DBUtil::SQLType::Oracle:
		return 41;
	case DB::DBUtil::SQLType::SQLite:
		return 25;
	case DB::DBUtil::SQLType::MySQL:
		return 28;
	case DB::DBUtil::SQLType::PostgreSQL:
		return 28;
	default:
	case DB::DBUtil::SQLType::WBEM:
	case DB::DBUtil::SQLType::Unknown:
		return 21;
	}
}

UTF8Char *DB::DBUtil::SDBDbl(UTF8Char *sqlstr, Double val, DB::DBUtil::SQLType sqlType)
{
	if (Math::IsInfinity(val) || Math::IsNAN(val))
	{
		return Text::StrDouble(sqlstr, 0);
	}
	else
	{
		return Text::StrDouble(sqlstr, val);
	}
}

UOSInt DB::DBUtil::SDBDblLeng(Double val, DB::DBUtil::SQLType sqlType)
{
	UTF8Char buff[128];
	return (UOSInt)(Text::StrDouble(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBSng(UTF8Char *sqlstr, Single val, DB::DBUtil::SQLType sqlType)
{
	return Text::StrDouble(sqlstr, val);
}

UOSInt DB::DBUtil::SDBSngLeng(Single val, DB::DBUtil::SQLType sqlType)
{
	UTF8Char buff[128];
	return (UOSInt)(Text::StrDouble(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBBool(UTF8Char *sqlStr, Bool val, DB::DBUtil::SQLType sqlType)
{
	if (sqlType == DB::DBUtil::SQLType::Oracle || sqlType == DB::DBUtil::SQLType::SQLite)
	{
		if (val)
		{
			*sqlStr++ = '1';
		}
		else
		{
			*sqlStr++ = '0';
		}
	}
	else
	{
		if (val)
		{
			*sqlStr++ = 'T';
			*sqlStr++ = 'R';
			*sqlStr++ = 'U';
			*sqlStr++ = 'E';
		}
		else
		{
			*sqlStr++ = 'F';
			*sqlStr++ = 'A';
			*sqlStr++ = 'L';
			*sqlStr++ = 'S';
			*sqlStr++ = 'E';
		}
	}
	*sqlStr = 0;
	return sqlStr;
}

UOSInt DB::DBUtil::SDBBoolLeng(Bool val, DB::DBUtil::SQLType sqlType)
{
	if (sqlType == DB::DBUtil::SQLType::Oracle)
	{
		return 1;
	}
	else
	{
		if (val)
			return 4;
		else
			return 5;
	}
}


UTF8Char *DB::DBUtil::SDBBin(UTF8Char *sqlstr, const UInt8 *buff, UOSInt size, DB::DBUtil::SQLType sqlType)
{
	UTF8Char *sptr;
	if (buff == 0)
	{
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));
	}
	if (sqlType == DB::DBUtil::SQLType::MySQL || sqlType == DB::DBUtil::SQLType::SQLite)
	{
		sptr = sqlstr;
		sptr = Text::StrConcatC(sptr, UTF8STRC("x'"));
		sptr = Text::StrHexBytes(sptr, buff, size, 0);
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		return Text::StrHexBytes(Text::StrConcatC(sqlstr, UTF8STRC("0x")), buff, size, 0);
	}
	else
	{
		/////////////////////////////////////
		return Text::StrConcatC(sqlstr, UTF8STRC("''"));
	}
}

UOSInt DB::DBUtil::SDBBinLeng(const UInt8 *buff, UOSInt size, DB::DBUtil::SQLType sqlType)
{
	if (buff == 0)
	{
		return 4;
	}
	if (sqlType == DB::DBUtil::SQLType::MySQL || sqlType == DB::DBUtil::SQLType::SQLite)
	{
		return (size << 1) + 3;
	}
	else if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		return (size << 1) + 2;
	}
	else
	{
		return 2;
	}
}

UTF8Char *DB::DBUtil::SDBVector(UTF8Char *sqlstr, Math::Geometry::Vector2D *vec, DB::DBUtil::SQLType sqlType)
{
	if (vec == 0)
	{
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));
	}
	if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Point)
		{
			Math::Geometry::Point *pt = (Math::Geometry::Point*)vec;
			UInt8 buff[22];
			WriteUInt32(buff, vec->GetSRID());
			buff[4] = 1;
			buff[5] = 12;
			Math::Coord2DDbl coord = pt->GetCenter();
			WriteDouble(&buff[6], coord.x);
			WriteDouble(&buff[14], coord.y);
			return SDBBin(sqlstr, buff, 22, sqlType);
		}
		else
		{
			return sqlstr;
		}
	}
	else if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		if (writer.ToText(&sb, vec))
		{
			sqlstr = Text::StrConcatC(sqlstr, UTF8STRC("GeomFromText('"));
			sqlstr = Text::StrConcatC(sqlstr, sb.ToString(), sb.GetLength());
			sqlstr = Text::StrConcatC(sqlstr, UTF8STRC("', "));
			sqlstr = Text::StrUInt32(sqlstr, vec->GetSRID());
			sqlstr = Text::StrConcatC(sqlstr, UTF8STRC(")"));
			return sqlstr;
		}
		else
		{
			return sqlstr;
		}
	}
	else if (sqlType == DB::DBUtil::SQLType::PostgreSQL)
	{
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		if (writer.ToText(&sb, vec))
		{
			sqlstr = Text::StrConcatC(sqlstr, UTF8STRC("ST_GeomFromText('"));
			sqlstr = Text::StrConcatC(sqlstr, sb.ToString(), sb.GetLength());
			sqlstr = Text::StrConcatC(sqlstr, UTF8STRC("', "));
			sqlstr = Text::StrUInt32(sqlstr, vec->GetSRID());
			sqlstr = Text::StrConcatC(sqlstr, UTF8STRC(")"));
			return sqlstr;
		}
		else
		{
			return sqlstr;
		}
	}
	else
	{
		return sqlstr;
	}
}

UOSInt DB::DBUtil::SDBVectorLeng(Math::Geometry::Vector2D *vec, DB::DBUtil::SQLType sqlType)
{
	if (vec == 0)
	{
		return 4;
	}
	if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Point)
		{
			return SDBBinLeng(0, 22, sqlType);
		}
		else
		{
			return 0;
		}
	}
	else if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		if (writer.ToText(&sb, vec))
		{
			UOSInt ret = 18 + sb.GetLength();
			sb.ClearStr();
			sb.AppendU32(vec->GetSRID());
			ret += sb.GetLength();
			return ret;
		}
		else
		{
			return 0;
		}
	}
	else if (sqlType == DB::DBUtil::SQLType::PostgreSQL)
	{
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		if (writer.ToText(&sb, vec))
		{
			UOSInt ret = 21 + sb.GetLength();
			sb.ClearStr();
			sb.AppendU32(vec->GetSRID());
			ret += sb.GetLength();
			return ret;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

UTF8Char *DB::DBUtil::SDBColUTF8(UTF8Char *sqlstr, const UTF8Char *colName, DB::DBUtil::SQLType sqlType)
{
	UTF8Char *sptr;
	UTF8Char c;
	switch (sqlType)
	{
	case DB::DBUtil::SQLType::MySQL:
		sptr = sqlstr;
		*sptr++ = '`';
		while ((c = *colName++) != 0)
		{
			if (c == '`')
			{
				*sptr++ = '`';
				*sptr++ = '`';
			}
			else
			{
				*sptr++ = c;
			}
		}
		*sptr++ = '`';
		*sptr = 0;
		return sptr;
	case DB::DBUtil::SQLType::MSSQL:
	case DB::DBUtil::SQLType::Access:
		sptr = sqlstr;
		*sptr++ = '[';
		while ((c = *colName++) != 0)
		{
			if (c == ']')
			{
				*sptr++ = ']';
				*sptr++ = ']';
			}
			else if (c == '.')
			{
				*sptr++ = ']';
				*sptr++ = '.';
				*sptr++ = '[';
			}
			else
			{
				*sptr++ = c;
			}
		}
		*sptr++ = ']';
		*sptr = 0;
		return sptr;
	case DB::DBUtil::SQLType::MDBTools:
		sptr = sqlstr;
		*sptr++ = '"';
		while ((c = *colName++) != 0)
		{
			if (c == '"')
			{
				*sptr++ = '"';
				*sptr++ = '"';
			}
			else
			{
				*sptr++ = c;
			}
		}
		*sptr++ = '"';
		*sptr = 0;
		return sptr;
	case DB::DBUtil::SQLType::PostgreSQL:
		sptr = sqlstr;
		if (Text::StringTool::IsNonASCII(colName))
		{
			UTF32Char c;
			*sptr++ = 'U';
			*sptr++ = '&';
			*sptr++ = '\"';
			while (true)
			{
				colName = Text::StrReadChar(colName, &c);
				if (c == 0)
					break;
				else if (c < 0x80)
					*sptr++ = (UTF8Char)c;
				else
				{
					*sptr++ = '\\';
					sptr = Text::StrHexVal16(sptr, (UInt16)c);
				}
			}
			*sptr++ = '\"';
			*sptr = 0;
		}
		else
		{
			*sptr++ = '\"';
			sptr = Text::StrConcat(sptr, colName);
			*sptr++ = '\"';
			*sptr = 0;
		}
		return sptr;
	case DB::DBUtil::SQLType::Oracle:
	case DB::DBUtil::SQLType::SQLite:
	case DB::DBUtil::SQLType::Unknown:
	case DB::DBUtil::SQLType::WBEM:
	default:
		return Text::StrConcat(sqlstr, colName);
	}
}

UOSInt DB::DBUtil::SDBColUTF8Leng(const UTF8Char *colName, DB::DBUtil::SQLType sqlType)
{
	UOSInt leng = 0;
	UTF8Char c;
	switch (sqlType)
	{
	case DB::DBUtil::SQLType::MySQL:
		while ((c = *colName++) != 0)
		{
			if (c == '`')
			{
				leng += 2;
			}
			else
			{
				leng += 1;
			}
		}
		leng += 2;
		return leng;
	case DB::DBUtil::SQLType::MSSQL:
	case DB::DBUtil::SQLType::Access:
		while ((c = *colName++) != 0)
		{
			if (c == ']')
			{
				leng += 2;
			}
			else
			{
				leng += 1;
			}
		}
		leng += 2;
		return leng;
	case DB::DBUtil::SQLType::MDBTools:
		while ((c = *colName++) != 0)
		{
			if (c == '\"')
			{
				leng += 2;
			}
			else
			{
				leng += 1;
			}
		}
		leng += 2;
		return leng;
	case DB::DBUtil::SQLType::PostgreSQL:
		if (Text::StringTool::IsNonASCII(colName))
		{
			UTF32Char c;
			leng += 4;
			while (true)
			{
				colName = Text::StrReadChar(colName, &c);
				if (c == 0)
					break;
				else if (c < 0x80)
					leng++;
				else
				{
					leng += 5;
				}
			}
			return leng;
		}
		else
		{
			leng += 2;
			leng += Text::StrCharCnt(colName);
			return leng;
		}
	case DB::DBUtil::SQLType::Oracle:
	case DB::DBUtil::SQLType::SQLite:
	case DB::DBUtil::SQLType::Unknown:
	case DB::DBUtil::SQLType::WBEM:
	default:
		return Text::StrCharCnt(colName);
	}
}

UTF8Char *DB::DBUtil::SDBColW(UTF8Char *sqlstr, const WChar *colName, DB::DBUtil::SQLType sqlType)
{
	const UTF8Char *sptr = Text::StrToUTF8New(colName);
	UTF8Char *ret = SDBColUTF8(sqlstr, sptr, sqlType);
	Text::StrDelNew(sptr);
	return ret;
}

UOSInt DB::DBUtil::SDBColWLeng(const WChar *colName, DB::DBUtil::SQLType sqlType)
{
	const UTF8Char *sptr = Text::StrToUTF8New(colName);
	UOSInt ret = SDBColUTF8Leng(sptr, sqlType);
	Text::StrDelNew(sptr);
	return ret;
}

UTF8Char *DB::DBUtil::SDBTrim(UTF8Char *sqlstr, Text::CString val, DB::DBUtil::SQLType sqlType)
{
	if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		return Text::StrConcatC(val.ConcatTo(Text::StrConcatC(sqlstr, UTF8STRC("LTRIM(RTRIM("))), UTF8STRC("))"));
	}
	else
	{
		return Text::StrConcatC(val.ConcatTo(Text::StrConcatC(sqlstr, UTF8STRC("TRIM("))), UTF8STRC(")"));
	}
}

UOSInt DB::DBUtil::SDBTrimLeng(Text::CString val, DB::DBUtil::SQLType sqlType)
{
	if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		return val.leng + 14;
	}
	else
	{
		return val.leng + 6;
	}
}

DB::DBUtil::ColType DB::DBUtil::ParseColType(DB::DBUtil::SQLType sqlType, const UTF8Char *tName, UOSInt *colSize)
{
	UTF8Char typeName[64];
	UOSInt typeNameLen;
	UOSInt tmp;
	UOSInt i;
	if (colSize == 0)
	{
		colSize = &tmp;
	}
	typeNameLen = (UOSInt)(Text::StrConcat(typeName, tName) - typeName);

	if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("varchar")))
		{
			if (typeName[7] == '(')
			{
				i = Text::StrIndexOfChar(typeName, ')');
				if (i != INVALID_INDEX)
				{
					typeName[i] = 0;
					*colSize = Text::StrToUInt32(&typeName[8]);
					typeName[i] = ')';
				}
				else
				{
					*colSize = Text::StrToUInt32(&typeName[8]);
				}
			}
			else
			{
				*colSize = 0;
			}
			return DB::DBUtil::CT_VarUTF32Char;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("char")))
		{
			if (typeName[4] == '(')
			{
				i = Text::StrIndexOfChar(typeName, ')');
				if (i != INVALID_INDEX)
				{
					typeName[i] = 0;
					*colSize = Text::StrToUInt32(&typeName[5]);
					typeName[i] = ')';
				}
				else
				{
					*colSize = Text::StrToUInt32(&typeName[5]);
				}
			}
			else
			{
				*colSize = 0;
			}
			return DB::DBUtil::CT_UTF32Char;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("bigint")))
		{
			if (Text::StrIndexOfC(typeName, typeNameLen, UTF8STRC("unsigned")) == INVALID_INDEX)
			{
				*colSize = 21;
				return DB::DBUtil::CT_Int64;
			}
			else
			{
				*colSize = 20;
				return DB::DBUtil::CT_UInt64;
			}
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("int")))
		{
			if (Text::StrIndexOfC(typeName, typeNameLen, UTF8STRC("unsigned")) == INVALID_INDEX)
			{
				*colSize = 11;
				return DB::DBUtil::CT_Int32;
			}
			else
			{
				*colSize = 10;
				return DB::DBUtil::CT_UInt32;
			}
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("smallint")))
		{
			if (Text::StrIndexOfC(typeName, typeNameLen, UTF8STRC("unsigned")) == INVALID_INDEX)
			{
				*colSize = 6;
				return DB::DBUtil::CT_Int16;
			}
			else
			{
				*colSize = 5;
				return DB::DBUtil::CT_UInt16;
			}
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("datetime")))
		{
			if (typeName[8] == '(')
			{
				i = Text::StrIndexOfChar(typeName, ')');
				if (i != INVALID_INDEX)
				{
					typeName[i] = 0;
					*colSize = Text::StrToUInt32(&typeName[9]);
					typeName[i] = ')';
				}
				else
				{
					*colSize = Text::StrToUInt32(&typeName[9]);
				}
			}
			else
			{
				*colSize = 0;
			}
			return DB::DBUtil::CT_DateTime;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("timestamp")))
		{
			if (typeName[9] == '(')
			{
				i = Text::StrIndexOfChar(typeName, ')');
				if (i != INVALID_INDEX)
				{
					typeName[i] = 0;
					*colSize = Text::StrToUInt32(&typeName[10]);
					typeName[i] = ')';
				}
				else
				{
					*colSize = Text::StrToUInt32(&typeName[10]);
				}
			}
			else
			{
				*colSize = 0;
			}
			return DB::DBUtil::CT_DateTime;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("double")))
		{
			*colSize = 8;
			return DB::DBUtil::CT_Double;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("float")))
		{
			*colSize = 4;
			return DB::DBUtil::CT_Float;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("longtext")))
		{
			*colSize = 0xffffffff;
			return DB::DBUtil::CT_VarUTF32Char;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("text")))
		{
			*colSize = 65535;
			return DB::DBUtil::CT_VarUTF32Char;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("tinyint(1) unsigned")))
		{
			*colSize = 1;
			return DB::DBUtil::CT_Bool;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("tinyint(1)")))
		{
			*colSize = 1;
			return DB::DBUtil::CT_Bool;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("tinyint(3) unsigned")))
		{
			*colSize = 3;
			return DB::DBUtil::CT_Byte;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("bit(1)")))
		{
			*colSize = 1;
			return DB::DBUtil::CT_Bool;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("geometry")))
		{
			return DB::DBUtil::CT_Vector;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("blob")))
		{
			*colSize = 0xffffffff;
			return DB::DBUtil::CT_Binary;
		}
		else
		{
			*colSize = 0;
			return DB::DBUtil::CT_Unknown;
		}
	}
	else if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("varchar")))
		{
			return DB::DBUtil::CT_VarUTF8Char;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("text")))
		{
			*colSize = 0x7FFFFFFF;
			return DB::DBUtil::CT_VarUTF8Char;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("char")))
		{
			return DB::DBUtil::CT_UTF8Char;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("int")))
		{
			return DB::DBUtil::CT_Int32;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("datetime")))
		{
			*colSize = 3;
			return DB::DBUtil::CT_DateTime;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("datetime2")))
		{
			if (typeName[9] == '(')
			{
				i = Text::StrIndexOfChar(typeName, ')');
				if (i != INVALID_INDEX)
				{
					typeName[i] = 0;
					*colSize = Text::StrToUInt32(&typeName[10]);
					typeName[i] = ')';
				}
				else
				{
					*colSize = Text::StrToUInt32(&typeName[10]);
				}
			}
			else
			{
				*colSize = 7;
			}
			*colSize = 7;
			return DB::DBUtil::CT_DateTime;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("datetimeoffset")))
		{
			if (typeName[14] == '(')
			{
				i = Text::StrIndexOfChar(typeName, ')');
				if (i != INVALID_INDEX)
				{
					typeName[i] = 0;
					*colSize = Text::StrToUInt32(&typeName[15]);
					typeName[i] = ')';
				}
				else
				{
					*colSize = Text::StrToUInt32(&typeName[15]);
				}
			}
			else
			{
				*colSize = 7;
			}
			return DB::DBUtil::CT_DateTimeTZ;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("float")))
		{
			return DB::DBUtil::CT_Double;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("bit")))
		{
			return DB::DBUtil::CT_Bool;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("bigint")))
		{
			return DB::DBUtil::CT_Int64;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("smallint")))
		{
			return DB::DBUtil::CT_Int16;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("tinyint")))
		{
			return DB::DBUtil::CT_Byte;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("nvarchar")))
		{
			return DB::DBUtil::CT_VarUTF16Char;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("ntext")))
		{
			*colSize = 0x3FFFFFFF;
			return DB::DBUtil::CT_VarUTF16Char;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("nchar")))
		{
			return DB::DBUtil::CT_UTF16Char;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("numeric")))
		{
			return DB::DBUtil::CT_Double;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("geometry")))
		{
			return DB::DBUtil::CT_Vector;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("date")))
		{
			return DB::DBUtil::CT_Date;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("sysname")))
		{
			*colSize = 128;
			return DB::DBUtil::CT_VarUTF16Char;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("binary")))
		{
			return DB::DBUtil::CT_Binary;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("varbinary")))
		{
			return DB::DBUtil::CT_Binary;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("image")))
		{
			*colSize = 0x7fffffff;
			return DB::DBUtil::CT_Binary;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("uniqueidentifier")))
		{
			return DB::DBUtil::CT_UUID;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("xml")))
		{
			*colSize = 1073741823;
			return DB::DBUtil::CT_VarUTF16Char;
		}
		else
		{
			return DB::DBUtil::CT_Unknown;
		}
	}
	else if (sqlType == DB::DBUtil::SQLType::SQLite)
	{
		if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("INTEGER")))
		{
			*colSize = 4;
			return DB::DBUtil::CT_Int32;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("INT")))
		{
			*colSize = 4;
			return DB::DBUtil::CT_Int32;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("MEDIUMINT")))
		{
			*colSize = 2;
			return DB::DBUtil::CT_Int16;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("TINYINT")))
		{
			*colSize = 1;
			return DB::DBUtil::CT_Byte;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("REAL")))
		{
			*colSize = 8;
			return DB::DBUtil::CT_Double;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("DOUBLE")))
		{
			*colSize = 8;
			return DB::DBUtil::CT_Double;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("DATETIME")))
		{
			*colSize = 3;
			return DB::DBUtil::CT_DateTime;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("BLOB")))
		{
			*colSize = 2147483647;
			return DB::DBUtil::CT_Binary;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("TEXT")))
		{
			*colSize = 2147483647;
			return DB::DBUtil::CT_VarUTF8Char;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("VARCHAR")))
		{
			*colSize = 2147483647;
			return DB::DBUtil::CT_VarUTF8Char;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("POINT")))
		{
			*colSize = 2147483647;
			return DB::DBUtil::CT_Vector;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("LINESTRING")))
		{
			*colSize = 2147483647;
			return DB::DBUtil::CT_Vector;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("POLYGON")))
		{
			*colSize = 2147483647;
			return DB::DBUtil::CT_Vector;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("BOOLEAN")))
		{
			*colSize = 1;
			return DB::DBUtil::CT_Byte;
		}
		else
		{
			*colSize = 0;
			return DB::DBUtil::CT_Unknown;
		}
	}
	else
	{
		return DB::DBUtil::CT_Unknown;
	}
}

UTF8Char *DB::DBUtil::ColTypeGetString(UTF8Char *sbuff, DB::DBUtil::ColType colType, UOSInt colSize)
{
	switch (colType)
	{
	case DB::DBUtil::CT_UInt32:
		return Text::StrConcatC(sbuff, UTF8STRC("UNSIGNED INTEGER"));
	case DB::DBUtil::CT_Int32:
		return Text::StrConcatC(sbuff, UTF8STRC("INTEGER"));
	case DB::DBUtil::CT_UTF8Char:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("CHAR(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_UTF16Char:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("UTF16CHAR(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_UTF32Char:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("UTF32CHAR(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_VarUTF8Char:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("VARCHAR(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_VarUTF16Char:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("VARUTF16CHAR(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_VarUTF32Char:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("VARUTF32CHAR(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_Date:
		return Text::StrConcatC(sbuff, UTF8STRC("DATE"));
	case DB::DBUtil::CT_DateTime:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("DATETIME(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_DateTimeTZ:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("DATETIMETZ(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_Double:
		return Text::StrConcatC(sbuff, UTF8STRC("DOUBLE"));
	case DB::DBUtil::CT_Float:
		return Text::StrConcatC(sbuff, UTF8STRC("FLOAT"));
	case DB::DBUtil::CT_Bool:
		return Text::StrConcatC(sbuff, UTF8STRC("BIT"));
	case DB::DBUtil::CT_Byte:
		return Text::StrConcatC(sbuff, UTF8STRC("TINYINT"));
	case DB::DBUtil::CT_Int16:
		return Text::StrConcatC(sbuff, UTF8STRC("SMALLINT"));
	case DB::DBUtil::CT_Int64:
		return Text::StrConcatC(sbuff, UTF8STRC("BIGINT"));
	case DB::DBUtil::CT_UInt16:
		return Text::StrConcatC(sbuff, UTF8STRC("UNSIGNED SMALLINT"));
	case DB::DBUtil::CT_UInt64:
		return Text::StrConcatC(sbuff, UTF8STRC("UNSIGNED BIGINT"));
	case DB::DBUtil::CT_Binary:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("BINARY(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_Vector:
		return Text::StrConcatC(sbuff, UTF8STRC("GEOMETRY"));
	case DB::DBUtil::CT_UUID:
		return Text::StrConcatC(sbuff, UTF8STRC("UUID"));
	case DB::DBUtil::CT_Unknown:
	default:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("UNKNOWN(")), colSize), UTF8STRC(")"));
	}
}

UTF8Char *DB::DBUtil::DB2FieldName(UTF8Char *fieldNameBuff, const UTF8Char *dbName)
{
	Bool nextUpper = false;
	UTF8Char c;
	while (true)
	{
		c = *dbName++;
		if (c == 0)
		{
			break;
		}
		else if (c == '_')
		{
			nextUpper = true;
		}
		else if (nextUpper)
		{
			nextUpper = false;
			*fieldNameBuff++ = Text::CharUtil::ToUpper(c);
		}
		else
		{
			*fieldNameBuff++ = Text::CharUtil::ToLower(c);
		}
	}
	*fieldNameBuff = 0;
	return fieldNameBuff;
}

UTF8Char *DB::DBUtil::Field2DBName(UTF8Char *dbNameBuff, const UTF8Char *fieldName)
{
	Bool isFirst = true;
	UTF8Char c;
	while (true)
	{
		c = *fieldName++;
		if (c == 0)
		{
			break;
		}
		else if (c >= 'A' && c <= 'Z')
		{
			if (isFirst)
			{
				*dbNameBuff++ = (UTF8Char)(c + 0x20);
				isFirst = false;
			}
			else
			{
				*dbNameBuff++ = '_';
				*dbNameBuff++ = (UTF8Char)(c + 0x20);
			}
		}
		else
		{
			*dbNameBuff++ = c;
			isFirst = false;
		}
	}
	*dbNameBuff = 0;
	return dbNameBuff;
}
