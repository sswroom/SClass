#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "DB/DBUtil.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

UTF8Char *DB::DBUtil::SDBStrUTF8(UTF8Char *sqlstr, const UTF8Char *val, ServerType svrType)
{
	UTF8Char *sptr;
	UTF8Char c;
	if (val == 0)
		return Text::StrConcat(sqlstr, (const UTF8Char*)"NULL");

	if (svrType == DB::DBUtil::SVR_TYPE_MYSQL)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_SQLITE)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_ACCESS)
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

	if (svrType == DB::DBUtil::SVR_TYPE_MYSQL)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_SQLITE)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_ACCESS)
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
		return Text::StrConcat(sqlstr, (const UTF8Char*)"NULL");

	if (svrType == DB::DBUtil::SVR_TYPE_MYSQL)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_SQLITE)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_ACCESS)
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

	if (svrType == DB::DBUtil::SVR_TYPE_MYSQL)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_SQLITE)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_ACCESS)
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

UTF8Char *DB::DBUtil::SDBDate(UTF8Char *sqlstr, Data::DateTime *dat, DB::DBUtil::ServerType svrType, Int8 tzQhr)
{
	UTF8Char *sptr;
	if (dat == 0)
		return Text::StrConcat(sqlstr, (const UTF8Char*)"NULL");
	Data::DateTime dt(dat);
	if (svrType == DB::DBUtil::SVR_TYPE_ACCESS)
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		*sptr++ = '#';
		sptr = dt.ToLocalStr(sptr);
		*sptr++ = '#';
		*sptr = 0;
		return sptr;
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		dt.ConvertTimeZoneQHR(tzQhr);
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		*sptr++ = '\'';
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_ORACLE)
	{
		dt.ToUTCTime();
		sptr = sqlstr;
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"TIMESTAMP '");
		sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:ss.fff");
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_MYSQL)
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

	if (svrType == DB::DBUtil::SVR_TYPE_ACCESS)
	{
		UTF8Char buff[100];
		return (UOSInt)(dat->ToLocalStr(buff) - buff + 2);
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL || svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		return 25;
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_ORACLE)
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
	if (svrType == DB::DBUtil::SVR_TYPE_ORACLE || svrType == DB::DBUtil::SVR_TYPE_SQLITE)
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
	if (svrType == DB::DBUtil::SVR_TYPE_ORACLE)
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
		return Text::StrConcat(sqlstr, (const UTF8Char*)"NULL");
	}
	if (svrType == DB::DBUtil::SVR_TYPE_MYSQL || svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		sptr = sqlstr;
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"x'");
		sptr = Text::StrHexBytes(sptr, buff, size, 0);
		*sptr++ = '\'';
		*sptr = 0;
		return sptr;
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		return Text::StrHexBytes(Text::StrConcat(sqlstr, (const UTF8Char*)"0x"), buff, size, 0);
	}
	else
	{
		/////////////////////////////////////
		return Text::StrConcat(sqlstr, (const UTF8Char*)"''");
	}
}

UOSInt DB::DBUtil::SDBBinLeng(const UInt8 *buff, UOSInt size, DB::DBUtil::ServerType svrType)
{
	if (buff == 0)
	{
		return 4;
	}
	if (svrType == DB::DBUtil::SVR_TYPE_MYSQL || svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		return (size << 1) + 3;
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		return (size << 1) + 2;
	}
	else
	{
		return 2;
	}
}

UTF8Char *DB::DBUtil::SDBVector(UTF8Char *sqlstr, Math::Vector2D *vec, Int32 srId, DB::DBUtil::ServerType svrType)
{
	if (vec == 0)
	{
		return Text::StrConcat(sqlstr, (const UTF8Char*)"NULL");
	}
	if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		if (vec->GetVectorType() == Math::Vector2D::VT_POINT)
		{
			Math::Point *pt = (Math::Point*)vec;
			UInt8 buff[22];
			WriteInt32(buff, srId);
			buff[4] = 1;
			buff[5] = 12;
			pt->GetCenter((Double*)&buff[6], (Double*)&buff[14]);
			return SDBBin(sqlstr, buff, 22, svrType);
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
	if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		if (vec->GetVectorType() == Math::Vector2D::VT_POINT)
		{
			return SDBBinLeng(0, 22, svrType);
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
	if (svrType == DB::DBUtil::SVR_TYPE_MYSQL)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL || svrType == DB::DBUtil::SVR_TYPE_ACCESS)
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
	else
	{
		return Text::StrConcat(sqlstr, colName);
	}
}

UOSInt DB::DBUtil::SDBColUTF8Leng(const UTF8Char *colName, DB::DBUtil::ServerType svrType)
{
	UOSInt leng = 0;
	UTF8Char c;
	if (svrType == DB::DBUtil::SVR_TYPE_MYSQL)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL || svrType == DB::DBUtil::SVR_TYPE_ACCESS)
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
	const UTF8Char *u8ptr = Text::StrToUTF8New(colName);
	UTF8Char *ret = SDBColUTF8(sqlstr, u8ptr, svrType);
	Text::StrDelNew(u8ptr);
	return ret;
}

