#include "Stdafx.h"
#include "DB/SQL/SQLUtil.h"

UnsafeArray<const UTF8Char> DB::SQL::SQLUtil::ParseNextWord(UnsafeArray<const UTF8Char> sql, NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType)
{
	sb->ClearStr();
	UnsafeArrayOpt<const UTF8Char> strStart = nullptr;
	UnsafeArray<const UTF8Char> nns;
	UTF8Char endChar = 0;
	UTF8Char escChar = 0;
	UTF8Char c;
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
		else if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			if (strStart.SetTo(nns))
			{
				sb->AppendC(nns, (UOSInt)(sql - nns - 1));
				return sql;
			}
		}
		else if (c == '(' || c == ')' || c == ',' || c == '=')
		{
			if (strStart.SetTo(nns))
			{
				sb->AppendC(nns, (UOSInt)(sql - nns - 1));
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

Optional<Data::VariItem> DB::SQL::SQLUtil::ParseValue(Text::CStringNN val, DB::SQLType sqlType)
{
	NN<Data::VariItem> item;
	Int32 i32;
	Int64 i64;
	Double dblVal;
	if (val.leng > 1 && val.v[0] == '\'' && val.v[val.leng - 1] == '\'')
	{
		NN<Text::String> s = Text::String::New(val.v + 1, val.leng - 2);
		item = Data::VariItem::NewStr(s);
		s->Release();
		return item;
	}
	else if (val.ToInt32(i32))
	{
		return Data::VariItem::NewI32(i32);
	}
	else if (val.ToInt64(i64))
	{
		return Data::VariItem::NewI64(i64);
	}
	else if (val.ToDouble(dblVal))
	{
		return Data::VariItem::NewF64(dblVal);
	}
	return nullptr;
}
