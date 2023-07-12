#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Win32/ServiceControl.h"

void __stdcall ServiceControl_WaitForExit(NotNullPtr<Core::IProgControl> progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl;
	while (!me->exited)
	{
		me->evt->Wait();
	}
}

UTF8Char **__stdcall ServiceControl_GetCommandLines(NotNullPtr<Core::IProgControl> progCtrl, UOSInt *cmdCnt)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl;
	*cmdCnt = 1;
	return &me->argv;
}

void Win32::ServiceControl_Create(NotNullPtr<Core::IProgControl> progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl;
	NEW_CLASS(me->evt, Sync::Event(true));
	me->exited = false;
	me->toRestart = false;
	me->argv = (UTF8Char*)Text::StrCopyNew((const UTF8Char*)"svchost");

	me->WaitForExit = ServiceControl_WaitForExit;
	me->GetCommandLines = ServiceControl_GetCommandLines;
	me->SignalExit = ServiceControl_SignalExit;
	me->SignalRestart = ServiceControl_SignalRestart;
}

void Win32::ServiceControl_Destroy(NotNullPtr<Core::IProgControl> progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl;
	Text::StrDelNew(me->argv);
	DEL_CLASS(me->evt);
}


void Win32::ServiceControl_SignalExit(NotNullPtr<Core::IProgControl> progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl;
	me->exited = true;
	me->evt->Set();
}

void Win32::ServiceControl_SignalRestart(Core::IProgControl* progCtrl)
{
	Win32::ServiceControl* me = (Win32::ServiceControl*)progCtrl;
	me->exited = true;
	me->toRestart = true;
	me->evt->Set();
}

