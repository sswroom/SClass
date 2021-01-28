#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/DDrawUI.h"
#include <windows.h>
#include <ddraw.h>

#define CLASSNAME "DDRAWUI"
#define DDDELETE(obj) if (obj) { obj->Release(); obj = 0;}

UI::DDrawUI *ddrawui_Obj;

LRESULT _stdcall DDrawWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ddrawui_Obj->WindowProc(hWnd, msg, wParam, lParam);
}

UI::DDrawUI::DDrawUI(void *hInst) : UI::MSWindowUI(hInst)
{
	WNDCLASSEXA wc;
	ATOM clsID;

	ddrawui_Obj = this;
	this->hInst = hInst;
	this->lpDD = 0;
	this->hAcc = 0;
	this->classReg = false;
	NEW_CLASS(wnds, Data::ArrayList<UI::DDrawWindow*>());

	if (DirectDrawCreateEx( NULL, (VOID**)&lpDD, IID_IDirectDraw7, NULL ) != DD_OK )
		return;

	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DDrawWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (HINSTANCE)hInst;
	wc.hIcon = 0;
	wc.hCursor = 0;
	wc.hbrBackground = 0;
	wc.lpszMenuName = 0;
	wc.lpszClassName = CLASSNAME;
	wc.hIconSm = 0;
	clsID = RegisterClassExA(&wc);

	classReg = (clsID != 0);
}

UI::DDrawUI::~DDrawUI()
{
	if (wnds->GetCount() > 0)
	{
		MessageBoxW(0, L"Error, Some windows does not closed", L"Error", MB_OK);
	}

	if (classReg)
	{
		UnregisterClassA(CLASSNAME, (HINSTANCE)hInst);
		classReg = false;
	}

	if (lpDD)
	{
		((LPDIRECTDRAW7)lpDD)->Release();
		lpDD = 0;
	}
	DEL_CLASS(wnds);
}

void UI::DDrawUI::UseAccel(Int32 resId, UI::DDrawWindow *wnd)
{
	this->hAcc = LoadAccelerators((HINSTANCE)hInst, MAKEINTRESOURCE(resId));
	this->hAccWnd = wnd->GetHWnd();
}

void UI::DDrawUI::ExitUI()
{
	if (classReg)
	{
		PostQuitMessage(0);
	}
}

void UI::DDrawUI::RunUI()
{
	if (classReg)
	{
		MSG msg;
		BOOL bRet;
		while (bRet = GetMessage(&msg, NULL, 0, 0))
		{
			if (bRet == -1)
			{
				break;
			}

			if (hAcc)
			{
				if( 0 == TranslateAccelerator( (HWND)hAccWnd, (HACCEL)hAcc, &msg ) )
				{
					TranslateMessage( &msg ); 
					DispatchMessage( &msg );
				}
			}
			else
			{
				TranslateMessage( &msg ); 
				DispatchMessage( &msg );
			}
		}
	}

}

void *UI::DDrawUI::GetDD()
{
	return this->lpDD;
}

UI::DDrawWindow *UI::DDrawUI::NewDDrawWindow(const WChar *title, UI::DDrawWindow::UIMode mode, UI::DDrawWindow::UpdateMode upMode, UI::DDrawWindow::UpdateHdlr hdlr, Int32 width, Int32 height, Bool fastUI)
{
	if (!classReg)
	{
		return 0;
	}

	HWND hWnd = CreateWindowA(CLASSNAME, "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, 0, (HINSTANCE)hInst, 0);
	if (hWnd == 0)
		return 0;
	UI::DDrawWindow *wnd;
	RECT rcWnd;
	RECT rcClient;
	SetWindowTextW(hWnd, title);
	GetWindowRect(hWnd, &rcWnd);
	GetClientRect(hWnd, &rcClient);
	rcWnd.right += width - (rcClient.right - rcClient.left);
	rcWnd.bottom += height - (rcClient.bottom - rcClient.top);
	MoveWindow(hWnd, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, FALSE);
	NEW_CLASS(wnd, UI::DDrawWindow(hWnd, this, mode, upMode, hdlr, fastUI));
	wnds->Add(wnd);
	return wnd;
}

OSInt UI::DDrawUI::WindowProc(void *hWnd, UInt32 msg, UInt32 wParam, Int32 lParam)
{
	OSInt i = this->wnds->GetCount();
	UI::DDrawWindow *wnd;
	while (i-- > 0)
	{
		wnd = (UI::DDrawWindow*)this->wnds->GetItem(i);
		if (wnd->GetHWnd() == hWnd)
			return wnd->WndProc(msg, wParam, lParam);
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);

}

void UI::DDrawUI::WindowClosed(UI::DDrawWindow *wnd)
{
	OSInt i = this->wnds->GetCount();
	while (i-- > 0)
	{
		if (this->wnds->GetItem(i) == wnd)
		{
			this->wnds->RemoveAt(i);
			DEL_CLASS(wnd);
			return;
		}
	}
}
