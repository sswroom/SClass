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

Int32 MyMain(Core::IProgControl *progCtrl);

struct LinuxProgControl : public Core::IProgControl
{
	UTF8Char **argv;
	UOSInt argc;
};

void LinuxProgControl_OnSignal(Int32 sigNum)
{
#if defined(SHOW_DEBUG)
	printf("Received signal %d\r\n", sigNum);
#endif
}

void __stdcall LinuxProgControl_WaitForExit(Core::IProgControl *progCtrl)
{
	signal(SIGINT, LinuxProgControl_OnSignal);
	signal(SIGPIPE, LinuxProgControl_OnSignal);
	signal(SIGTERM, LinuxProgControl_OnSignal);
	pause();
//	getchar();
}

UI::GUICore *__stdcall Core::IProgControl::CreateGUICore(Core::IProgControl *progCtrl)
{
	return 0;
}

UTF8Char **__stdcall LinuxProgControl_GetCommandLines(Core::IProgControl *progCtrl, UOSInt *cmdCnt)
{
	LinuxProgControl *ctrl = (LinuxProgControl*)progCtrl;
	*cmdCnt = ctrl->argc;
	return ctrl->argv;
}

void LinuxProgControl_Create(LinuxProgControl *ctrl, UOSInt argc, Char **argv)
{
	ctrl->argv = (UTF8Char**)argv;
	ctrl->argc = argc;

	ctrl->WaitForExit = LinuxProgControl_WaitForExit;
	ctrl->GetCommandLines = LinuxProgControl_GetCommandLines;
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
	LinuxProgControl_Create(&conCtrl, (UOSInt)argc, argv);
	ret = MyMain(&conCtrl);
	LinuxProgControl_Destroy(&conCtrl);
	Core::CoreEnd();
	return ret;
}

