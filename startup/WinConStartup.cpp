#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ConsoleControl.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

Int32 MyMain(Core::IProgControl *progCtrl);

Core::ConsoleControl *ConsoleControl_self;

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

void __stdcall ConsoleControl_WaitForExit(Core::IProgControl *progCtrl)
{
	Core::ConsoleControl *ctrl = (Core::ConsoleControl *)progCtrl;
	while (!ctrl->exited)
	{
		ctrl->evt->Wait();
	}
}

void __stdcall ConsoleControl_SignalExit(Core::IProgControl *progCtrl)
{
	Core::ConsoleControl *ctrl = (Core::ConsoleControl *)progCtrl;
	ctrl->exited = true;
	ctrl->evt->Set();
}

UTF8Char **__stdcall ConsoleControl_GetCommandLines(Core::IProgControl *progCtrl, UOSInt *cmdCnt)
{
	Core::ConsoleControl *ctrl = (Core::ConsoleControl *)progCtrl;
	if (ctrl->argv == 0)
	{
		Int32 argc;
		OSInt i;
		WChar **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
		ctrl->argc = (UOSInt)argc;
		ctrl->argv = MemAlloc(UTF8Char *, ctrl->argc);
		i = argc;
		while (i-- > 0)
		{
			ctrl->argv[i] = (UTF8Char*)Text::StrToUTF8New(argv[i]);
		}
		LocalFree(argv);
	}
	*cmdCnt = ctrl->argc;
	return ctrl->argv;
}

void ConsoleControl_Create(Core::ConsoleControl *ctrl)
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

void ConsoleControl_Destroy(Core::ConsoleControl *ctrl)
{
	ctrl->ended = true;
	ctrl->evt->Set();
	while (ctrl->ending)
	{
		ctrl->evt->Wait(100);
	}
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleControl_ExitHdlr, FALSE);
	DEL_CLASS(ctrl->evt);
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
}

UI::GUICore *Core::IProgControl::CreateGUICore(Core::IProgControl *progCtrl)
{
	return 0;
}

Int32 main()
{
	Int32 ret;
	Core::ConsoleControl conCtrl;
	Core::CoreStart();
	ConsoleControl_Create(&conCtrl);
	ret = MyMain(&conCtrl);
	ConsoleControl_Destroy(&conCtrl);
	Core::CoreEnd();
	return ret;
}