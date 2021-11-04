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
					DB::ColDef *col = 0;
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
						if (sb.EqualsICase((const UTF8Char*)"PRIMARY") && svrType == DB::DBUtil::ServerType::SQLite)
						{
							sql = ParseNextWord(sql, &sb, svrType);
							if (!sb.EqualsICase((const UTF8Char*)"KEY"))
							{
								printf("Expected key after primary, now is %s\r\n", sb.ToString());
								break;
							}
							sql = ParseNextWord(sql, &sb, svrType);
							if (!sb.Equals((const UTF8Char*)"("))
							{
								printf("Expected '(' after primary key, now is %s\r\n", sb.ToString());
								break;
							}
							sql = ParseNextWord(sql, &sb, svrType);
							Bool err = false;
							UOSInt i;
							while (true)
							{
								if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
								{
									printf("Expected column name in primary key list, now is %s\r\n", sb.ToString());
									err = true;
									break;
								}
								i = tab->GetColCnt();
								while (i-- > 0)
								{
									col = tab->GetCol(i);
									if (sb.Equals(col->GetColName()))
									{
										col->SetPK(true);
										break;
									}
								}
								sql = ParseNextWord(sql, &sb, svrType);
								if (sb.Equals((const UTF8Char*)","))
								{
									sql = ParseNextWord(sql, &sb, svrType);
								}
								else if (sb.Equals((const UTF8Char*)")"))
								{
									sql = ParseNextWord(sql, &sb, svrType);
									break;
								}
								else
								{
									printf("Unexpected word in primary key list: %s\r\n", sb.ToString());
									err = true;
									break;
								}
							}
							if (err)
							{
								break;
							}
							if (sb.Equals((const UTF8Char*)","))
							{
							}
							else if (sb.Equals((const UTF8Char*)")"))
							{
								NEW_CLASS(cmd, DB::SQL::CreateTableCommand(tab, true));
								tab = 0;
								break;
							}
							else
							{
								printf("Unknown word found: %s (After primary key)\r\n", sb.ToString());
								DEL_CLASS(col);
								break;
							}
						}
						else
						{
							UOSInt colSize;
							NEW_CLASS(col, DB::ColDef(sb.ToString()));
							sql = ParseNextWord(sql, &sb, svrType);
							if (svrType == DB::DBUtil::ServerType::SQLite && (sb.Equals((const UTF8Char*)",") || sb.Equals((const UTF8Char*)")")))
							{

							}
							else if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
							{
								printf("Expected column type, now is %s\r\n", sb.ToString());
								DEL_CLASS(col);
								break;
							}
							else
							{
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
							}
							if (sb.Equals((const UTF8Char*)"("))
							{
								sql = ParseNextWord(sql, &sb, svrType);
								if (!sb.ToUOSInt(&colSize))
								{
									printf("Unexpected column size: %s\r\n", sb.ToString());
									DEL_CLASS(col);
									break;
								}
								col->SetColSize(colSize);
								sql = ParseNextWord(sql, &sb, svrType);
								if (sb.Equals((const UTF8Char*)","))
								{
									sql = ParseNextWord(sql, &sb, svrType);
									if (!sb.ToUOSInt(&colSize))
									{
										printf("Unexpected column dp: %s\r\n", sb.ToString());
										DEL_CLASS(col);
										break;
									}
									col->SetColDP(colSize);
									sql = ParseNextWord(sql, &sb, svrType);
								}
								if (!sb.Equals((const UTF8Char*)")"))
								{
									printf("Unexpected word %s, expected ')'\r\n", sb.ToString());
									DEL_CLASS(col);
									break;
								}
								sql = ParseNextWord(sql, &sb, svrType);
							}

							if (sb.EqualsICase((const UTF8Char*)"CONSTRAINT"))
							{
								sql = ParseNextWord(sql, &sb, svrType);
								if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
								{
									printf("Unexpected constraint name: %s\r\n", sb.ToString());
									DEL_CLASS(col);
									break;
								}
								sql = ParseNextWord(sql, &sb, svrType);
							}
							if (sb.EqualsICase((const UTF8Char*)"PRIMARY"))
							{
								sql = ParseNextWord(sql, &sb, svrType);
								if (sb.EqualsICase((const UTF8Char*)"KEY"))
								{
									col->SetPK(true);
									sql = ParseNextWord(sql, &sb, svrType);
									if (sb.EqualsICase((const UTF8Char*)"ASC"))
									{
										sql = ParseNextWord(sql, &sb, svrType);
									}
									else if (sb.EqualsICase((const UTF8Char*)"DESC"))
									{
										sql = ParseNextWord(sql, &sb, svrType);
									}
								}
								else
								{
									printf("Expected 'key' after primary, now is %s\r\n", sb.ToString());
									DEL_CLASS(col);
									break;
								}
							}
							if (sb.EqualsICase((const UTF8Char*)"autoincrement"))
							{
								col->SetAutoInc(true);
								sql = ParseNextWord(sql, &sb, svrType);
							}
							else if (sb.EqualsICase((const UTF8Char*)"default"))
							{
								sql = ParseNextWord(sql, &sb, svrType);
								if (sb.Equals((const UTF8Char*)"("))
								{
									sql = ParseNextWord(sql, &sb, svrType);
									if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
									{
										printf("Unexpected default value: %s\r\n", sb.ToString());
										DEL_CLASS(col);
										break;
									}
									col->SetDefVal(sb.ToString());
									sql = ParseNextWord(sql, &sb, svrType);
									if (!sb.Equals((const UTF8Char*)")"))
									{
										printf("Unexpected end of default value: %s\r\n", sb.ToString());
										DEL_CLASS(col);
										break;
									}
									sql = ParseNextWord(sql, &sb, svrType);
								}
								else if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
								{
									printf("Unexpected default value: %s\r\n", sb.ToString());
									DEL_CLASS(col);
									break;
								}
								else
								{
									col->SetDefVal(sb.ToString());
									sql = ParseNextWord(sql, &sb, svrType);
								}
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
								printf("Unknown word found: %s (Column %s)\r\n", sb.ToString(), col->GetColName());
								DEL_CLASS(col);
								break;
							}
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
