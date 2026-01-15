#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "DB/DBTool.h"
#include "DB/OLEDBConn.h"
#include "Math/Math_C.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

struct DB::OLEDBConn::ClassData
{
	NN<IO::LogTool> log;
	UnsafeArrayOpt<const WChar> connStr;
};

DB::OLEDBConn::OLEDBConn(NN<IO::LogTool> log) : DB::DBConn(CSTR("OLEDBConn"))
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->log = log;
	data->connStr = nullptr;
}

void DB::OLEDBConn::Init(UnsafeArray<const WChar> connStr)
{
	UnsafeArray<const WChar> nnconnStr;
	if (this->clsData->connStr.SetTo(nnconnStr)) Text::StrDelNew(nnconnStr);
	this->clsData->connStr = Text::StrCopyNew(connStr);
	this->connErr = CE_COCREATE;
}

DB::OLEDBConn::OLEDBConn(UnsafeArray<const WChar> connStr, NN<IO::LogTool> log) : DB::DBConn(CSTR("OLEDBConn"))
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->log = log;
	data->connStr = Text::StrCopyNew(connStr);
	this->Init(connStr);
}

DB::OLEDBConn::~OLEDBConn()
{
	UnsafeArray<const WChar> connStr;
	if (this->clsData->connStr.SetTo(connStr)) Text::StrDelNew(connStr);
	MemFree(this->clsData);
}

DB::SQLType DB::OLEDBConn::GetSQLType() const
{
	return DB::SQLType::Unknown;
}

DB::DBConn::ConnType DB::OLEDBConn::GetConnType() const
{
	return DB::DBConn::CT_OLEDB;
}

Int8 DB::OLEDBConn::GetTzQhr() const
{
	return 0;
}

void DB::OLEDBConn::ForceTz(Int8 tzQhr)
{
}

void DB::OLEDBConn::GetConnName(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("OLEDB:"));
	UnsafeArray<const WChar> connStr;
	if (this->clsData->connStr.SetTo(connStr))
	{
		NN<Text::String> s = Text::String::NewNotNull(connStr);
		sb->Append(s);
		s->Release();
	}
}

void DB::OLEDBConn::Close()
{
}

IntOS DB::OLEDBConn::ExecuteNonQuery(Text::CStringNN sql)
{
	this->lastDataError = DE_CONN_ERROR;
	return -2;
}

/*IntOS DB::OLEDBConn::ExecuteNonQuery(const WChar *sql)
{
	ClassData *data = this->clsData;
	if (data->pSession == 0)
	{
		this->lastError = 3;
		return -2;
	}
	
	HRESULT hr;
	IDBCreateCommand *pIDBCreateCommand;
	ICommandText *pICommandText;
	hr = data->pSession->CreateSession(0, IID_IDBCreateCommand, (IUnknown**)&pIDBCreateCommand);
	if (FAILED(hr))
	{
		this->lastError = 4;
		return -2;
	}

	hr = pIDBCreateCommand->CreateCommand(0, IID_ICommandText, (IUnknown**)&pICommandText);
	if (FAILED(hr))
	{
		pIDBCreateCommand->Release();
		this->lastError = 4;
		return -2;
	}

	hr = pICommandText->SetCommandText(DBGUID_DEFAULT, sql);
	if (FAILED(hr))
	{
		pICommandText->Release();
		pIDBCreateCommand->Release();
		this->lastError = 4;
		return -2;
	}

	DBROWCOUNT ret = 0;
	hr = pICommandText->Execute(0, IID_NULL, 0, &ret, 0);

	if (FAILED(hr))
	{
		ISupportErrorInfo *pISupportErrorInfo;
		hr = pICommandText->QueryInterface(IID_ISupportErrorInfo, (void**)&pISupportErrorInfo);
		if (SUCCEEDED(hr))
		{
			if (SUCCEEDED(pISupportErrorInfo->InterfaceSupportsErrorInfo(IID_ICommandText)))
			{
				IErrorInfo *pIErrorInfoAll = 0;
				GetErrorInfo(0, &pIErrorInfoAll);
				if (pIErrorInfoAll)
				{
					BSTR bstr;

					pIErrorInfoAll->GetDescription(&bstr);
					if (bstr)
					{
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("ExecuteNonQuery: "));
						sb.AppendW(bstr);
						data->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::ErrorDetail);
						SysFreeString(bstr);
					}
					pIErrorInfoAll->Release();
				}
			}
			pISupportErrorInfo->Release();
		}
		pICommandText->Release();
		pIDBCreateCommand->Release();
		this->lastError = 5;
		return -2;
	}
	pICommandText->Release();
	pIDBCreateCommand->Release();
	return ret;
}*/

