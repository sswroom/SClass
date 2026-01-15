#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Library.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Win/WinCore.h"
#include <windows.h>
#ifndef __CYGWIN__
#include <crtdbg.h>
#endif

typedef BOOL (WINAPI *BoolFunc)();
typedef HRESULT (WINAPI *AwareFunc)(IntOS val);
typedef BOOL (WINAPI *AwareFunc2)(IntOS val);

Int32 MyMain(NN<Core::ProgControl> ctrl);

struct WinProgControl : public Core::ProgControl
{
	UI::InstanceHandle *hInst;
	UIntOS argc;
	UnsafeArrayOpt<UnsafeArray<UTF8Char>> argv;
};

void __stdcall WinProgControl_WaitForExit(NN<Core::ProgControl> progCtrl)
{
}

#ifdef _WIN32_WCE
WChar **CommandLineToArgvW(const WChar *cmdLine, Int32 *argc)
{
	Int32 cnt = 0;
	Int32 i;
	WChar c;
	Bool isC = false;
	Bool isQuote = false;
	const WChar *s = cmdLine;
	WChar *outPtr;
	IntOS chCnt = 0;
	while (true)
	{
		c = *s++;
		if (c == 0)
		{
			break;
		}
		else if (c == '"')
		{
			isQuote = !isQuote;
		}
		else if (c == ' ' && !isQuote)
		{
			isC = false;
		}
		else
		{
			if (!isC)
			{
				isC = true;
				cnt++;
			}
			chCnt++;
		}
	}
	if (chCnt <= 0)
		return 0;
	WChar **ret = (WChar **)LocalAlloc(LMEM_FIXED, sizeof(IntOS) * cnt + (chCnt + cnt) * sizeof(WChar));
	outPtr = (WChar*)&((UInt8*)ret)[sizeof(IntOS) * cnt];
	s = cmdLine;
	i = 0;
	while (true)
	{
		c = *s++;
		if (c == 0)
		{
			if (isC)
			{
				isC = false;
				*outPtr++ = 0;
			}
			break;
		}
		else if (c == '"')
		{
			isQuote = !isQuote;
		}
		else if (c == ' ' && !isQuote)
		{
			if (isC)
			{
				isC = false;
				*outPtr++ = 0;
			}
		}
		else
		{
			if (!isC)
			{
				isC = true;
				ret[i++] = outPtr;
			}
			*outPtr++ = c;
		}
	}
	*argc = cnt;
	return ret;
}
#endif

UnsafeArray<UnsafeArray<UTF8Char>> __stdcall WinProgControl_GetCommandLines(NN<Core::ProgControl> progCtrl, OutParam<UIntOS> cmdCnt)
{
	NN<WinProgControl> ctrl = NN<WinProgControl>::ConvertFrom(progCtrl);
	UnsafeArray<UnsafeArray<UTF8Char>> nnargv;
	if (!ctrl->argv.SetTo(nnargv))
	{
		Int32 argc;
		UIntOS i;
		WChar **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
		ctrl->argv = nnargv = MemAllocArr(UnsafeArray<UTF8Char>, (UInt32)argc);
		ctrl->argc = (UIntOS)argc;
		i = ctrl->argc;
		while (i-- > 0)
		{
			nnargv[i] = UnsafeArray<UTF8Char>::ConvertFrom(Text::StrToUTF8New(argv[i]));
		}
		LocalFree(argv);
	}
	cmdCnt.Set(ctrl->argc);
	return nnargv;
}

void __stdcall WinProgControl_SignalExit(NN<Core::ProgControl> progCtrl)
{

}

void __stdcall WinProgControl_SignalRestart(NN<Core::ProgControl> progCtrl)
{

}


void WinProgControl_Create(NN<WinProgControl> ctrl, UI::InstanceHandle *hInst)
{
	ctrl->argc = 0;
	ctrl->argv = 0;
	ctrl->hInst = hInst;
	ctrl->WaitForExit = WinProgControl_WaitForExit;
	ctrl->SignalExit = WinProgControl_SignalExit;
	ctrl->SignalRestart = WinProgControl_SignalRestart;
	ctrl->GetCommandLines = WinProgControl_GetCommandLines;
}

void WinProgControl_Destroy(NN<WinProgControl> ctrl)
{
	UnsafeArray<UnsafeArray<UTF8Char>> argv;
	if (ctrl->argv.SetTo(argv))
	{
		UIntOS i = ctrl->argc;
		while (i-- > 0)
		{
			Text::StrDelNew(UnsafeArray<const UTF8Char>(argv[i]));
		}
		MemFreeArr(argv);
		ctrl->argv = 0;
	}
}

Optional<UI::GUICore> Core::ProgControl::CreateGUICore(NN<Core::ProgControl> progCtrl)
{
	WinProgControl *ctrl = (WinProgControl*)progCtrl.Ptr();
	UI::Win::WinCore *ui;
	NEW_CLASS(ui, UI::Win::WinCore(ctrl->hInst));
	return ui;
}

#ifdef _WIN32_WCE
Int32 __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int nShowCmd)
#else
Int32 __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd)
#endif
{
	Int32 ret;
	Core::CoreStart();
	WinProgControl ctrl;
	void *dpiAware;
	Bool succ = false;
	
	{
		IO::Library lib((const UTF8Char*)"User32.dll");
		dpiAware = lib.GetFunc("SetProcessDpiAwarenessContext");
		if (dpiAware)
		{
			if (((AwareFunc2)dpiAware)(-4))
			{
				succ = true;
			}
			else if (((AwareFunc2)dpiAware)(-3))
			{
				succ = true;
			}
		}
	}
	if (!succ)
	{
		IO::Library lib((const UTF8Char*)"Shcore.dll");
		dpiAware = lib.GetFunc("SetProcessDpiAwareness");
		if (dpiAware)
		{
			((AwareFunc)dpiAware)(2);
			succ = true;
		}
	}

	if (!succ)
	{
		IO::Library lib((const UTF8Char*)"User32.dll");
		dpiAware = lib.GetFunc("SetProcessDPIAware");
		if (dpiAware)
		{
			((BoolFunc)dpiAware)();
			succ = true;
		}
	}

	WinProgControl_Create(ctrl, (UI::InstanceHandle*)hInst);
	ret = MyMain(ctrl);
	WinProgControl_Destroy(ctrl);
	Core::CoreEnd();
#ifndef __CYGWIN__
	_CrtDumpMemoryLeaks();
#endif
	return ret;
}