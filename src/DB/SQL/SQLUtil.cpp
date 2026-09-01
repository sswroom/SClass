#include "Stdafx.h"
#include "DB/SQL/SQLFunctionValue.h"
#include "DB/SQL/SQLObjectPath.h"
#include "DB/SQL/SQLUtil.h"
#include "DB/SQL/SQLValueBool.h"
#include "DB/SQL/SQLValueF64.h"
#include "DB/SQL/SQLValueI32.h"
#include "DB/SQL/SQLValueI64.h"
#include "DB/SQL/SQLValueNull.h"
#include "DB/SQL/SQLValueString.h"

UnsafeArray<const UTF8Char> DB::SQL::SQLUtil::ParseNextWord(UnsafeArray<const UTF8Char> sql, NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType)
{
	sb->ClearStr();
	UnsafeArrayOpt<const UTF8Char> strStart = nullptr;
	UnsafeArray<const UTF8Char> nns;
	UTF8Char endChar = 0;
	UTF8Char escChar = 0;
	UTF8Char c;
	Bool isNumber = false;
	while (true)
	{
		c = *sql++;
		if (c == 0)
		{
			sql--;
			if (strStart.SetTo(nns))
			{
				sb->AppendP(nns, sql);
			}
			return sql;
		}
		else if (endChar != 0)
		{
			if (c == endChar)
			{
				if (escChar == endChar && *sql == escChar)
				{
					sql++;
				}
				else if (strStart.SetTo(nns))
				{
					sb->AppendP(nns, sql);
					return sql;
				}
			}
			else if (c == escChar)
			{
				if (*sql != 0)
				{
					sql++;
				}
			}
		}
		else if (c == ';')
		{
			sql--;
			if (strStart.SetTo(nns))
			{
				sb->AppendP(nns, sql);
			}
			else
			{
				sb->AppendUTF8Char(c);
			}
			return sql;
		}
		else if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			if (strStart.SetTo(nns))
			{
				sb->AppendC(nns, (UIntOS)(sql - nns - 1));
				return sql;
			}
		}
		else if (c == '<')
		{
			if (strStart.SetTo(nns))
			{
				sb->AppendC(nns, (UIntOS)(sql - nns - 1));
				return sql - 1;
			}
			else
			{
				sb->AppendUTF8Char(c);
				if (*sql == '=' || *sql == '>')
				{
					sb->AppendUTF8Char(*sql++);
				}
				return sql;
			}
		}
		else if (c == '>')
		{
			if (strStart.SetTo(nns))
			{
				sb->AppendC(nns, (UIntOS)(sql - nns - 1));
				return sql - 1;
			}
			else
			{
				sb->AppendUTF8Char(c);
				if (*sql == '=')
				{
					sb->AppendUTF8Char(*sql++);
				}
				return sql;
			}
		}
		else if (c == '!' && *sql == '=')
		{
			if (strStart.SetTo(nns))
			{
				sb->AppendC(nns, (UIntOS)(sql - nns - 1));
				return sql - 1;
			}
			else
			{
				sb->AppendUTF8Char(c);
				sb->AppendUTF8Char(*sql++);
				return sql;
			}
		}
		else if (c == '(' || c == ')' || c == ',' || c == '=')
		{
			if (strStart.SetTo(nns))
			{
				sb->AppendC(nns, (UIntOS)(sql - nns - 1));
				return sql - 1;
			}
			else
			{
				sb->AppendUTF8Char(c);
				return sql;
			}
		}
		else if (c == '.' && !isNumber)
		{
			if (strStart.SetTo(nns))
			{
				sb->AppendC(nns, (UIntOS)(sql - nns - 1));
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
			if (strStart.IsNull())
			{
				strStart = sql - 1;
				isNumber = (c >= '0' && c <= '9');
				if (c == '\'')
				{
					endChar = '\'';
					if (sqlType == DB::SQLType::MySQL)
					{
						escChar = '\\';
					}
				}
				else if (c == '[' && sqlType == DB::SQLType::MSSQL)
				{
					endChar = ']';
				}
				else if (c == '`' && sqlType == DB::SQLType::MySQL)
				{
					endChar = '`';
				}
				else if (c == 'N' && sqlType == DB::SQLType::MSSQL && *sql == '\'')
				{
					sql++;
					endChar = '\'';
				}
				else if (c == '\"' && sqlType == DB::SQLType::PostgreSQL)
				{
					endChar = '\"';
				}
			}
		}
	}
}

