#include "Stdafx.h"
#include "DB/SQL/SQLUtil.h"

const UTF8Char *DB::SQL::SQLUtil::ParseNextWord(const UTF8Char *sql, NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType)
{
	sb->ClearStr();
	const UTF8Char *strStart = 0;
	UTF8Char endChar = 0;
	UTF8Char escChar = 0;
	UTF8Char c;
	while (true)
	{
		c = *sql++;
		if (c == 0)
		{
			sql--;
			if (strStart)
			{
				sb->AppendP(strStart, sql);
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
				else
				{
					sb->AppendP(strStart, sql);
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
			if (strStart)
			{
				sb->AppendC(strStart, (UOSInt)(sql - strStart - 1));
				return sql;
			}
		}
		else if (c == '(' || c == ')' || c == ',' || c == '=')
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

Data::VariItem *DB::SQL::SQLUtil::ParseValue(Text::CStringNN val, DB::SQLType sqlType)
{
	NotNullPtr<Data::VariItem> item;
	if (val.leng > 1 && val.v[0] == '\'' && val.v[val.leng - 1] == '\'')
	{
		NotNullPtr<Text::String> s = Text::String::New(val.v + 1, val.leng - 2);
		item = Data::VariItem::NewStr(s.Ptr());
		s->Release();
		return item.Ptr();
	}
	return 0;
}
