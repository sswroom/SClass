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
#include "UI/GUIForm.h"
#include "UI/GUIIcon.h"
#include "UI/GUILabel.h"
#include "UI/GUIMenu.h"
#include "UI/Win/WinButton.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinTimer.h"

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

IntOS __stdcall UI::GUIForm::FormWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam)
{
	Optional<UI::GUIForm> me = (UI::GUIForm*)(IntOS)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	NN<UI::GUIForm> nnme;
	UI::GUIControl *ctrl;
	NN<UI::GUIButton> btn;
	NN<UI::GUIForm> currDialog;
	RECT rc;
	NMHDR *nmhdr;
	UIntOS i;
	switch (msg)
	{
	case WM_COMMAND:
		if (lParam == 0)
		{
			if (me.SetTo(nnme))
			{
				if (wParam == 1)
				{
					if (nnme->okBtn.SetTo(btn))
					{
						btn->EventButtonClick();
						return 0;
					}
				}
				else if (wParam == 2)
				{
					if (nnme->cancelBtn.SetTo(btn))
					{
						btn->EventButtonClick();
						return 0;
					}
				}
				else
				{
					nnme->EventMenuClicked(LOWORD(wParam));
					return 0;
				}
			}
			else
			{
				printf("GUIForm: Me is null on WM_COMMAND\r\n");
			}
		}
		else
		{
			ctrl = (UI::GUIControl*)(IntOS)GetWindowLongPtr((HWND)lParam, GWL_USERDATA);
			if (ctrl)
			{
				return ctrl->OnNotify(HIWORD(wParam), 0);
			}
		}
		break;
	case WM_NOTIFY:
		nmhdr = (NMHDR*)lParam;
		ctrl = (UI::GUIControl*)(IntOS)GetWindowLongPtr(nmhdr->hwndFrom, GWL_USERDATA);
		if (ctrl)
		{
			return ctrl->OnNotify(nmhdr->code, (void *)lParam);
		}
		return 0;
	case WM_SIZE:
		if (me.SetTo(nnme))
		{
			GetWindowRect((HWND)nnme->hwnd.OrNull(), &rc);
			nnme->lxPos = rc.left * nnme->ddpi / nnme->hdpi;
			nnme->lyPos = rc.top * nnme->ddpi / nnme->hdpi;
			nnme->lxPos2 = rc.right * nnme->ddpi / nnme->hdpi;
			nnme->lyPos2 = rc.bottom * nnme->ddpi / nnme->hdpi;

			nnme->OnSizeChanged(false);
			nnme->UpdateChildrenSize(false);
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
		}
		else
		{
			printf("GUIForm: Me is null on WM_SIZE\r\n");
			return 0;
		}
	case WM_CLOSE:
		if (me.SetTo(nnme))
		{
			if (nnme->closingHdlr.func)
			{
				if (!nnme->closingHdlr.func(nnme->closingHdlr.userObj, CR_USER))
				{
					if (nnme->currDialog.SetTo(currDialog))
					{
						currDialog->Close();
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
				if (nnme->currDialog.SetTo(currDialog))
				{
					currDialog->Close();
				}
				DestroyWindow((HWND)hWnd);
			}
		}
		else
		{
			printf("GUIForm: Me is null on WM_CLOSE\r\n");
		}
		return 0;
	case WM_DESTROY:
		if (me.SetTo(nnme))
		{
			i = nnme->closeHandlers.GetCount();
			while (i-- > 0)
			{
				Data::CallbackStorage<FormClosedEvent> cb = nnme->closeHandlers.GetItem(i);
				cb.func(cb.userObj, nnme);
			}
			nnme->EventClosed();
			if (!nnme->isDialog)
			{
				nnme->DestroyObject();
				nnme.Delete();
			}
			else
			{
				nnme->isDialog = false;
			}
		}
		else
		{
			printf("GUIForm: Me is null on WM_DESTROY\r\n");
		}
		return 0;
	case WM_TIMER:
		if (me.SetTo(nnme))
		{
			nnme->EventTimer(wParam);
		}
		else
		{
			printf("GUIForm: Me is null on WM_TIMER\r\n");
		}
		return 0;
#ifndef _WIN32_WCE
	case WM_DROPFILES:
		if (me.SetTo(nnme))
		{
			nnme->OnDropFiles((void*)wParam);
		}
		else
		{
			printf("GUIForm: Me is null on WM_DROPFILES\r\n");
		}
		return 0;
#endif
	case WM_CTLCOLORSTATIC:
		{
			UI::GUILabel *lbl;
			lbl = (UI::GUILabel*)(IntOS)GetWindowLongPtr((HWND)lParam, GWL_USERDATA);
			if (lbl && lbl->GetObjectClass().Equals(CSTR("Label")))
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
		if (me.SetTo(nnme))
		{
			nnme->OnFocusLost();
		}
		else
		{
			printf("GUIForm: Me is null on WM_KILLFOCUS\r\n");
		}
		break;
	case WM_SETFOCUS:
		if (me.SetTo(nnme))
		{
			((UI::Win::WinCore*)nnme->ui.Ptr())->SetFocusWnd((ControlHandle*)hWnd, nnme->hAcc);
			nnme->OnFocus();
		}
		else
		{
			printf("GUIForm: Me is null on WM_SETFOCUS\r\n");
		}
		break;
	case WM_HOTKEY:
		if (wParam & 0xffff8000)
		{
			if (me.SetTo(nnme))
			{
				nnme->EventMenuClicked((UInt16)(wParam & 0x7fff));
			}
			else
			{
				printf("GUIForm: Me is null on WM_HOTKEY\r\n");
			}
		}
		break;
//	case WM_NCPAINT:
//		break;
	case WM_ERASEBKGND:
		if (me.SetTo(nnme))
		{
			if (nnme->fs)
			{
				return TRUE;
			}
			else
			{
				return nnme->MyEraseBkg((void*)wParam);
			}
		}
		else
		{
			printf("GUIForm: Me is null on WM_ERASEBKGND\r\n");
		}
		break;
	case WM_WINDOWPOSCHANGED:
		break;
	case WM_MOVE:
		if (me.SetTo(nnme))
		{
			GetWindowRect((HWND)nnme->hwnd.OrNull(), &rc);
			nnme->lxPos = rc.left * nnme->ddpi / nnme->hdpi;
			nnme->lyPos = rc.top * nnme->ddpi / nnme->hdpi;
			nnme->lxPos2 = rc.right * nnme->ddpi / nnme->hdpi;
			nnme->lyPos2 = rc.bottom * nnme->ddpi / nnme->hdpi;
			nnme->OnPosChanged(false);
		}
		else
		{
			printf("GUIForm: Me is null on WM_MOVE\r\n");
		}
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
		if (me.SetTo(nnme))
		{
			if (nnme->OnPaint())
				return TRUE;
		}
		else
		{
			printf("GUIForm: Me is null on WM_PAINT\r\n");
		}
		break;
	case WM_KEYUP:
		if (me.SetTo(nnme))
		{
			i = nnme->keyUpHandlers.GetCount();
			while (i-- > 0)
			{
				Data::CallbackStorage<KeyEvent> cb = nnme->keyUpHandlers.GetItem(i);
				cb.func(cb.userObj, wParam, (lParam & 0x1000000) != 0);
			}
		}
		else
		{
			printf("GUIForm: Me is null on WM_KEYUP\r\n");
		}
		break;
	case WM_KEYDOWN:
		if (me.SetTo(nnme))
		{
			i = nnme->keyDownHandlers.GetCount();
			while (i-- > 0)
			{
				Data::CallbackStorage<KeyEvent> cb = nnme->keyDownHandlers.GetItem(i);
				cb.func(cb.userObj, wParam, (lParam & 0x1000000) != 0);
			}
		}
		else
		{
			printf("GUIForm: Me is null on WM_KEYDOWN\r\n");
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
		if (me.SetTo(nnme))
		{
			nnme->OnDisplaySizeChange(LOWORD(lParam), HIWORD(lParam));
		}
		else
		{
			printf("GUIForm: Me is null on WM_DISPLAYCHANGE\r\n");
		}
		break;
#endif
	default:
		return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	};
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUIForm::Init(Optional<InstanceHandle> hInst)
{
	WNDCLASSW wc;
    wc.style = 0; 
	wc.lpfnWndProc = (WNDPROC)UI::GUIForm::FormWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst.OrNull(); 
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

void UI::GUIForm::Deinit(Optional<InstanceHandle> hInst)
{
	UnregisterClassW(L"WinForm", (HINSTANCE)hInst.OrNull());
}

void UI::GUIForm::UpdateHAcc()
{
	if (this->hAcc)
	{
		DestroyAcceleratorTable((HACCEL)this->hAcc);
		this->hAcc = 0;
	}
	NN<UI::GUIMainMenu> menu;
	if (this->menu.SetTo(menu))
	{
		UIntOS i;
		ACCEL *accels;
		Data::ArrayListNN<UI::GUIMenu::ShortcutKey> keys;
		NN<UI::GUIMenu::ShortcutKey> key;
		menu->GetAllKeys(keys);
		i = keys.GetCount();
		if (i > 0)
		{
			accels = MemAlloc(ACCEL, i);
			while (i-- > 0)
			{
				key = keys.GetItemNoCheck(i);
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
		((UI::Win::WinCore*)this->ui.Ptr())->SetFocusWnd(this->hwnd, this->hAcc);
	}
}
UI::GUIForm::GUIForm(NN<UI::GUICore> ui, Optional<ControlHandle> hWnd) : UI::GUIClientControl(ui, nullptr)
{
	this->hwnd = hWnd;
	this->hAcc = 0;
	this->virtualMode = true;
	this->currDialog = nullptr;
	this->fs = false;
}

Optional<UI::GUIForm> UI::GUIForm::FindForm(NN<UI::GUICore> ui, Text::CStringNN formName)
{
	HWND hWnd;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(formName.v);
	hWnd = FindWindowW(L"WinForm", wptr.Ptr());
	Text::StrDelNew(wptr);
	if (hWnd == 0)
		return nullptr;
	UI::GUIForm *frm;
	NEW_CLASS(frm, UI::GUIForm(ui, (ControlHandle*)hWnd));
	return frm;
}

UI::GUIForm::GUIForm(Optional<UI::GUIClientControl> parent, Double initW, Double initH, NN<UI::GUICore> ui) : UI::GUIClientControl(ui, parent)
{
	this->virtualMode = false;
	this->hAcc = 0;
	this->currDialog = nullptr;
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(((UI::Win::WinCore*)ui.Ptr())->GetHInst());
	}
	this->closingHdlr = 0;
	this->exitOnClose = false;
	this->nextTmrId = 1;
	this->ui = ui;
	this->isDialog = false;
	this->dialogResult = DR_UNKNOWN;
	this->menu = nullptr;
	this->okBtn = nullptr;
	this->cancelBtn = nullptr;
	this->fs = false;

	Math::Size2DDbl sz;
	Double initX;
	Double initY;
	NN<GUIClientControl> nnparent;
	if (parent.SetTo(nnparent))
	{
		sz = nnparent->GetClientSize();
		initX = (sz.x - initW) * 0.5;
		initY = (sz.y - initH) * 0.5;
	}
	else
	{
		POINT pt;
		Bool found = false;
		GetCursorPos(&pt);
		HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
		if (hMon)
		{
			this->ui->GetMonitorDPIs((MonitorHandle*)hMon, this->hdpi, this->ddpi);
			initW = initW * this->hdpi / this->ddpi;
			initH = initH * this->hdpi / this->ddpi;
#if defined(_WIN32_WCE)
			MONITORINFOEX info;
			info.cbSize = sizeof(info);
			if (::GetMonitorInfo(hMon, &info))
			{
				found = true;
				w = (info.rcMonitor.right - info.rcMonitor.left);
				h = (info.rcMonitor.bottom - info.rcMonitor.top);
				if (initW > w)
				{
					initW = w;
				}
				if (initH > h)
				{
					initH = h;
				}
				initX = info.rcMonitor.left + ((w - initW) * 0.5);
				initY = info.rcMonitor.top + ((h - initH) * 0.5);
			}
#else
			MONITORINFOEXW info;
			info.cbSize = sizeof(info);
			if (::GetMonitorInfoW(hMon, &info))
			{
				found = true;
				sz.x = (info.rcMonitor.right - info.rcMonitor.left);
				sz.y = (info.rcMonitor.bottom - info.rcMonitor.top);
				if (initW > sz.x)
				{
					initW = sz.x;
				}
				if (initH > sz.y)
				{
					initH = sz.y;
				}
				initX = info.rcMonitor.left + ((sz.x - initW) * 0.5);
				initY = info.rcMonitor.top + ((sz.y - initH) * 0.5);
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
			sz.x = (rc.right - rc.left);
			sz.y = (rc.bottom - rc.top);
			if (initW > sz.x)
			{
				initW = sz.x;
			}
			if (initH > sz.y)
			{
				initH = sz.y;
			}
			initX = (sz.x - initW) * 0.5;
			initY = (sz.y - initH) * 0.5;
		}
	}
	this->undockLeft = 0;
	this->undockTop = 0;
	this->undockRight = sz.x;
	this->undockBottom = sz.y;
	this->InitControl(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst(), parent, L"WinForm", (const UTF8Char*)"Form", WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, 0, initX, initY, initW, initH);// | WS_THICKFRAME | WS_MAXIMIZEBOX
	this->SetFont(nullptr, 0, false);
	SetNoResize(false);
}

UI::GUIForm::~GUIForm()
{
	if (this->virtualMode)
	{
	}
	else
	{
		UIntOS i = this->timers.GetCount();
		while (i-- > 0)
		{
			this->timers.GetItem(i).Delete();
		}
		this->menu.Delete();
		if (Sync::Interlocked::DecrementI32(useCnt) == 0)
		{
			Deinit(((UI::Win::WinCore*)this->ui.Ptr())->GetHInst());
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
		ShowWindow((HWND)this->hwnd.OrNull(), SW_SHOWNORMAL);
		break;
	case FS_MAXIMIZED:
		ShowWindow((HWND)this->hwnd.OrNull(), SW_MAXIMIZE);
		break;
	case FS_MINIMIZED:
		ShowWindow((HWND)this->hwnd.OrNull(), SW_MINIMIZE);
		break;
	}
}

UI::GUIForm::DialogResult UI::GUIForm::ShowDialog(Optional<UI::GUIForm> owner)
{
	NN<UI::GUIForm> frm;
	if (owner.SetTo(frm))
	{
		frm->SetEnabled(false);
		frm->currDialog = this;
	}
	this->isDialog = true;
	this->Show();
	while (this->isDialog)
	{
		ui->WaitForMessages();
		ui->ProcessMessages();
	}
	if (owner.SetTo(frm))
	{
		frm->currDialog = nullptr;
		frm->SetEnabled(true);
		frm->MakeForeground();
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
	Int32 style = GetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE);
	if (show)
	{
		SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, style | WS_CAPTION | WS_SYSMENU);
	}
	else
	{
		SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, style & ~(WS_CAPTION | WS_SYSMENU));
	}
}

void UI::GUIForm::SetAlwaysOnTop(Bool alwaysOnTop)
{
	if (alwaysOnTop)
	{
		SetWindowPos((HWND)this->hwnd.OrNull(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
	else
	{
		SetWindowPos((HWND)this->hwnd.OrNull(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
}

void UI::GUIForm::MakeActive()
{
	SetActiveWindow((HWND)this->hwnd.OrNull());
}

void UI::GUIForm::MakeForeground()
{
#ifdef _WIN32_WCE
	SetForegroundWindow((HWND)(((IntOS)this->hwnd) | 1));
#else
	SetForegroundWindow((HWND)this->hwnd.OrNull());
#endif
}

void UI::GUIForm::Close()
{
	NN<UI::GUIForm> currDialog;
	if (this->currDialog.SetTo(currDialog))
	{
		currDialog->Close();
	}
	DestroyWindow((HWND)hwnd.OrNull());
}

void UI::GUIForm::SetText(Text::CStringNN text)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(text.v);
	SetWindowTextW((HWND)this->hwnd.OrNull(), wptr.Ptr());
	Text::StrDelNew(wptr);
}

Math::Size2D<UIntOS> UI::GUIForm::GetSizeP()
{
	RECT rect;
	GetWindowRect((HWND)hwnd.OrNull(), &rect);
	return Math::Size2D<UIntOS>((UIntOS)(IntOS)(rect.right - rect.left),
		(UIntOS)(IntOS)(rect.bottom - rect.top));
}

void UI::GUIForm::SetExitOnClose(Bool exitOnClose)
{
	this->exitOnClose = exitOnClose;
}

void UI::GUIForm::SetNoResize(Bool noResize)
{
	Int32 style = GetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE);
	if (noResize)
	{
		SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, style & ~(WS_MAXIMIZEBOX | WS_THICKFRAME));
	}
	else
	{
		SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME);
	}
}

NN<UI::GUITimer> UI::GUIForm::AddTimer(UInt32 interval, UI::UIEvent handler, AnyType userObj)
{
	NN<UI::Win::WinTimer> tmr;
	NEW_CLASSNN(tmr, UI::Win::WinTimer(*this, this->nextTmrId++, interval, handler, userObj));
	this->timers.Add(tmr);
	return tmr;
}

void UI::GUIForm::RemoveTimer(NN<UI::GUITimer> tmr)
{
	UIntOS i = this->timers.GetCount();
	while (i-- > 0)
	{
		if (tmr.Ptr() == this->timers.GetItem(i).OrNull())
		{
			this->timers.RemoveAt(i);
			tmr.Delete();
			break;
		}
	}
}

void UI::GUIForm::SetMenu(NN<UI::GUIMainMenu> menu)
{
#ifndef _WIN32_WCE
	::SetMenu((HWND)this->hwnd.OrNull(), (HMENU)menu->GetHMenu());
#endif
	this->menu.Delete();
	this->menu = menu;
	this->UpdateHAcc();
}

Optional<UI::GUIMainMenu> UI::GUIForm::GetMenu()
{
	return this->menu;
}

void UI::GUIForm::UpdateMenu()
{
	DrawMenuBar((HWND)this->hwnd.OrNull());
	this->UpdateHAcc();
}

void UI::GUIForm::SetDefaultButton(NN<UI::GUIButton> btn)
{
	this->okBtn = btn;
	NN<UI::Win::WinButton>::ConvertFrom(btn)->SetDefaultBtnLook();
}

void UI::GUIForm::SetCancelButton(NN<UI::GUIButton> btn)
{
	this->cancelBtn = btn;
}

Optional<UI::GUIButton> UI::GUIForm::GetDefaultButton()
{
	return this->okBtn;
}

Optional<UI::GUIButton> UI::GUIForm::GetCancelButton()
{
	return this->cancelBtn;
}

Math::Size2DDbl UI::GUIForm::GetClientSize()
{
	RECT rc;
	GetClientRect((HWND)this->hwnd.OrNull(), &rc);
	return Math::Size2DDbl(rc.right - rc.left, rc.bottom - rc.top) * this->ddpi / this->hdpi;
}

Bool UI::GUIForm::IsChildVisible()
{
	return true;
}

Text::CStringNN UI::GUIForm::GetObjectClass() const
{
	return CSTR("WinForm");
}

IntOS UI::GUIForm::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIForm::OnSizeChanged(Bool updateScn)
{
	if (this->selfResize)
	{
		return;
	}
	HMONITOR hMon = MonitorFromWindow((HWND)this->hwnd.OrNull(), MONITOR_DEFAULTTONEAREST);
	if (hMon != (HMONITOR)this->currHMon.OrNull())
	{
		this->currHMon = (MonitorHandle*)hMon;
		this->OnMonitorChanged();
	}
	UIntOS i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->resizeHandlers.GetItem(i);
		cb.func(cb.userObj);
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
		WChar wbuff[256];
		NN<Text::String> *files = MemAlloc(NN<Text::String>, fileCnt);
		i = 0;
		while (i < fileCnt)
		{
			DragQueryFileW((HDROP)hDrop, i, wbuff, 256);
			files[i] = Text::String::NewNotNull(wbuff);
			i++;
		}
		UIntOS j = this->dropFileHandlers.GetCount();
		while (j-- > 0)
		{
			Data::CallbackStorage<FileEvent> cb = this->dropFileHandlers.GetItem(j);
			cb.func(cb.userObj, {files, (UIntOS)fileCnt});
		}
		while (fileCnt-- > 0)
		{
			files[fileCnt]->Release();
		}
		MemFree(files);
	}
#endif
}

void UI::GUIForm::EventMenuClicked(UInt16 cmdId)
{
	UIntOS i;
	i = this->menuClickedHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<MenuEvent> cb = this->menuClickedHandlers.GetItem(i);
		cb.func(cb.userObj, cmdId);
	}
}

void UI::GUIForm::SetSmallIcon(UI::GUIIcon *icon)
{
	if (icon)
	{
		SendMessage((HWND)this->hwnd.OrNull(), WM_SETICON, ICON_SMALL, (LPARAM)icon->GetHandle());
	}
	else
	{
		SendMessage((HWND)this->hwnd.OrNull(), WM_SETICON, ICON_SMALL, 0);
	}
}

void UI::GUIForm::SetLargeIcon(UI::GUIIcon *icon)
{
	SendMessage((HWND)this->hwnd.OrNull(), WM_SETICON, ICON_BIG, (LPARAM)icon->GetHandle());
}

void UI::GUIForm::ShowMouseCursor(Bool toShow)
{
	ShowCursor(toShow?TRUE:FALSE);
}

void UI::GUIForm::HandleFormClosed(FormClosedEvent handler, AnyType userObj)
{
	this->closeHandlers.Add({handler, userObj});
}

void UI::GUIForm::HandleDropFiles(FileEvent handler, AnyType userObj)
{
#ifndef _WIN32_WCE
	if (this->dropFileHandlers.GetCount() == 0)
	{
		IntOS style = GetWindowLongPtr((HWND)this->hwnd.OrNull(), GWL_EXSTYLE);
		UI::Win::WinCore::MSSetWindowObj(this->hwnd, GWL_EXSTYLE, style | WS_EX_ACCEPTFILES);
	}
#endif
	this->dropFileHandlers.Add({handler, userObj});
}

void UI::GUIForm::HandleMenuClicked(MenuEvent handler, AnyType userObj)
{
	this->menuClickedHandlers.Add({handler, userObj});
}

void UI::GUIForm::HandleKeyDown(KeyEvent handler, AnyType userObj)
{
	this->keyDownHandlers.Add({handler, userObj});
}

void UI::GUIForm::HandleKeyUp(KeyEvent handler, AnyType userObj)
{
	this->keyUpHandlers.Add({handler, userObj});
}

void UI::GUIForm::SetClosingHandler(FormClosingEvent handler, AnyType userObj)
{
	this->closingHdlr = {handler, userObj};
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
	NN<UI::GUIMainMenu> menu;
	if (this->menu.SetTo(menu))
	{
		menu->SetDPI(hdpi, ddpi);
	}
	Data::ArrayIterator<NN<GUIControl>> it = this->children.Iterator();
	while (it.HasNext())
	{
		it.Next()->SetDPI(hdpi, ddpi);
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

void UI::GUIForm::EventTimer(UIntOS tmrId)
{
	NN<UI::GUITimer> tmr;
	Data::ArrayIterator<NN<UI::GUITimer>> it = this->timers.Iterator();
	while (it.HasNext())
	{
		tmr = it.Next();
		if (NN<UI::Win::WinTimer>::ConvertFrom(tmr)->GetId() == tmrId)
		{
			tmr->EventTick();
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

void UI::GUIForm::OnDisplaySizeChange(UIntOS dispWidth, UIntOS dispHeight)
{
}

void UI::GUIForm::ToFullScn()
{
	if (this->fs)
		return;
#ifdef _WIN32_WCE
	HWND hWnd = (HWND)this->hwnd.OrNull();
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
	HWND hWnd = (HWND)this->hwnd.OrNull();
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
	SetWindowLong((HWND)this->hwnd.OrNull(), GWL_STYLE, (LONG)this->fsStyle);
	MoveWindow((HWND)this->hwnd.OrNull(), this->fsX, this->fsY, this->fsW, this->fsH, TRUE);

	SetWindowPos((HWND)this->hwnd.OrNull(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE); 

	NN<UI::GUIMainMenu> menu;
	if (this->menu.SetTo(menu))
	{
#ifdef _WIN32_WCE
//		::SetMenu((HWND)this->hwnd, (HMENU)menu->GetHMenu());
#else
		::SetMenu((HWND)this->hwnd.OrNull(), (HMENU)menu->GetHMenu());
#endif
	}
}

NN<UI::GUICore> UI::GUIForm::GetUI()
{
	return this->ui;
}
