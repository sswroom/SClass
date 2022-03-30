#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Win32/WMIQuery.h"
#include <wchar.h>
#include <initguid.h>
#include <comdef.h>
#include <wbemidl.h>

Int32 Win32::WMIQuery::securityCnt = 0;


void Win32::WMIQuery::InitQuery(const WChar *ns)
{
	this->pService = 0;
	this->tabNames = 0;
	this->lastDataError = DE_NO_ERROR;
	HRESULT ci = CoInitialize(NULL);
	HRESULT hr;
	if (securityCnt == 0)
	{
		hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
		securityCnt++;
	}
	if (securityCnt > 0)
	{
		IWbemLocator *pLocator;
		hr = CoCreateInstance(CLSID_WbemAdministrativeLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLocator);
		if (SUCCEEDED(hr))
		{
			IWbemServices *pServices;
			BSTR nsstr = SysAllocString(ns);
			hr = pLocator->ConnectServer(nsstr, NULL, NULL, NULL, 0, NULL, NULL, &pServices);
			pLocator->Release();
			SysFreeString(nsstr);
			if (SUCCEEDED(hr))
			{
				hr = CoSetProxyBlanket(pServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
				this->pService = pServices;
			}
		}
	}
	if (pService == 0)
	{
		if (ci == S_OK)
		{
			CoUninitialize();
		}
	}
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
	this->Close();
	if (this->tabNames)
	{
		UOSInt i = this->tabNames->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->tabNames->GetItem(i));
		}
		DEL_CLASS(this->tabNames);
	}
	SDEL_TEXT(this->ns);
}

Bool Win32::WMIQuery::IsError()
{
	return this->pService == 0;
}

DB::DBUtil::ServerType Win32::WMIQuery::GetSvrType()
{
	return DB::DBUtil::ServerType::WBEM;
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

void Win32::WMIQuery::GetConnName(Text::StringBuilderUTF8 *sb)
{
	sb->AppendC(UTF8STRC("WMI:"));
	sb->AppendW(this->ns);
}

void Win32::WMIQuery::Close()
{
	if (this->pService)
	{
		((IWbemServices *)this->pService)->Release();
		this->pService = 0;
		CoUninitialize();
	}
}

OSInt Win32::WMIQuery::ExecuteNonQuerySlow(const UTF8Char *sql)
{
	const WChar *wptr = Text::StrToWCharNew(sql);
	OSInt ret = ExecuteNonQuery(wptr);
	Text::StrDelNew(wptr);
	return ret;
}

OSInt Win32::WMIQuery::ExecuteNonQuery(const WChar *sql)
{
	this->lastDataError = DE_EXEC_SQL_ERROR;
	return -2;
}

DB::DBReader *Win32::WMIQuery::ExecuteReaderSlow(const UTF8Char *sqlCmd)
{
	const WChar *wptr = Text::StrToWCharNew(sqlCmd);
	DB::DBReader *r = ExecuteReader(wptr);
	Text::StrDelNew(wptr);
	return r;
}

DB::DBReader *Win32::WMIQuery::ExecuteReader(const WChar *sqlCmd)
{
	HRESULT hr;
	BSTR query = SysAllocString(sqlCmd);
	BSTR wql = SysAllocString(L"WQL");
	IEnumWbemClassObject *pEnum;
	hr = ((IWbemServices*)this->pService)->ExecQuery(wql, query, WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY, NULL, &pEnum);
	SysFreeString(wql);
	SysFreeString(query);
	if (SUCCEEDED(hr))
	{
		this->lastDataError = DE_NO_ERROR;
		Win32::WMIReader *reader;
		NEW_CLASS(reader, Win32::WMIReader(pEnum));
		return reader;
	}
	else
	{
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}
}

Bool Win32::WMIQuery::IsLastDataError()
{
	return this->lastDataError == DE_EXEC_SQL_ERROR;
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
	if (this->tabNames == 0)
	{
		IEnumWbemClassObject *pEnum;
		NEW_CLASS(this->tabNames, Data::ArrayListStrUTF8());
		HRESULT hr = ((IWbemServices *)this->pService)->CreateClassEnum(0, WBEM_FLAG_DEEP | WBEM_FLAG_RETURN_IMMEDIATELY, 0, &pEnum);
		if (SUCCEEDED(hr))
		{
			IWbemClassObject *pObject;
			ULONG returned;
			while (true)
			{
				hr = pEnum->Next(WBEM_INFINITE, 1, &pObject, &returned);
				if (SUCCEEDED(hr))
				{
					if (returned == 0)
						break;

					VARIANT v;
					VariantInit(&v);
					if (SUCCEEDED(pObject->Get(L"__CLASS", 0, &v, 0, 0)))
					{
						BSTR bs = V_BSTR(&v);
						this->tabNames->SortedInsert(Text::StrToUTF8New(bs));
					}
					VariantClear(&v);
					pObject->Release();
				}
				else
				{
					break;
				}
			}
			pEnum->Release();
		}
	}

	names->AddAll(this->tabNames);
	return this->tabNames->GetCount();
}

DB::DBReader *Win32::WMIQuery::GetTableData(const UTF8Char *tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	WChar wbuff[256];
	if (this->tabNames == 0)
	{
		Data::ArrayList<const UTF8Char*> names;
		this->GetTableNames(&names);
	}
	if (this->tabNames->SortedIndexOf(tableName) < 0)
	{
		return 0;
	}
	Text::StrUTF8_WChar(Text::StrConcat(wbuff, L"SELECT * FROM "), tableName, 0);
	return this->ExecuteReader(wbuff);
}

void Win32::WMIQuery::CloseReader(DB::DBReader *reader)
{
	Win32::WMIReader *r = (Win32::WMIReader*)reader;
	DEL_CLASS(r);
}

void Win32::WMIQuery::GetErrorMsg(Text::StringBuilderUTF8 *str)
{
}

void Win32::WMIQuery::Reconnect()
{
	this->Close();
	this->InitQuery(this->ns);
}

const WChar *Win32::WMIQuery::GetNS()
{
	return this->ns;
}

UOSInt Win32::WMIQuery::GetNSList(Data::ArrayList<const WChar *> *nsList)
{
	UOSInt ret = 0;
	Win32::WMIQuery *query;
	Win32::WMIReader *reader;
	WChar wbuff[256];
	WChar *wptr = Text::StrConcat(wbuff, L"ROOT\\");

	NEW_CLASS(query, Win32::WMIQuery(L"ROOT"));
	if (!query->IsError())
	{
		reader = (Win32::WMIReader*)query->ExecuteReader(L"select * from __NAMESPACE");
		if (reader)
		{
			while (reader->ReadNext())
			{
				if (reader->GetStr(L"Name", wptr))
				{
					nsList->Add(Text::StrCopyNew(wbuff));
				}
			}
			query->CloseReader(reader);
		}
	}
	DEL_CLASS(query);
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
