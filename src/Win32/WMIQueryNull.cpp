#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Win32/WMIQuery.h"

Int32 Win32::WMIQuery::securityCnt = 0;


void Win32::WMIQuery::InitQuery(const WChar *ns)
{
	this->pService = 0;
	this->tabNames = 0;
}

Win32::WMIQuery::WMIQuery() : DB::DBConn((const UTF8Char*)"WMIQuery")
{
	this->ns = Text::StrCopyNew(L"root\\WMI");
	this->InitQuery(this->ns);
}

Win32::WMIQuery::WMIQuery(const WChar *ns) : DB::DBConn((const UTF8Char*)"WMIQuery")
{
	this->ns = Text::StrCopyNew(ns);
	this->InitQuery(this->ns);
}

Win32::WMIQuery::~WMIQuery()
{
	SDEL_TEXT(this->ns);
}

Bool Win32::WMIQuery::IsError()
{
	return this->pService == 0;
}

DB::DBUtil::ServerType Win32::WMIQuery::GetSvrType()
{
	return DB::DBUtil::ServerType::Unknown;
}

DB::DBConn::ConnType Win32::WMIQuery::GetConnType()
{
	return DB::DBConn::CT_WMIQUERY;
}

Int8 Win32::WMIQuery::GetTzQhr()
{
	return Data::DateTime::GetLocalTzQhr();
}

void Win32::WMIQuery::ForceTz(Int8 tzQhr)
{
}

void Win32::WMIQuery::GetConnName(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"WMI:");
}

void Win32::WMIQuery::Close()
{
}

OSInt Win32::WMIQuery::ExecuteNonQuery(const UTF8Char *sql)
{
	this->lastDataError = DE_CONN_ERROR;
	return -2;
}

OSInt Win32::WMIQuery::ExecuteNonQuery(const WChar *sql)
{
	this->lastDataError = DE_CONN_ERROR;
	return -2;
}

DB::DBReader *Win32::WMIQuery::ExecuteReader(const UTF8Char *sqlCmd)
{
	this->lastDataError = DE_CONN_ERROR;
	return 0;
}

DB::DBReader *Win32::WMIQuery::ExecuteReader(const WChar *sqlCmd)
{
	this->lastDataError = DE_CONN_ERROR;
	return 0;
}

Bool Win32::WMIQuery::IsLastDataError()
{
	return false;
}

void *Win32::WMIQuery::BeginTransaction()
{
	return 0;
}

void Win32::WMIQuery::Commit(void *tran)
{
}

void Win32::WMIQuery::Rollback(void *tran)
{
}


UOSInt Win32::WMIQuery::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	if (this->pService == 0)
	{
		return 0;
	}
	return 0;
}

DB::DBReader *Win32::WMIQuery::GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, DB::QueryConditions *condition)
{
	WChar sbuff[256];
	Text::StrUTF8_WChar(Text::StrConcat(sbuff, L"SELECT * FROM "), tableName, 0);
	return this->ExecuteReader(sbuff);
}

void Win32::WMIQuery::CloseReader(DB::DBReader *reader)
{
}

void Win32::WMIQuery::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

void Win32::WMIQuery::Reconnect()
{
}

const WChar *Win32::WMIQuery::GetNS()
{
	return this->ns;
}

UOSInt Win32::WMIQuery::GetNSList(Data::ArrayList<const WChar *> *nsList)
{
	UOSInt ret = 0;
	return ret;
}

void Win32::WMIQuery::FreeNSList(Data::ArrayList<const WChar *> *nsList)
{
	UOSInt i = nsList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(nsList->GetItem(i));
	}
	nsList->Clear();
}
