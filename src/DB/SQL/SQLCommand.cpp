#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQL/CreateTableCommand.h"
#include "DB/SQL/SQLCommand.h"
#include "Text/CharUtil.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

const UTF8Char *DB::SQL::SQLCommand::ParseNextWord(const UTF8Char *sql, NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType)
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

Bool DB::SQL::SQLCommand::IsPunctuation(const UTF8Char *s)
{
	return Text::CharUtil::IsPunctuation(s[0]) && s[1] == 0;
}

DB::SQL::SQLCommand *DB::SQL::SQLCommand::Parse(const UTF8Char *sql, DB::SQLType sqlType)
{
	printf("SQLCommand: Cmd: %s\r\n", sql);
	DB::SQL::SQLCommand *cmd = 0;
	Text::StringBuilderUTF8 sb;
	sql = ParseNextWord(sql, sb, sqlType);
	if (sb.EqualsICase(UTF8STRC("CREATE")))
	{
		Bool isVirtual = false;
		sql = ParseNextWord(sql, sb, sqlType);
		if (sb.EqualsICase(UTF8STRC("VIRTUAL")))
		{
			isVirtual = true;
			sql = ParseNextWord(sql, sb, sqlType);
		}
		if (sb.EqualsICase(UTF8STRC("TABLE")))
		{
			sql = ParseNextWord(sql, sb, sqlType);
			if (sb.GetLength() == 0)
			{
				printf("SQLCommand: Missing table name\r\n");
			}
			else if (IsPunctuation(sb.ToString()))
			{
				printf("SQLCommand: Expect tableName, now is %s\r\n", sb.ToString());
			}
			else
			{
				DB::TableDef *tab;
				if (sqlType == DB::SQLType::SQLite && sb.StartsWith('\"') && sb.EndsWith('\"'))
				{
					sb.RemoveChars(1);
					NEW_CLASS(tab, DB::TableDef(CSTR_NULL, sb.ToCString().Substring(1)));
				}
				else
				{
					NEW_CLASS(tab, DB::TableDef(CSTR_NULL, sb.ToCString()));
				}
				sql = ParseNextWord(sql, sb, sqlType);
				if (sb.Equals(UTF8STRC("(")))
				{
					while (true)
					{
						sql = ParseNextWord(sql, sb, sqlType);
						if (sb.GetLength() == 0)
						{
							printf("SQLCommand: Expected column name\r\n");
							break;
						}
						else if (IsPunctuation(sb.ToString()))
						{
							printf("SQLCommand: Expected column name, now is %s\r\n", sb.ToString());
							break;
						}
						if (sb.EqualsICase(UTF8STRC("PRIMARY")) && sqlType == DB::SQLType::SQLite)
						{
							sql = ParseNextWord(sql, sb, sqlType);
							if (!sb.EqualsICase(UTF8STRC("KEY")))
							{
								printf("SQLCommand: Expected key after primary, now is %s\r\n", sb.ToString());
								break;
							}
							sql = ParseNextWord(sql, sb, sqlType);
							if (!sb.Equals(UTF8STRC("(")))
							{
								printf("SQLCommand: Expected '(' after primary key, now is %s\r\n", sb.ToString());
								break;
							}
							sql = ParseNextWord(sql, sb, sqlType);
							Bool err = false;
							UOSInt i;
							while (true)
							{
								if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
								{
									printf("SQLCommand: Expected column name in primary key list, now is %s\r\n", sb.ToString());
									err = true;
									break;
								}
								i = tab->GetColCnt();
								while (i-- > 0)
								{
									DB::ColDef *col;
									col = tab->GetCol(i);
									if (sb.Equals(col->GetColName()))
									{
										col->SetPK(true);
										break;
									}
								}
								sql = ParseNextWord(sql, sb, sqlType);
								if (sb.Equals(UTF8STRC(",")))
								{
									sql = ParseNextWord(sql, sb, sqlType);
								}
								else if (sb.Equals(UTF8STRC(")")))
								{
									sql = ParseNextWord(sql, sb, sqlType);
									break;
								}
								else
								{
									printf("SQLCommand: Unexpected word in primary key list: %s\r\n", sb.ToString());
									err = true;
									break;
								}
							}
							if (err)
							{
								break;
							}
							if (sb.Equals(UTF8STRC(",")))
							{
							}
							else if (sb.Equals(UTF8STRC(")")))
							{
								NEW_CLASS(cmd, DB::SQL::CreateTableCommand(tab, true));
								tab = 0;
								break;
							}
							else
							{
								printf("SQLCommand: Unknown word found: %s (After primary key)\r\n", sb.ToString());
								break;
							}
						}
						else if (sb.EqualsICase(UTF8STRC("CONSTRAINT")) && sqlType == DB::SQLType::SQLite)
						{
							UOSInt brkCnt = 0;
							while (true)
							{
								sql = ParseNextWord(sql, sb, sqlType);
								if (sql == 0)
								{
									break;
								}
								if (sb.Equals(UTF8STRC(",")) && brkCnt == 0)
								{
									break;
								}
								else if (sb.Equals(UTF8STRC("(")))
								{
									brkCnt++;
								}
								else if (sb.Equals(UTF8STRC(")")))
								{
									if (brkCnt == 0)
									{
										NEW_CLASS(cmd, DB::SQL::CreateTableCommand(tab, true));
										tab = 0;
										break;
									}
									brkCnt--;
								}
							}
							if (tab == 0)
							{
								break;
							}
						}
						else
						{
							UOSInt colSize;
							UOSInt colDP;
							NotNullPtr<DB::ColDef> col;
							if (sqlType == DB::SQLType::SQLite && sb.StartsWith('\"') && sb.EndsWith('\"'))
							{
								sb.RemoveChars(1);
								NEW_CLASSNN(col, DB::ColDef(sb.ToCString().Substring(1)));
							}
							else
							{
								NEW_CLASSNN(col, DB::ColDef(sb.ToCString()));
							}
							sql = ParseNextWord(sql, sb, sqlType);
							if (sqlType == DB::SQLType::SQLite && (sb.Equals(UTF8STRC(",")) || sb.Equals(UTF8STRC(")"))))
							{

							}
							else if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
							{
								printf("SQLCommand: Expected column type, now is %s\r\n", sb.ToString());
								col.Delete();
								break;
							}
							else
							{
								colSize = 0;
								colDP = 0;
								col->SetNativeType(sb.ToCString());
								DB::DBUtil::ColType colType = DB::DBUtil::ParseColType(sqlType, sb.ToString(), &colSize, &colDP);
								if (colType == DB::DBUtil::CT_Unknown)
								{
									printf("SQLCommand: Unsupported column type: %s\r\n", sb.ToString());
									col.Delete();
									break;
								}
								col->SetColType(colType);
								col->SetColSize(colSize);
								col->SetColDP(colDP);
								sql = ParseNextWord(sql, sb, sqlType);
							}
							if (sb.Equals(UTF8STRC("(")))
							{
								sql = ParseNextWord(sql, sb, sqlType);
								if (!sb.ToUOSInt(colSize))
								{
									printf("SQLCommand: Unexpected column size: %s\r\n", sb.ToString());
									col.Delete();
									break;
								}
								col->SetColSize(colSize);
								sql = ParseNextWord(sql, sb, sqlType);
								if (sb.Equals(UTF8STRC(",")))
								{
									sql = ParseNextWord(sql, sb, sqlType);
									if (!sb.ToUOSInt(colSize))
									{
										printf("SQLCommand: Unexpected column dp: %s\r\n", sb.ToString());
										col.Delete();
										break;
									}
									col->SetColDP(colSize);
									sql = ParseNextWord(sql, sb, sqlType);
								}
								if (!sb.Equals(UTF8STRC(")")))
								{
									printf("SQLCommand: Unexpected word %s, expected ')'\r\n", sb.ToString());
									col.Delete();
									break;
								}
								sql = ParseNextWord(sql, sb, sqlType);
							}

							Bool err = false;
							while (true)
							{
								if (sb.EqualsICase(UTF8STRC("CONSTRAINT")))
								{
									sql = ParseNextWord(sql, sb, sqlType);
									if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
									{
										printf("SQLCommand: Unexpected constraint name: %s\r\n", sb.ToString());
										err = true;
										break;
									}
									sql = ParseNextWord(sql, sb, sqlType);
								}
								else if (sb.EqualsICase(UTF8STRC("PRIMARY")))
								{
									sql = ParseNextWord(sql, sb, sqlType);
									if (sb.EqualsICase(UTF8STRC("KEY")))
									{
										col->SetPK(true);
										sql = ParseNextWord(sql, sb, sqlType);
										if (sb.EqualsICase(UTF8STRC("ASC")))
										{
											sql = ParseNextWord(sql, sb, sqlType);
										}
										else if (sb.EqualsICase(UTF8STRC("DESC")))
										{
											sql = ParseNextWord(sql, sb, sqlType);
										}
									}
									else
									{
										printf("SQLCommand: Expected 'key' after primary, now is %s\r\n", sb.ToString());
										err = true;
										break;
									}
								}
								else if (sb.EqualsICase(UTF8STRC("autoincrement")))
								{
									col->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
									sql = ParseNextWord(sql, sb, sqlType);
								}
								else if (sb.EqualsICase(UTF8STRC("UNIQUE")))
								{
									sql = ParseNextWord(sql, sb, sqlType);
								}
								else if (sb.EqualsICase(UTF8STRC("NOT")))
								{
									sql = ParseNextWord(sql, sb, sqlType);
									if (sb.EqualsICase(UTF8STRC("NULL")))
									{
										col->SetNotNull(true);
										sql = ParseNextWord(sql, sb, sqlType);
									}
									else
									{
										printf("SQLCommand: Expected 'NULL' after NOT, now is %s\r\n", sb.ToString());
										err = true;
										break;
									}
								}
								else if (sb.EqualsICase(UTF8STRC("default")))
								{
									sql = ParseNextWord(sql, sb, sqlType);
									if (sb.Equals(UTF8STRC("(")))
									{
										Text::StringBuilderUTF8 sbDef;
										UOSInt brkCnt = 1;
										while (true)
										{
											sql = ParseNextWord(sql, sb, sqlType);
											if (sb.GetLength() == 0)
											{
												printf("SQLCommand: Unexpected default value: %s\r\n", sb.ToString());
												err = true;
												break;
											}
											if (sb.Equals(UTF8STRC("(")))
											{
												sbDef.AppendUTF8Char('(');
												brkCnt++;
											}
											else if (sb.Equals(UTF8STRC(")")))
											{
												if (brkCnt == 1)
												{
													col->SetDefVal(sbDef.ToCString());
													break;
												}
												sbDef.AppendUTF8Char(')');
												brkCnt--;
											}
											else
											{
												sbDef.Append(sb.ToCString());
											}
										}
										if (err)
										{
											break;
										}
										sql = ParseNextWord(sql, sb, sqlType);
									}
									else if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
									{
										printf("SQLCommand: Unexpected default value: %s\r\n", sb.ToString());
										err = true;
										break;
									}
									else
									{
										col->SetDefVal(sb.ToCString());
										sql = ParseNextWord(sql, sb, sqlType);
									}
								}
								else if (sb.Equals(UTF8STRC(",")))
								{
									tab->AddCol(col);
									break;
								}
								else if (sb.Equals(UTF8STRC(")")))
								{
									tab->AddCol(col);
									NEW_CLASS(cmd, DB::SQL::CreateTableCommand(tab, true));
									tab = 0;
									break;
								}
								else
								{
									printf("SQLCommand: Unknown word found: %s (Column %s)\r\n", sb.ToString(), col->GetColName()->v);
									err = true;
									break;
								}
							}
							if (err)
							{
								col.Delete();
								break;
							}
							else if (tab == 0)
							{
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
			printf("SQLCommand: Unknown word after create: %s\r\n", sb.ToString());
		}
	}
	else
	{
		printf("SQLCommand: Unknown word: %s\r\n", sb.ToString());
	}
	return cmd;
}
