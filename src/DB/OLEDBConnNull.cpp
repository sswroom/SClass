#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "DB/DBTool.h"
#include "DB/OLEDBConn.h"
#include "Math/Math.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

struct DB::OLEDBConn::ClassData
{
	IO::LogTool *log;
	const WChar *connStr;
};

DB::OLEDBConn::OLEDBConn(IO::LogTool *log) : DB::DBConn(CSTR("OLEDBConn"))
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->log = log;
	data->connStr = 0;
}

void DB::OLEDBConn::Init(const WChar *connStr)
{
	SDEL_TEXT(this->clsData->connStr);
	this->clsData->connStr = Text::StrCopyNew(connStr);
	this->connErr = CE_COCREATE;
}

DB::OLEDBConn::OLEDBConn(const WChar *connStr, IO::LogTool *log) : DB::DBConn(CSTR("OLEDBConn"))
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->log = log;
	data->connStr = 0;
	this->Init(connStr);
}

DB::OLEDBConn::~OLEDBConn()
{
	SDEL_TEXT(this->clsData->connStr);
	MemFree(this->clsData);
}

DB::DBUtil::ServerType DB::OLEDBConn::GetSvrType()
{
	return DB::DBUtil::ServerType::Unknown;
}

DB::DBConn::ConnType DB::OLEDBConn::GetConnType()
{
	return DB::DBConn::CT_OLEDB;
}

Int8 DB::OLEDBConn::GetTzQhr()
{
	return 0;
}

void DB::OLEDBConn::ForceTz(Int8 tzQhr)
{
}

void DB::OLEDBConn::GetConnName(Text::StringBuilderUTF8 *sb)
{
	sb->AppendC(UTF8STRC("OLEDB:"));
	if (this->clsData->connStr)
	{
		Text::String *s = Text::String::NewNotNull(this->clsData->connStr);
		sb->Append(s);
		s->Release();
	}
}

void DB::OLEDBConn::Close()
{
}

OSInt DB::OLEDBConn::ExecuteNonQuery(Text::CString sql)
{
	this->lastDataError = DE_CONN_ERROR;
	return -2;
}

/*OSInt DB::OLEDBConn::ExecuteNonQuery(const WChar *sql)
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
						data->log->LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::ErrorDetail);
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

void DB::OLEDBConn::GetErrorMsg(Text::StringBuilderUTF8 *str)
{
}

Bool DB::OLEDBConn::IsLastDataError()
{
	return this->lastDataError == DE_EXEC_SQL_ERROR;
}

void DB::OLEDBConn::Reconnect()
{
}

UOSInt DB::OLEDBConn::QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names)
{
	return 0;
}

DB::DBReader *DB::OLEDBConn::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	UTF8Char tmpBuff[256];
	UTF8Char *sptr = tableName.ConcatTo(Text::StrConcatC(tmpBuff, UTF8STRC("select * from ")));
	return ExecuteReader(CSTRP(tmpBuff, sptr));
}

DB::DBReader *DB::OLEDBConn::ExecuteReader(Text::CString sql)
{
	this->lastDataError = DE_CONN_ERROR;
	return 0;
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
						data->log->LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::ErrorDetail);
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

void DB::OLEDBConn::CloseReader(DBReader *r)
{
	DB::OLEDBReader *reader = (DB::OLEDBReader*)r;
	DEL_CLASS(reader);
}

void *DB::OLEDBConn::BeginTransaction()
{
	return 0;
}

void DB::OLEDBConn::Commit(void *tran)
{
}

void DB::OLEDBConn::Rollback(void *tran)
{
}

DB::OLEDBConn::ConnError DB::OLEDBConn::GetConnError()
{
	return this->connErr;
}

const WChar *DB::OLEDBConn::GetConnStr()
{
	return this->clsData->connStr;
}
