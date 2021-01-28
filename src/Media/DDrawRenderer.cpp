#include "Stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Media/FrameInfo.h"
#include "Media/DDrawRenderer.h"

#define NFRAMEAVG 16
#define DDDELETE(obj) if (obj) { obj->Release(); obj = 0;}

void Media::DDrawRenderer::DrawOsd(void *drawSurface, WChar *osd)
{
	LPDIRECTDRAWSURFACE7 surface = (LPDIRECTDRAWSURFACE7)drawSurface;
	WChar *osdEnd = osd;
	while (*osdEnd++);

	if (surface == 0)
		return;
	if (osd == 0)
		return;
	if (*osd == 0)
		return;
	Int32 charWidth[256];

	BITMAPINFOHEADER bmpInfo;
	bmpInfo.biSize = sizeof(bmpInfo);
	HDC hdc = GetDC((HWND)hwnd);
	GetCharWidth(hdc, 0, 255, (int*)charWidth);
	long i = 0;

	long j = osdEnd - osd - 1;
	WChar *ptr = osd;
	while (j--)
	{
		if (*ptr > 127)
		{
			ptr++;
			i += 16;
		}
		else
			i += charWidth[*ptr++];
	}

	bmpInfo.biWidth = i;
	bmpInfo.biHeight = 18;
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 32;
	bmpInfo.biCompression = BI_RGB;
	bmpInfo.biSizeImage = 0;
	bmpInfo.biXPelsPerMeter = 72;
	bmpInfo.biYPelsPerMeter = 72;
	bmpInfo.biClrImportant = 0;
	bmpInfo.biClrUsed = 0;

	void *bmpP;
	MemLock();
	HDC hdcBmp = CreateCompatibleDC(hdc);
	HBITMAP hbmp = CreateDIBSection(hdcBmp, (BITMAPINFO*)&bmpInfo, 0, &bmpP, 0, 0);
	SelectObject(hdcBmp, hbmp);
	SetTextColor(hdcBmp, 0x00ff00);
	SetBkColor(hdcBmp, 0);
	TextOutW(hdcBmp, 0, 0, (LPCWSTR)osd, osdEnd - osd - 1);
	DeleteDC(hdcBmp);
	MemUnlock();

	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	HRESULT hr;
	hr= surface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
	if (hr == 0)
	{
		_asm
		{
			mov edi, ddsd.lpSurface
			mov esi, bmpP
			mov eax, bmpInfo.biHeight

			mov ebx, ddsd.lPitch
			//shl ebx,2
			mul ebx
			add edi, eax
			mov edx,bmpInfo.biHeight
			cld
osdlop:
			sub edi, ebx
			push edi
			mov ecx,bmpInfo.biWidth
			rep movsd
			pop edi
			dec edx
			jnz osdlop
		}
		surface->Unlock(0);
	}
	DeleteObject(hbmp);
	ReleaseDC((HWND)hwnd, hdc);
}

Int32 Media::DDrawRenderer::CreateSurface(Int32 fullScreen)
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddObj;
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 primarySurface;

	if (this->pSurface)
	{
		((LPDIRECTDRAWSURFACE7)this->pSurface)->Release();
		this->pSurface = 0;
	}
	if (this->vbSurface)
	{
		((LPDIRECTDRAWSURFACE7)this->vbSurface)->Release();
		this->vbSurface = 0;
	}
	bbSurface = 0;
	if (fullScreen)
	{

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
	    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;

		if (lpDD->CreateSurface( &ddsd, &primarySurface, NULL ) != DD_OK)
		{
			return 1;
		}
		pSurface = primarySurface;

	    DDSCAPS2 ddscaps;
		ZeroMemory( &ddscaps, sizeof( ddscaps ) );
	    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

		if (primarySurface->GetAttachedSurface( &ddscaps, (LPDIRECTDRAWSURFACE7*)&bbSurface ) != DD_OK)
		{
			return 1;
		}
	}
	else
	{
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags        = DDSD_CAPS;
	    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		lpDD->CreateSurface( &ddsd, &primarySurface, NULL );
		
		pSurface = primarySurface;
		if (primarySurface)
		{
			primarySurface->GetSurfaceDesc(&ddsd);
			ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			lpDD->CreateSurface( &ddsd, (LPDIRECTDRAWSURFACE7*)&vbSurface, NULL );
		}
		
	}
	if (clipper)
		primarySurface->SetClipper((LPDIRECTDRAWCLIPPER)clipper);

	return 0;
}

