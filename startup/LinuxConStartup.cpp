#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>

//#define SHOW_DEBUG
#if defined(DEBUGCON)
#define printf(fmt, ...) {Char sbuff[512]; sprintf(sbuff, fmt, __VA_ARGS__); syslog(LOG_DEBUG, sbuff);}
#endif

Int32 MyMain(NN<Core::ProgControl> progCtrl);

struct LinuxProgControl : public Core::ProgControl
{
	UnsafeArray<UnsafeArray<UTF8Char>> argv;
	UOSInt argc;
};

void LinuxProgControl_OnSignal(Int32 sigNum)
{
#if defined(SHOW_DEBUG)
	printf("Received signal %d\r\n", sigNum);
#endif
}

void __stdcall LinuxProgControl_WaitForExit(NN<Core::ProgControl> progCtrl)
{
	signal(SIGINT, LinuxProgControl_OnSignal);
	signal(SIGPIPE, LinuxProgControl_OnSignal);
	signal(SIGTERM, LinuxProgControl_OnSignal);
	pause();
//	getchar();
}

void __stdcall LinuxProgControl_SignalExit(NN<Core::ProgControl> progCtrl)
{
	raise(SIGINT);
}

Optional<UI::GUICore> __stdcall Core::ProgControl::CreateGUICore(NN<Core::ProgControl> progCtrl)
{
	return nullptr;
}

UnsafeArray<UnsafeArray<UTF8Char>> __stdcall LinuxProgControl_GetCommandLines(NN<Core::ProgControl> progCtrl, OutParam<UOSInt> cmdCnt)
{
	LinuxProgControl *ctrl = (LinuxProgControl*)progCtrl.Ptr();
	cmdCnt.Set(ctrl->argc);
	return ctrl->argv;
}

void LinuxProgControl_Create(NN<LinuxProgControl> ctrl, UOSInt argc, UnsafeArray<UnsafeArray<Char>> argv)
{
	ctrl->argv = UnsafeArray<UnsafeArray<UTF8Char>>::ConvertFrom(argv);
	ctrl->argc = argc;

	ctrl->WaitForExit = LinuxProgControl_WaitForExit;
	ctrl->GetCommandLines = LinuxProgControl_GetCommandLines;
	ctrl->SignalExit = LinuxProgControl_SignalExit;
	ctrl->SignalRestart = LinuxProgControl_SignalExit;
}

void LinuxProgControl_Destroy(NN<LinuxProgControl> ctrl)
{
}



Int32 main(int argc, char *argv[])
{
	Int32 ret;
	LinuxProgControl conCtrl;
	//signal(SIGCHLD, SIG_IGN);

	Core::CoreStart();
	LinuxProgControl_Create(conCtrl, (UOSInt)argc, (UnsafeArray<Char>*)argv);
	ret = MyMain(conCtrl);
	LinuxProgControl_Destroy(conCtrl);
	Core::CoreEnd();
	return ret;
}

