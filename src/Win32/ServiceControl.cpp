#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Win32/ServiceControl.h"

void __stdcall ServiceControl_WaitForExit(NotNullPtr<Core::IProgControl> progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl.Ptr();
	while (!me->exited)
	{
		me->evt->Wait();
	}
}

UTF8Char **__stdcall ServiceControl_GetCommandLines(NotNullPtr<Core::IProgControl> progCtrl, OutParam<UOSInt> cmdCnt)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl.Ptr();
	cmdCnt.Set(1);
	return &me->argv;
}

void Win32::ServiceControl_Create(NotNullPtr<Core::IProgControl> progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl.Ptr();
	NEW_CLASS(me->evt, Sync::Event(true));
	me->exited = false;
	me->toRestart = false;
	me->argv = (UTF8Char*)Text::StrCopyNewC(UTF8STRC("svchost")).Ptr();

	me->WaitForExit = ServiceControl_WaitForExit;
	me->GetCommandLines = ServiceControl_GetCommandLines;
	me->SignalExit = ServiceControl_SignalExit;
	me->SignalRestart = ServiceControl_SignalRestart;
}

void Win32::ServiceControl_Destroy(NotNullPtr<Core::IProgControl> progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl.Ptr();
	Text::StrDelNew(me->argv);
	DEL_CLASS(me->evt);
}


void Win32::ServiceControl_SignalExit(NotNullPtr<Core::IProgControl> progCtrl)
{
	Win32::ServiceControl *me = (Win32::ServiceControl*)progCtrl.Ptr();
	me->exited = true;
	me->evt->Set();
}

void Win32::ServiceControl_SignalRestart(NotNullPtr<Core::IProgControl> progCtrl)
{
	Win32::ServiceControl* me = (Win32::ServiceControl*)progCtrl.Ptr();
	me->exited = true;
	me->toRestart = true;
	me->evt->Set();
}

