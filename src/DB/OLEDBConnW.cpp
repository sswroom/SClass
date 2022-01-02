#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "DB/DBTool.h"
#include "DB/OLEDBConn.h"
#include "Math/Math.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include <windows.h>
#include <initguid.h>
#include <oledb.h>
#include <oledberr.h>
#include <msdaguid.h>
extern "C"
{
#include <msdasql.h>
}
#include <msdasc.h>

DB::OLEDBConn::ProviderInfo DB::OLEDBConn::providerInfo[] = {
	{"ADsDSOObject",                     "OLE DB Provider for Microsoft Directory Services",     DB::DBUtil::ServerType::Unknown},
	{"EMPOLEDBVS71",                     "VSEE Versioning Enlistment Manager Proxy Data Source", DB::DBUtil::ServerType::Unknown},
	{"ESRI.GeoDB.OLEDB",				 "Esri GeoDatabase OLE DB Provider",                     DB::DBUtil::ServerType::Unknown},
	{"MediaCatalogDB",                   "MediaCatalogDB OLE DB Provider",                       DB::DBUtil::ServerType::Unknown},
	{"MediaCatalogMergedDB",             "MediaCatalogMergedDB OLE DB Provider",                 DB::DBUtil::ServerType::Unknown},
	{"MediaCatalogWebDB",                "MediaCatalogWebDB OLE DB Provider",                    DB::DBUtil::ServerType::Unknown},
	{"Microsoft.SQLSERVER.CE.OLEDB.3.0", "SQL Server Compact OLE DB Provider",                   DB::DBUtil::ServerType::MSSQL},
	{"Microsoft.SQLSERVER.CE.OLEDB.3.5", "SQL Server Compact OLE DB Provider",                   DB::DBUtil::ServerType::MSSQL},
	{"Microsoft.SQLSERVER.CE.OLEDB.4.0", "SQL Server Compact OLE DB Provider",                   DB::DBUtil::ServerType::MSSQL},
	{"Microsoft.Jet.OLEDB.4.0",          "Microsoft Jet 4.0 OLE DB Provider",                    DB::DBUtil::ServerType::Access},
	{"MSDAORA",                          "Microsoft OLE DB Provider for Oracle",                 DB::DBUtil::ServerType::Oracle},
	{"MSDAOSP",                          "Microsoft OLE DB Simple Provider",                     DB::DBUtil::ServerType::Unknown},
	{"MSDataShape",                      "MSDataShape",                                          DB::DBUtil::ServerType::Unknown},
	{"MSDMine",                          "Microsoft OLE DB Provider for Data Mining Services",   DB::DBUtil::ServerType::Unknown},
	{"MSIDXS",                           "Microsoft OLE DB Provider for Indexing Services",      DB::DBUtil::ServerType::Unknown},
	{"MSOLAP",                           "Microsoft OLE DB Provider for OLAP Services 8.0",      DB::DBUtil::ServerType::Unknown},
	{"SQLNCLI",                          "SQL Native Client",                                    DB::DBUtil::ServerType::MSSQL},
	{"SQLNCLI10",                        "SQL Server Native Client 10.0",                        DB::DBUtil::ServerType::MSSQL},
	{"SQLNCLI11",                        "SQL Server Native Client 11.0",                        DB::DBUtil::ServerType::MSSQL},
	{"SQLOLEDB",                         "Microsoft OLE DB Provider for SQL Server",             DB::DBUtil::ServerType::MSSQL}
};

typedef struct
{
	IO::LogTool *log;
	const WChar *connStr;
	HRESULT ci;
	IDataInitialize *pIDataInitialize;
	IDBInitialize *pIDBInitialize;
	IDBCreateSession *pSession;
	ITransactionLocal *pITransactionLocal;
	Data::ArrayList<const UTF8Char *> *tableNames;
} ClassData;

typedef struct
{
	IRowset *pIRowset;
	HROW* hRows;
	UInt32 rowCnt;
	UInt32 nCols;
	DBCOLUMNINFO *dbColInfo;
	WChar *colNameBuff;
	IAccessor *pIAccessor;
	DBBINDING *dbBinding;
	HACCESSOR hAccessor;
	UOSInt rowSize;
	UInt8 *dataBuff;
	Bool rowValid;
	OSInt rowChanged;
} ClassDataR;

//https://github.com/StevenChangZH/OleDbVCExample/blob/master/OleDbProject/OleDbSQL.cpp

DB::OLEDBConn::OLEDBConn(IO::LogTool *log) : DB::DBConn((const UTF8Char*)"OLEDBConn")
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->log = log;
	data->ci = CoInitialize(NULL);
	data->pIDataInitialize = 0;
	data->pIDBInitialize = 0;
	data->pSession = 0;
	data->pITransactionLocal = 0;
	data->connStr = 0;
	data->tableNames = 0;
}

void DB::OLEDBConn::Init(const WChar *connStr)
{
	ClassData *data = (ClassData *)this->clsData;
	SDEL_TEXT(data->connStr);
	data->connStr = Text::StrCopyNew(connStr);
	HRESULT hr;
	hr = CoCreateInstance(CLSID_MSDAINITIALIZE, NULL, CLSCTX_INPROC_SERVER, IID_IDataInitialize, (LPVOID *) &data->pIDataInitialize);
	if (FAILED(hr))
	{
		this->connErr = CE_COCREATE;
		return;
	}

	hr = data->pIDataInitialize->GetDataSource(0, CLSCTX_INPROC_SERVER, (LPCOLESTR )connStr, IID_IDBInitialize, ( IUnknown** )&data->pIDBInitialize);
	if (FAILED(hr))
	{
		this->connErr = CE_GETDATASOURCE;
		return;
	}

	hr = data->pIDBInitialize->Initialize();
	if (FAILED(hr))
	{
		this->connErr = CE_DBINITIALIZE;

		if (data->log)
		{
			ISupportErrorInfo *pISupportErrorInfo;
			hr = data->pIDBInitialize->QueryInterface(IID_ISupportErrorInfo, (void**)&pISupportErrorInfo);
			if (SUCCEEDED(hr))
			{
				if (SUCCEEDED(pISupportErrorInfo->InterfaceSupportsErrorInfo(IID_IDBInitialize)))
				{
					IErrorInfo *pIErrorInfoAll = 0;
					GetErrorInfo(0, &pIErrorInfoAll);
					if (pIErrorInfoAll)
					{
						Text::StringBuilderUTF8 sb;
						BSTR bstr;

						pIErrorInfoAll->GetSource(&bstr);
						if (bstr)
						{
							sb.ClearStr();
							sb.Append((const UTF8Char*)"Source: ");
							const UTF8Char *csptr = Text::StrToUTF8New(bstr);
							sb.Append(csptr);
							Text::StrDelNew(csptr);
							data->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
							SysFreeString(bstr);
						}

						pIErrorInfoAll->GetDescription(&bstr);
						if (bstr)
						{
							sb.ClearStr();
							sb.Append((const UTF8Char*)"Description: ");
							const UTF8Char *csptr = Text::StrToUTF8New(bstr);
							sb.Append(csptr);
							Text::StrDelNew(csptr);
							data->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
							SysFreeString(bstr);
						}
						pIErrorInfoAll->Release();
					}
				}
				pISupportErrorInfo->Release();
			}
		}
		return;
	}

	hr = data->pIDBInitialize->QueryInterface(IID_IDBCreateSession, ( void** )&data->pSession);
	if (FAILED(hr))
	{
		this->connErr = CE_GET_CREATE_SESS;
		return;
	}
	this->connErr = CE_NONE;
}

