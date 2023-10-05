#include "Stdafx.h"
#include "DB/SQL/SQLUtil.h"

const UTF8Char *DB::SQL::SQLUtil::ParseNextWord(const UTF8Char *sql, NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType)
{
	sb->ClearStr();
	const UTF8Char *strStart = 0;
	UTF8Char c;
	while (true)
	{
		c = *sql++;
		if (c == 0)
		{
			sql--;
			if (strStart)
			{
				sb->AppendSlow(strStart);
			}
			return sql;
		}
		else if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			if (strStart)
			{
				sb->AppendC(strStart, (UOSInt)(sql - strStart - 1));
				return sql;
			}
		}
		else if (c == '(' || c == ')' || c == ',')
		{
			if (strStart)
			{
				sb->AppendC(strStart, (UOSInt)(sql - strStart - 1));
				return sql - 1;
			}
			else
			{
				sb->AppendUTF8Char(c);
				return sql;
			}
		}
		else
		{
			if (strStart == 0)
			{
				strStart = sql - 1;
			}
		}
	}
}
