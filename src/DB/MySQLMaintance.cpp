#include "Stdafx.h"
#include "DB/MySQLMaintance.h"
#include "DB/SQLBuilder.h"

DB::MySQLMaintance::MySQLMaintance(Net::MySQLTCPClient *cli, Bool needRelease)
{
	this->cli = cli;
	this->needRelease = needRelease;
}

DB::MySQLMaintance::~MySQLMaintance()
{
	if (this->needRelease)
	{
		SDEL_CLASS(this->cli);
	}
}

void DB::MySQLMaintance::RepairSchema(UnsafeArray<const UTF8Char> schema, NN<Text::StringBuilderUTF8> sb)
{
	Data::ArrayListStringNN tableNames;
	if (!this->cli->ChangeSchema(schema))
	{
		this->cli->GetLastErrorMsg(sb);
		sb->AppendC(UTF8STRC("\r\n"));
		return;
	}
	this->cli->QueryTableNames(CSTR_NULL, tableNames);
	Text::StringBuilderUTF8 sbDbg;
	UOSInt i = 0;
	UOSInt j = tableNames.GetCount();
	while (i < j)
	{
		this->RepairTable(Text::String::OrEmpty(tableNames.GetItem(i)), sb);
		i++;
	}
}

void DB::MySQLMaintance::RepairTable(NN<Text::String> tableName, NN<Text::StringBuilderUTF8> sb)
{
	DB::SQLBuilder sql(this->cli->GetSQLType(), this->cli->IsAxisAware(), this->cli->GetTzQhr());
	sql.AppendCmdC(CSTR("check table "));
	sql.AppendCol(tableName->v);
	NN<DB::DBReader> r;
	if (this->cli->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		UOSInt i;
		UOSInt j = r->ColCount();
		while (r->ReadNext())
		{
			i = 0;
			while (i < j)
			{
				if (i > 0)
					sb->AppendUTF8Char('\t');
				r->GetStr(i, sb);
				i++;
			}
			sb->AppendC(UTF8STRC("\r\n"));
		}
		this->cli->CloseReader(r);
	}
	else
	{
		sb->AppendC(UTF8STRC("Error in executing SQL: "));
		sb->AppendC(sql.ToString(), sql.GetLength());
		sb->AppendC(UTF8STRC("\r\n"));
		this->cli->GetLastErrorMsg(sb);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}
