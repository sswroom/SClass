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
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIDDrawControl.h"
#include "UI/GUIForm.h"
#include "UI/Win/WinCore.h"

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
		me->OnMouseDown(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_LEFT);
		return 0;
	case WM_LBUTTONUP:
		me->OnMouseUp(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_LEFT);
		return 0;
	case WM_LBUTTONDBLCLK:
		me->OnMouseDblClick(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_LEFT);
		return 0;
	case WM_RBUTTONDOWN:
		me->OnMouseDown(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_RIGHT);
		return 0;
	case WM_RBUTTONUP:
		me->OnMouseUp(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_RIGHT);
		return 0;
	case WM_RBUTTONDBLCLK:
		me->OnMouseDblClick(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_RIGHT);
		return 0;
	case WM_MBUTTONDOWN:
		me->OnMouseDown(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_MIDDLE);
		return 0;
	case WM_MBUTTONUP:
		me->OnMouseUp(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_MIDDLE);
		return 0;
	case WM_XBUTTONDOWN:
		me->OnMouseDown(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_X1);
		return 0;
	case WM_XBUTTONUP:
		me->OnMouseUp(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)), MBTN_X1);
		return 0;
	case WM_MOUSEMOVE:
		me->OnMouseMove(Math::Coord2D<OSInt>((Int16)LOWORD(lParam), (Int16)HIWORD(lParam)));
		return 0;
	case WM_MOUSEWHEEL:
		{
			Math::Coord2D<OSInt> scnPos = me->GetScreenPosP();
			me->OnMouseWheel(Math::Coord2D<OSInt>(-scnPos.x + (Int16)LOWORD(lParam), -scnPos.y + (Int16)HIWORD(lParam)), (Int16)HIWORD(wParam));
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
							me->OnGZoomBegin(Math::Coord2D<OSInt>(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top), gi.ullArguments);
						}
						else if (gi.dwFlags & GF_END)
						{
							RECT rcWnd;
							GetWindowRect((HWND)hWnd, &rcWnd);
							me->OnGZoomEnd(Math::Coord2D<OSInt>(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top), gi.ullArguments);
						}
						else
						{
							RECT rcWnd;
							GetWindowRect((HWND)hWnd, &rcWnd);
							me->OnGZoomStep(Math::Coord2D<OSInt>(gi.ptsLocation.x - rcWnd.left, gi.ptsLocation.y - rcWnd.top), gi.ullArguments);
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

void UI::GUIDDrawControl::Init(InstanceHandle *hInst)
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

void UI::GUIDDrawControl::Deinit(InstanceHandle *hInst)
{
	UnregisterClassW(CLASSNAME, (HINSTANCE)hInst);
}

void __stdcall UI::GUIDDrawControl::OnResized(AnyType userObj)
{
	NN<UI::GUIDDrawControl> me = userObj.GetNN<UI::GUIDDrawControl>();
	if (me->switching)
		return;
	if (me->currScnMode == SM_FS)
	{
	}
	else
	{
		Sync::MutexUsage mutUsage(me->surfaceMut);
		Math::Size2D<UOSInt> sz = me->GetSizeP();
		me->dispSize = sz;
		me->ReleaseSubSurface();
		me->CreateSubSurface();
		mutUsage.EndUse();

		if (me->debugWriter)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Surface size changed to "));
			sb.AppendUOSInt(me->dispSize.x);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(me->dispSize.y);
			sb.AppendC(UTF8STRC(", hMon="));
			sb.AppendOSInt((OSInt)me->GetHMonitor());
			me->debugWriter->WriteLine(sb.ToCString());
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
	NN<Media::MonitorSurface> primarySurface;
	NN<Media::MonitorSurface> buffSurface;
	this->ReleaseSurface();
	this->ReleaseSubSurface();

	if (this->debugWriter)
	{
		this->debugWriter->WriteLine(CSTR("Create Surface"));
	}

	if (this->currScnMode == SM_FS)
	{
		this->surfaceMon = this->GetHMonitor();
		Bool succ = this->surfaceMgr->CreatePrimarySurfaceWithBuffer(this->surfaceMon, primarySurface, buffSurface, Media::RotateType::None);
		if (succ)
		{
			this->primarySurface = primarySurface;
			this->buffSurface = buffSurface;
			this->bitDepth = primarySurface->info.storeBPP;
			this->scnW = primarySurface->info.dispSize.x;
			this->scnH = primarySurface->info.dispSize.y;
			primarySurface->info.rotateType = this->rotType;
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
		this->primarySurface = this->surfaceMgr->CreatePrimarySurface(this->surfaceMon, hWnd, Media::RotateType::None);
		if (this->primarySurface.SetTo(primarySurface))
		{
			primarySurface->info.rotateType = this->rotType;
			if (this->debugWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Primary surface desc: Size = "));
				sb.AppendUOSInt(primarySurface->info.dispSize.x);
				sb.AppendC(UTF8STRC(" x "));
				sb.AppendUOSInt(primarySurface->info.dispSize.y);
				sb.AppendC(UTF8STRC(", bpl = "));
				sb.AppendUOSInt(primarySurface->GetDataBpl());
				sb.AppendC(UTF8STRC(", hMon = "));
				sb.AppendOSInt((OSInt)this->surfaceMon);
				sb.AppendC(UTF8STRC(", hWnd = "));
				sb.AppendOSInt((OSInt)hWnd);
				this->debugWriter->WriteLine(sb.ToCString());
			}
			this->bitDepth = primarySurface->info.storeBPP;
			this->scnW = primarySurface->info.dispSize.x;
			this->scnH = primarySurface->info.dispSize.y;

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
	this->primarySurface.Delete();
}

void UI::GUIDDrawControl::CreateSubSurface()
{
	NN<Media::MonitorSurface> primarySurface;
	RECT rc;
	GetDrawingRect(&rc);
	if (this->debugWriter)
	{
		this->debugWriter->WriteLine(CSTR("Create Subsurface"));
	}

	if (rc.right <= rc.left || rc.bottom <= rc.top)
	{
	}
	else if (this->primarySurface.SetTo(primarySurface))
	{
		UInt32 w = (UInt32)(rc.right - rc.left);
		UInt32 h = (UInt32)(rc.bottom - rc.top);
		if (this->dispSize.x != w || this->dispSize.y != h)
		{
			if (this->debugWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("(CreateSubSurface) Surface size changed to "));
				sb.AppendU32(w);
				sb.AppendC(UTF8STRC(" x "));
				sb.AppendU32(h);
				this->debugWriter->WriteLine(sb.ToCString());
			}
			this->dispSize = Math::Size2D<UOSInt>(w, h);
		}
		this->bkBuffSize = this->dispSize;
		if (this->rotType == Media::RotateType::CW_90 || this->rotType == Media::RotateType::CW_270 || this->rotType == Media::RotateType::HFLIP_CW_90 || this->rotType == Media::RotateType::HFLIP_CW_270)
		{
			this->bkBuffSize = this->dispSize.SwapXY();
		}
		this->buffSurface = this->surfaceMgr->CreateSurface(this->bkBuffSize, primarySurface->info.storeBPP);
	}
}

void UI::GUIDDrawControl::ReleaseSubSurface()
{
	this->buffSurface.Delete();
}

UInt8 *UI::GUIDDrawControl::LockSurfaceBegin(UOSInt targetWidth, UOSInt targetHeight, OutParam<OSInt> bpl)
{
	NN<Media::MonitorSurface> buffSurface;
	this->surfaceMut.Lock();
	if (!this->buffSurface.SetTo(buffSurface))
	{
		this->surfaceMut.Unlock();
		return 0;
	}
	if (targetWidth == this->bkBuffSize.x && targetHeight == this->bkBuffSize.y)
	{
		UInt8 *dptr = buffSurface->LockSurface(bpl);
		if (dptr)
		{
			return dptr;
		}
	}
	this->surfaceMut.Unlock();
	return 0;
}

void UI::GUIDDrawControl::LockSurfaceEnd()
{
	NN<Media::MonitorSurface> buffSurface;
	if (this->buffSurface.SetTo(buffSurface)) buffSurface->UnlockSurface();
	this->surfaceMut.Unlock();
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

void UI::GUIDDrawControl::BeginUpdateSize()
{
	this->switching = true;
}

void UI::GUIDDrawControl::EndUpdateSize()
{
	this->switching = false;
}

UI::GUIDDrawControl::GUIDDrawControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool directMode, NN<Media::ColorManagerSess> colorSess) : UI::GUIControl(ui, parent)
{
	this->inited = false;
	this->primarySurface = 0;
	this->buffSurface = 0;
	this->imgCopy = 0;
	this->joystickId = 0;
	this->jsLastButtons = 0;
	this->focusing = false;
	this->rootForm = parent->GetRootForm();
	this->fullScnMode = SM_WINDOWED;
	this->directMode = directMode;
	this->switching = false;
	this->debugFS = 0;
	this->debugWriter = 0;
	NEW_CLASS(this->lib, IO::Library((const UTF8Char*)"User32.dll"));
#if defined(_DEBUG)
	{
		NN<IO::FileStream> fs;
		NEW_CLASSNN(fs, IO::FileStream(CSTR("Ddraw.log"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		this->debugFS = fs.Ptr();
		NEW_CLASS(this->debugWriter, Text::UTF8Writer(fs));
	}
#endif

	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		Init(((Win::WinCore*)ui.Ptr())->GetHInst());
	}
	this->HandleSizeChanged(OnResized, this);

	UInt32 style = WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((Win::WinCore*)ui.Ptr())->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"DDrawControl", style, 0, 0, 0, 640, 480);

	this->currScnMode = SM_VFS;
	this->surfaceMon = 0;
	NEW_CLASSNN(this->surfaceMgr, Media::DDrawManager(ui->GetMonitorMgr(), colorSess));
	if (((Media::DDrawManager*)this->surfaceMgr.Ptr())->IsError())
	{
	}
	else
	{
		MONITORINFOEXW monInfo;
		monInfo.cbSize = sizeof(monInfo);
		this->currMon = this->GetHMonitor();
		::GetMonitorInfoW((HMONITOR)this->currMon, &monInfo);
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
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		Deinit(((Win::WinCore*)ui.Ptr())->GetHInst());
	}
	if (this->joystickId > 0)
	{
		joyReleaseCapture(this->joystickId - 1);
		this->joystickId = 0;
	}
	this->ReleaseSurface();
	this->ReleaseSubSurface();

	this->surfaceMgr.Delete();
	SDEL_CLASS(this->imgCopy);
	if (this->debugWriter)
	{
		this->debugWriter->WriteLine(CSTR("Release DDraw"));
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
	NN<Media::MonitorSurface> primarySurface;
	NN<Media::MonitorSurface> buffSurface;
	Sync::MutexUsage mutUsage(this->surfaceMut);
	if (this->debugWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("DrawToScreen "));
		sb.AppendTSNoZone(Data::Timestamp::Now());
		this->debugWriter->WriteLine(sb.ToCString());
	}
	if (this->currScnMode == SM_FS)
	{
		if (this->primarySurface.SetTo(primarySurface))
		{
			primarySurface->DrawFromBuff();
		}
	}
	else if (this->currScnMode == SM_VFS)
	{
		if (this->primarySurface.SetTo(primarySurface) && this->buffSurface.SetTo(buffSurface))
		{
			if (!primarySurface->DrawFromSurface(buffSurface, true))
			{
				if (this->debugWriter)
				{
					this->debugWriter->WriteLine(CSTR("DrawToScreen: failed"));
				}
			}
		}
		else
		{
			if (this->debugWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("DrawToScreen: surface not found: "));
				this->debugWriter->WriteLine(sb.ToCString());
			}
		}
	}
	else if (this->currScnMode == SM_WINDOWED_DIR)
	{
		if (GetVisible() && this->primarySurface.SetTo(primarySurface) && this->buffSurface.SetTo(buffSurface))
		{
			primarySurface->DrawFromSurface(buffSurface, true);
		}
	}
	else
	{
		if (GetVisible())
		{
			if (this->primarySurface.SetTo(primarySurface) && this->buffSurface.SetTo(buffSurface))
			{
				primarySurface->DrawFromSurface(buffSurface, true);
			}
			else
			{
				if (this->debugWriter)
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("DrawToScreen: Surface not found wind: "));
					this->debugWriter->WriteLine(sb.ToCString());
				}
			}
//			ValidateRect((HWND)this->hwnd, &rcSrc);
		}
	}
}

void UI::GUIDDrawControl::DisplayFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> drawSize, Bool clearScn)
{
	NN<Media::MonitorSurface> primarySurface;
	if (this->primarySurface.SetTo(primarySurface))
	{
		primarySurface->DrawFromSurface(surface, tl, drawSize, clearScn, true);
	}
}

void UI::GUIDDrawControl::SwitchFullScreen(Bool fullScn, Bool vfs)
{
	if (this->debugWriter)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("SwitchFullScreen "));
		sb.AppendI32(fullScn?1:0);
		sb.AppendI32(vfs?1:0);
		sb.AppendC(UTF8STRC(", hMon="));
		sb.AppendOSInt((OSInt)this->GetHMonitor());
		this->debugWriter->WriteLine(sb.ToCString());
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
		this->BeginUpdateSize();
		LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)((Media::DDrawManager*)this->surfaceMgr.Ptr())->GetDD7(this->GetHMonitor());
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
			this->EndUpdateSize();
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
		this->dispSize.x = ddsd.dwWidth;
		this->dispSize.y = ddsd.dwHeight;

		CreateSurface();
		if (this->primarySurface.IsNull())
		{
			this->surfaceMgr->SetFSMode(this->GetHMonitor(), this->rootForm->GetHandle(), false);
			mutUsage.EndUse();
			this->rootForm->FromFullScn();
			this->EndUpdateSize();
			SwitchFullScreen(false, false);
			return;
		}
		mutUsage.EndUse();
		this->EndUpdateSize();
		this->OnSizeChanged(true);
	}
	else if (fullScn && vfs)
	{
		this->BeginUpdateSize();
		if (this->surfaceMon) this->surfaceMgr->SetFSMode(this->surfaceMon, this->rootForm->GetHandle(), false);
		if (this->imgCopy == 0)
		{
			NEW_CLASS(this->imgCopy, Media::ImageCopy());
			this->imgCopy->SetThreadPriority(Sync::ThreadUtil::TP_HIGHEST);
		}
		this->surfaceMgr->SetFSMode(this->GetHMonitor(), this->rootForm->GetHandle(), false);
		if (this->debugWriter)
		{
			Text::StringBuilderUTF8 sb;
			RECT rc;
			GetClientRect((HWND)this->hwnd, &rc);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.left);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.right);
			sb.AppendC(UTF8STRC("FS: Draw rect1 = "));
			sb.AppendI32(rc.left);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(rc.top);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(rc.right);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(rc.bottom);
			this->debugWriter->WriteLine(sb.ToCString());
			this->rootForm->ToFullScn();
			this->currScnMode = SM_VFS;
			GetClientRect((HWND)this->hwnd, &rc);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.left);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.right);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("FS: Draw rect2 = "));
			sb.AppendI32(rc.left);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(rc.top);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(rc.right);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(rc.bottom);
			this->debugWriter->WriteLine(sb.ToCString());
			this->rootForm->SetFormState(UI::GUIForm::FS_MAXIMIZED);
			GetClientRect((HWND)this->hwnd, &rc);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.left);
			ClientToScreen((HWND)this->hwnd, (POINT*)&rc.right);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("FS: Draw rect3 = "));
			sb.AppendI32(rc.left);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(rc.top);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(rc.right);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendI32(rc.bottom);
			this->debugWriter->WriteLine(sb.ToCString());
		}
		else
		{
			this->rootForm->ToFullScn();
			this->currScnMode = SM_VFS;
			this->rootForm->SetFormState(UI::GUIForm::FS_MAXIMIZED);
		}

		this->CreateSurface();
		mutUsage.EndUse();
		this->EndUpdateSize();
		this->OnSizeChanged(true);
	}
	else
	{
		this->BeginUpdateSize();
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
		this->EndUpdateSize();
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
	((Media::DDrawManager*)this->surfaceMgr.Ptr())->ReleaseDD7(this->currMon);
	this->currMon = hMon;
	monInfo.cbSize = sizeof(monInfo);
	::GetMonitorInfoW((HMONITOR)this->currMon, &monInfo);
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

void UI::GUIDDrawControl::SetRotateType(Media::RotateType rotType)
{
	NN<Media::MonitorSurface> primarySurface;
	if (this->rotType != rotType)
	{
		this->rotType = rotType;
		if (this->primarySurface.SetTo(primarySurface))
		{
			primarySurface->info.rotateType = rotType;
		}
		OnResized(this);
	}
}

Media::RotateType UI::GUIDDrawControl::GetRotateType() const
{
	return this->rotType;
}

void UI::GUIDDrawControl::OnMouseWheel(Math::Coord2D<OSInt> scnPos, Int32 amount)
{
}

void UI::GUIDDrawControl::OnMouseMove(Math::Coord2D<OSInt> scnPos)
{
}

void UI::GUIDDrawControl::OnMouseDown(Math::Coord2D<OSInt> scnPos, MouseButton button)
{
}

void UI::GUIDDrawControl::OnMouseUp(Math::Coord2D<OSInt> scnPos, MouseButton button)
{
}

void UI::GUIDDrawControl::OnMouseDblClick(Math::Coord2D<OSInt> scnPos, MouseButton button)
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

void UI::GUIDDrawControl::OnGZoomBegin(Math::Coord2D<OSInt> scnPos, UInt64 dist)
{
}

void UI::GUIDDrawControl::OnGZoomStep(Math::Coord2D<OSInt> scnPos, UInt64 dist)
{
}

void UI::GUIDDrawControl::OnGZoomEnd(Math::Coord2D<OSInt> scnPos, UInt64 dist)
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