void DB::OLEDBConn::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

Bool DB::OLEDBConn::IsLastDataError()
{
	return this->lastDataError == DE_EXEC_SQL_ERROR;
}

void DB::OLEDBConn::Reconnect()
{
}

UIntOS DB::OLEDBConn::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	return 0;
}

Optional<DB::DBReader> DB::OLEDBConn::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	UTF8Char tmpBuff[256];
	UnsafeArray<UTF8Char> sptr = tableName.ConcatTo(Text::StrConcatC(tmpBuff, UTF8STRC("select * from ")));
	return ExecuteReader(CSTRP(tmpBuff, sptr));
}

Optional<DB::DBReader> DB::OLEDBConn::ExecuteReader(Text::CStringNN sql)
{
	this->lastDataError = DE_CONN_ERROR;
	return nullptr;
}

/*DB::DBReader *DB::OLEDBConn::ExecuteReader(const WChar *sql)
{
	ClassData *data = this->clsData;
	if (data->pSession == 0)
	{
		this->lastError = 3;
		return 0;
	}
	
	HRESULT hr;
	IDBCreateCommand *pIDBCreateCommand;
	ICommandText *pICommandText;
	hr = data->pSession->CreateSession(0, IID_IDBCreateCommand, (IUnknown**)&pIDBCreateCommand);
	if (FAILED(hr))
	{
		this->lastError = 4;
		return 0;
	}

	hr = pIDBCreateCommand->CreateCommand(0, IID_ICommandText, (IUnknown**)&pICommandText);
	if (FAILED(hr))
	{
		pIDBCreateCommand->Release();
		this->lastError = 4;
		return 0;
	}

	hr = pICommandText->SetCommandText(DBGUID_DEFAULT, sql);
	if (FAILED(hr))
	{
		pICommandText->Release();
		pIDBCreateCommand->Release();
		this->lastError = 4;
		return 0;
	}

	IRowset *pIRowset;
	hr = pICommandText->Execute(0, IID_IRowset, 0, 0, (IUnknown**)&pIRowset);

	if (FAILED(hr))
	{
		ISupportErrorInfo *pISupportErrorInfo;
		hr = pICommandText->QueryInterface(IID_ISupportErrorInfo, (void**)&pISupportErrorInfo);
		if (SUCCEEDED(hr))
		{
			if (SUCCEEDED(pISupportErrorInfo->InterfaceSupportsErrorInfo(IID_ICommandText)))
			{
				IErrorInfo *pIErrorInfoAll = 0;
				GetErrorInfo(0, &pIErrorInfoAll);
				if (pIErrorInfoAll)
				{
					BSTR bstr;

					pIErrorInfoAll->GetDescription(&bstr);
					if (bstr)
					{
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("ExecuteReader: "));
						sb.AppendW(bstr);
						data->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::ErrorDetail);
						SysFreeString(bstr);
					}
					pIErrorInfoAll->Release();
				}
			}
			pISupportErrorInfo->Release();
		}
		pICommandText->Release();
		pIDBCreateCommand->Release();
		this->lastError = 5;
		return 0;
	}
	pICommandText->Release();
	pIDBCreateCommand->Release();

	DB::DBReader *r;
	NEW_CLASS(r, DB::OLEDBReader(pIRowset));
	return r;
}*/

void DB::OLEDBConn::CloseReader(NN<DBReader> r)
{
	DB::OLEDBReader *reader = (DB::OLEDBReader*)r.Ptr();
	DEL_CLASS(reader);
}

Optional<DB::DBTransaction> DB::OLEDBConn::BeginTransaction()
{
	return nullptr;
}

void DB::OLEDBConn::Commit(NN<DB::DBTransaction> tran)
{
}

void DB::OLEDBConn::Rollback(NN<DB::DBTransaction> tran)
{
}

DB::OLEDBConn::ConnError DB::OLEDBConn::GetConnError()
{
	return this->connErr;
}

UnsafeArrayOpt<const WChar> DB::OLEDBConn::GetConnStr()
{
	return this->clsData->connStr;
}
