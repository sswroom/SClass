#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQL/CreateTableCommand.h"
#include "DB/SQL/SQLCommand.h"
#include "Text/CharUtil.h"
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

Bool DB::SQL::SQLCommand::IsPunctuation(const UTF8Char *s)
{
	return Text::CharUtil::IsPunctuation(s[0]) && s[1] == 0;
}

DB::SQL::SQLCommand *DB::SQL::SQLCommand::Parse(const UTF8Char *sql, DB::DBUtil::ServerType svrType)
{
	printf("Cmd: %s\r\n", sql);
	DB::SQL::SQLCommand *cmd = 0;
	Text::StringBuilderUTF8 sb;
	sql = ParseNextWord(sql, &sb, svrType);
	if (sb.EqualsICase((const UTF8Char*)"CREATE"))
	{
		sql = ParseNextWord(sql, &sb, svrType);
		if (sb.EqualsICase((const UTF8Char*)"TABLE"))
		{
			sql = ParseNextWord(sql, &sb, svrType);
			if (sb.GetLength() == 0)
			{
				printf("Missing table name\r\n");
			}
			else if (IsPunctuation(sb.ToString()))
			{
				printf("Expect tableName, now is %s\r\n", sb.ToString());
			}
			else
			{
				DB::TableDef *tab;
				NEW_CLASS(tab, DB::TableDef(sb.ToString()));
				sql = ParseNextWord(sql, &sb, svrType);
				if (sb.Equals((const UTF8Char*)"("))
				{
					DB::ColDef *col;
					while (true)
					{
						sql = ParseNextWord(sql, &sb, svrType);
						if (sb.GetLength() == 0)
						{
							printf("Expected column name\r\n");
							break;
						}
						else if (IsPunctuation(sb.ToString()))
						{
							printf("Expected column name, now is %s\r\n", sb.ToString());
							break;
						}
						NEW_CLASS(col, DB::ColDef(sb.ToString()));
						sql = ParseNextWord(sql, &sb, svrType);
						if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
						{
							printf("Expected column type, now is %s\r\n", sb.ToString());
							DEL_CLASS(col);
							break;
						}
						UOSInt colSize;
						DB::DBUtil::ColType colType = DB::DBUtil::ParseColType(svrType, sb.ToString(), &colSize);
						if (colType == DB::DBUtil::CT_Unknown)
						{
							printf("Unsupported column type: %s\r\n", sb.ToString());
							DEL_CLASS(col);
							break;
						}
						col->SetColType(colType);
						col->SetColSize(colSize);
						sql = ParseNextWord(sql, &sb, svrType);
						if (sb.Equals((const UTF8Char*)"primary"))
						{
							sql = ParseNextWord(sql, &sb, svrType);
							if (sb.Equals((const UTF8Char*)"key"))
							{
								col->SetPK(true);
								sql = ParseNextWord(sql, &sb, svrType);
							}
							else
							{
								printf("Expected 'key' after primary, now is %s\r\n", sb.ToString());
								DEL_CLASS(col);
								break;
							}
						}
						if (sb.Equals((const UTF8Char*)"autoincrement"))
						{
							col->SetAutoInc(true);
							sql = ParseNextWord(sql, &sb, svrType);
						}
						if (sb.Equals((const UTF8Char*)","))
						{
							tab->AddCol(col);
						}
						else if (sb.Equals((const UTF8Char*)")"))
						{
							tab->AddCol(col);
							NEW_CLASS(cmd, DB::SQL::CreateTableCommand(tab, true));
							tab = 0;
							break;
						}
						else
						{
							printf("Unknown word found: %s\r\n", sb.ToString());
							DEL_CLASS(col);
							break;
						}
					}
				}
				SDEL_CLASS(tab);
			}
			return cmd;
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
	return cmd;
}
