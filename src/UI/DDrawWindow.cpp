#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/DDrawUI.h"
#include "Sync/Thread.h"
//#include "Manage/HiResClock.h"
//#include "Text/MyString.h"
#include <windows.h>
#include <ddraw.h>
#ifdef LOWORD
#undef LOWORD
#endif
#ifdef HIWORD
#undef HIWORD
#endif
#define LOWORD(v) ((WORD)(v))
#define HIWORD(v) (((WORD*)&(v))[1])

UInt32 __stdcall UI::DDrawWindow::PresentThread(void *obj)
{
	UI::DDrawWindow *me = (UI::DDrawWindow *)obj;
	me->presentBusy = false;
	me->presentRunning = true;
	while (!me->presentToStop)
	{
		if (me->presentNeeded)
		{
			Int32 thisIndex;			
			me->presentBusy = true;
			thisIndex = me->presentIndex;
			me->presentNeeded = false;

			if (me->isFullScn)
			{
				if (((LPDIRECTDRAWSURFACE7)me->primarySurface)->Flip(0, 0) == DDERR_SURFACELOST)
				{
					((LPDIRECTDRAWSURFACE7)me->primarySurface)->Restore();
					((LPDIRECTDRAWSURFACE7)me->fsbackSurface)->Restore();
				}
				me->presentBusy = false;
			}
			else
			{
				RECT rc1;
				GetClientRect((HWND)me->hWnd, &rc1);
				ClientToScreen((HWND)me->hWnd, (POINT*)&rc1.left);
				ClientToScreen((HWND)me->hWnd, (POINT*)&rc1.right);
				LPDIRECTDRAWSURFACE7 surface;
				Int32 ret;
				if (thisIndex == 0)
				{
					surface = (LPDIRECTDRAWSURFACE7)me->secondarySurface1;
				}
				else
				{
					surface = (LPDIRECTDRAWSURFACE7)me->secondarySurface2;
				}
				if ((rc1.right - rc1.left) == me->secSurfW && (rc1.bottom - rc1.top) == me->secSurfH)
				{
					if (me->fastUI)
					{
						DDSURFACEDESC2 ddsd;
						DDSURFACEDESC2 ddsd2;
						ddsd2.dwSize = sizeof(ddsd2);
						ddsd.dwSize = sizeof(ddsd);
						ret = -1;
						if (surface->Lock(0, &ddsd, DDLOCK_WAIT, 0) == DD_OK)
						{
							if (((LPDIRECTDRAWSURFACE7)me->primarySurface)->Lock(0, &ddsd2, DDLOCK_WAIT, 0) == DD_OK)
							{
								UInt8 *srcPtr = (UInt8*)ddsd.lpSurface;
								ret = DD_OK;
								if (rc1.top < 0)
								{
									srcPtr += -rc1.top * ddsd.lPitch;
									rc1.top = 0;
								}
								if (rc1.left < 0)
								{
									srcPtr += -rc1.left << 2;
									rc1.left = 0;
								}
								if (rc1.right > (Int32)ddsd2.dwWidth)
								{
									rc1.right = ddsd2.dwWidth;
								}
								if (rc1.bottom > (Int32)ddsd2.dwHeight)
								{
									rc1.bottom = ddsd2.dwHeight;
								}
								me->imgCopy->Copy32(srcPtr, ddsd.lPitch, ((UInt8*)ddsd2.lpSurface) + rc1.top * ddsd2.lPitch + (rc1.left << 2), ddsd2.lPitch, rc1.right - rc1.left, rc1.bottom - rc1.top);
								((LPDIRECTDRAWSURFACE7)me->primarySurface)->Unlock(0);
							}
							surface->Unlock(0);
						}
					}
					else
					{
						RECT rc2;
						rc2.left = 0;
						rc2.top = 0;
						rc2.right = me->secSurfW;
						rc2.bottom = me->secSurfH;
						ret = ((LPDIRECTDRAWSURFACE7)me->primarySurface)->Blt(&rc1, surface, &rc2, 0, 0);
					}
				}
				else
				{
					DDSURFACEDESC2 ddsd;
					DDSURFACEDESC2 ddsd2;
					ddsd2.dwSize = sizeof(ddsd2);
					ddsd.dwSize = sizeof(ddsd);
					ret = -1;
					if (surface->Lock(0, &ddsd, DDLOCK_WAIT, 0) == DD_OK)
					{
						if (((LPDIRECTDRAWSURFACE7)me->primarySurface)->Lock(0, &ddsd2, DDLOCK_WAIT, 0) == DD_OK)
						{
							ret = DD_OK;
							me->resizer->Resize((UInt8*)ddsd.lpSurface, ddsd.lPitch, ddsd.dwWidth, ddsd.dwHeight, 0, 0, ((UInt8*)ddsd2.lpSurface) + ddsd2.lPitch * rc1.top + (rc1.left << 2), ddsd2.lPitch, rc1.right - rc1.left, rc1.bottom - rc1.top);
							((LPDIRECTDRAWSURFACE7)me->primarySurface)->Unlock(0);
						}
						surface->Unlock(0);
					}
				}
				if (ret == DD_OK)
				{
					ret = 0;
				}
				else
				{
					ret = 1;
				}
			}

			me->presentBusy = false;
		}

		me->presentEvt->Wait();
	}
	me->presentRunning = false;
	return 0;
}

