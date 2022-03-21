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
}

IO::ServiceManager::~ServiceManager()
{
	if (this->clsData)
	{
		CloseServiceHandle((SC_HANDLE)this->clsData);
	}
}

Bool IO::ServiceManager::CreateService(Text::CString svcName, IO::ServiceInfo::ServiceState stype)
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

Bool IO::ServiceManager::DeleteService(Text::CString svcName)
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

Bool IO::ServiceManager::SetServiceDesc(Text::CString svcName, Text::CString svcDesc)
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
