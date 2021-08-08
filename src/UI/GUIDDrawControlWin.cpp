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
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 primarySurface;

	this->ReleaseSurface();
	this->ReleaseSubSurface();

	if (this->debugWriter)
	{
		this->debugWriter->WriteLine((const UTF8Char*)"Create Surface");
	}

	if (this->currScnMode == SM_FS)
	{
		LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(this->surfaceMon);
		this->surfaceMon = this->GetHMonitor();

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags        = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 2;
		HRESULT res = lpDD->CreateSurface( &ddsd, &primarySurface, NULL );
		if (res != DD_OK)
		{
			if (res == DDERR_UNSUPPORTEDMODE)
			{
				this->surfaceMgr->Reinit();
				lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(this->surfaceMon = this->GetHMonitor());
			}
		}
		this->pSurface = primarySurface;
		if (primarySurface)
		{
			ddsd.dwSize = sizeof(ddsd);
			primarySurface->GetSurfaceDesc(&ddsd);
			this->bitDepth = ddsd.ddpfPixelFormat.dwRGBBitCount;
			this->scnW = ddsd.dwWidth;
			this->scnH = ddsd.dwHeight;

			DDSCAPS2 ddscaps;
			ZeroMemory( &ddscaps, sizeof( ddscaps ) );
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
			
			primarySurface->GetAttachedSurface( &ddscaps, (LPDIRECTDRAWSURFACE7*)&this->surfaceBuff);
			this->surfaceNoRelease = true;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		MonitorHandle *hMon;
		LPDIRECTDRAW7 lpDD;
		if (this->currScnMode == SM_VFS)
		{
			lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(hMon = this->surfaceMon);
		}
		else if (this->currScnMode == SM_WINDOWED_DIR)
		{
			lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(hMon = this->surfaceMon);
		}
		else
		{
			lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(hMon = 0);
		}
		this->surfaceMon = hMon;

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags        = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		primarySurface = 0;
		HRESULT res = lpDD->CreateSurface( &ddsd, &primarySurface, NULL );
		if (res != DD_OK)
		{
			if (this->debugWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"Error in creating primary surface: 0x");
				sb.AppendHex32((UInt32)res);
				this->debugWriter->WriteLine(sb.ToString());
			}
			if (res == DDERR_NOCOOPERATIVELEVELSET)
			{
				lpDD->SetCooperativeLevel((HWND)this->hwnd, DDSCL_NORMAL);
				res = lpDD->CreateSurface( &ddsd, &primarySurface, NULL );
			}
			else if (res == DDERR_UNSUPPORTEDMODE)
			{
				this->surfaceMgr->Reinit();
				if (this->currScnMode == SM_VFS)
				{
					lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(hMon = this->GetHMonitor());
				}
				else if (this->currScnMode == SM_WINDOWED_DIR)
				{
					lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(hMon = this->GetHMonitor());
				}
				else
				{
					lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(hMon = 0);
				}
				this->surfaceMon = hMon;
			}
		}
		this->pSurface = primarySurface;
		if (primarySurface)
		{
			ddsd.dwSize = sizeof(ddsd);
			primarySurface->GetSurfaceDesc(&ddsd);

			if (this->debugWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"Primary surface desc: Size = ");
				sb.AppendU32(ddsd.dwWidth);
				sb.Append((const UTF8Char*)" x ");
				sb.AppendU32(ddsd.dwHeight);
				sb.Append((const UTF8Char*)", bpl = ");
				sb.AppendI32(ddsd.lPitch);
				sb.Append((const UTF8Char*)", hMon = ");
				sb.AppendOSInt((OSInt)hMon);
				this->debugWriter->WriteLine(sb.ToString());
			}
			this->bitDepth = ddsd.ddpfPixelFormat.dwRGBBitCount;
			this->scnW = ddsd.dwWidth;
			this->scnH = ddsd.dwHeight;

			CreateSubSurface();
			if (clipper)
				primarySurface->SetClipper((LPDIRECTDRAWCLIPPER)clipper);
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
	if (this->pSurface)
	{
		((LPDIRECTDRAWSURFACE7)this->pSurface)->Release();
		this->pSurface = 0;
	}
}

