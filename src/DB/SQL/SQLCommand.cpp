#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQL/SQLCommand.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

const UTF8Char *DB::SQL::SQLCommand::ParseNextWord(const UTF8Char *sql, Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType)
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
				sb->Append(strStart);
			}
			return sql;
		}
		else if (c == ' ' || c == '\t')
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
				sb->AppendChar(c, 1);
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

DB::SQL::SQLCommand *DB::SQL::SQLCommand::Parse(const UTF8Char *sql, DB::DBUtil::ServerType svrType)
{
	printf("Cmd: %s\r\n", sql);
	Text::StringBuilderUTF8 sb;
	sql = ParseNextWord(sql, &sb, svrType);
	if (sb.EqualsICase((const UTF8Char*)"CREATE"))
	{
		sql = ParseNextWord(sql, &sb, svrType);
		if (sb.EqualsICase((const UTF8Char*)"TABLE"))
		{
			//////////////////////////////////////
		}
		else
		{
			printf("Unknown word after create: %s\r\n", sb.ToString());
		}
	}
	else
	{
		printf("Unknown word: %s\r\n", sb.ToString());
	}
	return 0;
}