UInt32 Media::DDrawRenderer::ScnUpdater(void *lpVoid)
{
	Media::DDrawRenderer *me = (Media::DDrawRenderer*)lpVoid;
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)me->ddObj;
	RECT rcClient;
	me->scnUpdStat = 1;
	LARGE_INTEGER liUpdEnd;
	LARGE_INTEGER liUpdFreq;
	
	while ((me->scnUpdStat & 2) == 0)
	{
		me->updEvt->Wait(1000);

		if(me->active == 1)
		{
			if (me->updateFlag & 2)
			{
				// Erase the background
				DDBLTFX ddbltfx;
				ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
				ddbltfx.dwSize      = sizeof(ddbltfx);
				ddbltfx.dwFillColor = 0;

				if (me->fs)
				{
					if (me->bbSurface)
					{
						((LPDIRECTDRAWSURFACE7)me->bbSurface)->Blt( NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );
					}
				}
				else
				{
					if (me->pSurface)
					{
						me->GetDrawingRect(&rcClient);
						ClientToScreen((HWND)me->hwnd, (POINT*)&rcClient.left);
						ClientToScreen((HWND)me->hwnd, (POINT*)&rcClient.right);
						((LPDIRECTDRAWSURFACE7)me->pSurface)->Blt( &rcClient, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );
					}
				}

				me->updateFlag &= ~2;
				if (me->updateFlag == 0)
					if (me->busyEvent)
						me->busyEvent->Set();
			}
			if (me->updateFlag && me->ddLock == 0)
			{
				InterlockedIncrement((LONG*)&me->ddCount);
				if (me->fs)
				{
					if (me->bbSurface)
					{
						if (me->waitVBlank)
						{
							lpDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN ,0);
						}

						if (((LPDIRECTDRAWSURFACE7)me->pSurface)->Flip(NULL, 0) == DDERR_SURFACELOST)
						{
							((LPDIRECTDRAWSURFACE7)me->pSurface)->Restore();
							((LPDIRECTDRAWSURFACE7)me->bbSurface)->Restore();
						}
					}

				}
				else
				{
					RECT rcDest;
					me->GetDrawingRect(&rcClient);

					ClientToScreen((HWND)me->hwnd, (POINT*)&rcClient);
					ClientToScreen((HWND)me->hwnd, (POINT*)&rcClient.right);

					rcDest.left = rcClient.left;
					rcDest.top = rcClient.top;
					rcDest.right = rcClient.right;
					rcDest.bottom = rcClient.bottom;
					if (me->waitVBlank)
					{
						lpDD->WaitForVerticalBlank(DDWAITVB_BLOCKEND,0);
					}
					((LPDIRECTDRAWSURFACE7)me->pSurface)->Blt(&rcDest, (LPDIRECTDRAWSURFACE7)me->vbSurface, &rcDest, 0, 0);
				}
				QueryPerformanceCounter(&liUpdEnd);
				QueryPerformanceFrequency(&liUpdFreq);
				me->updDelayTot -= me->updDelayAll[me->updDelayInd];
				me->updDelayAll[me->updDelayInd] = (Int32)((liUpdEnd.QuadPart - me->updStart) * 1000 / liUpdFreq.QuadPart);
				me->updDelayTot += me->updDelayAll[me->updDelayInd];
				me->updDelayInd = (me->updDelayInd + 1) & 15;
				me->updDelayAvg = me->updDelayTot >> 4;

				me->updateFlag = 0;
				if (me->busyEvent)
					me->busyEvent->Set();
				InterlockedDecrement((LONG*)&me->ddCount);
			}
		}
		else
		{
			me->updateFlag = 0;
			if (me->busyEvent)
				me->busyEvent->Set();
		}
	}
	me->scnUpdStat = 0;
	return 0;
}

