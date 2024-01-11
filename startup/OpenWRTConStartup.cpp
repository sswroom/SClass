#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include <signal.h>
#include <unistd.h>

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl);

struct LinuxProgControl : public Core::IProgControl
{
	UTF8Char **argv;
	OSInt argc;
};

void LinuxProgControl_OnSignal(Int32 sigNum)
{
}

void __stdcall LinuxProgControl_WaitForExit(NotNullPtr<Core::IProgControl> progCtrl)
{
	signal(SIGINT, LinuxProgControl_OnSignal);
	signal(SIGPIPE, LinuxProgControl_OnSignal);
	pause();
//	getchar();
}

void __stdcall LinuxProgControl_SignalExit(NotNullPtr<Core::IProgControl> progCtrl)
{
	raise(SIGINT);
}

Optional<UI::GUICore> __stdcall Core::IProgControl::CreateGUICore(NotNullPtr<Core::IProgControl> progCtrl)
{
	return 0;
}

UTF8Char **__stdcall LinuxProgControl_GetCommandLines(NotNullPtr<Core::IProgControl> progCtrl, OutParam<UOSInt> cmdCnt)
{
	LinuxProgControl *ctrl = (LinuxProgControl*)progCtrl.Ptr();
	cmdCnt.Set(ctrl->argc);
	return ctrl->argv;
}

void LinuxProgControl_Create(LinuxProgControl *ctrl, OSInt argc, Char **argv)
{
	OSInt buffSize;
	OSInt i;
	ctrl->argv = (UTF8Char**)argv;
	ctrl->argc = argc;

	ctrl->WaitForExit = LinuxProgControl_WaitForExit;
	ctrl->GetCommandLines = LinuxProgControl_GetCommandLines;
	ctrl->SignalExit = LinuxProgControl_SignalExit;
	ctrl->SignalRestart = LinuxProgControl_SignalExit;
}

void LinuxProgControl_Destroy(LinuxProgControl *ctrl)
{
}

Int32 main(int argc, char *argv[])
{
	Int32 ret;
	LinuxProgControl conCtrl;
	//signal(SIGCHLD, SIG_IGN);

	Core::CoreStart();
	LinuxProgControl_Create(&conCtrl, argc, argv);
	ret = MyMain(conCtrl);
	LinuxProgControl_Destroy(&conCtrl);
	Core::CoreEnd();
	return ret;
}

