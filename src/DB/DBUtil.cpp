#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "DB/DBUtil.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "Math/WKTWriter.h"
#include "Text/CharUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

UTF8Char *DB::DBUtil::SDBStrUTF8(UTF8Char *sqlstr, const UTF8Char *val, ServerType svrType)
{
	UTF8Char *sptr;
	UTF8Char c;
	if (val == 0)
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));

	if (svrType == DB::DBUtil::ServerType::MySQL)
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
	else if (svrType == DB::DBUtil::ServerType::SQLite)
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
	else if (svrType == DB::DBUtil::ServerType::MSSQL)
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
	else if (svrType == DB::DBUtil::ServerType::Access)
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

UOSInt DB::DBUtil::SDBStrUTF8Leng(const UTF8Char *val, DB::DBUtil::ServerType svrType)
{
	UOSInt leng = 0;
	UTF8Char c;
	if (val == 0)
		return 4;

	if (svrType == DB::DBUtil::ServerType::MySQL)
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
	else if (svrType == DB::DBUtil::ServerType::SQLite)
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
	else if (svrType == DB::DBUtil::ServerType::MSSQL)
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
	else if (svrType == DB::DBUtil::ServerType::Access)
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

UTF8Char *DB::DBUtil::SDBStrW(UTF8Char *sqlstr, const WChar *val, DB::DBUtil::ServerType svrType)
{
	UTF8Char *sptr;
	UTF32Char c;
	if (val == 0)
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));

	if (svrType == DB::DBUtil::ServerType::MySQL)
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
	else if (svrType == DB::DBUtil::ServerType::SQLite)
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
	else if (svrType == DB::DBUtil::ServerType::MSSQL)
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
	else if (svrType == DB::DBUtil::ServerType::Access)
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

