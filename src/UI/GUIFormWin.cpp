#include "Stdafx.h"
#if defined(WINVER)
#undef WINVER
#endif
#define WINVER 0x601

#if defined(_WIN32_WINNT)
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x601

#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUIButton.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIForm.h"
#include "UI/GUIIcon.h"
#include "UI/GUILabel.h"
#include "UI/GUIMenu.h"
#include "UI/GUITimer.h"
#include "UI/MessageDialog.h"

#include <windows.h>
#undef GetMonitorInfo
#undef MK_SHIFT
#undef MK_CONTROL
#undef MK_ALT

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

#if defined(_WIN32_WCE)
#define GetWindowLongPtr(a, b) GetWindowLongW(a, b)
#endif

#define BGBRUSH ((HBRUSH)COLOR_BTNSHADOW)
#define HK_RETURN 1
#define HK_ESC 2
Int32 UI::GUIForm::useCnt = 0;

OSInt __stdcall UI::GUIForm::FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUIForm *me = (UI::GUIForm*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	UI::GUIControl *ctrl;
	RECT rc;
	NMHDR *nmhdr;
	UOSInt i;
	switch (msg)
	{
	case WM_COMMAND:
		if (lParam == 0)
		{
			if (wParam == 1)
			{
				if (me->okBtn)
				{
					me->okBtn->EventButtonClick();
					return 0;
				}
			}
			else if (wParam == 2)
			{
				if (me->cancelBtn)
				{
					me->cancelBtn->EventButtonClick();
					return 0;
				}
			}
			else
			{
				me->EventMenuClicked(LOWORD(wParam));
				return 0;
			}
		}
		else
		{
			ctrl = (UI::GUIControl*)(OSInt)GetWindowLongPtr((HWND)lParam, GWL_USERDATA);
			if (ctrl)
			{
				return ctrl->OnNotify(HIWORD(wParam), 0);
			}
		}
		break;
	case WM_NOTIFY:
		nmhdr = (NMHDR*)lParam;
		ctrl = (UI::GUIControl*)(OSInt)GetWindowLongPtr(nmhdr->hwndFrom, GWL_USERDATA);
		if (ctrl)
		{
			return ctrl->OnNotify(nmhdr->code, (void *)lParam);
		}
		return 0;
	case WM_SIZE:
		GetWindowRect((HWND)me->hwnd, &rc);
		me->lxPos = rc.left * me->ddpi / me->hdpi;
		me->lyPos = rc.top * me->ddpi / me->hdpi;
		me->lxPos2 = rc.right * me->ddpi / me->hdpi;
		me->lyPos2 = rc.bottom * me->ddpi / me->hdpi;

		me->OnSizeChanged(false);
		me->UpdateChildrenSize(false);
/*		{
			Int32 w = LOWORD(lParam);
			Int32 h = HIWORD(lParam);
			HDC hdcBmp = CreateCompatibleDC(0);

			HDC hdc = GetDC((HWND)hWnd);
			HBITMAP hbmp = CreateCompatibleBitmap(hdc, w, h);

			SelectObject(hdcBmp, hbmp);
			SendMessage((HWND)hWnd, WM_PRINT, (WPARAM)hdcBmp, PRF_CHILDREN | PRF_CLIENT);
			BitBlt(hdc, 0, 0, w, h, hdcBmp, 0, 0, SRCCOPY);
			ReleaseDC((HWND)hWnd, hdc);
			DeleteDC(hdcBmp);
			DeleteObject(hbmp);
		}*/

		InvalidateRect((HWND)hWnd, 0, true);
		return 0;//DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	case WM_CLOSE:
		if (me)
		{
			if (me->closingHdlr)
			{
				if (!me->closingHdlr(me->closingHdlrObj, CR_USER))
				{
					if (me->currDialog)
					{
						me->currDialog->Close();
					}
					DestroyWindow((HWND)hWnd);
				}
				else
				{
				}
				return 0;
			}
			else
			{
				if (me->currDialog)
				{
					me->currDialog->Close();
				}
				DestroyWindow((HWND)hWnd);
			}
		}
		return 0;
	case WM_DESTROY:
		i = me->closeHandlers->GetCount();
		while (i-- > 0)
		{
			((FormClosedEvent)me->closeHandlers->GetItem(i))(me->closeHandlersObj->GetItem(i), me);
		}
		me->EventClosed();
		if (!me->isDialog)
		{
			me->DestroyObject();
			DEL_CLASS(me);
		}
		else
		{
			me->isDialog = false;
		}
		return 0;
	case WM_TIMER:
		me->EventTimer(wParam);
		return 0;
#ifndef _WIN32_WCE
	case WM_DROPFILES:
		me->OnDropFiles((void*)wParam);
		return 0;
#endif
	case WM_CTLCOLORSTATIC:
		{
			UI::GUILabel *lbl;
			lbl = (UI::GUILabel*)(OSInt)GetWindowLongPtr((HWND)lParam, GWL_USERDATA);
			if (lbl)
			{
				if (lbl->HasTextColor())
				{
					UInt32 c = lbl->GetTextColor();
					SetTextColor((HDC)wParam, ((c & 0xff) << 16) | (c & 0xff00) | ((c & 0xff0000) >> 16));
				}
			}
		}
		break;
/*	case WM_GETMINMAXINFO:
		break;
	case WM_NCCREATE:
		break;
	case WM_NCCALCSIZE:
		break;*/
	case WM_CREATE:
		break;
	case WM_SETTEXT:
		break;
	case WM_SHOWWINDOW:
		break;
/*	case WM_WINDOWPOSCHANGING:
		break;
	case WM_ACTIVATEAPP:
		break;
	case WM_NCACTIVATE:
		break;*/
	case WM_GETTEXT:
		break;
	case WM_ACTIVATE:
/*		if (wParam == WA_INACTIVE)
		{
			Data::ArrayList<UI::GUIMenu::ShortcutKey*> keys;
			UI::GUIMenu::ShortcutKey *key;
			if (me->menu)
			{
				me->menu->GetAllKeys(&keys);
				i = keys.GetCount();
				while (i-- > 0)
				{
					key = keys.GetItem(i);
					UnregisterHotKey((HWND)me->hwnd, 0x8000 | key->cmdId);
				}
			}
		}
		else
		{
			Data::ArrayList<UI::GUIMenu::ShortcutKey*> keys;
			UI::GUIMenu::ShortcutKey *key;
			if (me->menu)
			{
				me->menu->GetAllKeys(&keys);
				i = keys.GetCount();
				while (i-- > 0)
				{
					key = keys.GetItem(i);
					RegisterHotKey((HWND)me->hwnd, 0x8000 | key->cmdId, key->keyModifier, key->shortcutKey);
				}
			}
		}*/
		break;
	case WM_IME_SETCONTEXT:
		break;
	case WM_IME_NOTIFY:
		break;
	case WM_KILLFOCUS:
		me->OnFocusLost();
		break;
	case WM_SETFOCUS:
		((UI::GUICoreWin*)me->ui)->SetFocusWnd(hWnd, me->hAcc);
		me->OnFocus();
		break;
	case WM_HOTKEY:
		if (wParam & 0xffff8000)
		{
			me->EventMenuClicked((UInt16)(wParam & 0x7fff));
		}
		break;
//	case WM_NCPAINT:
//		break;
	case WM_ERASEBKGND:
		if (me->fs)
		{
			return TRUE;
		}
		else
		{
			return me->MyEraseBkg((void*)wParam);
		}
		break;
	case WM_WINDOWPOSCHANGED:
		break;
	case WM_MOVE:
		GetWindowRect((HWND)me->hwnd, &rc);
		me->lxPos = rc.left * me->ddpi / me->hdpi;
		me->lyPos = rc.top * me->ddpi / me->hdpi;
		me->lxPos2 = rc.right * me->ddpi / me->hdpi;
		me->lyPos2 = rc.bottom * me->ddpi / me->hdpi;
		me->OnPosChanged(false);
		break;
/*	case WM_NOTIFYFORMAT:
		break;
	case WM_QUERYUISTATE:
		break;
	case WM_PARENTNOTIFY:
		break;*/
	case WM_GETICON:
		break;
	case WM_PAINT:
		if (me->OnPaint())
			return TRUE;
		break;
	case WM_KEYUP:
		i = me->keyUpHandlers->GetCount();
		while (i-- > 0)
		{
			me->keyUpHandlers->GetItem(i)(me->keyUpHandlersObj->GetItem(i), wParam, (lParam & 0x1000000) != 0);
		}
		break;
	case WM_KEYDOWN:
		i = me->keyDownHandlers->GetCount();
		while (i-- > 0)
		{
			me->keyDownHandlers->GetItem(i)(me->keyDownHandlersObj->GetItem(i), wParam, (lParam & 0x1000000) != 0);
		}
		break;
	case WM_SYSKEYDOWN:
		break;
/*	case WM_NCHITTEST:
		break;*/
	case WM_SETCURSOR:
		break;
/*	case WM_NCMOUSEMOVE:
		break;
	case WM_MOUSEACTIVATE:
		break;*/
	case WM_MOUSEMOVE:
		break;
	case WM_MOUSEWHEEL:
		break;
#if !defined(_WIN32_WCE)
	case WM_DISPLAYCHANGE:
		me->OnDisplaySizeChange(LOWORD(lParam), HIWORD(lParam));
		break;
#endif
	default:
		return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	};
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUIForm::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUIForm::FormWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
#ifdef _WIN32_WCE
    wc.hIcon = 0; 
#else
    wc.hIcon = LoadIcon((HINSTANCE) NULL, IDI_APPLICATION); 
#endif
    wc.hCursor = LoadCursor((HINSTANCE) NULL, 
        IDC_ARROW); 
    wc.hbrBackground = BGBRUSH;
    wc.lpszMenuName = 0; 
    wc.lpszClassName = L"WinForm"; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUIForm::Deinit(void *hInst)
{
	UnregisterClassW(L"WinForm", (HINSTANCE)hInst);
}

void UI::GUIForm::UpdateHAcc()
{
	if (this->hAcc)
	{
		DestroyAcceleratorTable((HACCEL)this->hAcc);
		this->hAcc = 0;
	}
	if (this->menu)
	{
		UOSInt i;
		ACCEL *accels;
		Data::ArrayList<UI::GUIMenu::ShortcutKey*> keys;
		UI::GUIMenu::ShortcutKey *key;
		this->menu->GetAllKeys(&keys);
		i = keys.GetCount();
		if (i > 0)
		{
			accels = MemAlloc(ACCEL, i);
			while (i-- > 0)
			{
				key = keys.GetItem(i);
				accels[i].fVirt = FVIRTKEY;
				if (key->keyModifier & UI::GUIMenu::KM_ALT)
					accels[i].fVirt |= FALT;
				if (key->keyModifier & UI::GUIMenu::KM_CONTROL)
					accels[i].fVirt |= FCONTROL;
				if (key->keyModifier & UI::GUIMenu::KM_SHIFT)
					accels[i].fVirt |= FSHIFT;
				accels[i].key = (WORD)GUIKey2OSKey(key->shortcutKey);
				accels[i].cmd = key->cmdId;
			}
			this->hAcc = CreateAcceleratorTable(accels, (int)keys.GetCount());
			MemFree(accels);
		}
	}
	if (this->IsFormFocused())
	{
		((UI::GUICoreWin*)this->ui)->SetFocusWnd(this->hwnd, this->hAcc);
	}
}
UI::GUIForm::GUIForm(ControlHandle *hWnd) : UI::GUIClientControl(0, 0)
{
	this->hwnd = hWnd;
	this->hAcc = 0;
	this->virtualMode = true;
	this->currDialog = 0;
	this->fs = false;
}

UI::GUIForm *UI::GUIForm::FindForm(const UTF8Char *formName)
{
	HWND hWnd;
	const WChar *wptr = Text::StrToWCharNew(formName);
	hWnd = FindWindowW(L"WinForm", wptr);
	Text::StrDelNew(wptr);
	if (hWnd == 0)
		return 0;
	UI::GUIForm *frm;
	NEW_CLASS(frm, UI::GUIForm((ControlHandle*)hWnd));
	return frm;
}

UI::GUIForm::GUIForm(UI::GUIClientControl *parent, Double initW, Double initH, UI::GUICore *ui) : UI::GUIClientControl(ui, parent)
{
	this->virtualMode = false;
	this->hAcc = 0;
	this->currDialog = 0;
	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		Init(((UI::GUICoreWin*)ui)->GetHInst());
	}
	NEW_CLASS(this->closeHandlers, Data::ArrayList<FormClosedEvent>());
	NEW_CLASS(this->closeHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->dropFileHandlers, Data::ArrayList<FileEvent>());
	NEW_CLASS(this->dropFileHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->menuClickedHandlers, Data::ArrayList<MenuEvent>());
	NEW_CLASS(this->menuClickedHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->keyDownHandlers, Data::ArrayList<KeyEvent>());
	NEW_CLASS(this->keyDownHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->keyUpHandlers, Data::ArrayList<KeyEvent>());
	NEW_CLASS(this->keyUpHandlersObj, Data::ArrayList<void *>());
	NEW_CLASS(this->timers, Data::ArrayList<UI::GUITimer*>());
	this->closingHdlr = 0;
	this->closingHdlrObj = 0;
	this->exitOnClose = false;
	this->nextTmrId = 1;
	this->ui = ui;
	this->isDialog = false;
	this->dialogResult = DR_UNKNOWN;
	this->menu = 0;
	this->okBtn = 0;
	this->cancelBtn = 0;
	this->fs = false;

	Double w;
	Double h;
	Double initX;
	Double initY;
	if (parent)
	{
		parent->GetClientSize(&w, &h);
		initX = (w - initW) * 0.5;
		initY = (h - initH) * 0.5;
	}
	else
	{
		POINT pt;
		Bool found = false;
		GetCursorPos(&pt);
		HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
		if (hMon)
		{
			this->ui->GetMonitorDPIs((MonitorHandle*)hMon, &this->hdpi, &this->ddpi);
			initW = initW * this->hdpi / this->ddpi;
			initH = initH * this->hdpi / this->ddpi;
#if defined(_WIN32_WCE)
			MONITORINFOEX info;
			info.cbSize = sizeof(info);
			if (::GetMonitorInfo(hMon, &info))
			{
				found = true;
				w = (info.rcMonitor.right - info.rcMonitor.left) * this->ddpi / this->hdpi;
				h = (info.rcMonitor.bottom - info.rcMonitor.top) * this->ddpi / this->hdpi;
				if (initW > w)
				{
					initW = w;
				}
				if (initH > h)
				{
					initH = h;
				}
				initX = (info.rcMonitor.left * this->ddpi / this->hdpi) + ((w - initW) * 0.5);
				initY = (info.rcMonitor.top * this->ddpi / this->hdpi) + ((h - initH) * 0.5);
			}
#else
			MONITORINFOEXW info;
			info.cbSize = sizeof(info);
			if (::GetMonitorInfoW(hMon, &info))
			{
				found = true;
				w = (info.rcMonitor.right - info.rcMonitor.left) * this->ddpi / this->hdpi;
				h = (info.rcMonitor.bottom - info.rcMonitor.top) * this->ddpi / this->hdpi;
				if (initW > w)
				{
					initW = w;
				}
				if (initH > h)
				{
					initH = h;
				}
				initX = (info.rcMonitor.left * this->ddpi / this->hdpi) + ((w - initW) * 0.5);
				initY = (info.rcMonitor.top * this->ddpi / this->hdpi) + ((h - initH) * 0.5);
			}
#endif
		}
		else
		{
			initW = initW * this->hdpi / this->ddpi;
			initH = initH * this->hdpi / this->ddpi;
		}
		if (!found)
		{
			RECT rc;
			GetClientRect(GetDesktopWindow(), &rc);
			w = (rc.right - rc.left) * this->ddpi / this->hdpi;
			h = (rc.bottom - rc.top) * this->ddpi / this->hdpi;
			if (initW > w)
			{
				initW = w;
			}
			if (initH > h)
			{
				initH = h;
			}
			initX = (w - initW) * 0.5;
			initY = (h - initH) * 0.5;
		}
	}
	this->undockLeft = 0;
	this->undockTop = 0;
	this->undockRight = w;
	this->undockBottom = h;
	this->InitControl(((UI::GUICoreWin*)this->ui)->GetHInst(), parent, L"WinForm", (const UTF8Char*)"Form", WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, 0, initX, initY, initW, initH);// | WS_THICKFRAME | WS_MAXIMIZEBOX
	this->SetFont(0, 0, 0, false);
	SetNoResize(false);
}

