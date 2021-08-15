#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIClientControl.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIDDrawControl.h"
#include "UI/GUIForm.h"

#if defined(WINVER)
#undef WINVER
#endif
#define WINVER 0x601

#if defined(_WIN32_WINNT)
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x601

#include <windows.h>
#include <ddraw.h>

typedef BOOL (WINAPI *GetGestureInfoFunc)(HGESTUREINFO hGestureInfo, PGESTUREINFO pGestureInfo);
typedef BOOL (WINAPI *CloseGestureInfoHandleFunc)(HGESTUREINFO hGestureInfo);

#ifndef GWL_USERDATA
#define GWL_USERDATA GWLP_USERDATA
#endif

#define CLASSNAME L"DDrawControl"
#undef MB_RIGHT
#undef GetMonitorInfo
Int32 UI::GUIDDrawControl::useCnt = 0;

OSInt __stdcall UI::GUIDDrawControl::FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)(OSInt)GetWindowLongPtr((HWND)hWnd, GWL_USERDATA);
	if (me == 0)
	{
		switch (msg)
		{
		case WM_PAINT:
		case WM_ERASEBKGND:
			return 1;
		default:
			return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
		}
	}
	switch (msg)
	{
	case WM_NCPAINT:
		return 1;
	case WM_PAINT:
		me->OnPaint();
		ValidateRect((HWND)hWnd, 0);
		return 1;
	case WM_ERASEBKGND:
		return 1;
	case WM_LBUTTONDOWN:
		me->OnMouseDown((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_LEFT);
		return 0;
	case WM_LBUTTONUP:
		me->OnMouseUp((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_LEFT);
		return 0;
	case WM_LBUTTONDBLCLK:
		me->OnMouseDblClick((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_LEFT);
		return 0;
	case WM_RBUTTONDOWN:
		me->OnMouseDown((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_RIGHT);
		return 0;
	case WM_RBUTTONUP:
		me->OnMouseUp((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_RIGHT);
		return 0;
	case WM_RBUTTONDBLCLK:
		me->OnMouseDblClick((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_RIGHT);
		return 0;
	case WM_MBUTTONDOWN:
		me->OnMouseDown((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_MIDDLE);
		return 0;
	case WM_MBUTTONUP:
		me->OnMouseUp((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_MIDDLE);
		return 0;
	case WM_XBUTTONDOWN:
		me->OnMouseDown((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_X1);
		return 0;
	case WM_XBUTTONUP:
		me->OnMouseUp((Int16)LOWORD(lParam), (Int16)HIWORD(lParam), MBTN_X1);
		return 0;
	case WM_MOUSEMOVE:
		me->OnMouseMove((Int16)LOWORD(lParam), (Int16)HIWORD(lParam));
		return 0;
	case WM_MOUSEWHEEL:
		{
			OSInt scnX;
			OSInt scnY;
			me->GetScreenPosP(&scnX, &scnY);
			me->OnMouseWheel(-scnX + (Int16)LOWORD(lParam), -scnY + (Int16)HIWORD(lParam), (Int16)HIWORD(wParam));
		}
		return 0;
	case WM_SIZE:
		me->OnSizeChanged(false);
		return 0;//DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	case WM_GESTURE:
		{
			GetGestureInfoFunc GetGestureInfoF = (GetGestureInfoFunc)me->lib->GetFunc("GetGestureInfo");
			CloseGestureInfoHandleFunc CloseGestureInfoHandleF = (CloseGestureInfoHandleFunc)me->lib->GetFunc("CloseGestureInfoHandle");
			if (GetGestureInfoF && CloseGestureInfoHandleF)
			{
				Bool handled = false;
				GESTUREINFO gi;  
				MemClear(&gi, sizeof(GESTUREINFO));
				gi.cbSize = sizeof(GESTUREINFO);
				if (GetGestureInfoF((HGESTUREINFO)lParam, &gi))
				{
					if (gi.dwID == GID_ZOOM)
					{
						handled = true;
						if (gi.dwFlags & GF_BEGIN)
						{
							RECT rcWnd;
							GetWindowRect((HWND)hWnd, &rcWnd);
							me->OnGZoomBegin(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top, gi.ullArguments);
						}
						else if (gi.dwFlags & GF_END)
						{
							RECT rcWnd;
							GetWindowRect((HWND)hWnd, &rcWnd);
							me->OnGZoomEnd(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top, gi.ullArguments);
						}
						else
						{
							RECT rcWnd;
							GetWindowRect((HWND)hWnd, &rcWnd);
							me->OnGZoomStep(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top, gi.ullArguments);
						}
					}
				}
				CloseGestureInfoHandleF((HGESTUREINFO)lParam);
				if (handled)
				{
					return 0;
				}
				else
				{
					return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
				}
			}
		}
		break;
	case WM_TIMER:
		if (me->focusing)
		{
			MMRESULT ret;
			JOYINFOEX info;
			info.dwSize = sizeof(JOYINFOEX);
			info.dwFlags = JOY_RETURNALL;
			ret = joyGetPosEx(me->joystickId - 1, &info);
			if (ret == JOYERR_NOERROR)
			{
				OSInt i = 1;
				UInt32 mask = 1;
				while (i <= 32)
				{
					if ((me->jsLastButtons & mask) != (info.dwButtons & mask))
					{
						if (info.dwButtons & mask)
						{
							me->OnJSButtonDown(i);
						}
						else
						{
							me->OnJSButtonUp(i);
						}
					}
					mask = mask << 1;
					i++;
				}
				me->OnJSAxis((Int32)(info.dwXpos - 0x8000), (Int32)(info.dwYpos - 0x8000), (Int32)(info.dwZpos - 0x8000), (Int32)(info.dwRpos - 0x8000));
				me->jsLastButtons = info.dwButtons;
			}
		}
		break;
	case WM_SETFOCUS:
		me->focusing = true;
		break;
	case WM_KILLFOCUS:
		me->focusing = false;
		break;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
}

void UI::GUIDDrawControl::Init(void *hInst)
{
	WNDCLASSW wc;
    wc.style = CS_DBLCLKS; 
	wc.lpfnWndProc = (WNDPROC)UI::GUIDDrawControl::FormWndProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = (HINSTANCE)hInst; 
    wc.hIcon = 0; 
    wc.hCursor = LoadCursor((HINSTANCE) NULL, 
        IDC_ARROW); 
    wc.hbrBackground = 0; 
    wc.lpszMenuName = 0; 
    wc.lpszClassName = CLASSNAME; 

    if (!RegisterClassW(&wc)) 
        return; 
}

void UI::GUIDDrawControl::Deinit(void *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void __stdcall UI::GUIDDrawControl::OnResized(void *userObj)
{
	UI::GUIDDrawControl *me = (UI::GUIDDrawControl*)userObj;
	if (me->switching)
		return;
	if (me->currScnMode == SM_FS)
	{
	}
	else
	{
		Sync::MutexUsage mutUsage(me->surfaceMut);
		me->GetSizeP(&me->surfaceW, &me->surfaceH);
		me->ReleaseSubSurface();
		me->CreateSubSurface();
		mutUsage.EndUse();

		if (me->debugWriter)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Surface size changed to ");
			sb.AppendUOSInt(me->surfaceW);
			sb.Append((const UTF8Char*)" x ");
			sb.AppendUOSInt(me->surfaceH);
			sb.Append((const UTF8Char*)", hMon=");
			sb.AppendOSInt((OSInt)me->GetHMonitor());
			me->debugWriter->WriteLine(sb.ToString());
		}
		if (me->inited)
		{
			me->OnSurfaceCreated();
		}
	}
}

void UI::GUIDDrawControl::GetDrawingRect(void *rc)
{
	if (this->currScnMode == SM_FS || this->currScnMode == SM_VFS)
	{
		RECT *rcSrc = (RECT*)rc;
		rcSrc->left = 0;
		rcSrc->top = 0;
		rcSrc->right = (LONG)this->scnW;
		rcSrc->bottom = (LONG)this->scnH;
		ClientToScreen((HWND)this->hwnd, (POINT*)&((RECT*)rc)->left);
		ClientToScreen((HWND)this->hwnd, (POINT*)&((RECT*)rc)->right);
	}
	else
	{
		GetClientRect((HWND)this->hwnd, (RECT*)rc);
		ClientToScreen((HWND)this->hwnd, (POINT*)&((RECT*)rc)->left);
		ClientToScreen((HWND)this->hwnd, (POINT*)&((RECT*)rc)->right);
	}
}

void UI::GUIDDrawControl::OnPaint()
{
	if (this->currScnMode != SM_FS && this->currScnMode != SM_VFS)
	{
		Sync::MutexUsage mutUsage(this->surfaceMut);
		DrawToScreen();
		mutUsage.EndUse();
	}
	else
	{
//		PAINTSTRUCT ps;
//		BeginPaint((HWND)this->hwnd, &ps);
//		EndPaint((HWND)this->hwnd, &ps);
	}
}

Bool UI::GUIDDrawControl::CreateSurface()
{
	this->ReleaseSurface();
	this->ReleaseSubSurface();

	if (this->debugWriter)
	{
		this->debugWriter->WriteLine((const UTF8Char*)"Create Surface");
	}

	if (this->currScnMode == SM_FS)
	{
		this->surfaceMon = this->GetHMonitor();
		Bool succ = this->surfaceMgr->CreatePrimarySurfaceWithBuffer(this->surfaceMon, &this->primarySurface, &this->buffSurface);
		if (succ)
		{
			this->bitDepth = this->primarySurface->info->storeBPP;
			this->scnW = this->primarySurface->info->dispWidth;
			this->scnH = this->primarySurface->info->dispHeight;
		}
		return succ;
	}
	else
	{
		ControlHandle *hWnd;
		if (this->currScnMode == SM_VFS)
		{
			this->surfaceMon = this->GetHMonitor();
			hWnd = 0;
		}
		else if (this->currScnMode == SM_WINDOWED_DIR)
		{
			this->surfaceMon = this->GetHMonitor();
			hWnd = this->GetHandle();
		}
		else
		{
			this->surfaceMon = 0;
			hWnd = this->GetHandle();
		}
		this->primarySurface = this->surfaceMgr->CreatePrimarySurface(this->surfaceMon, hWnd);
		if (this->primarySurface)
		{
			if (this->debugWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"Primary surface desc: Size = ");
				sb.AppendUOSInt(this->primarySurface->info->dispWidth);
				sb.Append((const UTF8Char*)" x ");
				sb.AppendUOSInt(this->primarySurface->info->dispHeight);
				sb.Append((const UTF8Char*)", bpl = ");
				sb.AppendUOSInt(this->primarySurface->GetDataBpl());
				sb.Append((const UTF8Char*)", hMon = ");
				sb.AppendOSInt((OSInt)this->surfaceMon);
				sb.Append((const UTF8Char*)", hWnd = ");
				sb.AppendOSInt((OSInt)hWnd);
				this->debugWriter->WriteLine(sb.ToString());
			}
			this->bitDepth = this->primarySurface->info->storeBPP;
			this->scnW = this->primarySurface->info->dispWidth;
			this->scnH = this->primarySurface->info->dispHeight;

			CreateSubSurface();
			return true;
		}
		else
		{
			return false;
		}
	}
}

void UI::GUIDDrawControl::ReleaseSurface()
{
	SDEL_CLASS(this->primarySurface);
}

void UI::GUIDDrawControl::CreateSubSurface()
{
	RECT rc;
	GetDrawingRect(&rc);
	if (this->debugWriter)
	{
		this->debugWriter->WriteLine((const UTF8Char*)"Create Subsurface");
	}

	if (rc.right <= rc.left || rc.bottom <= rc.top)
	{
	}
	else
	{
		UInt32 w = (UInt32)(rc.right - rc.left);
		UInt32 h = (UInt32)(rc.bottom - rc.top);
		if (this->surfaceW != w || this->surfaceH != h)
		{
			if (this->debugWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"(CreateSubSurface) Surface size changed to ");
				sb.AppendU32(w);
				sb.Append((const UTF8Char*)" x ");
				sb.AppendU32(h);
				this->debugWriter->WriteLine(sb.ToString());
			}
			this->surfaceW = w;
			this->surfaceH = h;
		}
		this->buffSurface = this->surfaceMgr->CreateSurface(w, h, this->primarySurface->info->storeBPP);
	}
}

void UI::GUIDDrawControl::ReleaseSubSurface()
{
	SDEL_CLASS(this->buffSurface);
}

UInt8 *UI::GUIDDrawControl::LockSurfaceBegin(UOSInt targetWidth, UOSInt targetHeight, UOSInt *bpl)
{
	this->surfaceMut->Lock();
	if (this->buffSurface == 0)
	{
		this->surfaceMut->Unlock();
		return 0;
	}
	if (targetWidth == this->surfaceW && targetHeight == this->surfaceH)
	{
		UInt8 *dptr = this->buffSurface->LockSurface((OSInt*)bpl);
		if (dptr)
		{
			return dptr;
		}
	}
	this->surfaceMut->Unlock();
	return 0;
}

void UI::GUIDDrawControl::LockSurfaceEnd()
{
	this->buffSurface->UnlockSurface();
	this->surfaceMut->Unlock();
}

Media::PixelFormat UI::GUIDDrawControl::GetPixelFormat()
{
	if (this->bitDepth == 16)
	{
		return Media::PF_LE_R5G6B5;
	}
	else
	{
		return Media::PF_B8G8R8A8;
	}
}
UI::GUIDDrawControl::GUIDDrawControl(GUICore *ui, UI::GUIClientControl *parent, Bool directMode, Media::ColorManagerSess *colorSess) : UI::GUIControl(ui, parent)
{
	this->inited = false;
	this->primarySurface = 0;
	this->buffSurface = 0;
	this->imgCopy = 0;
	this->joystickId = 0;
	this->jsLastButtons = 0;
	this->focusing = false;
	NEW_CLASS(this->surfaceMut, Sync::Mutex());
	this->rootForm = parent->GetRootForm();
	this->fullScnMode = SM_WINDOWED;
	this->directMode = directMode;
	this->switching = false;
	this->debugFS = 0;
	this->debugWriter = 0;
	NEW_CLASS(this->lib, IO::Library((const UTF8Char*)"User32.dll"));
#if defined(_DEBUG)
	{
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"Ddraw.log", IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		this->debugFS = fs;
		NEW_CLASS(this->debugWriter, Text::UTF8Writer(fs));
	}
#endif

	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		Init(((GUICoreWin*)ui)->GetHInst());
	}
	this->HandleSizeChanged(OnResized, this);

	UInt32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((GUICoreWin*)ui)->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"DDrawControl", style, 0, 0, 0, 640, 480);

	this->currScnMode = SM_VFS;
	this->surfaceMon = 0;
	NEW_CLASS(this->surfaceMgr, Media::DDrawManager(ui, colorSess));
	if (((Media::DDrawManager*)this->surfaceMgr)->IsError())
	{
	}
	else
	{
		MONITORINFOEXW monInfo;
		monInfo.cbSize = sizeof(monInfo);
		this->currMon = this->GetHMonitor();
		GetMonitorInfoW((HMONITOR)this->currMon, &monInfo);
		this->scnX = monInfo.rcMonitor.left;
		this->scnY = monInfo.rcMonitor.top;
		SwitchFullScreen(false, false);
	}

	UInt32 nPad = joyGetNumDevs();
	UInt32 i;
	MMRESULT ret;
	if (nPad > 0)
	{
		JOYINFOEX info;
		i = 0;
		while (i < nPad)
		{
			info.dwSize = sizeof(JOYINFOEX);
			info.dwFlags = JOY_RETURNALL;

			ret = joyGetPosEx(i, &info);
			if (ret == JOYERR_NOERROR)
			{
				this->joystickId = i + 1;
				SetTimer((HWND)this->hwnd, 1000, 18, 0);
				break;
			}
			i++;
		}

	}
	this->inited = true;
}

UI::GUIDDrawControl::~GUIDDrawControl()
{
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
		Deinit(((GUICoreWin*)ui)->GetHInst());
	}
	if (this->joystickId > 0)
	{
		joyReleaseCapture(this->joystickId - 1);
		this->joystickId = 0;
	}
	this->ReleaseSurface();
	this->ReleaseSubSurface();

	DEL_CLASS(this->surfaceMut);
	DEL_CLASS(this->surfaceMgr);
	SDEL_CLASS(this->imgCopy);
	if (this->debugWriter)
	{
		this->debugWriter->WriteLine((const UTF8Char*)"Release DDraw");
		DEL_CLASS(this->debugWriter);
		DEL_CLASS(this->debugFS);
		this->debugFS = 0;
		this->debugWriter = 0;
	}
	DEL_CLASS(this->lib);
}

void UI::GUIDDrawControl::SetUserFSMode(ScreenMode fullScnMode)
{
	this->fullScnMode = fullScnMode;
}

void UI::GUIDDrawControl::DrawToScreen()
{
	Sync::MutexUsage mutUsage(this->surfaceMut);
	if (this->debugWriter)
	{
		Text::StringBuilderUTF8 sb;
		Data::DateTime dt;
		dt.SetCurrTime();
		sb.Append((const UTF8Char*)"DrawToScreen ");
		sb.AppendDate(&dt);
		this->debugWriter->WriteLine(sb.ToString());
	}
	if (this->currScnMode == SM_FS)
	{
		if (this->primarySurface)
		{
			this->primarySurface->DrawFromBuff();
		}
	}
	else if (this->currScnMode == SM_VFS)
	{
		if (this->primarySurface && this->buffSurface)
		{
			if (!this->primarySurface->DrawFromSurface(this->buffSurface, true))
			{
				if (this->debugWriter)
				{
					this->debugWriter->WriteLine((const UTF8Char*)"DrawToScreen: failed");
				}
			}
		}
		else
		{
			if (this->debugWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"DrawToScreen: surface not found: ");
				this->debugWriter->WriteLine(sb.ToString());
			}
		}
	}
	else if (this->currScnMode == SM_WINDOWED_DIR)
	{
		if (GetVisible())
		{
			this->primarySurface->DrawFromSurface(this->buffSurface, true);
		}
	}
	else
	{
		if (GetVisible())
		{
			if (this->primarySurface && this->buffSurface)
			{
				this->primarySurface->DrawFromSurface(this->buffSurface, true);
			}
			else
			{
				if (this->debugWriter)
				{
					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"DrawToScreen: Surface not found wind: ");
					this->debugWriter->WriteLine(sb.ToString());
				}
			}
//			ValidateRect((HWND)this->hwnd, &rcSrc);
		}
	}
}

void UI::GUIDDrawControl::DrawFromBuff(UInt8 *buff, OSInt lineAdd, OSInt tlx, OSInt tly, UOSInt drawW, UOSInt drawH, Bool clearScn)
{
	if (primarySurface)
	{
		this->primarySurface->DrawFromMem(buff, lineAdd, tlx, tly, drawW, drawH, clearScn, true);
	}
}

void UI::GUIDDrawControl::SwitchFullScreen(Bool fullScn, Bool vfs)
{
	if (this->debugWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"SwitchFullScreen ");
		sb.AppendI32(fullScn?1:0);
		sb.AppendI32(vfs?1:0);
		sb.Append((const UTF8Char*)", hMon=");
		sb.AppendOSInt((OSInt)this->GetHMonitor());
		this->debugWriter->WriteLine(sb.ToString());
	}
	if (fullScn)
	{
		if (this->currScnMode == SM_VFS || this->currScnMode == SM_FS)
			return;
	}
	else
	{
		if (this->currScnMode == SM_WINDOWED || this->currScnMode == SM_WINDOWED_DIR)
			return;
	}
	Sync::MutexUsage mutUsage(this->surfaceMut);
	if (fullScn && !vfs)
	{
		this->switching = true;
		LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)((Media::DDrawManager*)this->surfaceMgr)->GetDD7(this->GetHMonitor());
		this->currScnMode = SM_FS;
		DDSURFACEDESC2 ddsd;
		ddsd.dwSize = sizeof(ddsd);

		this->rootForm->ToFullScn();
		this->surfaceMgr->SetFSMode(this->GetHMonitor(), this->rootForm->GetHandle(), true);
		if (lpDD->GetDisplayMode(&ddsd) != DD_OK)
		{
			this->surfaceMgr->SetFSMode(this->GetHMonitor(), this->rootForm->GetHandle(), false);
			mutUsage.EndUse();
			this->rootForm->FromFullScn();
			this->switching = false;
			SwitchFullScreen(false, false);
			return;
		}
/*		if (DD_OK != lpDD->SetDisplayMode(ddsd.dwWidth, ddsd.dwHeight, 32, 0, 0))
		{
			this->ddMgr->SetFSMode(this->GetHMonitor(), this->rootForm->GetHandle(), false);
			this->surfaceMut->Unlock();
			this->rootForm->FromFullScn();
			SwitchFullScreen(false);
			return;
		}*/
		this->surfaceW = ddsd.dwWidth;
		this->surfaceH = ddsd.dwHeight;

		CreateSurface();
		if (this->primarySurface == 0)
		{
			this->surfaceMgr->SetFSMode(this->GetHMonitor(), this->rootForm->GetHandle(), false);
			mutUsage.EndUse();
			this->rootForm->FromFullScn();
			this->switching = false;
			SwitchFullScreen(false, false);
			return;
		}
		mutUsage.EndUse();
		this->switching = false;
		this->OnSizeChanged(true);
	}
	else if (fullScn && vfs)
	{
		this->switching = true;
		if (this->surfaceMon) this->surfaceMgr->SetFSMode(this->surfaceMon, this->rootForm->GetHandle(), false);
		if (this->imgCopy == 0)
		{
			NEW_CLASS(this->imgCopy, Media::ImageCopy());
			this->imgCopy->SetThreadPriority(Sync::Thread::TP_HIGHEST);
		}
		this->surfaceMgr->SetFSMode(this->GetHMonitor(), this->rootForm->GetHandle(), false);
		if (this->debugWriter)
		{
			Text::StringBuilderUTF8 sb;
			RECT rc;
			GetClientRect((HWND)this->hwnd, &rc);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.left);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.right);
			sb.Append((const UTF8Char*)"FS: Draw rect1 = ");
			sb.AppendI32(rc.left);
			sb.Append((const UTF8Char*)", ");
			sb.AppendI32(rc.top);
			sb.Append((const UTF8Char*)", ");
			sb.AppendI32(rc.right);
			sb.Append((const UTF8Char*)", ");
			sb.AppendI32(rc.bottom);
			this->debugWriter->WriteLine(sb.ToString());
			this->rootForm->ToFullScn();
			this->currScnMode = SM_VFS;
			GetClientRect((HWND)this->hwnd, &rc);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.left);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.right);
			sb.ClearStr();
			sb.Append((const UTF8Char*)"FS: Draw rect2 = ");
			sb.AppendI32(rc.left);
			sb.Append((const UTF8Char*)", ");
			sb.AppendI32(rc.top);
			sb.Append((const UTF8Char*)", ");
			sb.AppendI32(rc.right);
			sb.Append((const UTF8Char*)", ");
			sb.AppendI32(rc.bottom);
			this->debugWriter->WriteLine(sb.ToString());
			this->rootForm->SetFormState(UI::GUIForm::FS_MAXIMIZED);
			GetClientRect((HWND)this->hwnd, &rc);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.left);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.right);
			sb.ClearStr();
			sb.Append((const UTF8Char*)"FS: Draw rect3 = ");
			sb.AppendI32(rc.left);
			sb.Append((const UTF8Char*)", ");
			sb.AppendI32(rc.top);
			sb.Append((const UTF8Char*)", ");
			sb.AppendI32(rc.right);
			sb.Append((const UTF8Char*)", ");
			sb.AppendI32(rc.bottom);
			this->debugWriter->WriteLine(sb.ToString());
		}
		else
		{
			this->rootForm->ToFullScn();
			this->currScnMode = SM_VFS;
			this->rootForm->SetFormState(UI::GUIForm::FS_MAXIMIZED);
		}

		this->CreateSurface();
		mutUsage.EndUse();
		this->switching = false;
		this->OnSizeChanged(true);
	}
	else
	{
		this->switching = true;
		if (this->surfaceMon) this->surfaceMgr->SetFSMode(this->surfaceMon, this->rootForm->GetHandle(), false);
		this->rootForm->FromFullScn();
		if (this->directMode)
		{
			this->currScnMode = SM_WINDOWED_DIR;
		}
		else
		{
			this->currScnMode = SM_WINDOWED;
		}
		this->surfaceMgr->SetFSMode(0, this->rootForm->GetHandle(), false);

		this->CreateSurface();
		mutUsage.EndUse();
		this->switching = false;
		this->OnSizeChanged(true);
	}
}