Int32 UI::DDrawWindow::CreatePrimarySurface()
{
	DDSURFACEDESC2 ddsd;
	DestroyPrimarySurface();

	if (this->isFullScn)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
	    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;

		if (((LPDIRECTDRAW7)lpDD)->CreateSurface( &ddsd, (LPDIRECTDRAWSURFACE7*)&primarySurface, NULL ) != DD_OK)
		{
			return 1;
		}

	    DDSCAPS2 ddscaps;
		ZeroMemory( &ddscaps, sizeof( ddscaps ) );
	    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

		if (((LPDIRECTDRAWSURFACE7)primarySurface)->GetAttachedSurface( &ddscaps, (LPDIRECTDRAWSURFACE7*)&fsbackSurface ) != DD_OK)
		{
			return 2;
		}
	}
	else
	{
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags        = DDSD_CAPS;
	    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		((LPDIRECTDRAW7)lpDD)->CreateSurface( &ddsd, (LPDIRECTDRAWSURFACE7*)&primarySurface, NULL );

		if (pcClipper)
			((LPDIRECTDRAWSURFACE7)primarySurface)->SetClipper((LPDIRECTDRAWCLIPPER)pcClipper);
	}
	return 0;
}

void UI::DDrawWindow::DestroyPrimarySurface()
{
	if (primarySurface)
	{
		((LPDIRECTDRAWSURFACE7)primarySurface)->Release();
		primarySurface = 0;
	}
	fsbackSurface = 0;
}

Int32 UI::DDrawWindow::CreateSecondarySurface()
{
	DDSURFACEDESC2 ddsd;
	RECT rect;
	GetClientRect((HWND)hWnd, &rect);
	Int32 thisW = rect.right - rect.left;
	Int32 thisH = rect.bottom - rect.top;
	if (isFullScn || (thisW == this->secSurfW && thisH == this->secSurfH))
	{
		return 0;
	}

	DestroySecondarySurface();

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth        = this->secSurfW  = thisW;
	ddsd.dwHeight       = this->secSurfH  = thisH;
	((LPDIRECTDRAW7)lpDD)->CreateSurface( &ddsd, (LPDIRECTDRAWSURFACE7*)&secondarySurface1, NULL );
	((LPDIRECTDRAW7)lpDD)->CreateSurface( &ddsd, (LPDIRECTDRAWSURFACE7*)&secondarySurface2, NULL );

	return 0;
}

void UI::DDrawWindow::DestroySecondarySurface()
{
	if (this->secondarySurface1)
	{
		((LPDIRECTDRAWSURFACE7)secondarySurface1)->Release();
		secondarySurface1 = 0;
	}
	if (this->secondarySurface2)
	{
		((LPDIRECTDRAWSURFACE7)secondarySurface2)->Release();
		secondarySurface2 = 0;
	}
}