UI::GUIForm::~GUIForm()
{
	if (this->virtualMode)
	{
	}
	else
	{
		UOSInt i = this->timers->GetCount();
		while (i-- > 0)
		{
			UI::GUITimer *tmr = this->timers->GetItem(i);
			DEL_CLASS(tmr);
		}
		DEL_CLASS(this->timers);
		DEL_CLASS(this->dropFileHandlers);
		DEL_CLASS(this->dropFileHandlersObj);
		DEL_CLASS(this->closeHandlers);
		DEL_CLASS(this->closeHandlersObj);
		DEL_CLASS(this->menuClickedHandlers);
		DEL_CLASS(this->menuClickedHandlersObj);
		DEL_CLASS(this->keyDownHandlers);
		DEL_CLASS(this->keyDownHandlersObj);
		DEL_CLASS(this->keyUpHandlers);
		DEL_CLASS(this->keyUpHandlersObj);
		if (this->menu)
		{
			DEL_CLASS(this->menu);
			this->menu = 0;
		}
		if (Sync::Interlocked::Decrement(&useCnt) == 0)
		{
			Deinit(((UI::GUICoreWin*)this->ui)->GetHInst());
		}
		if (this->hAcc)
		{
			DestroyAcceleratorTable((HACCEL)this->hAcc);
			this->hAcc = 0;
		}
	}
}