Bool UI::GUIDDrawControl::IsFullScreen()
{
	return this->currScnMode == SM_FS || this->currScnMode == SM_VFS;
}

void UI::GUIDDrawControl::ChangeMonitor(MonitorHandle *hMon)
{
	MONITORINFOEXW monInfo;
	((Media::DDrawManager*)this->surfaceMgr)->ReleaseDD7(this->currMon);
	this->currMon = hMon;
	monInfo.cbSize = sizeof(monInfo);
	GetMonitorInfoW((HMONITOR)this->currMon, &monInfo);
	this->scnX = monInfo.rcMonitor.left;
	this->scnY = monInfo.rcMonitor.top;
	if (this->currScnMode == SM_WINDOWED_DIR)
	{
		this->CreateSurface();
	}
	this->OnMonitorChanged();
}

UInt32 UI::GUIDDrawControl::GetRefreshRate()
{
	return this->surfaceMgr->GetRefreshRate(this->GetHMonitor());
}

Bool UI::GUIDDrawControl::IsSurfaceReady()
{
	return this->buffSurface != 0;
}

void UI::GUIDDrawControl::OnMouseWheel(OSInt x, OSInt y, Int32 amount)
{
}

void UI::GUIDDrawControl::OnMouseMove(OSInt x, OSInt y)
{
}