Optional<DB::SQL::SQLValue> DB::SQL::SQLUtil::ParseNativeValue(Text::CStringNN val, DB::SQLType sqlType)
{
	NN<SQLValue> item;
	Int32 i32;
	Int64 i64;
	Double dblVal;
	if (val.leng > 1 && val.v[0] == '\'' && val.v[val.leng - 1] == '\'')
	{
		NEW_CLASSNN(item, DB::SQL::SQLValueString(Text::CStringNN(val.v + 1, val.leng - 2)));
		return item;
	}
	else if (val.ToInt32(i32))
	{
		NEW_CLASSNN(item, DB::SQL::SQLValueI32(i32));
		return item;
	}
	else if (val.ToInt64(i64))
	{
		NEW_CLASSNN(item, DB::SQL::SQLValueI64(i64));
		return item;
	}
	else if (val.ToDouble(dblVal))
	{
		NEW_CLASSNN(item, DB::SQL::SQLValueF64(dblVal));
		return item;
	}
	else if (val.EqualsICase(UTF8STRC("TRUE")))
	{
		NEW_CLASSNN(item, DB::SQL::SQLValueBool(true));
		return item;
	}
	else if (val.EqualsICase(UTF8STRC("FALSE")))
	{
		NEW_CLASSNN(item, DB::SQL::SQLValueBool(false));
		return item;
	}
	else if (val.EqualsICase(UTF8STRC("NULL")))
	{
		NEW_CLASSNN(item, DB::SQL::SQLValueNull());
		return item;
	}
	else
	{
		return nullptr;
	}
	return nullptr;
}

Optional<DB::SQL::SQLValue> DB::SQL::SQLUtil::ParseValueAndNext(InOutParam<UnsafeArray<const UTF8Char>> sql, NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType)
{
	UnsafeArray<const UTF8Char> nextSql;
	UnsafeArray<const UTF8Char> sqlPtr = sql.Get();
	sqlPtr = ParseNextWord(sqlPtr, sb, sqlType);
	if (sb->GetLength() == 0)
		return nullptr;
	NN<SQLValue> val;
	if (ParseNativeValue(sb->ToCString(), sqlType).SetTo(val))
	{
		sql.Set(sqlPtr);
		return val;
	}
	NN<SQLObjectPath> objPath;
	NN<SQLObjectPath> childPath;
	NEW_CLASSNN(objPath, SQLObjectPath(sb->ToCString(), nullptr));
	while (true)
	{
		nextSql = ParseNextWord(sqlPtr, sb, sqlType);
		if (sb->GetLength() == 0)
		{
			sql.Set(sqlPtr);
			objPath.Delete();
			return nullptr;
		}
		if (sb->Equals(UTF8STRC(".")))
		{
			sqlPtr = ParseNextWord(nextSql, sb, sqlType);
			if (sb->GetLength() == 0)
			{
				sql.Set(sqlPtr);
				objPath.Delete();
				return nullptr;
			}
			NEW_CLASSNN(childPath, SQLObjectPath(sb->ToCString(), objPath));
			objPath = childPath;
		}
		else if (sb->Equals(UTF8STRC("(")))
		{
			sqlPtr = nextSql;
			NN<SQLFunctionValue> func;
			NN<SQLValue> param;
			NEW_CLASSNN(func, SQLFunctionValue(objPath));
			if (!ParseValueAndNext(sqlPtr, sb, sqlType).SetTo(param))
			{
				sqlPtr = ParseNextWord(sqlPtr, sb, sqlType);
				if (!sb->Equals(UTF8STRC(")")))
				{
					sql.Set(sqlPtr);
					func.Delete();
					return nullptr;
				}
				sql.Set(sqlPtr);
				return func;
			}
			func->AddParam(param);
			while (true)
			{
				sqlPtr = ParseNextWord(sqlPtr, sb, sqlType);
				if (sb->Equals(UTF8STRC(")")))
				{
					sql.Set(sqlPtr);
					return func;
				}
				else if (!sb->Equals(UTF8STRC(",")))
				{
					sql.Set(sqlPtr);
					func.Delete();
					return nullptr;
				}
				if (!ParseValueAndNext(sqlPtr, sb, sqlType).SetTo(param))
				{
					sql.Set(sqlPtr);
					func.Delete();
					return nullptr;
				}
				func->AddParam(param);
			}
			return nullptr;
		}
		else
		{
			sql.Set(sqlPtr);
			return objPath;
		}
	}
	return nullptr;
}

void DB::SQL::SQLUtil::ParseColumnWord(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType)
{
	if (sqlType == DB::SQLType::MSSQL)
	{
		if (sb->StartsWith('[') && sb->EndsWith(']'))
		{
			sb->RemoveChars(1);
			sb->SetSubstr(1);
		}
	}
	else if (sqlType == DB::SQLType::MySQL)
	{
		if (sb->StartsWith('`') && sb->EndsWith('`'))
		{
			sb->RemoveChars(1);
			sb->SetSubstr(1);
		}
	}
	else if (sqlType == DB::SQLType::PostgreSQL)
	{
		if (sb->StartsWith('\"') && sb->EndsWith('\"'))
		{
			sb->RemoveChars(1);
			sb->SetSubstr(1);
		}
	}
}