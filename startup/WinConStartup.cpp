#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ConsoleControl.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

Int32 MyMain(NN<Core::IProgControl> progCtrl);

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

void __stdcall ConsoleControl_WaitForExit(NN<Core::IProgControl> progCtrl)
{
	Core::ConsoleControl *ctrl = (Core::ConsoleControl *)progCtrl.Ptr();
	while (!ctrl->exited)
	{
		ctrl->evt->Wait();
	}
}

void __stdcall ConsoleControl_SignalExit(NN<Core::IProgControl> progCtrl)
{
	Core::ConsoleControl *ctrl = (Core::ConsoleControl *)progCtrl.Ptr();
	ctrl->exited = true;
	ctrl->evt->Set();
}

UTF8Char **__stdcall ConsoleControl_GetCommandLines(NN<Core::IProgControl> progCtrl, OutParam<UOSInt> cmdCnt)
{
	Core::ConsoleControl *ctrl = (Core::ConsoleControl *)progCtrl.Ptr();
	if (ctrl->argv == 0)
	{
		Int32 argc;
		OSInt i;
		WChar *cmdLine = GetCommandLineW();
		WChar **argv = CommandLineToArgvW(cmdLine, &argc);
		ctrl->argc = (UOSInt)argc;
		ctrl->argv = MemAlloc(UTF8Char *, ctrl->argc);
		i = argc;
		while (i-- > 0)
		{
			ctrl->argv[i] = (UTF8Char*)Text::StrToUTF8New(argv[i]).Ptr();
		}
		LocalFree(argv);
	}
	cmdCnt.Set(ctrl->argc);
	return ctrl->argv;
}

void ConsoleControl_Create(NN<Core::ConsoleControl> ctrl)
{
	ConsoleControl_self = ctrl;
	ctrl->argc = 0;
	ctrl->argv = 0;
	ctrl->exited = false;
	ctrl->ending = false;
	ctrl->ended = false;
	NEW_CLASS(ctrl->evt, Sync::Event(true));
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
	DEL_CLASS(ctrl->evt);
#ifndef __CYGWIN__
	if (ctrl->argv)
	{
		UOSInt i = ctrl->argc;
		while (i-- > 0)
		{
			Text::StrDelNew(ctrl->argv[i]);
		}
		MemFree(ctrl->argv);
		ctrl->argv = 0;
	}
#endif
}

Optional<UI::GUICore> Core::IProgControl::CreateGUICore(NN<Core::IProgControl> progCtrl)
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
	conCtrl.argv = (UTF8Char**)argv;
#endif
	ret = MyMain(conCtrl);
	ConsoleControl_Destroy(conCtrl);
	Core::CoreEnd();
	return ret;
}
