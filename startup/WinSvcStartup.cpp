#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/FileVersion.h"
#include "IO/Path.h"
#include "IO/ServiceManager.h"
#include "Media/GDIEngine.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Win32/ServiceControl.h"
#include "Win32/WindowsEvent.h"
#include <windows.h>
#include <stdio.h>

#undef CreateService

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl);

SERVICE_STATUS          gSvcStatus; 
SERVICE_STATUS_HANDLE   gSvcStatusHandle; 
NotNullPtr<Win32::ServiceControl> progCtrl;
WChar svcName[256];

void SvcReportEvent(const WChar *svcName, const WChar *szFunction) 
{ 
	Win32::WindowsEvent evt(svcName);
	WChar wbuff[256];
	Text::StrHexVal32(Text::StrConcat(Text::StrConcat(wbuff, szFunction), L" failed with "), GetLastError());
	evt.WriteEvent(0xC0020001L, Win32::WindowsEvent::EventType::Error, wbuff, 0, 0);
}

VOID ReportSvcStatus( DWORD dwCurrentState,
                      DWORD dwWin32ExitCode,
                      DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ( (dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED) )
		gSvcStatus.dwCheckPoint = 0;
	else gSvcStatus.dwCheckPoint = dwCheckPoint++;

	SetServiceStatus( gSvcStatusHandle, &gSvcStatus );
}

void SvcUninstall(const WChar *svcName)
{
	IO::ServiceManager svcMgr;
	NotNullPtr<Text::String> s = Text::String::NewNotNull(svcName);
	if (svcMgr.ServiceDelete(s->ToCString()))
	{
		printf("Service deleted successfully\n");
	}
	else
	{
		printf("DeleteService failed (%d)\n", (UInt32)GetLastError()); 
	}
	s->Release();
}

void SvcInstall(const WChar *svcName, const WChar *svcDesc)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::ServiceManager svcMgr;
	NotNullPtr<Text::String> s = Text::String::NewNotNull(svcName);
	NotNullPtr<Text::String> sDesc = Text::String::NewNotNull(svcDesc);
	sptr = IO::Path::GetProcessFileName(sbuff);
	if (svcMgr.ServiceCreate(s->ToCString(), sDesc->ToCString(), CSTRP(sbuff, sptr), IO::ServiceInfo::ServiceState::Active))
	{
		printf("Service installed successfully\n");
	}
	else
	{
		printf("CreateService failed (%d)\n", GetLastError()); 
	}
	sDesc->Release();
	s->Release();
}


VOID WINAPI SvcCtrlHandler( DWORD dwCtrl )
{
   switch(dwCtrl) 
   {  
      case SERVICE_CONTROL_STOP: 
         ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		 Win32::ServiceControl_SignalExit(progCtrl);
         ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
         
         return;
 
      case SERVICE_CONTROL_INTERROGATE: 
         break; 
 
      default: 
         break;
   } 
   
}

VOID SvcInit( DWORD dwArgc, LPWSTR *lpszArgv)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	ReportSvcStatus( SERVICE_RUNNING, NO_ERROR, 0 );

	UOSInt i;
	sptr = IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\');
	if (i != INVALID_INDEX)
	{
		sbuff[i] = 0;
	}
	IO::Path::SetCurrDirectory(sbuff);
	Int32 ret = MyMain(progCtrl);
	if (progCtrl->toRestart)
	{
		exit(ret);
	}
	else
	{
		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
	}
}

VOID WINAPI SvcMain( DWORD dwArgc, LPWSTR *lpszArgv )
{
	Core::CoreStart();
	progCtrl = MemAllocNN(Win32::ServiceControl, 1);
	Win32::ServiceControl_Create(progCtrl);

	gSvcStatusHandle = RegisterServiceCtrlHandlerW( svcName, SvcCtrlHandler);

	if( gSvcStatusHandle == 0)
	{ 
		SvcReportEvent(svcName, L"RegisterServiceCtrlHandler");
	}
	else
	{
		gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
		gSvcStatus.dwServiceSpecificExitCode = 0;    

		ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );

		SvcInit( dwArgc, lpszArgv );
	}

	Win32::ServiceControl_Destroy(progCtrl);
	MemFreeNN(progCtrl);
	Core::CoreEnd();
}

int main(int argc, char *argv[]) 
{ 
	WChar svcDesc[512];
	UTF8Char tmpStr[512];
	Bool found = false;
	Core::CoreStart();
	{
		IO::FileVersion *ver = IO::FileVersion::Open(0);
		if (ver)
		{
			Int32 lang = ver->GetFirstLang();
			ver->GetInternalName(lang, tmpStr);
			Text::StrUTF8_WChar(svcName, tmpStr, 0);
			ver->GetFileDescription(lang, tmpStr);
			Text::StrUTF8_WChar(svcDesc, tmpStr, 0);
			DEL_CLASS(ver);
			found = true;
		}
	}
	Core::CoreEnd();
	if (!found)
	{
		printf("Version Information not found\n");
		return 1;
	}

	if(argc == 2 && Text::StrEquals(argv[1], "install"))
	{
		Core::CoreStart();
		SvcInstall(svcName, svcDesc);
		Core::CoreEnd();
		return 0;
	}
	else if (argc == 2 && Text::StrEquals(argv[1], "uninstall"))
	{
		Core::CoreStart();
		SvcUninstall(svcName);
		Core::CoreEnd();
		return 0;
	}
	else if (argc == 2 && Text::StrEquals(argv[1], "run"))
	{
		Core::CoreStart();
		progCtrl = MemAllocNN(Win32::ServiceControl, 1);
		Win32::ServiceControl_Create(progCtrl);
		Int32 ret = MyMain(progCtrl);
		Win32::ServiceControl_Destroy(progCtrl);
		MemFreeNN(progCtrl);
		Core::CoreEnd();
		return ret;
	}

	SERVICE_TABLE_ENTRYW DispatchTable[] = 
	{
		{ svcName, (LPSERVICE_MAIN_FUNCTIONW) SvcMain }, 
		{ NULL, NULL } 
	};
 
	if (!StartServiceCtrlDispatcherW( DispatchTable )) 
	{ 
		Core::CoreStart();
		SvcReportEvent(svcName, L"StartServiceCtrlDispatcher");
		Core::CoreEnd();
	}
	return 0;
} 

Optional<UI::GUICore> Core::IProgControl::CreateGUICore(NotNullPtr<Core::IProgControl> progCtrl)
{
	return 0;
}
