#include "Stdafx.h"
#include "IO/PrivilegeManager.h"
#include <windows.h>

struct IO::PrivilegeManager::ClassData
{
	HANDLE hToken;
};

Bool PrivilegeManager_EnablePermission(HANDLE hProcToken, UnsafeArray<const WChar> wName)
{
	Bool succ = false;
    HRESULT hr; 
    TOKEN_PRIVILEGES tp = {0}; 
    LUID luid; 

    if (!LookupPrivilegeValue(NULL, wName.Ptr(), &luid)) 
    { 
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        printf("Failed to lookup privilege value. hr=0x%08lx\n", hr); 
        return false;
    } 

    // Enable just the SYSTEMTIME privilege 
    tp.PrivilegeCount = 1; 
    tp.Privileges[0].Luid = luid; 
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

    if (!AdjustTokenPrivileges(hProcToken, FALSE, &tp, 0, NULL, NULL)) 
    { 
        hr = HRESULT_FROM_WIN32(GetLastError()); 
        printf("Failed to adjust process token privileges. hr=0x%08lx\n", hr); 
    } 
    else 
    { 
		succ = true;
    } 
    
    return succ;
}

IO::PrivilegeManager::PrivilegeManager()
{
	this->clsData = MemAllocNN(ClassData);
	this->clsData->hToken = 0;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &this->clsData->hToken)) 
    {
		HRESULT hr = HRESULT_FROM_WIN32(GetLastError()); 
        printf("Failed to open process token. hr=0x%08lx\n", hr); 
    } 
}

IO::PrivilegeManager::~PrivilegeManager()
{
    if (this->clsData->hToken != 0) 
    { 
        CloseHandle(this->clsData->hToken); 
    }
	MemFreeNN(this->clsData);
}

Bool IO::PrivilegeManager::EnableSystemTime()
{
	if (this->clsData->hToken == 0)
		return false;
	return PrivilegeManager_EnablePermission(this->clsData->hToken, L"SeSystemtimePrivilege");
}