Media::DDrawRenderer::DDrawRenderer(void *hwnd)
{
	UInt32 threadId;
	Int32 i;
	this->hwnd = hwnd;
	this->framePresented = 0;
	this->scnUpdStat = 0;
	this->active = 1;
	this->waitVBlank = 0;
	this->updDelayAll = (Int32*)MemAlloc(NFRAMEAVG * sizeof(Int32));
	i = NFRAMEAVG;
	while (i-- > 0)
	{
		this->updDelayAll[i] = 0;
	}
	this->updDelayTot = 0;
	this->updDelayAvg = 0;
	this->updDelayInd = 0;

	this->ddObj = 0;
	this->pSurface = 0;
	this->ddLock = 0;
	this->ddCount = 0;

	LPDIRECTDRAW7 lpDD;
	if (DirectDrawCreateEx( NULL, (VOID**)&lpDD, IID_IDirectDraw7, NULL ) != DD_OK )
	{
	}
	else
	{
		NEW_CLASS(updEvt, Event());
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ScnUpdater, (void*)this, 0, (DWORD*)&threadId);
		this->ddObj = lpDD;
	}
}

Media::DDrawRenderer::~DDrawRenderer()
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddObj;
	scnUpdStat = scnUpdStat | 2;
	updEvt->Set();
	while (scnUpdStat)
	{
		Sleep(1);
	}
	DEL_CLASS(updEvt);
	MemFree(this->updDelayAll);

	if (lpDD)
	{
		lpDD->RestoreDisplayMode();
		lpDD->SetCooperativeLevel((HWND)hwnd, DDSCL_NORMAL);
	}
	if (pSurface)
	{
		((LPDIRECTDRAWSURFACE7)pSurface)->Release();
		pSurface = 0;
	}
	if (clipper)
	{
		((LPDIRECTDRAWCLIPPER)clipper)->Release();
		clipper = 0;
	}
	if (lpDD)
	{
		lpDD->Release();
		ddObj = 0;
	}
}

Int32 Media::DDrawRenderer::PresentNext(FrameInfo *frameInfo, void *frameData, WChar *osd)
{
	LARGE_INTEGER liCnt;
	if (ddLock == 0)
	{
		InterlockedIncrement((LONG*)&ddCount);
		
		framePresented++;
		
		QueryPerformanceCounter(&liCnt);
		updStart = liCnt.QuadPart;
		RECT rcArea;
		LPDIRECTDRAWSURFACE7 surface;
		DDSURFACEDESC2 ddsd;
		if (fs)
		{
			surface = (LPDIRECTDRAWSURFACE7)bbSurface;
			rcArea.left = 0;
			rcArea.top = 0;
			rcArea.right = this->scnWidth;
			rcArea.bottom = this->scnHeight;

		}
		else
		{
			surface = (LPDIRECTDRAWSURFACE7)vbSurface;
			this->GetDrawingRect(&rcArea);
			if (rcArea.right > scnWidth)
				rcArea.right = scnWidth;
			if (rcArea.bottom > scnHeight)
				rcArea.bottom = scnHeight;
		}

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		if (DD_OK == surface->Lock(&rcArea, &ddsd, DDLOCK_WAIT, 0))
		{
			UInt8 *sptr = (UInt8*)frameData;
			Int32 sbpl = frameInfo->width << 2;
			UInt8 *dptr = (UInt8*)ddsd.lpSurface;
			Int32 dbpl = ddsd.lPitch;
			Int32 dwidth;
			Int32 width = dwidth = rcArea.right - rcArea.left;
			Int32 height = rcArea.bottom - rcArea.top;
			Int32 vBefore = 0;
			Int32 vAfter = 0;
			Int32 hBefore = 0;
			Int32 hAfter = 0;
			if (height > frameInfo->height)
			{
				vBefore = height - frameInfo->height;
				vAfter = vBefore >> 1;
				vBefore -= vAfter;
			}
			else if (height < frameInfo->height)
			{
				sptr = sptr + sbpl * ((frameInfo->height - height) >> 1);
			}

			if (width > frameInfo->width)
			{
				hBefore = width - frameInfo->width;
				hAfter = hBefore >> 1;
				hBefore -= hAfter;
			}
			else if (width < frameInfo->height)
			{
				sptr = sptr + (((frameInfo->width - width) >> 1) << 2);
			}

			_asm
			{
				cld
				mov esi,sptr
				mov edi,dptr
				mov eax,0
				mov ebx,vBefore
				cmp ebx,0
				jz pnlop2
pnlop:
				push edi
				mov ecx,dwidth
				rep stosd
				pop edi
				add edi,dbpl
				dec ebx
				jnz pnlop
pnlop2:
				mov ebx,height
				cmp hBefore,0
				jnz pnlop3
				cmp hAfter,0
				jnz pnlop3

pnlop4:
				push edi
				push esi
				mov ecx,width
				rep movsd
				pop esi
				pop edi
				add esi,sbpl
				add edi,dbpl
				dec ebx
				jnz pnlop4
				jmp pnlop5

pnlop3:
				push edi
				push esi
				mov ecx,hBefore
				rep stosd
				mov ecx,width
				rep movsd
				mov ecx,hAfter
				rep stosd
				pop esi
				pop edi
				add esi,sbpl
				add edi,dbpl
				dec ebx
				jnz pnlop3
pnlop5:
				mov ebx,vAfter
				cmp ebx,0
				jz pnlop6
pnlop7:
				push edi
				mov ecx,dwidth
				rep stosd
				pop edi
				add edi,dbpl
				dec ebx
				jnz pnlop7
pnlop6:
			}

			surface->Unlock(0);
			//DrawOsd(surface, );
		}

		this->updateFlag = 1;
		this->updEvt->Set();

		InterlockedDecrement((LONG*)&ddCount);
	}
	return 0;
}