void UI::GUIDDrawControl::CreateSubSurface()
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(this->surfaceMon);
	DDSURFACEDESC2 ddsd;
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
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_DEPTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth = (UInt32)(rc.right - rc.left);
		ddsd.dwHeight = (UInt32)(rc.bottom - rc.top);
		ddsd.dwDepth = this->bitDepth;
		if (this->surfaceW != ddsd.dwWidth || this->surfaceH != ddsd.dwHeight)
		{
			if (this->debugWriter)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"(CreateSubSurface) Surface size changed to ");
				sb.AppendU32(ddsd.dwWidth);
				sb.Append((const UTF8Char*)" x ");
				sb.AppendU32(ddsd.dwHeight);
				this->debugWriter->WriteLine(sb.ToString());
			}
			this->surfaceW = ddsd.dwWidth;
			this->surfaceH = ddsd.dwHeight;
		}
		void *surface = 0;
		HRESULT res = lpDD->CreateSurface( &ddsd, (LPDIRECTDRAWSURFACE7*)&surface, NULL );
		this->surfaceBuff = surface;
		this->surfaceNoRelease = false;
	}
}

void UI::GUIDDrawControl::ReleaseSubSurface()
{
	if (this->surfaceNoRelease)
	{
		this->surfaceBuff = 0;
	}
	else
	{
		if (this->surfaceBuff)
		{
			((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Release();
			this->surfaceBuff = 0;
		}
	}
}

Bool UI::GUIDDrawControl::CreateClipper(void *lpDD)
{
	LPDIRECTDRAWCLIPPER pcClipper = (LPDIRECTDRAWCLIPPER)this->clipper;
	if (pcClipper)
	{
		pcClipper->Release();
		this->clipper = 0;
	}

	((LPDIRECTDRAW7)lpDD)->CreateClipper( 0, &pcClipper, NULL );
	if (pcClipper->SetHWnd( 0, (HWND)this->hwnd ) != DD_OK)
	{
		pcClipper->Release();
		pcClipper = 0;
	}
	else
	{
		this->clipper = pcClipper;
		return true;
	}
	return false;
}

UInt8 *UI::GUIDDrawControl::LockSurfaceBegin(UOSInt targetWidth, UOSInt targetHeight, UOSInt *bpl)
{
	RECT rcSrc;
	this->surfaceMut->Lock();
	if (targetWidth == this->surfaceW && targetHeight == this->surfaceH)
	{
		HRESULT hRes;
		DDSURFACEDESC2 ddsd2;
		MemClear(&ddsd2, sizeof(ddsd2));
		ddsd2.dwSize = sizeof(ddsd2);
		rcSrc.left = 0;
		rcSrc.top = 0;
		rcSrc.right = (LONG)this->surfaceW;
		rcSrc.bottom = (LONG)this->surfaceH;

		hRes = ((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Lock(&rcSrc, &ddsd2, DDLOCK_WAIT, 0);
		if (hRes == DDERR_SURFACELOST)
		{
			hRes = ((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Lock(&rcSrc, &ddsd2, DDLOCK_WAIT, 0);
		}
		if (hRes == DD_OK)
		{
			*bpl = (ULONG)ddsd2.lPitch;
			return (UInt8*)ddsd2.lpSurface;
		}
	}
	this->surfaceMut->Unlock();
	return 0;
}

void UI::GUIDDrawControl::LockSurfaceEnd()
{
	((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Unlock(0);
	this->surfaceMut->Unlock();
}

UInt8 *UI::GUIDDrawControl::LockSurfaceDirect(UOSInt *bpl)
{
	if (this->surfaceBuff == 0)
		return 0;
	RECT rcSrc;
	HRESULT hRes;
	DDSURFACEDESC2 ddsd2;
	MemClear(&ddsd2, sizeof(ddsd2));
	ddsd2.dwSize = sizeof(ddsd2);
	rcSrc.left = 0;
	rcSrc.top = 0;
	rcSrc.right = (LONG)this->surfaceW;
	rcSrc.bottom = (LONG)this->surfaceH;

	hRes = ((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Lock(&rcSrc, &ddsd2, DDLOCK_WAIT, 0);
	if (hRes == DDERR_SURFACELOST)
	{
		hRes = ((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Lock(&rcSrc, &ddsd2, DDLOCK_WAIT, 0);
	}
	if (hRes == DD_OK)
	{
		*bpl = (ULONG)ddsd2.lPitch;
		return (UInt8*)ddsd2.lpSurface;
	}
	return 0;
}

void UI::GUIDDrawControl::LockSurfaceUnlock()
{
	((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Unlock(0);
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
	this->clipper = 0;
	this->pSurface = 0;
	this->surfaceBuff = 0;
	this->imgCopy = 0;
	this->surfaceNoRelease = false;
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
	this->clipper = 0;
	this->surfaceMon = 0;
	NEW_CLASS(this->surfaceMgr, Media::DDrawManager(ui, colorSess));
	if (this->surfaceMgr->IsError())
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
/*		lpDD->SetCooperativeLevel((HWND)this->hwnd, DDSCL_NORMAL);
		LPDIRECTDRAWCLIPPER pcClipper;
		lpDD->CreateClipper( 0, &pcClipper, NULL );
		if (pcClipper->SetHWnd( 0, (HWND)this->hwnd ) != DD_OK)
		{
			pcClipper->Release();
			pcClipper = 0;
			return;
		}
		this->clipper = pcClipper;

		CreateSurface();*/
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
	if (clipper)
	{
		((LPDIRECTDRAWCLIPPER)clipper)->Release();
		clipper = 0;
	}
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
	RECT rcSrc;
	RECT rcDest;
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
		if (this->pSurface)
		{
			if (((LPDIRECTDRAWSURFACE7)this->pSurface)->Flip(0, 0) == DDERR_SURFACELOST)
			{
				((LPDIRECTDRAWSURFACE7)this->pSurface)->Restore();
				if (this->surfaceBuff)
				{
					((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Restore();
				}
			}
			//ValidateRect((HWND)this->hwnd, 0);
		}
	}
	else if (this->currScnMode == SM_VFS)
	{
		GetDrawingRect(&rcDest);
		rcSrc.left = 0;
		rcSrc.top = 0;
		rcSrc.right = (LONG)this->surfaceW;
		rcSrc.bottom = (LONG)this->surfaceH;
		if (this->pSurface && this->surfaceBuff)
		{
			DDSURFACEDESC2 ddsd;
			DDSURFACEDESC2 ddsd2;
			MemClear(&ddsd, sizeof(ddsd));
			MemClear(&ddsd2, sizeof(ddsd2));
			ddsd.dwSize = sizeof(ddsd);
			ddsd2.dwSize = sizeof(ddsd2);

			HRESULT hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
 			if (hRes == DDERR_SURFACELOST)
			{
				((LPDIRECTDRAWSURFACE7)this->pSurface)->Restore();
				hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
			}
			if (hRes == DD_OK)
			{
				hRes = ((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Lock(&rcSrc, &ddsd2, DDLOCK_WAIT, 0);
				if (hRes == DDERR_SURFACELOST)
				{
					hRes = ((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Lock(&rcSrc, &ddsd2, DDLOCK_WAIT, 0);
				}
				if (hRes == DD_OK)
				{
					this->surfaceMgr->WaitForVBlank(this->surfaceMon);
					if (this->bitDepth == 32)
					{
						this->imgCopy->Copy32((UInt8*)ddsd2.lpSurface, ddsd2.lPitch, (UInt8*)ddsd.lpSurface, ddsd.lPitch, this->surfaceW, this->surfaceH);
					}
					else if (this->bitDepth == 16)
					{
						this->imgCopy->Copy16((UInt8*)ddsd2.lpSurface, ddsd2.lPitch, (UInt8*)ddsd.lpSurface, ddsd.lPitch, this->surfaceW, this->surfaceH);
					}
 					((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Unlock(0);
				}
				else
				{
					if (this->debugWriter)
					{
						Text::StringBuilderUTF8 sb;
						sb.Append((const UTF8Char*)"DrawToScreen: Error in subsurface: 0x");
						sb.AppendHex32((UInt32)hRes);
						this->debugWriter->WriteLine(sb.ToString());
					}
					hRes = 0;
				}
				((LPDIRECTDRAWSURFACE7)this->pSurface)->Unlock(0);
			}
			else
			{
				if (this->debugWriter)
				{
					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"DrawToScreen: Error in primary surface: 0x");
					sb.AppendHex32((UInt32)hRes);
					this->debugWriter->WriteLine(sb.ToString());
				}
				hRes = 0;
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
			if (this->imgCopy == 0)
			{
				NEW_CLASS(this->imgCopy, Media::ImageCopy());
				this->imgCopy->SetThreadPriority(Sync::Thread::TP_HIGHEST);
			}

			GetDrawingRect(&rcDest);
			rcSrc.left = 0;
			rcSrc.top = 0;
			rcSrc.right = (LONG)this->surfaceW;
			rcSrc.bottom = (LONG)this->surfaceH;
			if (this->pSurface && this->surfaceBuff)
			{
				DDSURFACEDESC2 ddsd;
				DDSURFACEDESC2 ddsd2;
				MemClear(&ddsd, sizeof(ddsd));
				MemClear(&ddsd2, sizeof(ddsd2));
				ddsd.dwSize = sizeof(ddsd);
				ddsd2.dwSize = sizeof(ddsd2);

				rcDest.left -= (LONG)this->scnX;
				rcDest.top -= (LONG)this->scnY;
				rcDest.right -= (LONG)this->scnX;
				rcDest.bottom -= (LONG)this->scnY;

				HRESULT hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Lock(&rcDest, &ddsd, DDLOCK_WAIT, 0);
 				if (hRes == DDERR_SURFACELOST)
				{
					((LPDIRECTDRAWSURFACE7)this->pSurface)->Restore();
					hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Lock(&rcDest, &ddsd, DDLOCK_WAIT, 0);
				}
				if (hRes == DD_OK)
				{
					hRes = ((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Lock(&rcSrc, &ddsd2, DDLOCK_WAIT, 0);
					if (hRes == DDERR_SURFACELOST)
					{
						hRes = ((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Lock(&rcSrc, &ddsd2, DDLOCK_WAIT, 0);
					}
					if (hRes == DD_OK)
					{
						this->surfaceMgr->WaitForVBlank(this->surfaceMon);
						if (this->bitDepth == 32)
						{
							this->imgCopy->Copy32((UInt8*)ddsd2.lpSurface, ddsd2.lPitch, (UInt8*)ddsd.lpSurface, ddsd.lPitch, this->surfaceW, this->surfaceH);
						}
						else if (this->bitDepth == 16)
						{
							this->imgCopy->Copy16((UInt8*)ddsd2.lpSurface, ddsd2.lPitch, (UInt8*)ddsd.lpSurface, ddsd.lPitch, this->surfaceW, this->surfaceH);
						}
 						((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Unlock(0);
					}
					else
					{
						hRes = 0;
					}
					((LPDIRECTDRAWSURFACE7)this->pSurface)->Unlock(0);
				}
				else
				{
					hRes = 0;
				}
			}
		}
	}
	else
	{
		if (GetVisible())
		{
			GetDrawingRect(&rcDest);
			rcSrc.left = 0;
			rcSrc.top = 0;
			rcSrc.right = (LONG)this->surfaceW;
			rcSrc.bottom = (LONG)this->surfaceH;
			if (this->pSurface && this->surfaceBuff)
			{
				HRESULT hRes;
				this->surfaceMgr->WaitForVBlank(this->surfaceMon);
				if ((hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Blt(&rcDest, (LPDIRECTDRAWSURFACE7)this->surfaceBuff, &rcSrc, 0, 0)) == DDERR_SURFACELOST)
				{
					hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Restore();
					if (hRes == DDERR_WRONGMODE)
					{
						this->CreateSurface();
					}
					else
					{
						((LPDIRECTDRAWSURFACE7)this->pSurface)->Blt(&rcDest, (LPDIRECTDRAWSURFACE7)this->surfaceBuff, &rcSrc, 0, 0);
					}
				}
				else if (hRes != DD_OK)
				{
					if (this->debugWriter)
					{
						Text::StringBuilderUTF8 sb;
						sb.Append((const UTF8Char*)"DrawToScreen: Error in surface blt: 0x");
						sb.AppendHex32((UInt32)hRes);
						this->debugWriter->WriteLine(sb.ToString());
					}
					hRes = 0;
				}
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
	RECT rcSrc;
	RECT rcDest;
	if (this->imgCopy == 0)
	{
		NEW_CLASS(this->imgCopy, Media::ImageCopy());
		this->imgCopy->SetThreadPriority(Sync::Thread::TP_HIGHEST);
	}

	if (this->currScnMode == SM_VFS)
	{
		GetDrawingRect(&rcDest);
		rcSrc.left = 0;
		rcSrc.top = 0;
		rcSrc.right = (LONG)this->surfaceW;
		rcSrc.bottom = (LONG)this->surfaceH;
		if (this->pSurface)
		{
			DDSURFACEDESC2 ddsd;
			MemClear(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);

			HRESULT hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
 			if (hRes == DDERR_SURFACELOST)
			{
				hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Restore();
				if (hRes == DDERR_WRONGMODE)
				{
					this->CreateSurface();
					if (this->pSurface)
					{
						hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
					}
				}
				else
				{
					hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
				}
			}
			if (hRes == DD_OK)
			{
				this->surfaceMgr->WaitForVBlank(this->surfaceMon);
				if (this->bitDepth == 32)
				{
					this->imgCopy->Copy32(buff, lineAdd, ((UInt8*)ddsd.lpSurface) + ddsd.lPitch * tly + (tlx << 2), ddsd.lPitch, drawW, drawH);
				}
				else if (this->bitDepth == 16)
				{
					this->imgCopy->Copy16(buff, lineAdd, ((UInt8*)ddsd.lpSurface) + ddsd.lPitch * tly + (tlx << 2), ddsd.lPitch, drawW, drawH);
				}
				if (clearScn)
				{
					if (tly > 0)
					{
						MemClear(ddsd.lpSurface, (UOSInt)(ddsd.lPitch * tly));
					}
					if (tly + (OSInt)drawH < (OSInt)this->surfaceH)
					{
						MemClear(((UInt8*)ddsd.lpSurface) + ddsd.lPitch * (tly + (OSInt)drawH), (ULONG)ddsd.lPitch * (this->surfaceH - drawH - (UOSInt)tly));
					}
					if (tlx > 0)
					{
						UInt8 *dptr = ((UInt8*)ddsd.lpSurface) + ddsd.lPitch * tly;
						UOSInt byteSize = (UOSInt)tlx * (this->bitDepth >> 3);
						UOSInt i = drawH;
						while (i-- > 0)
						{
							MemClear(dptr, byteSize);
							dptr += ddsd.lPitch;
						}
					}
					if (tlx + (OSInt)drawW < (OSInt)this->surfaceW)
					{
						UInt8 *dptr = ((UInt8*)ddsd.lpSurface) + ddsd.lPitch * tly + (tlx + (OSInt)drawW) * (this->bitDepth >> 3);
						UOSInt byteSize = (this->surfaceW - (UOSInt)tlx - drawW) * (this->bitDepth >> 3);
						UOSInt i = drawH;
						while (i-- > 0)
						{
							MemClear(dptr, byteSize);
							dptr += ddsd.lPitch;
						}
					}
				}
				((LPDIRECTDRAWSURFACE7)this->pSurface)->Unlock(0);
			}
			else
			{
				hRes = 0;
			}
		}
	}
	else if (this->surfaceBuff)
	{
		DDSURFACEDESC2 ddsd;
		MemClear(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		rcSrc.left = 0;
		rcSrc.top = 0;
		rcSrc.right = (LONG)this->surfaceW;
		rcSrc.bottom = (LONG)this->surfaceH;

		if (this->currScnMode == SM_WINDOWED_DIR)
		{
			GetDrawingRect(&rcDest);
			HRESULT hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Lock(&rcDest, &ddsd, DDLOCK_WAIT, 0);
 			if (hRes == DDERR_SURFACELOST)
			{
				((LPDIRECTDRAWSURFACE7)this->pSurface)->Restore();
				hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Lock(&rcDest, &ddsd, DDLOCK_WAIT, 0);
			}
			if (hRes == DD_OK)
			{
				if (this->bitDepth == 32)
				{
					this->imgCopy->Copy32(buff, lineAdd, ((UInt8*)ddsd.lpSurface) + ddsd.lPitch * tly + (tlx << 2), ddsd.lPitch, drawW, drawH);
				}
				else if (this->bitDepth == 16)
				{
					this->imgCopy->Copy16(buff, lineAdd, ((UInt8*)ddsd.lpSurface) + ddsd.lPitch * tly + (tlx << 2), ddsd.lPitch, drawW, drawH);
				}
				((LPDIRECTDRAWSURFACE7)this->pSurface)->Unlock(0);
			}
		}
		else
		{
			HRESULT hRes = ((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
 			if (hRes == DDERR_SURFACELOST)
			{
				((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Restore();
				hRes = ((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Lock(&rcSrc, &ddsd, DDLOCK_WAIT, 0);
			}
			if (hRes == DD_OK)
			{
				if (this->bitDepth == 32)
				{
					this->imgCopy->Copy32(buff, lineAdd, ((UInt8*)ddsd.lpSurface) + ddsd.lPitch * tly + (tlx << 2), ddsd.lPitch, drawW, drawH);
				}
				else if (this->bitDepth == 16)
				{
					this->imgCopy->Copy16(buff, lineAdd, ((UInt8*)ddsd.lpSurface) + ddsd.lPitch * tly + (tlx << 2), ddsd.lPitch, drawW, drawH);
				}
				((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Unlock(0);
			}

			if (this->currScnMode == SM_FS)
			{
				if (this->pSurface)
				{
					if (((LPDIRECTDRAWSURFACE7)this->pSurface)->Flip(0, 0) == DDERR_SURFACELOST)
					{
						((LPDIRECTDRAWSURFACE7)this->pSurface)->Restore();
						if (this->surfaceBuff)
						{
							((LPDIRECTDRAWSURFACE7)this->surfaceBuff)->Restore();
						}
					}
					//ValidateRect((HWND)this->hwnd, 0);
				}
			}
			else if (GetVisible())
			{
				GetDrawingRect(&rcDest);
				if (this->pSurface && this->surfaceBuff)
				{
					HRESULT hRes;
					this->surfaceMgr->WaitForVBlank(this->surfaceMon);
					if ((hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Blt(&rcDest, (LPDIRECTDRAWSURFACE7)this->surfaceBuff, &rcSrc, 0, 0)) == DDERR_SURFACELOST)
					{
						hRes = ((LPDIRECTDRAWSURFACE7)this->pSurface)->Restore();
						if (hRes == DDERR_WRONGMODE)
						{
							this->CreateSurface();
						}
						else
						{
							((LPDIRECTDRAWSURFACE7)this->pSurface)->Blt(&rcDest, (LPDIRECTDRAWSURFACE7)this->surfaceBuff, &rcSrc, 0, 0);
						}
					}
					else if (hRes != DD_OK)
					{
						if (this->debugWriter)
						{
							Text::StringBuilderUTF8 sb;
							sb.Append((const UTF8Char*)"DrawFromBuff: Error in surface blt: 0x");
							sb.AppendHex32((UInt32)hRes);
							this->debugWriter->WriteLine(sb.ToString());
						}
					}
				}
			}
		}
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
		LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(this->GetHMonitor());
		this->currScnMode = SM_FS;
		DDSURFACEDESC2 ddsd;
		ddsd.dwSize = sizeof(ddsd);

		LPDIRECTDRAWCLIPPER pcClipper = (LPDIRECTDRAWCLIPPER)this->clipper;
		if (pcClipper)
		{
			pcClipper->Release();
			this->clipper = 0;
		}

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
		if (this->pSurface == 0)
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
		LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(this->GetHMonitor());
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

		this->CreateClipper(lpDD);
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
		LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->surfaceMgr->GetDD7(0);
		this->surfaceMgr->SetFSMode(0, this->rootForm->GetHandle(), false);

		this->CreateClipper(lpDD);
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
	this->surfaceMgr->ReleaseDD7(this->currMon);
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
