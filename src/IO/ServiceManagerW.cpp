#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/ServiceManager.h"
#include "Text/MyStringW.h"
#include <windows.h>
#undef CreateService

IO::ServiceManager::ServiceManager()
{
	this->clsData = (ClassData*)OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
	if (this->clsData == 0)
	{
		this->clsData = (ClassData*)OpenSCManager(0, 0, SC_MANAGER_ENUMERATE_SERVICE);
	}
}

IO::ServiceManager::~ServiceManager()
{
	if (this->clsData)
	{
		CloseServiceHandle((SC_HANDLE)this->clsData);
	}
}

Bool IO::ServiceManager::ServiceCreate(Text::CString svcName, IO::ServiceInfo::ServiceState stype)
{
	if (this->clsData == 0)
		return false;

	WChar szPath[512];
	DWORD startType;
	switch (stype)
	{
	case IO::ServiceInfo::ServiceState::Active:
		startType = SERVICE_AUTO_START;
		break;
	case IO::ServiceInfo::ServiceState::ManualStart:
		startType = SERVICE_DEMAND_START;
		break;
	case IO::ServiceInfo::ServiceState::Inactive:
		startType = SERVICE_DISABLED;
		break;
	default:
		return false;
	}
	IO::Path::GetProcessFileNameW(szPath);
	WChar wname[512];
	Text::StrUTF8_WChar(wname, svcName.v, 0);

	SC_HANDLE schService = CreateServiceW((SC_HANDLE)this->clsData, wname, wname, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, startType, SERVICE_ERROR_NORMAL, szPath, 0, 0, 0, 0, 0); 
    if (schService == 0) 
    {
		return false;
    }
    CloseServiceHandle(schService);
	return true;
}

Bool IO::ServiceManager::ServiceDelete(Text::CString svcName)
{
	if (this->clsData == 0)
		return false;

	WChar wname[512];
	Text::StrUTF8_WChar(wname, svcName.v, 0);
	SC_HANDLE schService = OpenServiceW((SC_HANDLE)this->clsData, wname, DELETE);
    if (schService == 0)
    { 
		return false;
    }

	Bool succ = (::DeleteService(schService) != 0);
	CloseServiceHandle(schService);
	return succ;
}

Bool IO::ServiceManager::ServiceSetDesc(Text::CString svcName, Text::CString svcDesc)
{
	if (this->clsData == 0)
		return false;

	WChar wname[512];
	Text::StrUTF8_WChar(wname, svcName.v, 0);
	SC_HANDLE schService = OpenServiceW((SC_HANDLE)this->clsData, wname, SERVICE_CHANGE_CONFIG);
	if (schService == 0)
	{
		return false;
	}
	Text::StrUTF8_WChar(wname, svcDesc.v, 0);
	SERVICE_DESCRIPTIONW desc;
	desc.lpDescription = (LPWSTR)wname;
	Bool succ = (ChangeServiceConfig2W(schService, SERVICE_CONFIG_DESCRIPTION, &desc) != 0);
	CloseServiceHandle(schService);
	return succ;
}

Bool IO::ServiceManager::ServiceStart(Text::CString svcName)
{
	if (this->clsData == 0)
		return false;

	WChar wname[512];
	Text::StrUTF8_WChar(wname, svcName.v, 0);
	SC_HANDLE schService = OpenServiceW((SC_HANDLE)this->clsData, wname, SERVICE_START);
	if (schService == 0)
	{
		return false;
	}
	Bool succ = (StartServiceW(schService, 0, 0) != 0);
	CloseServiceHandle(schService);
	return succ;
}

Bool IO::ServiceManager::ServiceStop(Text::CString svcName)
{
	if (this->clsData == 0)
		return false;

	WChar wname[512];
	Text::StrUTF8_WChar(wname, svcName.v, 0);
	SC_HANDLE schService = OpenServiceW((SC_HANDLE)this->clsData, wname, SERVICE_STOP);
	if (schService == 0)
	{
		return false;
	}
	SERVICE_STATUS status;
	Bool succ = (ControlService(schService, SERVICE_CONTROL_STOP, &status) != 0);
	CloseServiceHandle(schService);
	return succ;
}

Bool IO::ServiceManager::ServiceGetDetail(Text::CString svcName, ServiceDetail* svcDetail)
{
	if (this->clsData == 0)
		return false;

	WChar wname[512];
	Text::StrUTF8_WChar(wname, svcName.v, 0);
	SC_HANDLE schService = OpenServiceW((SC_HANDLE)this->clsData, wname, SERVICE_QUERY_STATUS);
	if (schService == 0)
	{
		return false;
	}
	UInt8* buff = MemAlloc(UInt8, 8192);
	SERVICE_STATUS_PROCESS *status = (SERVICE_STATUS_PROCESS*)buff;
	DWORD bytesNeeded;
	Bool succ = (QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, buff, 8192, &bytesNeeded) != 0);
	CloseServiceHandle(schService);
	if (succ)
	{
		switch (status->dwCurrentState)
		{
		case SERVICE_RUNNING:
			svcDetail->status = IO::ServiceInfo::RunStatus::Running;
			break;
		case SERVICE_CONTINUE_PENDING:
		case SERVICE_START_PENDING:
			svcDetail->status = IO::ServiceInfo::RunStatus::Starting;
			break;
		case SERVICE_STOP_PENDING:
			svcDetail->status = IO::ServiceInfo::RunStatus::Stopping;
			break;
		case SERVICE_STOPPED:
			svcDetail->status = IO::ServiceInfo::RunStatus::Stopped;
			break;
		case SERVICE_PAUSE_PENDING:
		case SERVICE_PAUSED:
		default:
			svcDetail->status = IO::ServiceInfo::RunStatus::Unknown;
			break;
		}
		svcDetail->procId = status->dwProcessId;
		svcDetail->memoryUsage = 0;
		svcDetail->startTimeTicks = 0;
	}
	MemFree(buff);
	return succ;
}

UOSInt IO::ServiceManager::QueryServiceList(Data::ArrayList<ServiceItem*>* svcList)
{
	DWORD bytesNeeded;
	DWORD nServices;
	if (EnumServicesStatusW((SC_HANDLE)this->clsData, SERVICE_WIN32, SERVICE_STATE_ALL, 0, 0, &bytesNeeded, &nServices, 0))
	{
		return 0;
	}
	DWORD lastError = GetLastError();
	if (lastError != ERROR_INSUFFICIENT_BUFFER && lastError != ERROR_MORE_DATA)
	{
		return 0;
	}
	UOSInt ret = 0;
	ServiceItem *svc;
	ENUM_SERVICE_STATUSW* services = (ENUM_SERVICE_STATUSW*)MAlloc(bytesNeeded);
	if (EnumServicesStatusW((SC_HANDLE)this->clsData, SERVICE_WIN32, SERVICE_STATE_ALL, services, bytesNeeded, &bytesNeeded, &nServices, 0))
	{
		UOSInt i = 0;
		while (i < nServices)
		{
			svc = MemAlloc(ServiceItem, 1);
			svc->name = Text::String::NewNotNull(services[i].lpServiceName);
			svc->state = IO::ServiceInfo::ServiceState::Active;
			svcList->Add(svc);
			ret++;
			i++;
		}
	}
	MemFree(services);
	return ret;
}

void IO::ServiceManager::FreeServiceList(Data::ArrayList<ServiceItem*>* svcList)
{
	ServiceItem *svc;
	UOSInt i = svcList->GetCount();
	while (i-- > 0)
	{
		svc = svcList->GetItem(i);
		svc->name->Release();
		MemFree(svc);
	}
	svcList->Clear();
}
