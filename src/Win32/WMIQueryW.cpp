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


void Win32::WMIQuery::InitQuery(UnsafeArray<const WChar> ns)
{
	this->pService = 0;
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
			BSTR nsstr = SysAllocString(ns.Ptr());
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

Win32::WMIQuery::WMIQuery(UnsafeArray<const WChar> ns) : DB::DBConn(CSTR("WMIQuery"))
{
	this->ns = Text::StrCopyNew(ns);
	this->InitQuery(this->ns);
}

Win32::WMIQuery::~WMIQuery()
{
	this->Close();
	Text::StrDelNew(this->ns);
}

Bool Win32::WMIQuery::IsError()
{
	return this->pService == 0;
}

DB::SQLType Win32::WMIQuery::GetSQLType() const
{
	return DB::SQLType::WBEM;
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

void Win32::WMIQuery::GetConnName(NN<Text::StringBuilderUTF8> sb)
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

OSInt Win32::WMIQuery::ExecuteNonQuery(Text::CStringNN sql)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(sql.v);
	OSInt ret = this->ExecuteNonQueryW(wptr);
	Text::StrDelNew(wptr);
	return ret;
}

OSInt Win32::WMIQuery::ExecuteNonQueryW(UnsafeArray<const WChar> sql)
{
	this->lastDataError = DE_EXEC_SQL_ERROR;
	return -2;
}

Optional<DB::DBReader> Win32::WMIQuery::ExecuteReader(Text::CStringNN sqlCmd)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(sqlCmd.v);
	Optional<DB::DBReader> r = this->ExecuteReaderW(wptr);
	Text::StrDelNew(wptr);
	return r;
}

Optional<DB::DBReader> Win32::WMIQuery::ExecuteReaderW(UnsafeArray<const WChar> sqlCmd)
{
	HRESULT hr;
	BSTR query = SysAllocString(sqlCmd.Ptr());
	BSTR wql = SysAllocString(L"WQL");
	IEnumWbemClassObject *pEnum;
	hr = ((IWbemServices*)this->pService)->ExecQuery(wql, query, WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY, NULL, &pEnum);
	SysFreeString(wql);
	SysFreeString(query);
	if (SUCCEEDED(hr))
	{
		this->lastDataError = DE_NO_ERROR;
		NN<Win32::WMIReader> reader;
		NEW_CLASSNN(reader, Win32::WMIReader(pEnum));
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

Optional<DB::DBTransaction> Win32::WMIQuery::BeginTransaction()
{
	return 0;
}

void Win32::WMIQuery::Commit(NN<DB::DBTransaction> tran)
{
}

void Win32::WMIQuery::Rollback(NN<DB::DBTransaction> tran)
{
}

UOSInt Win32::WMIQuery::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
		return 0;
	if (this->pService == 0)
	{
		return 0;
	}
	UOSInt initCnt = names->GetCount();
	IEnumWbemClassObject *pEnum;
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
					names->Add(Text::String::NewNotNull(bs));
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
	return names->GetCount() - initCnt;
}

Optional<DB::DBReader> Win32::WMIQuery::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	WChar wbuff[256];
	Text::StrUTF8_WChar(Text::StrConcat(wbuff, L"SELECT * FROM "), tableName.v, 0);
	return this->ExecuteReaderW(wbuff);
}

void Win32::WMIQuery::CloseReader(NN<DB::DBReader> reader)
{
	Win32::WMIReader *r = (Win32::WMIReader*)reader.Ptr();
	DEL_CLASS(r);
}

void Win32::WMIQuery::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

void Win32::WMIQuery::Reconnect()
{
	this->Close();
	this->InitQuery(this->ns);
}

UnsafeArray<const WChar> Win32::WMIQuery::GetNS()
{
	return this->ns;
}

UOSInt Win32::WMIQuery::GetNSList(NN<Data::ArrayListArr<const WChar>> nsList)
{
	UOSInt ret = 0;
	Win32::WMIQuery *query;
	NN<Win32::WMIReader> reader;
	WChar wbuff[256];
	UnsafeArray<WChar> wptr = Text::StrConcat(wbuff, L"ROOT\\");

	NEW_CLASS(query, Win32::WMIQuery(L"ROOT"));
	if (!query->IsError())
	{
		if (Optional<Win32::WMIReader>::ConvertFrom(query->ExecuteReaderW(L"select * from __NAMESPACE")).SetTo(reader))
		{
			while (reader->ReadNext())
			{
				if (reader->GetStr(L"Name", wptr).NotNull())
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

void Win32::WMIQuery::FreeNSList(NN<Data::ArrayListArr<const WChar>> nsList)
{
	UOSInt i = nsList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(nsList->GetItemNoCheck(i));
	}
	nsList->Clear();
}
