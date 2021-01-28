#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Win32/ServiceManager.h"
#include <windows.h>
#undef CreateService

Win32::ServiceManager::ServiceManager()
{
	this->hand = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
}

Win32::ServiceManager::~ServiceManager()
{
	if (this->hand)
	{
		CloseServiceHandle((SC_HANDLE)this->hand);
	}
}

Bool Win32::ServiceManager::CreateService(const WChar *svcName, StartType stype)
{
	if (this->hand == 0)
		return false;

	WChar szPath[512];
	DWORD startType;
	switch (stype)
	{
	case ST_AUTO:
		startType = SERVICE_AUTO_START;
		break;
	case ST_MANUAL:
		startType = SERVICE_DEMAND_START;
		break;
	case ST_DISABLE:
	default:
		startType = SERVICE_DISABLED;
		break;
	}
	IO::Path::GetProcessFileNameW(szPath);

	SC_HANDLE schService = CreateServiceW((SC_HANDLE)this->hand, svcName, svcName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, startType, SERVICE_ERROR_NORMAL, szPath, 0, 0, 0, 0, 0); 
    if (schService == 0) 
    {
		return false;
    }
    CloseServiceHandle(schService);
	return true;
}

Bool Win32::ServiceManager::DeleteService(const WChar *svcName)
{
	if (this->hand == 0)
		return false;

	SC_HANDLE schService = OpenServiceW((SC_HANDLE)this->hand, svcName, DELETE);
    if (schService == 0)
    { 
		return false;
    }

	Bool succ = (::DeleteService(schService) != 0);
	CloseServiceHandle(schService);
	return succ;
}

Bool Win32::ServiceManager::SetServiceDesc(const WChar *svcName, const WChar *svcDesc)
{
	if (this->hand == 0)
		return false;

	SC_HANDLE schService = OpenServiceW((SC_HANDLE)this->hand, svcName, SERVICE_CHANGE_CONFIG);
	if (schService == 0)
	{
		return false;
	}
	SERVICE_DESCRIPTIONW desc;
	desc.lpDescription = (LPWSTR)svcDesc;
	Bool succ = (ChangeServiceConfig2W(schService, SERVICE_CONFIG_DESCRIPTION, &desc) != 0);
	CloseServiceHandle(schService);
	return succ;
}
