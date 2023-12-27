#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Win32/WMIQuery.h"

Int32 Win32::WMIQuery::securityCnt = 0;


void Win32::WMIQuery::InitQuery(const WChar *ns)
{
	this->pService = 0;
}

Win32::WMIQuery::WMIQuery() : DB::DBConn(CSTR("WMIQuery"))
{
	this->ns = Text::StrCopyNew(L"root\\WMI");
	this->InitQuery(this->ns);
}

Win32::WMIQuery::WMIQuery(const WChar *ns) : DB::DBConn(CSTR("WMIQuery"))
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

DB::SQLType Win32::WMIQuery::GetSQLType() const
{
	return DB::SQLType::Unknown;
}

DB::DBConn::ConnType Win32::WMIQuery::GetConnType() const
{
	return DB::DBConn::CT_WMIQUERY;
}

Int8 Win32::WMIQuery::GetTzQhr() const
{
	return Data::DateTimeUtil::GetLocalTzQhr();
}

void Win32::WMIQuery::ForceTz(Int8 tzQhr)
{
}

void Win32::WMIQuery::GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("WMI:"));
}

void Win32::WMIQuery::Close()
{
}

OSInt Win32::WMIQuery::ExecuteNonQuery(Text::CStringNN sql)
{
	this->lastDataError = DE_CONN_ERROR;
	return -2;
}

OSInt Win32::WMIQuery::ExecuteNonQueryW(const WChar *sql)
{
	this->lastDataError = DE_CONN_ERROR;
	return -2;
}

DB::DBReader *Win32::WMIQuery::ExecuteReader(Text::CStringNN sqlCmd)
{
	this->lastDataError = DE_CONN_ERROR;
	return 0;
}

DB::DBReader *Win32::WMIQuery::ExecuteReaderW(const WChar *sqlCmd)
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


UOSInt Win32::WMIQuery::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListStringNN> names)
{
	if (this->pService == 0)
	{
		return 0;
	}
	return 0;
}

DB::DBReader *Win32::WMIQuery::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	WChar wbuff[256];
	Text::StrUTF8_WChar(Text::StrConcat(wbuff, L"SELECT * FROM "), tableName.v, 0);
	return this->ExecuteReaderW(wbuff);
}

void Win32::WMIQuery::CloseReader(NotNullPtr<DB::DBReader> reader)
{
}

void Win32::WMIQuery::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
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
