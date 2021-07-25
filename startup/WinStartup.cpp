#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Library.h"
#include "Media/GDIEngineC.h"
#include "Text/MyStringW.h"
#include "UI/GUICoreWin.h"
#include <windows.h>

typedef BOOL (WINAPI *BoolFunc)();
typedef HRESULT (WINAPI *AwareFunc)(OSInt val);
typedef BOOL (WINAPI *AwareFunc2)(OSInt val);

Int32 MyMain(Core::IProgControl *ctrl);

struct WinProgControl : public Core::IProgControl
{
	void *hInst;
	UOSInt argc;
	UTF8Char **argv;
};

void __stdcall WinProgControl_WaitForExit(Core::IProgControl *progCtrl)
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
	OSInt chCnt = 0;
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
	WChar **ret = (WChar **)LocalAlloc(LMEM_FIXED, sizeof(OSInt) * cnt + (chCnt + cnt) * sizeof(WChar));
	outPtr = (WChar*)&((UInt8*)ret)[sizeof(OSInt) * cnt];
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

UTF8Char **__stdcall WinProgControl_GetCommandLines(Core::IProgControl *progCtrl, UOSInt *cmdCnt)
{
	WinProgControl *ctrl = (WinProgControl*)progCtrl;
	if (ctrl->argv == 0)
	{
		Int32 argc;
		UOSInt i;
		WChar **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
		ctrl->argv = MemAlloc(UTF8Char *, (UInt32)argc);
		ctrl->argc = (UOSInt)argc;
		i = ctrl->argc;
		while (i-- > 0)
		{
			ctrl->argv[i] = (UTF8Char*)Text::StrToUTF8New(argv[i]);
		}
		LocalFree(argv);
	}
	*cmdCnt = ctrl->argc;
	return ctrl->argv;
}

void WinProgControl_Create(WinProgControl *ctrl, void *hInst)
{
	ctrl->argc = 0;
	ctrl->argv = 0;
	ctrl->hInst = hInst;
	ctrl->WaitForExit = WinProgControl_WaitForExit;
	ctrl->GetCommandLines = WinProgControl_GetCommandLines;
}

void WinProgControl_Destroy(WinProgControl *ctrl)
{
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
	WinProgControl *ctrl = (WinProgControl*)progCtrl;
	UI::GUICoreWin *ui;
	NEW_CLASS(ui, UI::GUICoreWin(ctrl->hInst));
	return ui;
}

#ifdef _WIN32_WCE
Int32 __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, Int32 nShowCmd)
#else
Int32 __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, Int32 nShowCmd)
#endif
{
	Int32 ret;
	Core::CoreStart();
	WinProgControl ctrl;
	IO::Library *lib;
	void *dpiAware;
	Bool succ = false;
	
	NEW_CLASS(lib, IO::Library((const UTF8Char*)"User32.dll"));
	dpiAware = lib->GetFunc("SetProcessDpiAwarenessContext");
	if (dpiAware)
	{
		((AwareFunc2)dpiAware)(-3);
		succ = true;
	}
	DEL_CLASS(lib);
	if (!succ)
	{
		NEW_CLASS(lib, IO::Library((const UTF8Char*)"Shcore.dll"));
		dpiAware = lib->GetFunc("SetProcessDpiAwareness");
		if (dpiAware)
		{
			((AwareFunc)dpiAware)(2);
			succ = true;
		}
		DEL_CLASS(lib);
	}

	if (!succ)
	{
		NEW_CLASS(lib, IO::Library((const UTF8Char*)"User32.dll"));
		dpiAware = lib->GetFunc("SetProcessDPIAware");
		if (dpiAware)
		{
			((BoolFunc)dpiAware)();
			succ = true;
		}
		DEL_CLASS(lib);
	}

	WinProgControl_Create(&ctrl, hInst);
	ret = MyMain(&ctrl);
	WinProgControl_Destroy(&ctrl);
	Core::CoreEnd();
	return ret;
}