DB::OLEDBConn::OLEDBConn(const WChar *connStr, IO::LogTool *log) : DB::DBConn((const UTF8Char*)"OLEDBConn")
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->log = log;
	data->ci = CoInitialize(NULL);
	data->pIDataInitialize = 0;
	data->pIDBInitialize = 0;
	data->pSession = 0;
	data->pITransactionLocal = 0;
	data->connStr = 0;
	data->tableNames = 0;
	this->Init(connStr);
}

DB::OLEDBConn::~OLEDBConn()
{
	ClassData *data = (ClassData *)this->clsData;
	if (data->pITransactionLocal)
	{
		data->pITransactionLocal->Abort(0, FALSE, FALSE); 
		data->pITransactionLocal->Release();
		data->pITransactionLocal = 0;
	}
	if (data->pSession)
	{
		data->pSession->Release();
		data->pSession = 0;
	}
	if (data->pIDBInitialize)
	{
		data->pIDBInitialize->Release();
		data->pIDBInitialize = 0;
	}
	if (data->pIDataInitialize)
	{
		data->pIDataInitialize->Release();
		data->pIDataInitialize = 0;
	}
	if (data->ci == S_OK)
	{
		CoUninitialize();
	}
	if (data->tableNames)
	{
		UOSInt i = data->tableNames->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(data->tableNames->GetItem(i));
		}
		DEL_CLASS(data->tableNames);
		data->tableNames = 0;
	}
	SDEL_TEXT(data->connStr);
	MemFree(data);
}

DB::DBUtil::ServerType DB::OLEDBConn::GetSvrType()
{
	ClassData *data = (ClassData *)this->clsData;
	if (data->connStr)
	{
		UOSInt i = Text::StrIndexOfICase(data->connStr, L"Provider=");
		if (i != INVALID_INDEX)
		{
			Text::StringBuilderUTF8 sb;
			UOSInt j = Text::StrIndexOf(&data->connStr[i + 9], ';');
			if (j != INVALID_INDEX)
			{
				const WChar *wptr = Text::StrCopyNewC(&data->connStr[i + 9], (UOSInt)j);
				const UTF8Char *csptr = Text::StrToUTF8New(wptr);
				sb.Append(csptr);
				Text::StrDelNew(csptr);
				Text::StrDelNew(wptr);
			}
			else
			{
				const UTF8Char *csptr = Text::StrToUTF8New(&data->connStr[i + 9]);
				sb.Append(csptr);
				Text::StrDelNew(csptr);
			}
			if (sb.EqualsICase((const UTF8Char*)"SQLOLEDB"))
			{
				return DB::DBUtil::ServerType::MSSQL;
			}
			else if (sb.EqualsICase((const UTF8Char*)"Microsoft.SQLSERVER.CE.OLEDB.3.0"))
			{
				return DB::DBUtil::ServerType::MSSQL;
			}
			else if (sb.EqualsICase((const UTF8Char*)"Microsoft.SQLSERVER.CE.OLEDB.3.5"))
			{
				return DB::DBUtil::ServerType::MSSQL;
			}
			else if (sb.EqualsICase((const UTF8Char*)"Microsoft.SQLSERVER.CE.OLEDB.4.0"))
			{
				return DB::DBUtil::ServerType::MSSQL;
			}
			else if (sb.EqualsICase((const UTF8Char*)"Microsoft.Jet.OLEDB.4.0"))
			{
				return DB::DBUtil::ServerType::Access;
			}
			return DB::DBUtil::ServerType::Unknown;
		}
	}
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

void DB::OLEDBConn::GetConnName(Text::StringBuilderUTF *sb)
{
	ClassData *data = (ClassData *)this->clsData;
	sb->Append((const UTF8Char*)"OLEDB:");
	if (data->connStr)
	{
		const UTF8Char *csptr = Text::StrToUTF8New(data->connStr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
	}
}

void DB::OLEDBConn::Close()
{
}

OSInt DB::OLEDBConn::ExecuteNonQuery(const UTF8Char *sql)
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->pSession == 0)
	{
		this->lastDataError = DE_CONN_ERROR;
		return -2;
	}
	
	HRESULT hr;
	IDBCreateCommand *pIDBCreateCommand;
	ICommandText *pICommandText;
	hr = data->pSession->CreateSession(0, IID_IDBCreateCommand, (IUnknown**)&pIDBCreateCommand);
	if (FAILED(hr))
	{
		this->lastDataError = DE_INIT_SQL_ERROR;
		return -2;
	}

	hr = pIDBCreateCommand->CreateCommand(0, IID_ICommandText, (IUnknown**)&pICommandText);
	if (FAILED(hr))
	{
		pIDBCreateCommand->Release();
		this->lastDataError = DE_INIT_SQL_ERROR;
		return -2;
	}

	const WChar *wptr = Text::StrToWCharNew(sql);
	hr = pICommandText->SetCommandText(DBGUID_DEFAULT, wptr);
	if (FAILED(hr))
	{
		Text::StrDelNew(wptr);
		pICommandText->Release();
		pIDBCreateCommand->Release();
		this->lastDataError = DE_INIT_SQL_ERROR;
		return -2;
	}

	DBROWCOUNT ret = 0;
	hr = pICommandText->Execute(0, IID_NULL, 0, &ret, 0);
	Text::StrDelNew(wptr);

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
						sb.Append((const UTF8Char*)"ExecuteNonQuery: ");
						const UTF8Char *csptr = Text::StrToUTF8New(bstr);
						sb.Append(csptr);
						Text::StrDelNew(csptr);
						data->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
						SysFreeString(bstr);
					}
					pIErrorInfoAll->Release();
				}
			}
			pISupportErrorInfo->Release();
		}
		pICommandText->Release();
		pIDBCreateCommand->Release();
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return -2;
	}
	this->lastDataError = DE_NO_ERROR;
	pICommandText->Release();
	pIDBCreateCommand->Release();
	return ret;
}