void UI::GUIForm::SetFormState(UI::GUIForm::FormState fs)
{
	switch (fs)
	{
	case FS_NORMAL:
		ShowWindow((HWND)this->hwnd, SW_SHOWNORMAL);
		break;
	case FS_MAXIMIZED:
		ShowWindow((HWND)this->hwnd, SW_MAXIMIZE);
		break;
	case FS_MINIMIZED:
		ShowWindow((HWND)this->hwnd, SW_MINIMIZE);
		break;
	}
}

UI::GUIForm::DialogResult UI::GUIForm::ShowDialog(UI::GUIForm *owner)
{
	if (owner)
	{
		owner->SetEnabled(false);
		owner->currDialog = this;
	}
	this->isDialog = true;
	this->Show();
	while (this->isDialog)
	{
		ui->WaitForMessages();
		ui->ProcessMessages();
	}
	if (owner)
	{
		owner->currDialog = 0;
		owner->SetEnabled(true);
		owner->MakeForeground();
	}
	return this->dialogResult;
}

void UI::GUIForm::SetDialogResult(UI::GUIForm::DialogResult dr)
{
	if (this->isDialog)
	{
		this->dialogResult = dr;
		this->Close();
	}
}

void UI::GUIForm::ShowTitleBar(Bool show)
{
	Int32 style = GetWindowLong((HWND)this->hwnd, GWL_STYLE);
	if (show)
	{
		SetWindowLong((HWND)this->hwnd, GWL_STYLE, style | WS_CAPTION | WS_SYSMENU);
	}
	else
	{
		SetWindowLong((HWND)this->hwnd, GWL_STYLE, style & ~(WS_CAPTION | WS_SYSMENU));
	}
}

