#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include <signal.h>
#include <unistd.h>

Int32 MyMain(NN<Core::ProgControl> progCtrl);

struct LinuxProgControl : public Core::ProgControl
{
	UnsafeArray<UnsafeArray<UTF8Char>> argv;
	IntOS argc;
};

void LinuxProgControl_OnSignal(Int32 sigNum)
{
}

void __stdcall LinuxProgControl_WaitForExit(NN<Core::ProgControl> progCtrl)
{
	signal(SIGINT, LinuxProgControl_OnSignal);
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

UnsafeArray<UnsafeArray<UTF8Char>> __stdcall LinuxProgControl_GetCommandLines(NN<Core::ProgControl> progCtrl, OutParam<UIntOS> cmdCnt)
{
	LinuxProgControl *ctrl = (LinuxProgControl*)progCtrl.Ptr();
	cmdCnt.Set(ctrl->argc);
	return ctrl->argv;
}

void LinuxProgControl_Create(NN<LinuxProgControl> ctrl, IntOS argc, UnsafeArray<UnsafeArray<Char>> argv)
{
	IntOS buffSize;
	IntOS i;
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
	signal(SIGPIPE, SIG_IGN);

	Core::CoreStart();
	LinuxProgControl_Create(conCtrl, argc, (UnsafeArray<Char>*)argv);
	ret = MyMain(conCtrl);
	LinuxProgControl_Destroy(conCtrl);
	Core::CoreEnd();
	return ret;
}

