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

void DB::MySQLMaintance::RepairSchema(const UTF8Char *schema, Text::StringBuilderUTF *sb)
{
	Data::ArrayList<const UTF8Char*> tableNames;
	if (!this->cli->ChangeSchema(schema))
	{
		this->cli->GetErrorMsg(sb);
		sb->Append((const UTF8Char*)"\r\n");
		return;
	}
	this->cli->GetTableNames(&tableNames);
	Text::StringBuilderUTF8 sbDbg;
	UOSInt i = 0;
	UOSInt j = tableNames.GetCount();
	while (i < j)
	{
		this->RepairTable(tableNames.GetItem(i), sb);
		i++;
	}
}

void DB::MySQLMaintance::RepairTable(const UTF8Char *tableName, Text::StringBuilderUTF *sb)
{
	DB::SQLBuilder sql(this->cli->GetSvrType(), this->cli->GetTzQhr());
	sql.AppendCmd((const UTF8Char*)"check table ");
	sql.AppendCol(tableName);
	DB::DBReader *r = this->cli->ExecuteReader(sql.ToString());
	if (r)
	{
		UOSInt i;
		UOSInt j = r->ColCount();
		while (r->ReadNext())
		{
			i = 0;
			while (i < j)
			{
				if (i > 0)
					sb->AppendChar('\t', 1);
				r->GetStr(i, sb);
				i++;
			}
			sb->Append((const UTF8Char*)"\r\n");
		}
		this->cli->CloseReader(r);
	}
	else
	{
		sb->Append((const UTF8Char*)"Error in executing SQL: ");
		sb->Append(sql.ToString());
		sb->Append((const UTF8Char*)"\r\n");
		this->cli->GetErrorMsg(sb);
		sb->Append((const UTF8Char*)"\r\n");
	}
}