#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQL/SQLCommand.h"
#include "DB/SQL/SQLCreateTableCommand.h"
#include "DB/SQL/SQLInsertCommand.h"
#include "DB/SQL/SQLSelectCommand.h"
#include "DB/SQL/SQLSetConfigCommand.h"
#include "DB/SQL/SQLShowDatabasesCommand.h"
#include "DB/SQL/SQLUseCommand.h"
#include "DB/SQL/SQLUtil.h"
#include "Text/CharUtil.h"
#include "Text/StringBuilderUTF8.h"

//#define VERBOSE
#include <stdio.h>

Bool DB::SQL::SQLCommand::IsPunctuation(UnsafeArray<const UTF8Char> s)
{
	return Text::CharUtil::IsPunctuation(s[0]) && s[1] == 0;
}

void DB::SQL::SQLCommand::ParseColumnName(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType)
{
	UTF8Char c = sb->v[0];
	if (c == '\"' && sqlType == DB::SQLType::PostgreSQL && sb->EndsWith('\"') && sb->leng > 1)
	{
		sb->RemoveChars(1);
		sb->SetSubstr(1);
	}
}

Optional<DB::SQL::SQLCommand> DB::SQL::SQLCommand::Parse(UnsafeArray<const UTF8Char> sql, DB::SQLType sqlType)
{
#if defined(VERBOSE)
	printf("SQLCommand: Cmd: %s\r\n", sql);
#endif
	Optional<DB::SQL::SQLCommand> cmd = nullptr;
	Text::StringBuilderUTF8 sb;
	sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
	if (sb.EqualsICase(UTF8STRC("CREATE")))
	{
//		Bool isVirtual = false;
		sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (sb.EqualsICase(UTF8STRC("VIRTUAL")))
		{
//			isVirtual = true;
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		}
		if (sb.EqualsICase(UTF8STRC("TABLE")))
		{
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (sb.GetLength() == 0)
			{
				printf("SQLCommand: Missing table name\r\n");
			}
			else if (IsPunctuation(sb.ToString()))
			{
				printf("SQLCommand: Expect tableName, now is %s\r\n", sb.ToPtr());
			}
			else
			{
				NN<DB::TableDef> tab;
				Bool tabUsed = false;
				if (sqlType == DB::SQLType::SQLite && sb.StartsWith('\"') && sb.EndsWith('\"'))
				{
					sb.RemoveChars(1);
					NEW_CLASSNN(tab, DB::TableDef(nullptr, sb.ToCString().Substring(1)));
				}
				else
				{
					NEW_CLASSNN(tab, DB::TableDef(nullptr, sb.ToCString()));
				}
				sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
				if (sb.Equals(UTF8STRC("(")))
				{
					while (true)
					{
						sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
						if (sb.GetLength() == 0)
						{
							printf("SQLCommand: Expected column name\r\n");
							break;
						}
						else if (IsPunctuation(sb.ToString()))
						{
							printf("SQLCommand: Expected column name, now is %s\r\n", sb.ToPtr());
							break;
						}
						if (sb.EqualsICase(UTF8STRC("PRIMARY")) && sqlType == DB::SQLType::SQLite)
						{
							sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
							if (!sb.EqualsICase(UTF8STRC("KEY")))
							{
								printf("SQLCommand: Expected key after primary, now is %s\r\n", sb.ToPtr());
								break;
							}
							sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
							if (!sb.Equals(UTF8STRC("(")))
							{
								printf("SQLCommand: Expected '(' after primary key, now is %s\r\n", sb.ToPtr());
								break;
							}
							sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
							Bool err = false;
							Data::ArrayIterator<NN<DB::ColDef>> it;
							while (true)
							{
								if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
								{
									printf("SQLCommand: Expected column name in primary key list, now is %s\r\n", sb.ToPtr());
									err = true;
									break;
								}
								it = tab->ColIterator();
								while (it.HasNext())
								{
									NN<DB::ColDef> col;
									col = it.Next();
									if (sb.Equals(col->GetColName()))
									{
										col->SetPK(true);
										break;
									}
								}
								sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
								if (sb.Equals(UTF8STRC(",")))
								{
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
								}
								else if (sb.Equals(UTF8STRC(")")))
								{
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
									break;
								}
								else
								{
									printf("SQLCommand: Unexpected word in primary key list: %s\r\n", sb.ToPtr());
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
								NEW_CLASSOPT(cmd, DB::SQL::SQLCreateTableCommand(tab, true));
								tabUsed = true;
								break;
							}
							else
							{
								printf("SQLCommand: Unknown word found: %s (After primary key)\r\n", sb.ToPtr());
								break;
							}
						}
						else if (sb.EqualsICase(UTF8STRC("CONSTRAINT")) && sqlType == DB::SQLType::SQLite)
						{
							UIntOS brkCnt = 0;
							while (true)
							{
								sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
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
										NEW_CLASSOPT(cmd, DB::SQL::SQLCreateTableCommand(tab, true));
										tabUsed = true;
										break;
									}
									brkCnt--;
								}
								if (*sql == 0)
								{
									break;
								}
							}
							if (tabUsed)
							{
								break;
							}
						}
						else
						{
							UIntOS colSize;
							UIntOS colDP;
							NN<DB::ColDef> col;
							if (sqlType == DB::SQLType::SQLite && sb.StartsWith('\"') && sb.EndsWith('\"'))
							{
								sb.RemoveChars(1);
								NEW_CLASSNN(col, DB::ColDef(sb.ToCString().Substring(1)));
							}
							else
							{
								NEW_CLASSNN(col, DB::ColDef(sb.ToCString()));
							}
							sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
							if (sqlType == DB::SQLType::SQLite && (sb.Equals(UTF8STRC(",")) || sb.Equals(UTF8STRC(")"))))
							{

							}
							else if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
							{
								printf("SQLCommand: Expected column type, now is %s\r\n", sb.ToPtr());
								col.Delete();
								break;
							}
							else
							{
								colSize = 0;
								colDP = 0;
								col->SetNativeType(sb.ToCString());
								DB::DBUtil::ColType colType = DB::DBUtil::ParseColType(sqlType, sb.ToString(), colSize, colDP);
								if (colType == DB::DBUtil::CT_Unknown)
								{
									printf("SQLCommand: Unsupported column type: %s\r\n", sb.ToPtr());
									col.Delete();
									break;
								}
								col->SetColType(colType);
								col->SetColSize(colSize);
								col->SetColDP(colDP);
								sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
							}
							if (sb.Equals(UTF8STRC("(")))
							{
								sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
								if (!sb.ToUIntOS(colSize))
								{
									printf("SQLCommand: Unexpected column size: %s\r\n", sb.ToPtr());
									col.Delete();
									break;
								}
								col->SetColSize(colSize);
								sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
								if (sb.Equals(UTF8STRC(",")))
								{
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
									if (!sb.ToUIntOS(colSize))
									{
										printf("SQLCommand: Unexpected column dp: %s\r\n", sb.ToPtr());
										col.Delete();
										break;
									}
									col->SetColDP(colSize);
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
								}
								if (!sb.Equals(UTF8STRC(")")))
								{
									printf("SQLCommand: Unexpected word %s, expected ')'\r\n", sb.ToPtr());
									col.Delete();
									break;
								}
								sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
							}

							Bool err = false;
							while (true)
							{
								if (sb.EqualsICase(UTF8STRC("CONSTRAINT")))
								{
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
									if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
									{
										printf("SQLCommand: Unexpected constraint name: %s\r\n", sb.ToPtr());
										err = true;
										break;
									}
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
								}
								else if (sb.EqualsICase(UTF8STRC("PRIMARY")))
								{
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
									if (sb.EqualsICase(UTF8STRC("KEY")))
									{
										col->SetPK(true);
										sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
										if (sb.EqualsICase(UTF8STRC("ASC")))
										{
											sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
										}
										else if (sb.EqualsICase(UTF8STRC("DESC")))
										{
											sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
										}
									}
									else
									{
										printf("SQLCommand: Expected 'key' after primary, now is %s\r\n", sb.ToPtr());
										err = true;
										break;
									}
								}
								else if (sb.EqualsICase(UTF8STRC("autoincrement")))
								{
									col->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
								}
								else if (sb.EqualsICase(UTF8STRC("UNIQUE")))
								{
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
								}
								else if (sb.EqualsICase(UTF8STRC("NOT")))
								{
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
									if (sb.EqualsICase(UTF8STRC("NULL")))
									{
										col->SetNotNull(true);
										sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
									}
									else
									{
										printf("SQLCommand: Expected 'NULL' after NOT, now is %s\r\n", sb.ToPtr());
										err = true;
										break;
									}
								}
								else if (sb.EqualsICase(UTF8STRC("default")))
								{
									sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
									if (sb.Equals(UTF8STRC("(")))
									{
										Text::StringBuilderUTF8 sbDef;
										UIntOS brkCnt = 1;
										while (true)
										{
											sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
											if (sb.GetLength() == 0)
											{
												printf("SQLCommand: Unexpected default value: %s\r\n", sb.ToPtr());
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
										sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
									}
									else if (sb.GetLength() == 0 || IsPunctuation(sb.ToString()))
									{
										printf("SQLCommand: Unexpected default value: %s\r\n", sb.ToPtr());
										err = true;
										break;
									}
									else
									{
										col->SetDefVal(sb.ToCString());
										sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
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
									NEW_CLASSOPT(cmd, DB::SQL::SQLCreateTableCommand(tab, true));
									tabUsed = true;
									break;
								}
								else
								{
									printf("SQLCommand: Unknown word found: %s (Column %s)\r\n", sb.ToPtr(), col->GetColName()->v.Ptr());
									err = true;
									break;
								}
							}
							if (err)
							{
								col.Delete();
								break;
							}
							else if (tabUsed)
							{
								break;
							}
						}
					}
				}
				if (!tabUsed)
				{
					tab.Delete();
				}
			}
			return cmd;
		}
		else
		{
			printf("SQLCommand: Unknown word after create: %s\r\n", sb.ToPtr());
		}
	}
	else if (sb.EqualsICase(UTF8STRC("SHOW")) && sqlType == DB::SQLType::MySQL)
	{
		sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (sb.EqualsICase(UTF8STRC("DATABASES")))
		{
			NEW_CLASSOPT(cmd, DB::SQL::SQLShowDatabasesCommand());
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (sb.leng == 0)
			{
			}
			else
			{
				printf("SQLCommand: Unknown word after show database: %s\r\n", sb.ToPtr());
			}
		}
		else
		{
			printf("SQLCommand: Unknown word after show: %s\r\n", sb.ToPtr());
		}
	}
	else if (sb.EqualsICase(UTF8STRC("USE")))
	{
		sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (sb.GetLength() == 0)
		{
			printf("SQLCommand: Missing database name\r\n");
		}
		else if (IsPunctuation(sb.ToString()))
		{
			printf("SQLCommand: Expect database name, now is %s\r\n", sb.ToPtr());
		}
		else
		{
			SQLUtil::ParseColumnWord(sb, sqlType);
			NEW_CLASSOPT(cmd, DB::SQL::SQLUseCommand(sb.ToCString()));
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (sb.leng == 0)
			{
			}
			else
			{
				printf("SQLCommand: Unknown word after use: %s\r\n", sb.ToPtr());
			}
		}
	}
	else if (sb.EqualsICase(UTF8STRC("SET")))
	{
		sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (sb.GetLength() == 0)
		{
			printf("SQLCommand: Missing database name\r\n");
			return nullptr;
		}
		DB::SQL::SQLSetConfigCommand::ConfigLife cfgLife = DB::SQL::SQLSetConfigCommand::ConfigLife::Default;
		if (sb.EqualsICase(UTF8STRC("SESSION")))
		{
			cfgLife = DB::SQL::SQLSetConfigCommand::ConfigLife::Session;
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		}
		else if (sb.EqualsICase(UTF8STRC("LOCAL")))
		{
			cfgLife = DB::SQL::SQLSetConfigCommand::ConfigLife::Local;
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		}
		if (sb.GetLength() == 0)
		{
			printf("SQLCommand: Missing config name\r\n");
			return nullptr;
		}
		NN<Text::String> cfgName = Text::String::New(sb.ToCString());
		sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (!sb.Equals(UTF8STRC("=")) && !sb.EqualsICase(UTF8STRC("TO")))
		{
			printf("SQLCommand: Expected '=' or 'TO', now is %s\r\n", sb.ToPtr());
			cfgName->Release();
			return nullptr;
		}
		Text::CString cfgVal = nullptr;
		sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (sb.GetLength() == 0)
		{
			printf("SQLCommand: Missing config value\r\n");
			cfgName->Release();
			return nullptr;
		}
		else if (sb.EqualsICase(UTF8STRC("DEFAULT")))
		{
		}
		else
		{
			cfgVal = sb.ToCString();
		}
		NEW_CLASSOPT(cmd, DB::SQL::SQLSetConfigCommand(cfgLife, cfgName, cfgVal));
		cfgName->Release();
	}
	else if (sb.EqualsICase(UTF8STRC("SELECT")))
	{
		NN<SQLSelectCommand> selCmd;
		NEW_CLASSNN(selCmd, DB::SQL::SQLSelectCommand());
		cmd = selCmd;
		NN<SQLValue> val;
		while (true)
		{
			if (!SQLUtil::ParseValueAndNext(sql, sb, sqlType).SetTo(val))
			{
				printf("SQLCommand: Select list is not supported\r\n");
				cmd.Delete();
				return nullptr;
			}
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (sb.Equals(UTF8STRC(",")))
			{
				selCmd->AddColumn(val, nullptr);
			}
			else if (sb.EqualsICase(UTF8STRC("FROM")) || sb.EqualsICase(UTF8STRC("WHERE")) || sb.EqualsICase(UTF8STRC("GROUP")) || sb.EqualsICase(UTF8STRC("ORDER")) || sb.EqualsICase(UTF8STRC("LIMIT")))
			{
				selCmd->AddColumn(val, nullptr);
				break;
			}
			else if (sb.Equals(UTF8STRC(";")) || sb.Equals(UTF8STRC(")")))
			{
				selCmd->AddColumn(val, nullptr);
				return selCmd;
			}
			else
			{
				if (sb.EqualsICase(UTF8STRC("AS")))
				{
					sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
				}
				if (sb.GetLength() == 0)
				{
					printf("SQLCommand: Missing column alias\r\n");
					cmd.Delete();
					val.Delete();
					return nullptr;
				}
				else if (IsPunctuation(sb.ToString()))
				{
					printf("SQLCommand: Expect column alias, now is %s\r\n", sb.ToPtr());
					cmd.Delete();
					val.Delete();
					return nullptr;
				}
				else
				{
					selCmd->AddColumn(val, sb.ToCString());
				}
			}
		}
		printf("SQLCommand: Not support select command after select list: %s\r\n", sb.v.Ptr());
		cmd.Delete();
		return nullptr;
	}
	else if (sb.EqualsICase(UTF8STRC("INSERT")))
	{
		sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (!sb.EqualsICase(UTF8STRC("INTO")))
		{
			printf("SQLCommand: Expected 'INTO' after INSERT, now is %s\r\n", sb.ToPtr());
			return nullptr;
		}
		sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (sb.GetLength() == 0)
		{
			printf("SQLCommand: Missing table name after INSERT\r\n");
			return nullptr;
		}
		NN<DB::SQL::SQLObjectPath> objPath;
		NN<DB::SQL::SQLObjectPath> childPath;
		NEW_CLASSNN(objPath, DB::SQL::SQLObjectPath(sb.ToCString(), nullptr));
		while (true)
		{
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (sb.Equals(UTF8STRC(".")))
			{
				sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
				if (sb.GetLength() == 0)
				{
					objPath.Delete();
					return nullptr;
				}
				NEW_CLASSNN(childPath, SQLObjectPath(sb.ToCString(), objPath));
				objPath = childPath;
			}
			else if (sb.Equals(UTF8STRC("(")))
			{
				break;
			}
			else
			{
				printf("SQLCommand: Unknown word after table name: %s\r\n", sb.ToPtr());
				objPath.Delete();
				return nullptr;
			}
		}

		NN<SQLInsertCommand> insCmd;
		NEW_CLASSNN(insCmd, DB::SQL::SQLInsertCommand(objPath));
		cmd = insCmd;
		while (true)
		{
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (sb.GetLength() == 0)
			{
				printf("SQLCommand: Missing table name\r\n");
				cmd.Delete();
				return nullptr;
			}
			ParseColumnName(sb, sqlType);
			insCmd->AddColumn(sb.ToCString());
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (sb.Equals(UTF8STRC(",")))
			{
			}
			else if (sb.Equals(UTF8STRC(")")))
			{
				break;
			}
			else
			{
				printf("SQLCommand: Unknown word after column name: %s\r\n", sb.ToPtr());
				cmd.Delete();
				return nullptr;
			}
		}
		sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
		if (!sb.EqualsICase(UTF8STRC("VALUES")))
		{
			printf("SQLCommand: Expected 'VALUES' after column list, now is %s\r\n", sb.ToPtr());
			cmd.Delete();
			return nullptr;
		}
		Data::ArrayListNN<SQLValue> row;
		while (true)
		{
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (!sb.Equals(UTF8STRC("(")))
			{
				printf("SQLCommand: Expected '(' for value list, now is %s\r\n", sb.ToPtr());
				cmd.Delete();
				return nullptr;
			}
			while (true)
			{
				NN<SQLValue> val;
				if (!SQLUtil::ParseValueAndNext(sql, sb, sqlType).SetTo(val))
				{
					printf("SQLCommand: Value list is not supported\r\n");
					cmd.Delete();
					return nullptr;
				}
				row.Add(val);
				sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
				if (sb.Equals(UTF8STRC(",")))
				{
				}
				else if (sb.Equals(UTF8STRC(")")))
				{
					break;
				}
				else
				{
					printf("SQLCommand: Unknown word after value: %s\r\n", sb.ToPtr());
					cmd.Delete();
					row.DeleteAll();
					return nullptr;
				}
			}
			if (row.GetCount() != insCmd->GetColumnCount())
			{
				printf("SQLCommand: Value count %d not match column count %d\r\n", (UInt32)row.GetCount(), (UInt32)insCmd->GetColumnCount());
				cmd.Delete();
				row.DeleteAll();
				return nullptr;
			}
			insCmd->AddRow(row);
			row.Clear();
			sql = SQLUtil::ParseNextWord(sql, sb, sqlType);
			if (sb.Equals(UTF8STRC(",")))
			{
			}
			else if (sb.Equals(UTF8STRC(";")))
			{
				return insCmd;
			}
			else
			{
				printf("SQLCommand: Unknown word after value list: %s\r\n", sb.ToPtr());
				cmd.Delete();
				return nullptr;
			}
		}
	}
	else
	{
		printf("SQLCommand: Unknown word: %s\r\n", sb.ToPtr());
	}
	return cmd;
}