void UI::DDrawWindow::UpdateSurface()
{
	LPDIRECTDRAWSURFACE7 surface = 0;
	DDSURFACEDESC2 ddsd2;
	if (this->isFullScn)
	{
		surface = (LPDIRECTDRAWSURFACE7)this->fsbackSurface;
		if (surface)
		{
			ZeroMemory(&ddsd2, sizeof(ddsd2));
			ddsd2.dwSize = sizeof(ddsd2);
			if (surface->Lock(0, &ddsd2, DDLOCK_WAIT, 0) == DD_OK)
			{
				this->hdlr((UInt8*)ddsd2.lpSurface, ddsd2.dwWidth, ddsd2.dwHeight, ddsd2.lPitch, this->frameId++);
				surface->Unlock(0);
			}
		}
	}
	else
	{
		this->presentIndex = this->presentIndex ^ 1;
		if (this->presentIndex == 0)
		{
			surface = (LPDIRECTDRAWSURFACE7)this->secondarySurface1;
		}
		else
		{
			surface = (LPDIRECTDRAWSURFACE7)this->secondarySurface2;
		}
		if (surface)
		{
			ZeroMemory(&ddsd2, sizeof(ddsd2));
			ddsd2.dwSize = sizeof(ddsd2);
			if (surface->Lock(0, &ddsd2, DDLOCK_WAIT, 0) == DD_OK)
			{
				this->hdlr((UInt8*)ddsd2.lpSurface, ddsd2.dwWidth, ddsd2.dwHeight, ddsd2.lPitch, this->frameId++);
				surface->Unlock(0);
			}
		}
	}
}

UI::DDrawWindow::DDrawWindow(void *hWnd, UI::DDrawUI *ui, UI::DDrawWindow::UIMode mode, UI::DDrawWindow::UpdateMode upMode, UI::DDrawWindow::UpdateHdlr hdlr, Bool fastUI)
{
	this->hWnd = hWnd;
	this->ui = ui;
	this->mode = mode;
	this->hdlr = hdlr;
	this->upMode = upMode;
	this->quitOnClose = true;
	this->lpDD = ui->GetDD();
	this->isFullScn = false;
	this->fastUI = fastUI;
	this->isVisible = false;

	this->primarySurface = 0;
	this->fsbackSurface = 0;
	this->secondarySurface1 = 0;
	this->secondarySurface2 = 0;
	this->secSurfH = 0;
	this->secSurfW = 0;
	this->frameId = 0;
	this->mouseDownHdlr = 0;
	this->mouseUpHdlr = 0;
	this->mouseMoveHdlr = 0;
	this->mouseWheelHdlr = 0;
	this->keyDownHdlr = 0;
	this->keyUpHdlr = 0;
	this->szChgHdlr = 0;
	this->closeHdlr = 0;
	this->lastMouseBtn = 0;

	NEW_CLASS(resizer, Media::Resizer::LanczosResizerH8_8(3, 3, Media::AT_NO_ALPHA));
	NEW_CLASS(imgCopy, Media::ImageCopy());
	NEW_CLASS(presentEvt, Sync::Event(L"UI.DDrawWindow.presentEvt"));
	presentToStop = false;
	presentRunning = false;
	presentNeeded = true;
	presentBusy = false;
	presentIndex = 0;
	RECT rc;
	GetWindowRect((HWND)hWnd, &rc);
	this->initW = rc.right - rc.left;
	this->initH = rc.bottom - rc.top;
	GetClientRect((HWND)hWnd, &rc);
	this->targetW = rc.right - rc.left;
	this->targetH = rc.bottom - rc.top;
	SwitchFullscreen(false);
	CreateSecondarySurface();
	Sync::Thread::Create(PresentThread, this);
}