UOSInt DB::DBUtil::SDBStrWLeng(const WChar *val, DB::DBUtil::ServerType svrType)
{
	UOSInt leng = 0;
	UTF32Char c;
	if (val == 0)
		return 4;

	if (svrType == DB::DBUtil::ServerType::MySQL)
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
	else if (svrType == DB::DBUtil::ServerType::SQLite)
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
	else if (svrType == DB::DBUtil::ServerType::MSSQL)
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
	else if (svrType == DB::DBUtil::ServerType::Access)
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

UTF8Char *DB::DBUtil::SDBInt32(UTF8Char *sqlstr, Int32 val, DB::DBUtil::ServerType svrType)
{
	return Text::StrInt32(sqlstr, val);
}

UOSInt DB::DBUtil::SDBInt32Leng(Int32 val, DB::DBUtil::ServerType svrType)
{
	UTF8Char buff[12];
	return (UOSInt)(Text::StrInt32(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBInt64(UTF8Char *sqlstr, Int64 val, DB::DBUtil::ServerType svrType)
{
	return Text::StrInt64(sqlstr, val);
}

UOSInt DB::DBUtil::SDBInt64Leng(Int64 val, DB::DBUtil::ServerType svrType)
{
	UTF8Char buff[22];
	return (UOSInt)(Text::StrInt64(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBUInt32(UTF8Char *sqlstr, UInt32 val, DB::DBUtil::ServerType svrType)
{
	return Text::StrUInt32(sqlstr, val);
}

UOSInt DB::DBUtil::SDBUInt32Leng(UInt32 val, DB::DBUtil::ServerType svrType)
{
	UTF8Char buff[12];
	return (UOSInt)(Text::StrUInt32(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBUInt64(UTF8Char *sqlstr, UInt64 val, DB::DBUtil::ServerType svrType)
{
	return Text::StrUInt64(sqlstr, val);
}

UOSInt DB::DBUtil::SDBUInt64Leng(UInt64 val, DB::DBUtil::ServerType svrType)
{
	UTF8Char buff[22];
	return (UOSInt)(Text::StrUInt64(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBDate(UTF8Char *sqlstr, Data::DateTime *dat, DB::DBUtil::ServerType svrType, Int8 tzQhr)
{
	UTF8Char *sptr;
	if (dat == 0)
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));
	Data::DateTime dt(dat);
	if (svrType == DB::DBUtil::ServerType::Access)
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		*sptr++ = '#';
		sptr = dt.ToLocalStr(sptr);
		*sptr++ = '#';
		*sptr = 0;
		return sptr;
	}
	else if (svrType == DB::DBUtil::ServerType::MSSQL)
	{
		dt.ConvertTimeZoneQHR(tzQhr);
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (svrType == DB::DBUtil::ServerType::SQLite)
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (svrType == DB::DBUtil::ServerType::Oracle)
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		sptr = Text::StrConcatC(sptr, UTF8STRC("TIMESTAMP '"));
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (svrType == DB::DBUtil::ServerType::MySQL)
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

UOSInt DB::DBUtil::SDBDateLeng(Data::DateTime *dat, DB::DBUtil::ServerType svrType)
{
	if (dat == 0)
		return 4;

	if (svrType == DB::DBUtil::ServerType::Access)
	{
		UTF8Char buff[100];
		return (UOSInt)(dat->ToLocalStr(buff) - buff + 2);
	}
	else if (svrType == DB::DBUtil::ServerType::MSSQL || svrType == DB::DBUtil::ServerType::SQLite)
	{
		return 25;
	}
	else if (svrType == DB::DBUtil::ServerType::Oracle)
	{
		return 31;
	}
	else
	{
		return 21;
	}
}

UTF8Char *DB::DBUtil::SDBDbl(UTF8Char *sqlstr, Double val, DB::DBUtil::ServerType svrType)
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

UOSInt DB::DBUtil::SDBDblLeng(Double val, DB::DBUtil::ServerType svrType)
{
	UTF8Char buff[128];
	return (UOSInt)(Text::StrDouble(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBSng(UTF8Char *sqlstr, Single val, DB::DBUtil::ServerType svrType)
{
	return Text::StrDouble(sqlstr, val);
}

UOSInt DB::DBUtil::SDBSngLeng(Single val, DB::DBUtil::ServerType svrType)
{
	UTF8Char buff[128];
	return (UOSInt)(Text::StrDouble(buff, val) - buff);
}

UTF8Char *DB::DBUtil::SDBBool(UTF8Char *sqlStr, Bool val, DB::DBUtil::ServerType svrType)
{
	if (svrType == DB::DBUtil::ServerType::Oracle || svrType == DB::DBUtil::ServerType::SQLite)
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

UOSInt DB::DBUtil::SDBBoolLeng(Bool val, DB::DBUtil::ServerType svrType)
{
	if (svrType == DB::DBUtil::ServerType::Oracle)
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


UTF8Char *DB::DBUtil::SDBBin(UTF8Char *sqlstr, const UInt8 *buff, UOSInt size, DB::DBUtil::ServerType svrType)
{
	UTF8Char *sptr;
	if (buff == 0)
	{
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));
	}
	if (svrType == DB::DBUtil::ServerType::MySQL || svrType == DB::DBUtil::ServerType::SQLite)
	{
		sptr = sqlstr;
		sptr = Text::StrConcatC(sptr, UTF8STRC("x'"));
		sptr = Text::StrHexBytes(sptr, buff, size, 0);
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (svrType == DB::DBUtil::ServerType::MSSQL)
	{
		return Text::StrHexBytes(Text::StrConcatC(sqlstr, UTF8STRC("0x")), buff, size, 0);
	}
	else
	{
		/////////////////////////////////////
		return Text::StrConcatC(sqlstr, UTF8STRC("''"));
	}
}

UOSInt DB::DBUtil::SDBBinLeng(const UInt8 *buff, UOSInt size, DB::DBUtil::ServerType svrType)
{
	if (buff == 0)
	{
		return 4;
	}
	if (svrType == DB::DBUtil::ServerType::MySQL || svrType == DB::DBUtil::ServerType::SQLite)
	{
		return (size << 1) + 3;
	}
	else if (svrType == DB::DBUtil::ServerType::MSSQL)
	{
		return (size << 1) + 2;
	}
	else
	{
		return 2;
	}
}

UTF8Char *DB::DBUtil::SDBVector(UTF8Char *sqlstr, Math::Vector2D *vec, DB::DBUtil::ServerType svrType)
{
	if (vec == 0)
	{
		return Text::StrConcatC(sqlstr, UTF8STRC("NULL"));
	}
	if (svrType == DB::DBUtil::ServerType::MSSQL)
	{
		if (vec->GetVectorType() == Math::Vector2D::VectorType::Point)
		{
			Math::Point *pt = (Math::Point*)vec;
			UInt8 buff[22];
			WriteUInt32(buff, vec->GetSRID());
			buff[4] = 1;
			buff[5] = 12;
			Math::Coord2DDbl coord = pt->GetCenter();
			WriteDouble(&buff[6], coord.x);
			WriteDouble(&buff[14], coord.y);
			return SDBBin(sqlstr, buff, 22, svrType);
		}
		else
		{
			return sqlstr;
		}
	}
	else if (svrType == DB::DBUtil::ServerType::MySQL)
	{
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		if (writer.GenerateWKT(&sb, vec))
		{
			sqlstr = Text::StrConcatC(sqlstr, UTF8STRC("geometry::STGeomFromText('"));
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

UOSInt DB::DBUtil::SDBVectorLeng(Math::Vector2D *vec, DB::DBUtil::ServerType svrType)
{
	if (vec == 0)
	{
		return 4;
	}
	if (svrType == DB::DBUtil::ServerType::MSSQL)
	{
		if (vec->GetVectorType() == Math::Vector2D::VectorType::Point)
		{
			return SDBBinLeng(0, 22, svrType);
		}
		else
		{
			return 0;
		}
	}
	else if (svrType == DB::DBUtil::ServerType::MySQL)
	{
		Math::WKTWriter writer;
		Text::StringBuilderUTF8 sb;
		if (writer.GenerateWKT(&sb, vec))
		{
			UOSInt ret = 30 + sb.GetLength();
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

UTF8Char *DB::DBUtil::SDBColUTF8(UTF8Char *sqlstr, const UTF8Char *colName, DB::DBUtil::ServerType svrType)
{
	UTF8Char *sptr;
	UTF8Char c;
	if (svrType == DB::DBUtil::ServerType::MySQL)
	{
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
	}
	else if (svrType == DB::DBUtil::ServerType::MSSQL || svrType == DB::DBUtil::ServerType::Access)
	{
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
	}
	else if (svrType == DB::DBUtil::ServerType::MDBTools)
	{
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
	}
	else
	{
		return Text::StrConcat(sqlstr, colName);
	}
}

UOSInt DB::DBUtil::SDBColUTF8Leng(const UTF8Char *colName, DB::DBUtil::ServerType svrType)
{
	UOSInt leng = 0;
	UTF8Char c;
	if (svrType == DB::DBUtil::ServerType::MySQL)
	{
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
	}
	else if (svrType == DB::DBUtil::ServerType::MSSQL || svrType == DB::DBUtil::ServerType::Access)
	{
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
	}
	else
	{
		return Text::StrCharCnt(colName);
	}
}

UTF8Char *DB::DBUtil::SDBColW(UTF8Char *sqlstr, const WChar *colName, DB::DBUtil::ServerType svrType)
{
	const UTF8Char *sptr = Text::StrToUTF8New(colName);
	UTF8Char *ret = SDBColUTF8(sqlstr, sptr, svrType);
	Text::StrDelNew(sptr);
	return ret;
}

UOSInt DB::DBUtil::SDBColWLeng(const WChar *colName, DB::DBUtil::ServerType svrType)
{
	const UTF8Char *sptr = Text::StrToUTF8New(colName);
	UOSInt ret = SDBColUTF8Leng(sptr, svrType);
	Text::StrDelNew(sptr);
	return ret;
}

UTF8Char *DB::DBUtil::SDBTrim(UTF8Char *sqlstr, Text::CString val, DB::DBUtil::ServerType svrType)
{
	if (svrType == DB::DBUtil::ServerType::MSSQL)
	{
		return Text::StrConcatC(val.ConcatTo(Text::StrConcatC(sqlstr, UTF8STRC("LTRIM(RTRIM("))), UTF8STRC("))"));
	}
	else
	{
		return Text::StrConcatC(val.ConcatTo(Text::StrConcatC(sqlstr, UTF8STRC("TRIM("))), UTF8STRC(")"));
	}
}

UOSInt DB::DBUtil::SDBTrimLeng(Text::CString val, DB::DBUtil::ServerType svrType)
{
	if (svrType == DB::DBUtil::ServerType::MSSQL)
	{
		return val.leng + 14;
	}
	else
	{
		return val.leng + 6;
	}
}

DB::DBUtil::ColType DB::DBUtil::ParseColType(DB::DBUtil::ServerType svrType, const UTF8Char *tName, UOSInt *colSize)
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

	if (svrType == DB::DBUtil::ServerType::MySQL)
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
			return DB::DBUtil::CT_VarChar;
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
			return DB::DBUtil::CT_Char;
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
			*colSize = 8;
			return DB::DBUtil::CT_DateTime2;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("timestamp")))
		{
			*colSize = 8;
			return DB::DBUtil::CT_DateTime2;
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
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("text")))
		{
			*colSize = 65535;
			return DB::DBUtil::CT_VarChar;
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
		else
		{
			*colSize = 0;
			return DB::DBUtil::CT_Unknown;
		}
	}
	else if (svrType == DB::DBUtil::ServerType::MSSQL)
	{
		*colSize = 0;
		if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("varchar")))
		{
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("text")))
		{
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("char")))
		{
			return DB::DBUtil::CT_Char;
		}
		else if (Text::StrStartsWithC(typeName, typeNameLen, UTF8STRC("int")))
		{
			return DB::DBUtil::CT_Int32;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("datetime")))
		{
			return DB::DBUtil::CT_DateTime;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("datetime2")))
		{
			return DB::DBUtil::CT_DateTime2;
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
			return DB::DBUtil::CT_Int64;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("nvarchar")))
		{
			return DB::DBUtil::CT_NVarChar;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("ntext")))
		{
			return DB::DBUtil::CT_NVarChar;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("nchar")))
		{
			return DB::DBUtil::CT_NChar;
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
			return DB::DBUtil::CT_DateTime;
		}
		else if (Text::StrEqualsC(typeName, typeNameLen, UTF8STRC("sysname")))
		{
			*colSize = 128;
			return DB::DBUtil::CT_NVarChar;
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
		else
		{
			return DB::DBUtil::CT_Unknown;
		}
	}
	else if (svrType == DB::DBUtil::ServerType::SQLite)
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
			*colSize = 16;
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
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrEqualsICaseC(typeName, typeNameLen, UTF8STRC("VARCHAR")))
		{
			*colSize = 2147483647;
			return DB::DBUtil::CT_VarChar;
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
	case DB::DBUtil::CT_VarChar:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("VARCHAR(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_Char:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("CHAR(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_NVarChar:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("NVARCHAR(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_NChar:
		return Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("NCHAR(")), colSize), UTF8STRC(")"));
	case DB::DBUtil::CT_DateTime:
		return Text::StrConcatC(sbuff, UTF8STRC("DATETIME"));
	case DB::DBUtil::CT_DateTime2:
		return Text::StrConcatC(sbuff, UTF8STRC("DATETIME2"));
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