UOSInt DB::DBUtil::SDBColWLeng(const WChar *colName, DB::DBUtil::ServerType svrType)
{
	const UTF8Char *u8ptr = Text::StrToUTF8New(colName);
	UOSInt ret = SDBColUTF8Leng(u8ptr, svrType);
	Text::StrDelNew(u8ptr);
	return ret;
}

UTF8Char *DB::DBUtil::SDBTrim(UTF8Char *sqlstr, const UTF8Char *val, DB::DBUtil::ServerType svrType)
{
	if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		return Text::StrConcat(Text::StrConcat(Text::StrConcat(sqlstr, (const UTF8Char*)"LTRIM(RTRIM("), val), (const UTF8Char*)"))");
	}
	else
	{
		return Text::StrConcat(Text::StrConcat(Text::StrConcat(sqlstr, (const UTF8Char*)"TRIM("), val), (const UTF8Char*)")");
	}
}

UOSInt DB::DBUtil::SDBTrimLeng(const UTF8Char *val, DB::DBUtil::ServerType svrType)
{
	if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		return Text::StrCharCnt(val) + 14;
	}
	else
	{
		return Text::StrCharCnt(val) + 6;
	}
}

DB::DBUtil::ColType DB::DBUtil::ParseColType(DB::DBUtil::ServerType svrType, const UTF8Char *tName, UOSInt *colSize)
{
	UTF8Char typeName[64];
	UOSInt tmp;
	OSInt i;
	if (colSize == 0)
	{
		colSize = &tmp;
	}
	Text::StrConcat(typeName, tName);

	if (svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		if (Text::StrStartsWith(typeName, (const UTF8Char*)"varchar"))
		{
			if (typeName[7] == '(')
			{
				i = Text::StrIndexOf(typeName, (const UTF8Char*)")");
				typeName[i] = 0;
				*colSize = Text::StrToUInt32(&typeName[8]);
				typeName[i] = ')';
			}
			else
			{
				*colSize = 0;
			}
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"char"))
		{
			if (typeName[4] == '(')
			{
				i = Text::StrIndexOf(typeName, (const UTF8Char*)")");
				typeName[i] = 0;
				*colSize = Text::StrToUInt32(&typeName[5]);
				typeName[i] = ')';
			}
			else
			{
				*colSize = 0;
			}
			return DB::DBUtil::CT_Char;
		}
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"bigint"))
		{
			if (Text::StrIndexOf(typeName, (const UTF8Char*)"unsigned") == -1)
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
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"int"))
		{
			if (Text::StrIndexOf(typeName, (const UTF8Char*)"unsigned") == -1)
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
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"smallint"))
		{
			if (Text::StrIndexOf(typeName, (const UTF8Char*)"unsigned") == -1)
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
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"datetime"))
		{
			*colSize = 8;
			return DB::DBUtil::CT_DateTime2;
		}
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"timestamp"))
		{
			*colSize = 8;
			return DB::DBUtil::CT_DateTime2;
		}
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"double"))
		{
			*colSize = 8;
			return DB::DBUtil::CT_Double;
		}
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"float"))
		{
			*colSize = 4;
			return DB::DBUtil::CT_Float;
		}
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"longtext"))
		{
			*colSize = 0xffffffff;
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"text"))
		{
			*colSize = 65535;
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrCompare(typeName, (const UTF8Char*)"tinyint(1) unsigned") == 0)
		{
			*colSize = 1;
			return DB::DBUtil::CT_Bool;
		}
		else if (Text::StrCompare(typeName, (const UTF8Char*)"tinyint(1)") == 0)
		{
			*colSize = 1;
			return DB::DBUtil::CT_Bool;
		}
		else if (Text::StrCompare(typeName, (const UTF8Char*)"tinyint(3) unsigned") == 0)
		{
			*colSize = 3;
			return DB::DBUtil::CT_Byte;
		}
		else if (Text::StrCompare(typeName, (const UTF8Char*)"bit(1)") == 0)
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
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		*colSize = 0;
		if (Text::StrEquals(typeName, (const UTF8Char*)"varchar"))
		{
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrEquals(typeName, (const UTF8Char*)"text"))
		{
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"char"))
		{
			return DB::DBUtil::CT_Char;
		}
		else if (Text::StrStartsWith(typeName, (const UTF8Char*)"int"))
		{
			return DB::DBUtil::CT_Int32;
		}
		else if (Text::StrEquals(typeName, (const UTF8Char*)"datetime"))
		{
			return DB::DBUtil::CT_DateTime;
		}
		else if (Text::StrEquals(typeName, (const UTF8Char*)"datetime2"))
		{
			return DB::DBUtil::CT_DateTime2;
		}
		else if (Text::StrEquals(typeName, (const UTF8Char*)"float"))
		{
			return DB::DBUtil::CT_Double;
		}
		else if (Text::StrEquals(typeName, (const UTF8Char*)"bit"))
		{
			return DB::DBUtil::CT_Bool;
		}
		else if (Text::StrEquals(typeName, (const UTF8Char*)"bigint"))
		{
			return DB::DBUtil::CT_Int64;
		}
		else if (Text::StrEquals(typeName, (const UTF8Char*)"smallint"))
		{
			return DB::DBUtil::CT_Int64;
		}
		else if (Text::StrEquals(typeName, (const UTF8Char*)"nvarchar"))
		{
			return DB::DBUtil::CT_NVarChar;
		}
		else if (Text::StrEquals(typeName, (const UTF8Char*)"numeric"))
		{
			return DB::DBUtil::CT_Double;
		}
		else if (Text::StrEquals(typeName, (const UTF8Char*)"geometry"))
		{
			return DB::DBUtil::CT_Vector;
		}
		else
		{
			return DB::DBUtil::CT_Unknown;
		}
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		if (Text::StrEqualsICase(typeName, (const UTF8Char*)"INTEGER"))
		{
			*colSize = 4;
			return DB::DBUtil::CT_Int32;
		}
		else if (Text::StrEqualsICase(typeName, (const UTF8Char*)"REAL"))
		{
			*colSize = 8;
			return DB::DBUtil::CT_Double;
		}
		else if (Text::StrEqualsICase(typeName, (const UTF8Char*)"BLOB"))
		{
			*colSize = 2147483647;
			return DB::DBUtil::CT_Binary;
		}
		else if (Text::StrEqualsICase(typeName, (const UTF8Char*)"TEXT"))
		{
			*colSize = 2147483647;
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrEqualsICase(typeName, (const UTF8Char*)"VARCHAR"))
		{
			*colSize = 2147483647;
			return DB::DBUtil::CT_VarChar;
		}
		else if (Text::StrEqualsICase(typeName, (const UTF8Char*)"BOOLEAN"))
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
		return Text::StrConcat(sbuff, (const UTF8Char*)"UNSIGNED INTEGER");
	case DB::DBUtil::CT_Int32:
		return Text::StrConcat(sbuff, (const UTF8Char*)"INTEGER");
	case DB::DBUtil::CT_VarChar:
		return Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"VARCHAR("), colSize), (const UTF8Char*)")");
	case DB::DBUtil::CT_Char:
		return Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"CHAR("), colSize), (const UTF8Char*)")");
	case DB::DBUtil::CT_NVarChar:
		return Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"NVARCHAR("), colSize), (const UTF8Char*)")");
	case DB::DBUtil::CT_NChar:
		return Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"NCHAR("), colSize), (const UTF8Char*)")");
	case DB::DBUtil::CT_DateTime:
		return Text::StrConcat(sbuff, (const UTF8Char*)"DATETIME");
	case DB::DBUtil::CT_DateTime2:
		return Text::StrConcat(sbuff, (const UTF8Char*)"DATETIME2");
	case DB::DBUtil::CT_Double:
		return Text::StrConcat(sbuff, (const UTF8Char*)"DOUBLE");
	case DB::DBUtil::CT_Float:
		return Text::StrConcat(sbuff, (const UTF8Char*)"FLOAT");
	case DB::DBUtil::CT_Bool:
		return Text::StrConcat(sbuff, (const UTF8Char*)"BIT");
	case DB::DBUtil::CT_Byte:
		return Text::StrConcat(sbuff, (const UTF8Char*)"TINYINT");
	case DB::DBUtil::CT_Int16:
		return Text::StrConcat(sbuff, (const UTF8Char*)"SMALLINT");
	case DB::DBUtil::CT_Int64:
		return Text::StrConcat(sbuff, (const UTF8Char*)"BIGINT");
	case DB::DBUtil::CT_UInt16:
		return Text::StrConcat(sbuff, (const UTF8Char*)"UNSIGNED SMALLINT");
	case DB::DBUtil::CT_UInt64:
		return Text::StrConcat(sbuff, (const UTF8Char*)"UNSIGNED BIGINT");
	case DB::DBUtil::CT_Binary:
		return Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"BINARY("), colSize), (const UTF8Char*)")");
	case DB::DBUtil::CT_Vector:
		return Text::StrConcat(sbuff, (const UTF8Char*)"GEOMETRY");
	case DB::DBUtil::CT_Unknown:
	default:
		return Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"UNKNOWN("), colSize), (const UTF8Char*)")");
	}
}