void UI::GUIForm::SetAlwaysOnTop(Bool alwaysOnTop)
{
	if (alwaysOnTop)
	{
		SetWindowPos((HWND)this->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
	else
	{
		SetWindowPos((HWND)this->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
}

void UI::GUIForm::MakeActive()
{
	SetActiveWindow((HWND)this->hwnd);
}

void UI::GUIForm::MakeForeground()
{
#ifdef _WIN32_WCE
	SetForegroundWindow((HWND)(((OSInt)this->hwnd) | 1));
#else
	SetForegroundWindow((HWND)this->hwnd);
#endif
}

void UI::GUIForm::Close()
{
	if (this->currDialog)
	{
		this->currDialog->Close();
	}
	DestroyWindow((HWND)hwnd);
}

void UI::GUIForm::SetText(const UTF8Char *text)
{
	const WChar *wptr = Text::StrToWCharNew(text);
	SetWindowTextW((HWND)this->hwnd, wptr);
	Text::StrDelNew(wptr);
}

void UI::GUIForm::GetSizeP(UOSInt *width, UOSInt *height)
{
	RECT rect;
	GetWindowRect((HWND)hwnd, &rect);
	if (width)
		*width = (UOSInt)(OSInt)(rect.right - rect.left);
	if (height)
		*height = (UOSInt)(OSInt)(rect.bottom - rect.top);
}

void UI::GUIForm::SetExitOnClose(Bool exitOnClose)
{
	this->exitOnClose = exitOnClose;
}

void UI::GUIForm::SetNoResize(Bool noResize)
{
	Int32 style = GetWindowLong((HWND)this->hwnd, GWL_STYLE);
	if (noResize)
	{
		SetWindowLong((HWND)this->hwnd, GWL_STYLE, style & ~(WS_MAXIMIZEBOX | WS_THICKFRAME));
	}
	else
	{
		SetWindowLong((HWND)this->hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME);
	}
}

UI::GUITimer *UI::GUIForm::AddTimer(UInt32 interval, UI::UIEvent handler, void *userObj)
{
	UI::GUITimer *tmr;
	NEW_CLASS(tmr, UI::GUITimer(ui, this, this->nextTmrId++, interval, handler, userObj));
	this->timers->Add(tmr);
	return tmr;
}

void UI::GUIForm::RemoveTimer(UI::GUITimer *tmr)
{
	UOSInt i = this->timers->GetCount();
	while (i-- > 0)
	{
		if (tmr == this->timers->GetItem(i))
		{
			this->timers->RemoveAt(i);
			DEL_CLASS(tmr);
			break;
		}
	}
}

void UI::GUIForm::SetMenu(UI::GUIMainMenu *menu)
{
#ifndef _WIN32_WCE
	::SetMenu((HWND)this->hwnd, (HMENU)menu->GetHMenu());
#endif
	if (this->menu)
	{
		DEL_CLASS(this->menu);
	}
	this->menu = menu;
	this->UpdateHAcc();
}

UI::GUIMainMenu *UI::GUIForm::GetMenu()
{
	return this->menu;
}

void UI::GUIForm::UpdateMenu()
{
	DrawMenuBar((HWND)this->hwnd);
	this->UpdateHAcc();
}

void UI::GUIForm::SetDefaultButton(UI::GUIButton *btn)
{
	this->okBtn = btn;
	btn->SetDefaultBtnLook();
}

void UI::GUIForm::SetCancelButton(UI::GUIButton *btn)
{
	this->cancelBtn = btn;
}

void UI::GUIForm::GetClientSize(Double *w, Double *h)
{
	RECT rc;
	GetClientRect((HWND)this->hwnd, &rc);
	if (w)
		*w = (rc.right - rc.left) * this->ddpi / this->hdpi;
	if (h)
		*h = (rc.bottom - rc.top) * this->ddpi / this->hdpi;
}

Bool UI::GUIForm::IsChildVisible()
{
	return true;
}

Text::CString UI::GUIForm::GetObjectClass()
{
	return CSTR("WinForm");
}

OSInt UI::GUIForm::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIForm::OnSizeChanged(Bool updateScn)
{
	if (this->selfResize)
	{
		return;
	}
	HMONITOR hMon = MonitorFromWindow((HWND)this->hwnd, MONITOR_DEFAULTTONEAREST);
	if (hMon != (HMONITOR)this->currHMon)
	{
		this->currHMon = (MonitorHandle*)hMon;
		this->OnMonitorChanged();
	}
	UOSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
	}
}

Bool UI::GUIForm::OnPaint()
{
	return false;
}

void UI::GUIForm::OnDropFiles(void *hDrop)
{
#ifndef _WIN32_WCE
	UInt32 fileCnt = DragQueryFileW((HDROP)hDrop, (UINT)-1, 0, 0);
	if (fileCnt > 0)
	{
		UInt32 i;
		WChar sbuff[256];
		const UTF8Char **files = MemAlloc(const UTF8Char *, fileCnt);
		i = 0;
		while (i < fileCnt)
		{
			DragQueryFileW((HDROP)hDrop, i, sbuff, 256);
			files[i] = Text::StrToUTF8New(sbuff);
			i++;
		}
		UOSInt j = this->dropFileHandlers->GetCount();
		while (j-- > 0)
		{
			this->dropFileHandlers->GetItem(j)(this->dropFileHandlersObj->GetItem(j), files, (OSInt)fileCnt);
		}
		while (fileCnt-- > 0)
		{
			Text::StrDelNew(files[fileCnt]);
		}
		MemFree(files);
	}
#endif
}

void UI::GUIForm::EventMenuClicked(UInt16 cmdId)
{
	UOSInt i;
	i = this->menuClickedHandlers->GetCount();
	while (i-- > 0)
	{
		this->menuClickedHandlers->GetItem(i)(this->menuClickedHandlersObj->GetItem(i), cmdId);
	}
}

void UI::GUIForm::SetSmallIcon(UI::GUIIcon *icon)
{
	if (icon)
	{
		SendMessage((HWND)this->hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon->GetHandle());
	}
	else
	{
		SendMessage((HWND)this->hwnd, WM_SETICON, ICON_SMALL, 0);
	}
}

void UI::GUIForm::SetLargeIcon(UI::GUIIcon *icon)
{
	SendMessage((HWND)this->hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon->GetHandle());
}

void UI::GUIForm::ShowMouseCursor(Bool toShow)
{
	ShowCursor(toShow?TRUE:FALSE);
}

void UI::GUIForm::HandleFormClosed(FormClosedEvent handler, void *userObj)
{
	this->closeHandlers->Add(handler);
	this->closeHandlersObj->Add(userObj);
}

void UI::GUIForm::HandleDropFiles(FileEvent handler, void *userObj)
{
#ifndef _WIN32_WCE
	if (this->dropFileHandlers->GetCount() == 0)
	{
		OSInt style = GetWindowLongPtr((HWND)this->hwnd, GWL_EXSTYLE);
		UI::GUICoreWin::MSSetWindowObj(this->hwnd, GWL_EXSTYLE, style | WS_EX_ACCEPTFILES);
	}
#endif
	this->dropFileHandlers->Add(handler);
	this->dropFileHandlersObj->Add(userObj);
}

void UI::GUIForm::HandleMenuClicked(MenuEvent handler, void *userObj)
{
	this->menuClickedHandlers->Add(handler);
	this->menuClickedHandlersObj->Add(userObj);
}

void UI::GUIForm::HandleKeyDown(KeyEvent handler, void *userObj)
{
	this->keyDownHandlers->Add(handler);
	this->keyDownHandlersObj->Add(userObj);
}

void UI::GUIForm::HandleKeyUp(KeyEvent handler, void *userObj)
{
	this->keyUpHandlers->Add(handler);
	this->keyUpHandlersObj->Add(userObj);
}

void UI::GUIForm::SetClosingHandler(FormClosingEvent handler, void *userObj)
{
	this->closingHdlr = handler;
	this->closingHdlrObj = userObj;
}

void UI::GUIForm::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->UpdatePos(true);
	if (this->hFont)
	{
		this->InitFont();
	}
	else
	{
		this->UpdateFont();
	}

	if (this->menu)
	{
		this->menu->SetDPI(hdpi, ddpi);
	}
	UOSInt i = this->children->GetCount();
	while (i-- > 0)
	{
		this->children->GetItem(i)->SetDPI(hdpi, ddpi);
	}
	this->UpdateChildrenSize(true);
}

void UI::GUIForm::EventClosed()
{
	if (this->exitOnClose)
	{
		this->ui->Exit();
	}
}

void UI::GUIForm::EventTimer(UOSInt tmrId)
{
	UI::GUITimer *tmr;
	UOSInt i = this->timers->GetCount();
	while (i-- > 0)
	{
		tmr = this->timers->GetItem(i);
		if (tmr->GetId() == tmrId)
		{
			tmr->OnTick();
			break;
		}
	}
}

void UI::GUIForm::OnFocus()
{
}

void UI::GUIForm::OnFocusLost()
{
}

void UI::GUIForm::OnDisplaySizeChange(UOSInt dispWidth, UOSInt dispHeight)
{
}

void UI::GUIForm::ToFullScn()
{
	if (this->fs)
		return;
#ifdef _WIN32_WCE
	HWND hWnd = (HWND)this->hwnd;
	this->fs = true;

/*	WINDOWINFO wndInfo;
	wndInfo.cbSize = sizeof(wndInfo);
	GetWindowInfo(hWnd, &wndInfo);
	this->fsX = wndInfo.rcWindow.left;
	this->fsY = wndInfo.rcWindow.top;
	this->fsW = wndInfo.rcWindow.right - wndInfo.rcWindow.left;
	this->fsH = wndInfo.rcWindow.bottom - wndInfo.rcWindow.top;
	this->fsStyle = wndInfo.dwStyle;*/
	UInt32 dwStyle  = this->fsStyle;
	dwStyle &= ~WS_SYSMENU;
	dwStyle &= ~WS_OVERLAPPED;
	dwStyle &= ~WS_CAPTION;
	dwStyle &= ~WS_THICKFRAME;
	dwStyle &= ~WS_MINIMIZEBOX;
	SetWindowLong(hWnd, GWL_STYLE, dwStyle);
//	::SetMenu(hWnd, 0);
#else
	HWND hWnd = (HWND)this->hwnd;
	this->fs = true;
	WINDOWINFO wndInfo;
	wndInfo.cbSize = sizeof(wndInfo);
	GetWindowInfo(hWnd, &wndInfo);
	this->fsX = wndInfo.rcWindow.left;
	this->fsY = wndInfo.rcWindow.top;
	this->fsW = wndInfo.rcWindow.right - wndInfo.rcWindow.left;
	this->fsH = wndInfo.rcWindow.bottom - wndInfo.rcWindow.top;
	this->fsStyle = wndInfo.dwStyle;
	UInt32 dwStyle  = this->fsStyle;
	dwStyle &= (UInt32)~WS_SYSMENU;
	dwStyle &= (UInt32)~WS_OVERLAPPED;
	dwStyle &= (UInt32)~WS_CAPTION;
	dwStyle &= (UInt32)~WS_THICKFRAME;
	dwStyle &= (UInt32)~WS_MINIMIZEBOX;
	SetWindowLong(hWnd, GWL_STYLE, (LONG)dwStyle);

	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE); 

	::SetMenu(hWnd, 0);
#endif
}

void UI::GUIForm::FromFullScn()
{
	if (!this->fs)
		return;
	this->fs = false;
	SetWindowLong((HWND)this->hwnd, GWL_STYLE, (LONG)this->fsStyle);
	MoveWindow((HWND)this->hwnd, this->fsX, this->fsY, this->fsW, this->fsH, TRUE);

	SetWindowPos((HWND)this->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE); 

	if (this->menu)
	{
#ifdef _WIN32_WCE
//		::SetMenu((HWND)this->hwnd, (HMENU)menu->GetHMenu());
#else
		::SetMenu((HWND)this->hwnd, (HMENU)menu->GetHMenu());
#endif
	}
}

UI::GUICore *UI::GUIForm::GetUI()
{
	return this->ui;
}
