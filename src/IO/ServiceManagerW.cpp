#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/ServiceManager.h"
#include "Manage/Process.h"
#include "Text/MyStringW.h"
#include <windows.h>
#undef CreateService

IO::ServiceInfo::RunStatus ServiceManager_CurrentState2RunStatus(DWORD dwCurrentState)
{
	switch (dwCurrentState)
	{
	case SERVICE_RUNNING:
		return IO::ServiceInfo::RunStatus::Running;
	case SERVICE_CONTINUE_PENDING:
	case SERVICE_START_PENDING:
		return IO::ServiceInfo::RunStatus::Starting;
	case SERVICE_STOP_PENDING:
		return IO::ServiceInfo::RunStatus::Stopping;
	case SERVICE_STOPPED:
		return IO::ServiceInfo::RunStatus::Stopped;
	case SERVICE_PAUSE_PENDING:
	case SERVICE_PAUSED:
	default:
		return IO::ServiceInfo::RunStatus::Unknown;
	}

}

OSInt IO::ServiceManager::ServiceComparator::Compare(ServiceItem* a, ServiceItem* b) const
{
	return a->name->CompareTo(b->name);
}

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

Bool IO::ServiceManager::ServiceCreate(Text::CString svcName, Text::CString svcDesc, Text::CString cmdLine, IO::ServiceInfo::ServiceState stype)
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
	WChar wname[512];
	Text::StrUTF8_WChar(szPath, cmdLine.v, 0);
	Text::StrUTF8_WChar(wname, svcName.v, 0);

	SC_HANDLE schService = CreateServiceW((SC_HANDLE)this->clsData, wname, wname, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, startType, SERVICE_ERROR_NORMAL, szPath, 0, 0, 0, 0, 0); 
    if (schService == 0) 
    {
		return false;
    }
	if (svcDesc.v)
	{
		Text::StrUTF8_WChar(wname, svcDesc.v, 0);
		SERVICE_DESCRIPTIONW desc;
		desc.lpDescription = (LPWSTR)wname;
		ChangeServiceConfig2W(schService, SERVICE_CONFIG_DESCRIPTION, &desc);
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

Bool IO::ServiceManager::ServiceEnable(Text::CString svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceDisable(Text::CString svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceGetDetail(Text::CString svcName, ServiceDetail* svcDetail)
{
	if (this->clsData == 0)
		return false;

	WChar wname[512];
	Text::StrUTF8_WChar(wname, svcName.v, 0);
	SC_HANDLE schService = OpenServiceW((SC_HANDLE)this->clsData, wname, SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG);
	if (schService == 0)
	{
		return false;
	}
	UInt8* buff = MemAlloc(UInt8, 8192);
	SERVICE_STATUS_PROCESS *status = (SERVICE_STATUS_PROCESS*)buff;
	DWORD bytesNeeded;
	Bool succ = (QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, buff, 8192, &bytesNeeded) != 0);
	if (succ)
	{
		svcDetail->status = ServiceManager_CurrentState2RunStatus(status->dwCurrentState);
		svcDetail->procId = status->dwProcessId;
		svcDetail->memoryUsage = 0;
		svcDetail->startTime = 0;
		svcDetail->enabled = IO::ServiceInfo::ServiceState::Unknown;
	}
	QUERY_SERVICE_CONFIGW* svcConfig = (QUERY_SERVICE_CONFIGW*)buff;
	if (QueryServiceConfigW(schService, svcConfig, 8192, &bytesNeeded) != 0)
	{
		switch (svcConfig->dwStartType)
		{
		case SERVICE_BOOT_START:
		case SERVICE_SYSTEM_START:
		case SERVICE_AUTO_START:
			svcDetail->enabled = IO::ServiceInfo::ServiceState::Active;
			break;
		case SERVICE_DEMAND_START:
			svcDetail->enabled = IO::ServiceInfo::ServiceState::ManualStart;
			break;
		case SERVICE_DISABLED:
			svcDetail->enabled = IO::ServiceInfo::ServiceState::Inactive;
			break;
		}
	}
	CloseServiceHandle(schService);
	if (succ && svcDetail->procId)
	{
		Manage::Process proc(svcDetail->procId, false);
		svcDetail->memoryUsage = proc.GetMemorySize();
		svcDetail->startTime = proc.GetStartTime();
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
			svc->state = IO::ServiceInfo::ServiceState::Unknown;
			svc->runStatus = ServiceManager_CurrentState2RunStatus(services[i].ServiceStatus.dwCurrentState);
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
