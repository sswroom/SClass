#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ConsoleControl.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

Int32 MyMain(NN<Core::ProgControl> progCtrl);

NN<Core::ConsoleControl> ConsoleControl_self;

Int32 __stdcall ConsoleControl_ExitHdlr(UInt32 evtType)
{
	switch (evtType)
	{
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		ConsoleControl_self->ending = true;
		ConsoleControl_self->exited = true;
		ConsoleControl_self->evt->Set();
		while (!ConsoleControl_self->ended)
		{
			ConsoleControl_self->evt->Wait(1000);
		}
		ConsoleControl_self->ending = false;
		ConsoleControl_self->evt->Set();
		break;
	default:
		ConsoleControl_self->exited = true;
		ConsoleControl_self->evt->Set();
		break;
	}
	return TRUE;
}

void __stdcall ConsoleControl_WaitForExit(NN<Core::ProgControl> progCtrl)
{
	NN<Core::ConsoleControl> ctrl = NN<Core::ConsoleControl>::ConvertFrom(progCtrl);
	while (!ctrl->exited)
	{
		ctrl->evt->Wait();
	}
}

void __stdcall ConsoleControl_SignalExit(NN<Core::ProgControl> progCtrl)
{
	NN<Core::ConsoleControl> ctrl = NN<Core::ConsoleControl>::ConvertFrom(progCtrl);
	ctrl->exited = true;
	ctrl->evt->Set();
}

UnsafeArray<UnsafeArray<UTF8Char>> __stdcall ConsoleControl_GetCommandLines(NN<Core::ProgControl> progCtrl, OutParam<UOSInt> cmdCnt)
{
	NN<Core::ConsoleControl> ctrl = NN<Core::ConsoleControl>::ConvertFrom(progCtrl);
	UnsafeArray<UnsafeArray<UTF8Char>> nnargv;
	if (!ctrl->argv.SetTo(nnargv))
	{
		Int32 argc;
		OSInt i;
		WChar *cmdLine = GetCommandLineW();
		WChar **argv = CommandLineToArgvW(cmdLine, &argc);
		ctrl->argc = (UOSInt)argc;
		ctrl->argv = nnargv = MemAllocArr(UnsafeArray<UTF8Char>, ctrl->argc);
		i = argc;
		while (i-- > 0)
		{
			nnargv[i] = UnsafeArray<UTF8Char>::ConvertFrom(Text::StrToUTF8New(argv[i]));
		}
		LocalFree(argv);
	}
	cmdCnt.Set(ctrl->argc);
	return nnargv;
}

void ConsoleControl_Create(NN<Core::ConsoleControl> ctrl)
{
	ConsoleControl_self = ctrl;
	ctrl->argc = 0;
	ctrl->argv = 0;
	ctrl->exited = false;
	ctrl->ending = false;
	ctrl->ended = false;
	NEW_CLASSNN(ctrl->evt, Sync::Event(true));
	ctrl->WaitForExit = ConsoleControl_WaitForExit;
	ctrl->GetCommandLines = ConsoleControl_GetCommandLines;
	ctrl->SignalExit = ConsoleControl_SignalExit;
	ctrl->SignalRestart = ConsoleControl_SignalExit;

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleControl_ExitHdlr, TRUE);
}

void ConsoleControl_Destroy(NN<Core::ConsoleControl> ctrl)
{
	ctrl->ended = true;
	ctrl->evt->Set();
	while (ctrl->ending)
	{
		ctrl->evt->Wait(100);
	}
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleControl_ExitHdlr, FALSE);
	ctrl->evt.Delete();
#ifndef __CYGWIN__
	UnsafeArray<UnsafeArray<UTF8Char>> argv;
	if (ctrl->argv.SetTo(argv))
	{
		UOSInt i = ctrl->argc;
		while (i-- > 0)
		{
			Text::StrDelNew(UnsafeArray<const UTF8Char>(argv[i]));
		}
		MemFreeArr(argv);
	}
#endif
}

Optional<UI::GUICore> Core::ProgControl::CreateGUICore(NN<Core::ProgControl> progCtrl)
{
	return 0;
}

Int32 main(int argc, char *argv[])
{
	Int32 ret;
	Core::ConsoleControl conCtrl;
	Core::CoreStart();
	ConsoleControl_Create(conCtrl);
#ifdef __CYGWIN__
	conCtrl.argc = argc;
	conCtrl.argv = (UnsafeArray<UTF8Char>*)argv;
#endif
	ret = MyMain(conCtrl);
	ConsoleControl_Destroy(conCtrl);
	Core::CoreEnd();
	return ret;
}
