#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Win32/ServiceControl.h"

void __stdcall ServiceControl_WaitForExit(Core::IProgControl *progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl;
	while (!me->exited)
	{
		me->evt->Wait();
	}
}

UTF8Char **__stdcall ServiceControl_GetCommandLines(Core::IProgControl *progCtrl, UOSInt *cmdCnt)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl;
	*cmdCnt = 1;
	return &me->argv;
}

void Win32::ServiceControl_Create(Core::IProgControl *progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl;
	NEW_CLASS(me->evt, Sync::Event(true, (const UTF8Char*)"Win32.ServiceControl.evt"));
	me->exited = false;
	me->argv = (UTF8Char*)Text::StrCopyNew((const UTF8Char*)"svchost");

	me->WaitForExit = ServiceControl_WaitForExit;
	me->GetCommandLines = ServiceControl_GetCommandLines;
}

void Win32::ServiceControl_Destroy(Core::IProgControl *progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl;
	Text::StrDelNew(me->argv);
	DEL_CLASS(me->evt);
}


void Win32::ServiceControl_SignalExit(Core::IProgControl *progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl;
	me->exited = true;
	me->evt->Set();
}