void Media::DDrawRenderer::InitDisplay(Bool fullScreen, Int32 preferW, Int32 preferH)
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddObj;
	DDSURFACEDESC2 ddsd;
	long movW;
	long movH;
	HRESULT hr;

	ddLock = 1;
	while (ddCount > 0)
		Sleep(1);

	long dwStyle;
	if (this->clipper)
	{
		((LPDIRECTDRAWCLIPPER)this->clipper)->Release();
		this->clipper = 0;
	}

	if (fullScreen)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));
	    ddsd.dwSize = sizeof( ddsd );
		dwStyle  = GetWindowStyle( (HWND)this->hwnd );
		dwStyle &= ~WS_SYSMENU;
	    dwStyle &= ~WS_OVERLAPPED;
		dwStyle &= ~WS_CAPTION;
		dwStyle &= ~WS_THICKFRAME;
		dwStyle &= ~WS_MINIMIZEBOX;
		SetWindowLong( (HWND)this->hwnd, GWL_STYLE, dwStyle );
		SetMenu((HWND)this->hwnd, 0);
		lpDD->SetCooperativeLevel( (HWND)this->hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
		if (lpDD->GetDisplayMode(&ddsd) != DD_OK)
		{
			ddLock = 0;
			InitDisplay(0, preferW, preferH);
			return;
		}

		long scnChged = 0;

//		if (forceFullSize)
//		{
		hr = ((LPDIRECTDRAW7)lpDD)->SetDisplayMode(scnWidth = ddsd.dwWidth, scnHeight = ddsd.dwHeight, 32, 0, 0);
		if (hr == DD_OK) scnChged = 1;
//		}

		if (!scnChged && movW <= 640)
		{
			hr = lpDD->SetDisplayMode(scnWidth = 640, scnHeight = 480, 32, 120, 0);
			if (hr != DD_OK)
			{
				if (DD_OK != (hr = lpDD->SetDisplayMode(scnWidth = 640, scnHeight = 480, 32, 60, 0)))
					hr = lpDD->SetDisplayMode(scnWidth = ddsd.dwWidth, scnHeight = ddsd.dwHeight, 32, 0, 0);
			}
			if (hr == DD_OK) scnChged = 1;
		}
		if (!scnChged && movW == 848 && movH <= 480)
		{
			hr = lpDD->SetDisplayMode(scnWidth = 848, scnHeight = 480, 32, 120, 0);
			if (hr != DD_OK)
			{
				hr = lpDD->SetDisplayMode(scnWidth = 848, scnHeight = 480, 32, 60, 0);
			}
			if (hr == DD_OK) scnChged = 1;
		}
		if (!scnChged && movW <= 848 && movH <= 600)
		{
			hr = lpDD->SetDisplayMode(scnWidth = 800, scnHeight = 600, 32, 120, 0);
			if (hr != DD_OK)
			{
				if (DD_OK != (hr = lpDD->SetDisplayMode(scnWidth = 800, scnHeight = 600, 32, 60, 0)))
					hr = lpDD->SetDisplayMode(scnWidth = ddsd.dwWidth, scnHeight = ddsd.dwHeight, 32, 0, 0);
			}
			if (hr == DD_OK) scnChged = 1;
		}
		if (!scnChged && movW <= 800)
		{
			hr = lpDD->SetDisplayMode(scnWidth = 800, scnHeight = 600, 32, 120, 0);
			if (hr != DD_OK)
			{
				if (DD_OK != (hr = lpDD->SetDisplayMode(scnWidth = 800, scnHeight = 600, 32, 60, 0)))
					hr = lpDD->SetDisplayMode(scnWidth = ddsd.dwWidth, scnHeight = ddsd.dwHeight, 32, 0, 0);
			}
			if (hr == DD_OK) scnChged = 1;
		}
		if (!scnChged)
			hr = lpDD->SetDisplayMode(scnWidth = ddsd.dwWidth, scnHeight = ddsd.dwHeight, 32, 0, 0);

		if (hr != DD_OK)
		{
			ddLock = 0;
			InitDisplay(0, preferW, preferH);
			return;
		}

		ZeroMemory(&ddsd, sizeof(ddsd));
	    ddsd.dwSize = sizeof( ddsd );
		if (lpDD->GetDisplayMode(&ddsd) == DD_OK)
		{
		}

		if (CreateSurface(fullScreen))
		{
			ddLock = 0;
			InitDisplay(0, preferW, preferH);
			return;
		}

		SetForegroundWindow((HWND)this->hwnd);
		fs = 1;
		LARGE_INTEGER liCnt;
		QueryPerformanceCounter(&liCnt);
		updStart = liCnt.QuadPart;
		this->updateFlag = 3;
		this->updEvt->Set();
	}
	else
	{
		fs = 0;
		lpDD->RestoreDisplayMode();
		lpDD->SetCooperativeLevel( (HWND)this->hwnd, DDSCL_NORMAL );

		ZeroMemory(&ddsd, sizeof(ddsd));
	    ddsd.dwSize = sizeof( ddsd );
		if (lpDD->GetDisplayMode(&ddsd) == DD_OK)
		{
			scnWidth = ddsd.dwWidth;
			scnHeight = ddsd.dwHeight;
		}

		dwStyle  = GetWindowStyle( (HWND)this->hwnd );
		dwStyle &= ~WS_POPUP;
	    dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;
		SetWindowLong( (HWND)this->hwnd, GWL_STYLE, dwStyle );
		SetMenu((HWND)this->hwnd, (HMENU)hMainMenu);
		LPDIRECTDRAWCLIPPER pcClipper;
		lpDD->CreateClipper( 0, &pcClipper, NULL );
		if (pcClipper->SetHWnd( 0, (HWND)this->hwnd ) != DD_OK)
		{
			DDDELETE(pcClipper)
			ddLock = 0;
			return;
		}
		this->clipper = pcClipper;

		CreateSurface(0);
	}
	ddLock = 0;
}

void Media::DDrawRenderer::Redraw()
{
	this->updateFlag |= 2;
	this->updEvt->Set();
}

void Media::DDrawRenderer::GetDrawingRect(void *rc)
{
	if (fs)
	{
		((RECT*)rc)->left = 0;
		((RECT*)rc)->top = 0;
		((RECT*)rc)->right = this->scnWidth;
		((RECT*)rc)->bottom = this->scnHeight;
	}
	else
	{
		GetClientRect((HWND)hwnd, (RECT*)rc);
		ClientToScreen((HWND)hwnd, (POINT*)&((RECT*)rc)->left);
		ClientToScreen((HWND)hwnd, (POINT*)&((RECT*)rc)->right);
	}
}