/*OSInt DB::OLEDBConn::ExecuteNonQuery(const WChar *sql)
{
	ClassData *data = (ClassData*)this->clsData;
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
						sb.Append((const UTF8Char*)"ExecuteNonQuery: ");
						sb.AppendW(bstr);
						data->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
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

void DB::OLEDBConn::GetErrorMsg(Text::StringBuilderUTF *str)
{
	IErrorInfo *pIErrorInfoAll = 0;
	GetErrorInfo(0, &pIErrorInfoAll);
	if (pIErrorInfoAll)
	{
		BSTR bstr;

		pIErrorInfoAll->GetDescription(&bstr);
		if (bstr)
		{
			str->Append((const UTF8Char*)"Description: ");
			const UTF8Char *csptr = Text::StrToUTF8New(bstr);
			str->Append(csptr);
			Text::StrDelNew(csptr);
			SysFreeString(bstr);
		}
		pIErrorInfoAll->Release();
	}
}

Bool DB::OLEDBConn::IsLastDataError()
{
	return this->lastDataError == DE_EXEC_SQL_ERROR;
}

void DB::OLEDBConn::Reconnect()
{
}

UOSInt DB::OLEDBConn::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->tableNames)
	{
	}
	else
	{
		HRESULT hr;
		UTF8Char sbuff[256];
		IDBSchemaRowset *pIDBSchemaRowset;
		IRowset *pIRowset;
		NEW_CLASS(data->tableNames, Data::ArrayList<const UTF8Char*>());

		hr = data->pSession->CreateSession(0, IID_IDBSchemaRowset, (IUnknown**)&pIDBSchemaRowset);
		if (SUCCEEDED(hr))
		{
			VARIANT rgRestrictions[CRESTRICTIONS_DBSCHEMA_TABLES];
			UOSInt i = CRESTRICTIONS_DBSCHEMA_TABLES;
			while (i-- > 0)
			{
				VariantInit(&rgRestrictions[i]);
			}
			rgRestrictions[3].vt = VT_BSTR;
			rgRestrictions[3].bstrVal = SysAllocString(OLESTR("TABLE"));
			hr = pIDBSchemaRowset->GetRowset(NULL, DBSCHEMA_TABLES, CRESTRICTIONS_DBSCHEMA_TABLES, rgRestrictions, IID_IRowset, 0, NULL, (IUnknown**)&pIRowset);
			VariantClear(&rgRestrictions[3]);
			if (SUCCEEDED(hr))
			{
				UOSInt tableNameCol = 3;
				DB::OLEDBReader *rdr;
				NEW_CLASS(rdr, DB::OLEDBReader(pIRowset, -1));
				i = rdr->ColCount();
				while (i-- > 0)
				{
					if (rdr->GetName(i, sbuff))
					{
						if (Text::StrEquals(sbuff, (const UTF8Char*)"TABLE_NAME"))
						{
							tableNameCol = i;
							break;
						}
					}
				}
				while (rdr->ReadNext())
				{
					if (rdr->GetStr(tableNameCol, sbuff, sizeof(sbuff)))
					{
						data->tableNames->Add(Text::StrCopyNew(sbuff));
					}
				}
				DEL_CLASS(rdr);
			}
			pIDBSchemaRowset->Release();
		}
	}
	UOSInt i = 0;
	UOSInt j = data->tableNames->GetCount();
	while (i < j)
	{
		names->Add(data->tableNames->GetItem(i));
		i++;
	}
	return data->tableNames->GetCount();
}

DB::DBReader *DB::OLEDBConn::GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	UTF8Char tmpBuff[256];
	UTF8Char *sptr = Text::StrConcat(Text::StrConcat(tmpBuff, (const UTF8Char*)"select * from "), tableName);
	return ExecuteReaderC(tmpBuff, (UOSInt)(sptr - tmpBuff));
}

DB::DBReader *DB::OLEDBConn::ExecuteReader(const UTF8Char *sql)
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->pSession == 0)
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	
	HRESULT hr;
	IDBCreateCommand *pIDBCreateCommand;
	ICommandText *pICommandText;
	hr = data->pSession->CreateSession(0, IID_IDBCreateCommand, (IUnknown**)&pIDBCreateCommand);
	if (FAILED(hr))
	{
		this->lastDataError = DE_INIT_SQL_ERROR;
		return 0;
	}

	hr = pIDBCreateCommand->CreateCommand(0, IID_ICommandText, (IUnknown**)&pICommandText);
	if (FAILED(hr))
	{
		pIDBCreateCommand->Release();
		this->lastDataError = DE_INIT_SQL_ERROR;
		return 0;
	}

	const WChar *wptr = Text::StrToWCharNew(sql);
	hr = pICommandText->SetCommandText(DBGUID_DEFAULT, wptr);
	if (FAILED(hr))
	{
		Text::StrDelNew(wptr);
		pICommandText->Release();
		pIDBCreateCommand->Release();
		this->lastDataError = DE_INIT_SQL_ERROR;
		return 0;
	}

	DBROWCOUNT rowChanged = -1;
	IRowset *pIRowset;
	hr = pICommandText->Execute(0, IID_IRowset, 0, &rowChanged, (IUnknown**)&pIRowset);
	Text::StrDelNew(wptr);

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
						sb.Append((const UTF8Char*)"ExecuteReader: ");
						const UTF8Char *csptr = Text::StrToUTF8New(bstr);
						sb.Append(csptr);
						Text::StrDelNew(csptr);
						data->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
						SysFreeString(bstr);
					}
					pIErrorInfoAll->Release();
				}
			}
			pISupportErrorInfo->Release();
		}
		pICommandText->Release();
		pIDBCreateCommand->Release();
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}
	pICommandText->Release();
	pIDBCreateCommand->Release();
	this->lastDataError = DE_NO_ERROR;

	DB::DBReader *r;
	NEW_CLASS(r, DB::OLEDBReader(pIRowset, rowChanged));
	return r;
}

/*DB::DBReader *DB::OLEDBConn::ExecuteReader(const WChar *sql)
{
	ClassData *data = (ClassData*)this->clsData;
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
						sb.Append((const UTF8Char*)"ExecuteReader: ");
						sb.AppendW(bstr);
						data->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
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
	ClassData *data = (ClassData*)this->clsData;
	if (data->pITransactionLocal == 0)
	{
		data->pSession->QueryInterface(IID_ITransactionLocal, (void**)&data->pITransactionLocal);
		data->pITransactionLocal->StartTransaction(ISOLATIONLEVEL_READCOMMITTED, 0, NULL, NULL);
		return data->pITransactionLocal;
	}
	return 0;
}

void DB::OLEDBConn::Commit(void *tran)
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->pITransactionLocal)
	{
		data->pITransactionLocal->Commit(FALSE, XACTTC_SYNC_PHASEONE, 0);
		data->pITransactionLocal->Release();
		data->pITransactionLocal = 0;
	}
}

void DB::OLEDBConn::Rollback(void *tran)
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->pITransactionLocal)
	{
		data->pITransactionLocal->Abort(0, FALSE, FALSE); 
		data->pITransactionLocal->Release();
		data->pITransactionLocal = 0;
	}
}

DB::OLEDBConn::ConnError DB::OLEDBConn::GetConnError()
{
	return this->connErr;
}

const WChar *DB::OLEDBConn::GetConnStr()
{
	ClassData *data = (ClassData*)this->clsData;
	return data->connStr;
}

DB::OLEDBReader::OLEDBReader(void *pIRowset, OSInt rowChanged)
{
	ClassDataR *data = MemAlloc(ClassDataR, 1);
	this->clsData = data;
	data->rowChanged = rowChanged;
	data->pIRowset = (IRowset*)pIRowset;
	data->hRows = 0;
	data->rowCnt = 0;
	data->nCols = 0;
	data->dbColInfo = 0;
	data->colNameBuff = 0;
	data->pIAccessor = 0;
	data->hAccessor = 0;
	data->dbBinding = 0;
	data->rowSize = 0;
	data->dataBuff = 0;
	data->rowValid = false;

	HRESULT hr;
	IColumnsInfo *pIColumnsInfo;
	hr = data->pIRowset->QueryInterface(IID_IColumnsInfo,(void**)&pIColumnsInfo);
	if (SUCCEEDED(hr))
	{
		LPWSTR pStringBuffer;
		DBORDINAL cColumns;
		hr = pIColumnsInfo->GetColumnInfo(&cColumns, &data->dbColInfo, &pStringBuffer);
		pIColumnsInfo->Release();
		if (SUCCEEDED(hr))
		{
			data->nCols = (UInt32)cColumns;
			if (data->nCols > 0)
			{
				OSInt i;
				data->dbBinding = MemAlloc(DBBINDING, data->nCols);
				MemClear(data->dbBinding, sizeof(DBBINDING) * data->nCols);
				data->colNameBuff = pStringBuffer;

				UOSInt dwOffset = 0;
				i = 0;
				while ((UOSInt)i < data->nCols)
				{
					data->dbBinding[i].iOrdinal   = data->dbColInfo[i].iOrdinal;
					data->dbBinding[i].dwPart     = DBPART_VALUE | DBPART_LENGTH | DBPART_STATUS;
					data->dbBinding[i].obStatus   = (DBBYTEOFFSET)dwOffset;
					data->dbBinding[i].obLength   = (DBBYTEOFFSET)dwOffset + sizeof(DBSTATUS);
					data->dbBinding[i].obValue    = (DBBYTEOFFSET)dwOffset + sizeof(DBSTATUS) + sizeof(DBLENGTH);
					data->dbBinding[i].dwMemOwner = DBMEMOWNER_CLIENTOWNED;
					data->dbBinding[i].eParamIO   = DBPARAMIO_NOTPARAM;
					data->dbBinding[i].bPrecision = data->dbColInfo[i].bPrecision;
					data->dbBinding[i].bScale     = data->dbColInfo[i].bScale;
					data->dbBinding[i].wType      = data->dbColInfo[i].wType;
					data->dbBinding[i].cbMaxLen   = data->dbColInfo[i].ulColumnSize;
					dwOffset = data->dbBinding[i].cbMaxLen + data->dbBinding[i].obValue;
					if (dwOffset & 3)
					{
						dwOffset += 4 - (dwOffset & 3);
					}
					i++;
				}
				data->rowSize = dwOffset;
				data->dataBuff = MemAlloc(UInt8, data->rowSize);

				data->pIRowset->QueryInterface(IID_IAccessor,(void**)&data->pIAccessor);
				hr = data->pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, cColumns, data->dbBinding, 0, &data->hAccessor, 0);
			}
		}
	}

}

DB::OLEDBReader::~OLEDBReader()
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (data->hRows)
	{
		if (data->rowCnt)
		{
			data->pIRowset->ReleaseRows(data->rowCnt, data->hRows, 0, 0, 0);
		}
		CoTaskMemFree(data->hRows);
		data->hRows = 0;
	}
	if (data->dataBuff)
	{
		MemFree(data->dataBuff);
		data->dataBuff = 0;
	}
	if (data->pIAccessor)
	{
		DBREFCOUNT cnt;
		if (data->hAccessor)
		{
			data->pIAccessor->ReleaseAccessor(data->hAccessor, &cnt);
			data->hAccessor = 0;
		}
		data->pIAccessor->Release();
		data->pIAccessor = 0;
	}
	if (data->dbBinding)
	{
		MemFree(data->dbBinding);
		data->dbBinding = 0;
	}
	if (data->colNameBuff)
	{
		CoTaskMemFree(data->colNameBuff);
		data->colNameBuff = 0;
	}
	if (data->dbColInfo)
	{
		CoTaskMemFree(data->dbColInfo);
		data->dbColInfo = 0;
	}
	data->pIRowset->Release();
	MemFree(data);
}

Bool DB::OLEDBReader::ReadNext()
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	DBCOUNTITEM cRowsObtained = 0;
	HRESULT hr;
	data->rowValid = false;
	if (data->dataBuff == 0 || data->hAccessor == 0)
	{
		return false;
	}
	if (data->hRows)
	{
		if (data->rowCnt)
		{
			data->pIRowset->ReleaseRows(data->rowCnt, data->hRows, 0, 0, 0);
		}
		CoTaskMemFree(data->hRows);
		data->hRows = 0;
	}
	hr = data->pIRowset->GetNextRows(DB_NULL_HCHAPTER, 0, 1, &cRowsObtained, &data->hRows);
	data->rowCnt = (UInt32)cRowsObtained;
	if (SUCCEEDED(hr) && data->rowCnt > 0)
	{
		hr = data->pIRowset->GetData(data->hRows[0], data->hAccessor, data->dataBuff);
		if (SUCCEEDED(hr))
		{
			data->rowValid = true;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

UOSInt DB::OLEDBReader::ColCount()
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	return data->nCols;
}

OSInt DB::OLEDBReader::GetRowChanged()
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	return data->rowChanged;
}

Int32 DB::OLEDBReader::GetInt32(UOSInt colIndex)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return 0;
	DBSTATUS *status = (DBSTATUS*)&data->dataBuff[data->dbBinding[colIndex].obStatus];
	DBLENGTH *valLen = (DBLENGTH*)&data->dataBuff[data->dbBinding[colIndex].obLength];
	UInt8 *val = &data->dataBuff[data->dbBinding[colIndex].obValue];
	if (*status == DBSTATUS_S_ISNULL)
	{
		return 0;
	}
	switch (data->dbColInfo[colIndex].wType)
	{
	case DBTYPE_I2:
		if (*valLen == 2)
		{
			return ReadInt16(val);
		}
		return 0;
	case DBTYPE_I4:
		if (*valLen == 4)
		{
			return ReadInt32(val);
		}
		return 0;
	case DBTYPE_R4:
		if (*valLen == 4)
		{
			return Math::Double2Int32(ReadFloat(val));
		}
		return 0;
	case DBTYPE_R8:
		if (*valLen == 8)
		{
			return Math::Double2Int32(ReadDouble(val));
		}
		return 0;
	case DBTYPE_UI1:
		if (*valLen == 1)
		{
			return val[0];
		}
		return 0;
	case DBTYPE_I1:
		if (*valLen == 1)
		{
			return (Int8)val[0];
		}
		return 0;
	case DBTYPE_UI2:
		if (*valLen == 2)
		{
			return ReadUInt16(val);
		}
		return 0;
	case DBTYPE_UI4:
		if (*valLen == 4)
		{
			return ReadInt32(val);
		}
		return 0;
	case DBTYPE_I8:
		if (*valLen == 8)
		{
			return ReadInt32(val);
		}
		return 0;
	case DBTYPE_UI8:
		if (*valLen == 8)
		{
			return ReadInt32(val);
		}
		return 0;
	default:
		return 0;
	}
	return 0;
}

Int64 DB::OLEDBReader::GetInt64(UOSInt colIndex)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return 0;
	DBSTATUS *status = (DBSTATUS*)&data->dataBuff[data->dbBinding[colIndex].obStatus];
	DBLENGTH *valLen = (DBLENGTH*)&data->dataBuff[data->dbBinding[colIndex].obLength];
	UInt8 *val = &data->dataBuff[data->dbBinding[colIndex].obValue];
	if (*status == DBSTATUS_S_ISNULL)
	{
		return 0;
	}
	switch (data->dbColInfo[colIndex].wType)
	{
	case DBTYPE_I2:
		if (*valLen == 2)
		{
			return ReadInt16(val);
		}
		return 0;
	case DBTYPE_I4:
		if (*valLen == 4)
		{
			return ReadInt32(val);
		}
		return 0;
	case DBTYPE_R4:
		if (*valLen == 4)
		{
			return Math::Double2Int64(ReadFloat(val));
		}
		return 0;
	case DBTYPE_R8:
		if (*valLen == 8)
		{
			return Math::Double2Int64(ReadDouble(val));
		}
		return 0;
	case DBTYPE_UI1:
		if (*valLen == 1)
		{
			return val[0];
		}
		return 0;
	case DBTYPE_I1:
		if (*valLen == 1)
		{
			return (Int8)val[0];
		}
		return 0;
	case DBTYPE_UI2:
		if (*valLen == 2)
		{
			return ReadUInt16(val);
		}
		return 0;
	case DBTYPE_UI4:
		if (*valLen == 4)
		{
			return ReadUInt32(val);
		}
		return 0;
	case DBTYPE_I8:
		if (*valLen == 8)
		{
			return ReadInt64(val);
		}
		return 0;
	case DBTYPE_UI8:
		if (*valLen == 8)
		{
			return ReadInt64(val);
		}
		return 0;
	default:
		return 0;
	}
	return 0;
}

WChar *DB::OLEDBReader::GetStr(UOSInt colIndex, WChar *buff)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return 0;
	DBSTATUS *status = (DBSTATUS*)&data->dataBuff[data->dbBinding[colIndex].obStatus];
	DBLENGTH *valLen = (DBLENGTH*)&data->dataBuff[data->dbBinding[colIndex].obLength];
	UInt8 *val = &data->dataBuff[data->dbBinding[colIndex].obValue];
	if (*status == DBSTATUS_S_ISNULL)
	{
		return 0;
	}
	switch (data->dbColInfo[colIndex].wType)
	{
	case DBTYPE_I2:
		if (*valLen == 2)
		{
			return Text::StrInt16(buff, ReadInt16(val));
		}
		return 0;
	case DBTYPE_I4:
		if (*valLen == 4)
		{
			return Text::StrInt32(buff, ReadInt32(val));
		}
		return 0;
	case DBTYPE_R4:
		if (*valLen == 4)
		{
			return Text::StrDouble(buff, ReadFloat(val));
		}
		return 0;
	case DBTYPE_R8:
		if (*valLen == 8)
		{
			return Text::StrDouble(buff, ReadDouble(val));
		}
		return 0;
	case DBTYPE_UI1:
		if (*valLen == 1)
		{
			return Text::StrUInt16(buff, val[0]);
		}
		return 0;
	case DBTYPE_I1:
		if (*valLen == 1)
		{
			return Text::StrInt16(buff, (Int8)val[0]);
		}
		return 0;
	case DBTYPE_UI2:
		if (*valLen == 2)
		{
			return Text::StrUInt16(buff, ReadUInt16(val));
		}
		return 0;
	case DBTYPE_UI4:
		if (*valLen == 4)
		{
			return Text::StrUInt32(buff, ReadUInt32(val));
		}
		return 0;
	case DBTYPE_I8:
		if (*valLen == 8)
		{
			return Text::StrInt64(buff, ReadInt64(val));
		}
		return 0;
	case DBTYPE_UI8:
		if (*valLen == 8)
		{
			return Text::StrUInt64(buff, ReadUInt64(val));
		}
		return 0;
	case DBTYPE_WSTR:
		return Text::StrConcatC(buff, (const WChar*)val, *valLen / sizeof(WChar));
	case DBTYPE_DBDATE:
		if (*valLen == 6)
		{
			Data::DateTime dt;
			dt.SetValue(ReadUInt16(val), ReadUInt16(&val[2]), ReadUInt16(&val[4]), 0, 0, 0, 0, 0);
			UTF8Char sbuff[32];
			dt.ToString(sbuff);
			return Text::StrUTF8_WChar(buff, sbuff, 0);
		}
		return 0;
	case DBTYPE_DBTIME:
		if (*valLen == 6)
		{
			Data::DateTime dt;
			dt.SetValue(1970, 1, 1, ReadUInt16(val), ReadUInt16(&val[2]), ReadUInt16(&val[4]), 0, 0);
			UTF8Char sbuff[32];
			dt.ToString(sbuff);
			return Text::StrUTF8_WChar(buff, sbuff, 0);
		}
		return 0;
	case DBTYPE_DBTIMESTAMP:
		if (*valLen == 16)
		{
			Data::DateTime dt;
			dt.SetValue(ReadUInt16(val), ReadUInt16(&val[2]), ReadUInt16(&val[4]), ReadInt16(&val[6]), ReadInt16(&val[8]), ReadInt16(&val[10]), ReadInt32(&val[12]), 0);
			UTF8Char sbuff[32];
			dt.ToString(sbuff);
			return Text::StrUTF8_WChar(buff, sbuff, 0);
		}
		return 0;
	case DBTYPE_NULL:
	default:
		return 0;
	}
}

Bool DB::OLEDBReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return false;
	DBSTATUS *status = (DBSTATUS*)&data->dataBuff[data->dbBinding[colIndex].obStatus];
	DBLENGTH *valLen = (DBLENGTH*)&data->dataBuff[data->dbBinding[colIndex].obLength];
	UInt8 *val = &data->dataBuff[data->dbBinding[colIndex].obValue];
	if (*status == DBSTATUS_S_ISNULL)
	{
		return false;
	}
	switch (data->dbColInfo[colIndex].wType)
	{
	case DBTYPE_I2:
		if (*valLen == 2)
		{
			sb->AppendI16(ReadInt16(val));
			return true;
		}
		return false;
	case DBTYPE_I4:
		if (*valLen == 4)
		{
			sb->AppendI32(ReadInt32(val));
			return true;
		}
		return false;
	case DBTYPE_R4:
		if (*valLen == 4)
		{
			Text::SBAppendF32(sb, ReadFloat(val));
			return true;
		}
		return false;
	case DBTYPE_R8:
		if (*valLen == 8)
		{
			Text::SBAppendF64(sb, ReadDouble(val));
			return true;
		}
		return false;
	case DBTYPE_UI1:
		if (*valLen == 1)
		{
			sb->AppendU16(val[0]);
			return true;
		}
		return false;
	case DBTYPE_I1:
		if (*valLen == 1)
		{
			sb->AppendI16((Int8)val[0]);
			return true;
		}
		return false;
	case DBTYPE_UI2:
		if (*valLen == 2)
		{
			sb->AppendU16(ReadUInt16(val));
			return true;
		}
		return false;
	case DBTYPE_UI4:
		if (*valLen == 4)
		{
			sb->AppendU32(ReadUInt32(val));
			return true;
		}
		return false;
	case DBTYPE_I8:
		if (*valLen == 8)
		{
			sb->AppendI64(ReadInt64(val));
			return true;
		}
		return false;
	case DBTYPE_UI8:
		if (*valLen == 8)
		{
			sb->AppendU64(ReadUInt64(val));
			return true;
		}
		return false;
	case DBTYPE_WSTR:
		{
			const WChar *wptr = Text::StrCopyNewC((const WChar*)val, *valLen / sizeof(WChar));
			const UTF8Char *csptr = Text::StrToUTF8New(wptr);
			sb->Append(csptr);
			Text::StrDelNew(csptr);
			Text::StrDelNew(wptr);
		}
		return true;
	case DBTYPE_DBDATE:
		if (*valLen == 6)
		{
			Data::DateTime dt;
			dt.SetValue(ReadUInt16(val), ReadUInt16(&val[2]), ReadUInt16(&val[4]), 0, 0, 0, 0, 0);
			sb->AppendDate(&dt);
			return true;
		}
		return 0;
	case DBTYPE_DBTIME:
		if (*valLen == 6)
		{
			Data::DateTime dt;
			dt.SetValue(1970, 1, 1, ReadUInt16(val), ReadUInt16(&val[2]), ReadUInt16(&val[4]), 0, 0);
			sb->AppendDate(&dt);
			return true;
		}
		return 0;
	case DBTYPE_DBTIMESTAMP:
		if (*valLen == 16)
		{
			Data::DateTime dt;
			dt.SetValue(ReadUInt16(val), ReadUInt16(&val[2]), ReadUInt16(&val[4]), ReadInt16(&val[6]), ReadInt16(&val[8]), ReadInt16(&val[10]), ReadInt32(&val[12]), 0);
			sb->AppendDate(&dt);
			return true;
		}
		return false;
	case DBTYPE_NULL:
	default:
		return false;
	}
}

Text::String *DB::OLEDBReader::GetNewStr(UOSInt colIndex)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return 0;
	DBSTATUS *status = (DBSTATUS*)&data->dataBuff[data->dbBinding[colIndex].obStatus];
	DBLENGTH *valLen = (DBLENGTH*)&data->dataBuff[data->dbBinding[colIndex].obLength];
	UInt8 *val = &data->dataBuff[data->dbBinding[colIndex].obValue];
	if (*status == DBSTATUS_S_ISNULL)
	{
		return 0;
	}
	WChar sbuff[64];
	switch (data->dbColInfo[colIndex].wType)
	{
	case DBTYPE_WSTR:
		{
			WChar *tmpBuff = MemAlloc(WChar, (*valLen / sizeof(WChar*)) + 1);
			MemCopyNO(tmpBuff, val, *valLen);
			tmpBuff[*valLen / sizeof(WChar*)] = 0;
			Text::String *ret = Text::String::New(tmpBuff, *valLen / sizeof(WChar*));
			MemFree(tmpBuff);
			return ret;
		}
	default:
		if (GetStr(colIndex, sbuff))
		{
			return Text::String::NewNotNull(sbuff);
		}
		return 0;
	}
	return 0;
}

UTF8Char *DB::OLEDBReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return 0;
	DBSTATUS *status = (DBSTATUS*)&data->dataBuff[data->dbBinding[colIndex].obStatus];
	DBLENGTH *valLen = (DBLENGTH*)&data->dataBuff[data->dbBinding[colIndex].obLength];
	UInt8 *val = &data->dataBuff[data->dbBinding[colIndex].obValue];
	if (*status == DBSTATUS_S_ISNULL)
	{
		return 0;
	}
	switch (data->dbColInfo[colIndex].wType)
	{
	case DBTYPE_I2:
		if (*valLen == 2)
		{
			return Text::StrInt16(buff, ReadInt16(val));
		}
		return 0;
	case DBTYPE_I4:
		if (*valLen == 4)
		{
			return Text::StrInt32(buff, ReadInt32(val));
		}
		return 0;
	case DBTYPE_R4:
		if (*valLen == 4)
		{
			return Text::StrDouble(buff, ReadFloat(val));
		}
		return 0;
	case DBTYPE_R8:
		if (*valLen == 8)
		{
			return Text::StrDouble(buff, ReadDouble(val));
		}
		return 0;
	case DBTYPE_UI1:
		if (*valLen == 1)
		{
			return Text::StrUInt16(buff, val[0]);
		}
		return 0;
	case DBTYPE_I1:
		if (*valLen == 1)
		{
			return Text::StrInt16(buff, (Int8)val[0]);
		}
		return 0;
	case DBTYPE_UI2:
		if (*valLen == 2)
		{
			return Text::StrUInt16(buff, ReadUInt16(val));
		}
		return 0;
	case DBTYPE_UI4:
		if (*valLen == 4)
		{
			return Text::StrUInt32(buff, ReadUInt32(val));
		}
		return 0;
	case DBTYPE_I8:
		if (*valLen == 8)
		{
			return Text::StrInt64(buff, ReadInt64(val));
		}
		return 0;
	case DBTYPE_UI8:
		if (*valLen == 8)
		{
			return Text::StrUInt64(buff, ReadUInt64(val));
		}
		return 0;
	case DBTYPE_WSTR:
		return Text::StrWChar_UTF8C(buff, (const WChar*)val, (UOSInt)*valLen / sizeof(WChar));
	case DBTYPE_DBDATE:
		if (*valLen == 6)
		{
			Data::DateTime dt;
			dt.SetValue(ReadUInt16(val), ReadUInt16(&val[2]), ReadUInt16(&val[4]), 0, 0, 0, 0, 0);
			return dt.ToString(buff);
		}
		return 0;
	case DBTYPE_DBTIME:
		if (*valLen == 6)
		{
			Data::DateTime dt;
			dt.SetValue(1970, 1, 1, ReadUInt16(val), ReadUInt16(&val[2]), ReadUInt16(&val[4]), 0, 0);
			return dt.ToString(buff);
		}
		return 0;
	case DBTYPE_DBTIMESTAMP:
		if (*valLen == 16)
		{
			Data::DateTime dt;
			dt.SetValue(ReadUInt16(val), ReadUInt16(&val[2]), ReadInt16(&val[4]), ReadInt16(&val[6]), ReadInt16(&val[8]), ReadInt16(&val[10]), ReadInt32(&val[12]), 0);
			return dt.ToString(buff);
		}
		return 0;
	case DBTYPE_NULL:
	default:
		return 0;
	}
}

DB::DBReader::DateErrType DB::OLEDBReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return DB::DBReader::DET_ERROR;
	DBSTATUS *status = (DBSTATUS*)&data->dataBuff[data->dbBinding[colIndex].obStatus];
	DBLENGTH *valLen = (DBLENGTH*)&data->dataBuff[data->dbBinding[colIndex].obLength];
	UInt8 *val = &data->dataBuff[data->dbBinding[colIndex].obValue];
	if (*status == DBSTATUS_S_ISNULL)
	{
		return DB::DBReader::DET_NULL;
	}
	switch (data->dbColInfo[colIndex].wType)
	{
	case DBTYPE_DBDATE:
		if (*valLen == 6)
		{
			outVal->SetValue(ReadUInt16(val), ReadUInt16(&val[2]), ReadUInt16(&val[4]), 0, 0, 0, 0, 0);
			return DB::DBReader::DET_OK;
		}
		return DB::DBReader::DET_ERROR;
	case DBTYPE_DBTIME:
		if (*valLen == 6)
		{
			outVal->SetValue(1970, 1, 1, ReadUInt16(val), ReadUInt16(&val[2]), ReadUInt16(&val[4]), 0, 0);
			return DB::DBReader::DET_OK;
		}
		return DB::DBReader::DET_ERROR;
	case DBTYPE_DBTIMESTAMP:
		if (*valLen == 16)
		{
			outVal->SetValue(ReadUInt16(val), ReadUInt16(&val[2]), ReadInt16(&val[4]), ReadInt16(&val[6]), ReadInt16(&val[8]), ReadInt16(&val[10]), ReadInt32(&val[12]), 0);
			return DB::DBReader::DET_OK;
		}
		return DB::DBReader::DET_ERROR;
	case DBTYPE_NULL:
	default:
		return DB::DBReader::DET_ERROR;
	}
}

Double DB::OLEDBReader::GetDbl(UOSInt colIndex)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return 0;
	DBSTATUS *status = (DBSTATUS*)&data->dataBuff[data->dbBinding[colIndex].obStatus];
	DBLENGTH *valLen = (DBLENGTH*)&data->dataBuff[data->dbBinding[colIndex].obLength];
	UInt8 *val = &data->dataBuff[data->dbBinding[colIndex].obValue];
	if (*status == DBSTATUS_S_ISNULL)
	{
		return 0;
	}
	switch (data->dbColInfo[colIndex].wType)
	{
	case DBTYPE_I2:
		if (*valLen == 2)
		{
			return ReadInt16(val);
		}
		return 0;
	case DBTYPE_I4:
		if (*valLen == 4)
		{
			return ReadInt32(val);
		}
		return 0;
	case DBTYPE_R4:
		if (*valLen == 4)
		{
			return ReadFloat(val);
		}
		return 0;
	case DBTYPE_R8:
		if (*valLen == 8)
		{
			return ReadDouble(val);
		}
		return 0;
	case DBTYPE_UI1:
		if (*valLen == 1)
		{
			return val[0];
		}
		return 0;
	case DBTYPE_I1:
		if (*valLen == 1)
		{
			return (Int8)val[0];
		}
		return 0;
	case DBTYPE_UI2:
		if (*valLen == 2)
		{
			return ReadUInt16(val);
		}
		return 0;
	case DBTYPE_UI4:
		if (*valLen == 4)
		{
			return ReadUInt32(val);
		}
		return 0;
	case DBTYPE_I8:
		if (*valLen == 8)
		{
			return (Double)ReadInt64(val);
		}
		return 0;
	case DBTYPE_UI8:
		if (*valLen == 8)
		{
			return (Double)ReadUInt64(val);
		}
		return 0;
	default:
		return 0;
	}
	return 0;
}

Bool DB::OLEDBReader::GetBool(UOSInt colIndex)
{
	return GetInt32(colIndex) != 0;
}

UOSInt DB::OLEDBReader::GetBinarySize(UOSInt colIndex)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return 0;
	DBLENGTH *valLen = (DBLENGTH*)&data->dataBuff[data->dbBinding[colIndex].obLength];
	return *valLen;
}

UOSInt DB::OLEDBReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return 0;
	DBSTATUS *status = (DBSTATUS*)&data->dataBuff[data->dbBinding[colIndex].obStatus];
	if (*status == DBSTATUS_S_ISNULL)
	{
		return 0;
	}
	DBLENGTH *valLen = (DBLENGTH*)&data->dataBuff[data->dbBinding[colIndex].obLength];
	UInt8 *val = &data->dataBuff[data->dbBinding[colIndex].obValue];
	if (*valLen > 0)
	{
		MemCopyNO(buff, val, *valLen);
	}
	return *valLen;
}

Math::Vector2D *DB::OLEDBReader::GetVector(UOSInt colIndex)
{
//	ClassDataR *data = (ClassDataR*)this->clsData;
	/////////////////////////////////////////
	return 0;
}

Bool DB::OLEDBReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return false;
}

UTF8Char *DB::OLEDBReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (data->dbColInfo == 0 || colIndex >= data->nCols)
		return 0;
	if (data->dbColInfo[colIndex].pwszName == 0)
		return 0;
	return Text::StrWChar_UTF8(buff, data->dbColInfo[colIndex].pwszName);
}

Bool DB::OLEDBReader::IsNull(UOSInt colIndex)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (!data->rowValid || colIndex >= data->nCols)
		return true;
	DBSTATUS *status = (DBSTATUS*)&data->dataBuff[data->dbBinding[colIndex].obStatus];
	return *status == DBSTATUS_S_ISNULL;
}

DB::DBUtil::ColType DB::OLEDBReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (data->dbColInfo == 0 || colIndex >= data->nCols)
	{
		if (colSize)
			*colSize = 0;
		return DB::DBUtil::CT_Unknown;
	}
	if (colSize)
	{
		*colSize = data->dbColInfo[colIndex].ulColumnSize;
	}
	return DBType2ColType(data->dbColInfo[colIndex].wType);
}

Bool DB::OLEDBReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	ClassDataR *data = (ClassDataR*)this->clsData;
	if (data->dbColInfo == 0 || colIndex >= data->nCols)
	{
		return false;
	}
	if (data->dbColInfo[colIndex].pwszName)
	{
		const UTF8Char *csptr = Text::StrToUTF8New(data->dbColInfo[colIndex].pwszName);
		colDef->SetColName(csptr);
		Text::StrDelNew(csptr);
	}
	else
	{
		colDef->SetColName((const UTF8Char*)"");
	}
	colDef->SetColType(DBType2ColType(data->dbColInfo[colIndex].wType));
	colDef->SetColSize(data->dbColInfo[colIndex].ulColumnSize);
	colDef->SetColDP(data->dbColInfo[colIndex].bPrecision);
	colDef->SetNotNull((data->dbColInfo[colIndex].dwFlags & DBCOLUMNFLAGS_ISNULLABLE) == 0);
	colDef->SetPK((data->dbColInfo[colIndex].dwFlags & DBCOLUMNFLAGS_ISROWID) != 0);
	colDef->SetAutoInc((data->dbColInfo[colIndex].dwFlags & DBCOLUMNFLAGS_ISROWVER) != 0);

	return true;
}

DB::DBUtil::ColType DB::OLEDBReader::DBType2ColType(UInt16 dbType)
{
	switch (dbType)
	{
	case DBTYPE_I1:
		return DB::DBUtil::CT_Int16;
	case DBTYPE_UI1:
		return DB::DBUtil::CT_UInt16;
	case DBTYPE_I2:
		return DB::DBUtil::CT_Int16;
	case DBTYPE_UI2:
		return DB::DBUtil::CT_UInt16;
	case DBTYPE_I4:
		return DB::DBUtil::CT_Int32;
	case DBTYPE_UI4:
		return DB::DBUtil::CT_UInt32;
	case DBTYPE_I8:
		return DB::DBUtil::CT_Int64;
	case DBTYPE_UI8:
		return DB::DBUtil::CT_UInt64;
	case DBTYPE_R4:
		return DB::DBUtil::CT_Float;
	case DBTYPE_R8:
		return DB::DBUtil::CT_Double;
	case DBTYPE_DBDATE:
	case DBTYPE_DBTIME:
	case DBTYPE_DBTIMESTAMP:
	case DBTYPE_FILETIME:
	case DBTYPE_DATE:
		return DB::DBUtil::CT_DateTime;
	case DBTYPE_BSTR:
		return DB::DBUtil::CT_VarChar;
	case DBTYPE_BOOL:
		return DB::DBUtil::CT_Bool;
	case DBTYPE_BYTES:
		return DB::DBUtil::CT_Byte;
	case DBTYPE_STR:
	case DBTYPE_WSTR:
		return DB::DBUtil::CT_VarChar;
	case DBTYPE_VARIANT:
	case DBTYPE_IUNKNOWN:
	case DBTYPE_DECIMAL:
	case DBTYPE_GUID:
	case DBTYPE_VECTOR:
	case DBTYPE_NUMERIC:
	case DBTYPE_UDT:
	case DBTYPE_HCHAPTER:
	case DBTYPE_PROPVARIANT:
	case DBTYPE_VARNUMERIC:
	case DBTYPE_CY:
	case DBTYPE_IDISPATCH:
	case DBTYPE_ERROR:
	case DBTYPE_EMPTY:
	case DBTYPE_NULL:
	default:
		return DB::DBUtil::CT_Unknown;
	}
}