void UI::GUIDDrawControl::OnMouseDown(OSInt x, OSInt y, MouseButton button)
{
}

void UI::GUIDDrawControl::OnMouseUp(OSInt x, OSInt y, MouseButton button)
{
}

void UI::GUIDDrawControl::OnMouseDblClick(OSInt x, OSInt y, MouseButton button)
{
	if (button == MBTN_LEFT)
	{
		if (this->fullScnMode == SM_WINDOWED || this->fullScnMode == SM_WINDOWED_DIR)
		{
		}
		else
		{
			if (this->currScnMode == SM_VFS || this->currScnMode == SM_FS)
			{
				this->SwitchFullScreen(false, false);
				this->OnSizeChanged(true);
			}
			else
			{
				this->SwitchFullScreen(true, this->fullScnMode == SM_VFS);
				this->OnSizeChanged(true);
			}
		}
	}
}

void UI::GUIDDrawControl::OnGZoomBegin(OSInt x, OSInt y, UInt64 dist)
{
}

void UI::GUIDDrawControl::OnGZoomStep(OSInt x, OSInt y, UInt64 dist)
{
}

void UI::GUIDDrawControl::OnGZoomEnd(OSInt x, OSInt y, UInt64 dist)
{
}

void UI::GUIDDrawControl::OnJSButtonDown(OSInt buttonId)
{
}

void UI::GUIDDrawControl::OnJSButtonUp(OSInt buttonId)
{
}

void UI::GUIDDrawControl::OnJSAxis(OSInt axis1, OSInt axis2, OSInt axis3, OSInt axis4)
{
}