UI::DDrawWindow::~DDrawWindow()
{
	this->presentToStop = true;
	this->presentEvt->Set();
	while (presentRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	DEL_CLASS(presentEvt);
	DEL_CLASS(resizer);
	DEL_CLASS(imgCopy);
	DestroySecondarySurface();
	DestroyPrimarySurface();
}

void UI::DDrawWindow::Show()
{
	ShowWindow((HWND)hWnd, SW_SHOW);
	this->isVisible = true;
}

void UI::DDrawWindow::SwitchFullscreen(Bool fullScreen)
{
	DDSURFACEDESC2 ddsd;
	Int32 dwStyle;
	RECT rect;
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)this->lpDD;

	if (fullScreen)
	{
		if (!this->isFullScn)
		{
			GetWindowRect((HWND)hWnd, &rect);
			this->fsWinX = rect.left;
			this->fsWinY = rect.top;
			this->fsWinW = rect.right - rect.left;
			this->fsWinH = rect.bottom - rect.top;
		}
		dwStyle  = GetWindowLong( (HWND)hWnd, GWL_STYLE );
		dwStyle &= ~WS_SYSMENU;
	    dwStyle &= ~WS_OVERLAPPED;
		dwStyle &= ~WS_CAPTION;
		dwStyle &= ~WS_THICKFRAME;
		dwStyle &= ~WS_MINIMIZEBOX;
		SetWindowLong( (HWND)hWnd, GWL_STYLE, dwStyle );

		ZeroMemory(&ddsd, sizeof(ddsd));
	    ddsd.dwSize = sizeof( ddsd );
		lpDD->SetCooperativeLevel( (HWND)hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
		if (lpDD->GetDisplayMode(&ddsd) != DD_OK)
		{
			SwitchFullscreen(false);
			return;
		}

		this->scnWidth = ddsd.dwWidth;
		this->scnHeight = ddsd.dwHeight;
		this->scnRate = ddsd.dwRefreshRate;
		HRESULT hr = lpDD->SetDisplayMode(this->scnWidth, this->scnHeight, 32, 0, 0);
		if (hr != DD_OK)
		{
			SwitchFullscreen(false);
			return;
		}
		this->isFullScn = true;
		
		SetForegroundWindow((HWND)hWnd);
		CreatePrimarySurface();
//		CreateSecondarySurface();
//		Redraw();
	}
	else
	{
		Bool needSetSize = this->isFullScn;
		this->isFullScn = false;
		lpDD->RestoreDisplayMode();
		lpDD->SetCooperativeLevel( (HWND)hWnd, DDSCL_NORMAL );

		ZeroMemory(&ddsd, sizeof(ddsd));
	    ddsd.dwSize = sizeof( ddsd );
		if (lpDD->GetDisplayMode(&ddsd) == DD_OK)
		{
			this->scnWidth = ddsd.dwWidth;
			this->scnHeight = ddsd.dwHeight;
			this->scnRate = ddsd.dwRefreshRate;
		}

		dwStyle  = GetWindowLong( (HWND)hWnd, GWL_STYLE );
		dwStyle &= ~WS_POPUP;
	    dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;
		SetWindowLong( (HWND)hWnd, GWL_STYLE, dwStyle );
		if (needSetSize)
		{
			MoveWindow((HWND)hWnd, fsWinX, fsWinY, fsWinW, fsWinH, TRUE);
		}
		
		lpDD->CreateClipper( 0, (LPDIRECTDRAWCLIPPER*)&pcClipper, NULL );
		if (((LPDIRECTDRAWCLIPPER)pcClipper)->SetHWnd( 0, (HWND)hWnd ) != DD_OK)
		{
			/////////////////////////////////////
			MessageBoxW(0, L"Error in creating clipper", L"Error", MB_OK);
		}
		CreatePrimarySurface();
	}
}

OSInt UI::DDrawWindow::WndProc(UInt32 msg, UInt32 wParam, Int32 lParam)
{
	switch (msg)
	{
	case WM_PAINT:
		if (this->isFullScn)
		{
			UpdateSurface();
			this->presentNeeded = true;
			ValidateRect((HWND)this->hWnd, 0);
			this->presentEvt->Set();
		}
		else
		{
			RECT rect;
			GetClientRect((HWND)this->hWnd, &rect);
			UpdateSurface();
			this->presentNeeded = true;
			ValidateRect((HWND)this->hWnd, &rect);
			this->presentEvt->Set();
		}
		break;
	case WM_CLOSE:
		DestroyWindow((HWND)hWnd);
		break;
	case WM_DESTROY:
		this->isVisible = false;
		if (this->closeHdlr)
		{
			this->closeHdlr();
		}
		if (this->quitOnClose)
		{
			ui->ExitUI();
		}
		ui->WindowClosed(this);
		return DefWindowProc((HWND)hWnd, msg, wParam, lParam);
	case WM_SYSCOMMAND:
		if (this->isFullScn)
		{
			switch(wParam & 0xfff0)
			{
			case SC_SIZE:
			case SC_MAXIMIZE:
				return 0;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		if ((this->lastMouseBtn & 1) == 0)
		{
			this->lastMouseBtn |= 1;
			if (this->mouseDownHdlr)
			{
				this->mouseDownHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_LEFT);
			}
		}
		break;
	case WM_LBUTTONUP:
		if (this->lastMouseBtn & 1)
		{
			this->lastMouseBtn &= ~1;
			if (this->mouseUpHdlr)
			{
				this->mouseUpHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_LEFT);
			}
		}
		break;
	case WM_RBUTTONDOWN:
		if ((this->lastMouseBtn & 2) == 0)
		{
			this->lastMouseBtn |= 2;
			if (this->mouseDownHdlr)
			{
				this->mouseDownHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_RIGHT);
			}
		}
		break;
	case WM_RBUTTONUP:
		if (this->lastMouseBtn & 2)
		{
			this->lastMouseBtn &= ~2;
			if (this->mouseUpHdlr)
			{
				this->mouseUpHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_RIGHT);
			}
		}
		break;
	case WM_MBUTTONDOWN:
		if ((this->lastMouseBtn & 4) == 0)
		{
			this->lastMouseBtn |= 4;
			if (this->mouseDownHdlr)
			{
				this->mouseDownHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_MIDDLE);
			}
		}
		break;
	case WM_MBUTTONUP:
		if (this->lastMouseBtn & 4)
		{
			this->lastMouseBtn &= ~4;
			if (this->mouseUpHdlr)
			{
				this->mouseUpHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_MIDDLE);
			}
		}
		break;
	case WM_XBUTTONDOWN:
		if (wParam & 0x10000)
		{
			if ((this->lastMouseBtn & 0x10) == 0)
			{
				this->lastMouseBtn |= 0x10;
				if (this->mouseDownHdlr)
				{
					this->mouseDownHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_X1);
				}
			}
		}
		else if (wParam & 0x20000)
		{
			if ((this->lastMouseBtn & 0x20) == 0)
			{
				this->lastMouseBtn |= 0x20;
				if (this->mouseDownHdlr)
				{
					this->mouseDownHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_X2);
				}
			}
		}
		break;
	case WM_XBUTTONUP:
		if (wParam & 0x10000)
		{
			if (this->lastMouseBtn & 0x10)
			{
				this->lastMouseBtn &= ~0x10;
				if (this->mouseUpHdlr)
				{
					this->mouseUpHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_X1);
				}
			}
		}
		else if (wParam & 0x20000)
		{
			if (this->lastMouseBtn & 0x20)
			{
				this->lastMouseBtn &= ~0x20;
				if (this->mouseUpHdlr)
				{
					this->mouseUpHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_X2);
				}
			}
		}
		break;
	case WM_MOUSEMOVE:
		if (this->lastMouseBtn != (Int32)wParam)
		{
			if ((wParam & 1) == 0 && (this->lastMouseBtn & 1) == 1)
			{
				if (this->mouseUpHdlr)
					this->mouseUpHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_LEFT);
				this->lastMouseBtn &= ~1;
			}
			if ((wParam & 2) == 0 && (this->lastMouseBtn & 2) == 2)
			{
				if (this->mouseUpHdlr)
					this->mouseUpHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_RIGHT);
				this->lastMouseBtn &= ~2;
			}
			if ((wParam & 0x10) == 0 && (this->lastMouseBtn & 0x10) == 0x10)
			{
				if (this->mouseUpHdlr)
					this->mouseUpHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_MIDDLE);
				this->lastMouseBtn &= ~0x10;
			}
			if ((wParam & 0x20) == 0 && (this->lastMouseBtn & 0x20) == 0x20)
			{
				if (this->mouseUpHdlr)
					this->mouseUpHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_X1);
				this->lastMouseBtn &= ~0x20;
			}
			if ((wParam & 0x40) == 0 && (this->lastMouseBtn & 0x40) == 0x40)
			{
				if (this->mouseUpHdlr)
					this->mouseUpHdlr(LOWORD(lParam), HIWORD(lParam), MBTN_X2);
				this->lastMouseBtn &= ~0x40;
			}
		}

		if (this->mouseMoveHdlr)
		{
			this->mouseMoveHdlr(LOWORD(lParam), HIWORD(lParam), (MouseButton)((wParam & 3) | ((wParam & 0x70) >> 2)));
		}
		break;
	case WM_MOUSEWHEEL:
		if (this->mouseWheelHdlr)
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			ScreenToClient((HWND)this->hWnd, &pt);
			this->mouseWheelHdlr(pt.x, pt.y, (Int16)HIWORD(wParam));
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (this->keyDownHdlr)
		{
			this->keyDownHdlr(wParam);
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (this->keyUpHdlr)
		{
			this->keyUpHdlr(wParam);
		}
		break;
	case WM_SIZE:
		if (this->mode == UI::DDrawWindow::UIM_DYNAMIC)
		{
			CreateSecondarySurface();
			if (this->szChgHdlr)
			{
				this->szChgHdlr(this->secSurfW, this->secSurfH);
			}
		}
		break;
	case WM_SIZING:
		{
			RECT *rcNewSize = (RECT*)(OSInt)lParam;
			if (this->mode == UI::DDrawWindow::UIM_FIXSIZE)
			{
				if (wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT)
				{
					rcNewSize->top = rcNewSize->bottom - initH;
				}
				else if (wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT)
				{
					rcNewSize->bottom = rcNewSize->top + initH;
				}
				if (wParam == WMSZ_TOPLEFT || wParam == WMSZ_LEFT || wParam == WMSZ_BOTTOMLEFT)
				{
					rcNewSize->left = rcNewSize->right - initW;
				}
				else if (wParam == WMSZ_TOPRIGHT || wParam == WMSZ_RIGHT || wParam == WMSZ_BOTTOMRIGHT)
				{
					rcNewSize->right = rcNewSize->left + initW;
				}
				return TRUE;
			}
			else if (this->mode == UI::DDrawWindow::UIM_ARATIO)
			{
				Int32 thisW = rcNewSize->right - rcNewSize->left - initW + targetW;
				Int32 thisH = rcNewSize->bottom - rcNewSize->top - initH + targetH;
				if (wParam == WMSZ_LEFT || wParam == WMSZ_RIGHT)
				{
					thisH = MulDiv(thisW, targetH, targetW);
				}
				else if (wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM)
				{
					thisW = MulDiv(thisH, targetW, targetH);
				}
				else if (thisW * targetH > thisH * targetW)
				{
					thisH = MulDiv(thisW, targetH, targetW);
				}
				else
				{
					thisW = MulDiv(thisH, targetW, targetH);
				}
				thisW += initW - targetW;
				thisH += initH - targetH;
				if (wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT)
				{
					rcNewSize->top = rcNewSize->bottom - thisH;
				}
				else
				{
					rcNewSize->bottom = rcNewSize->top + thisH;
				}
				if (wParam == WMSZ_TOPLEFT || wParam == WMSZ_LEFT || wParam == WMSZ_BOTTOMLEFT)
				{
					rcNewSize->left = rcNewSize->right - thisW;
				}
				else
				{
					rcNewSize->right = rcNewSize->left + thisW;
				}
				return TRUE;
			}
		}
		break;
	}
	return DefWindowProc((HWND)hWnd, msg, wParam, lParam); 
}

void UI::DDrawWindow::SetQuitOnClose(Bool quitOnClose)
{
	this->quitOnClose = quitOnClose;
}

void *UI::DDrawWindow::GetHWnd()
{
	return this->hWnd;
}

void UI::DDrawWindow::Redraw()
{
	if (!this->isVisible)
		return;
	UpdateSurface();
	this->presentNeeded = true;
	ValidateRect((HWND)this->hWnd, 0);
	this->presentEvt->Set();
}

void UI::DDrawWindow::SetMouseDownHdlr(MouseHdlr hdlr)
{
	this->mouseDownHdlr = hdlr;
}

void UI::DDrawWindow::SetMouseUpHdlr(MouseHdlr hdlr)
{
	this->mouseUpHdlr = hdlr;
}

void UI::DDrawWindow::SetMouseMoveHdlr(MouseHdlr hdlr)
{
	this->mouseMoveHdlr = hdlr;
}

void UI::DDrawWindow::SetMouseWheelHdlr(MouseWheelHdlr hdlr)
{
	this->mouseWheelHdlr = hdlr;
}

void UI::DDrawWindow::SetKeyDownHdlr(KeyInputHdlr hdlr)
{
	this->keyDownHdlr = hdlr;
}

void UI::DDrawWindow::SetKeyUpHdlr(KeyInputHdlr hdlr)
{
	this->keyUpHdlr = hdlr;
}

void UI::DDrawWindow::SetSizeChangeHdlr(SizeChgHdlr hdlr)
{
	this->szChgHdlr = hdlr;
}

void UI::DDrawWindow::SetCloseHdlr(ActionHdlr hdlr)
{
	this->closeHdlr = hdlr;
}
