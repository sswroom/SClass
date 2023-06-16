#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DebugWriter.h"
#include "Math/Math.h"
#include "Media/FrameInfo.h"
#include "Media/DDrawRenderer2.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/Event.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

#define FRAMEBUFFSIZE 5
#define FRAMESRCSIZE 5
#define NFRAMEAVG 16
#define DDDELETE(obj) if (obj) { obj->Release(); obj = 0;}

void Media::DDrawRenderer2::GetDrawingRect(void *rc)
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

void Media::DDrawRenderer2::CreateSurface()
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddObj;
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 primarySurface;

	this->ReleaseSurface();
	this->ReleaseSubSurface();

	this->bbSurface = 0;

	if (this->fs)
	{

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
	    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;

		if (lpDD->CreateSurface( &ddsd, &primarySurface, NULL ) != DD_OK)
		{
			return; //1
		}
		pSurface = primarySurface;

	    DDSCAPS2 ddscaps;
		ZeroMemory( &ddscaps, sizeof( ddscaps ) );
	    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

		if (primarySurface->GetAttachedSurface( &ddscaps, (LPDIRECTDRAWSURFACE7*)&bbSurface ) != DD_OK)
		{
			return; //1
		}
	}
	else
	{
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags        = DDSD_CAPS;
	    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		HRESULT res = lpDD->CreateSurface( &ddsd, &primarySurface, NULL );
		pSurface = primarySurface;
		if (primarySurface)
		{
			CreateSubSurface();
			if (clipper)
				primarySurface->SetClipper((LPDIRECTDRAWCLIPPER)clipper);
		}
	}

	return; //0
}

void Media::DDrawRenderer2::ReleaseSurface()
{
	if (this->pSurface)
	{
		((LPDIRECTDRAWSURFACE7)this->pSurface)->Release();
		this->pSurface = 0;
	}
}

void Media::DDrawRenderer2::CreateSubSurface()
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddObj;
	DDSURFACEDESC2 ddsd;
	RECT rc;
	GetDrawingRect(&rc);
	if (rc.right <= rc.left || rc.bottom <= rc.top)
	{
	}
	else
	{
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth = rc.right - rc.left;
		ddsd.dwHeight = rc.bottom - rc.top;
		this->surfaceW = ddsd.dwWidth;
		this->surfaceH = ddsd.dwHeight;
		OSInt i = FRAMEBUFFSIZE;
		while (i-- > 0)
		{
			HRESULT res = lpDD->CreateSurface( &ddsd, (LPDIRECTDRAWSURFACE7*)&surfaceBuff[i].surface, NULL );
		}
		surfaceBuffStart = 0;
		surfaceBuffEnd = 0;
	}
}

void Media::DDrawRenderer2::ReleaseSubSurface()
{
	OSInt i = FRAMEBUFFSIZE;
	while (i-- > 0)
	{
		if (this->surfaceBuff[i].surface)
		{
			((LPDIRECTDRAWSURFACE7)this->surfaceBuff[i].surface)->Release();
			this->surfaceBuff[i].surface = 0;
		}
	}
}

void Media::DDrawRenderer2::CreateFrameSrcBuff()
{
	OSInt i = FRAMESRCSIZE;
	while (i-- > 0)
	{
		this->frameSources[i].frameData = MemAlloc(UInt8, this->frameSrcSize);
	}
}

void Media::DDrawRenderer2::ReleaseFrameSrcBuff()
{
	OSInt i = FRAMESRCSIZE;
	while (i-- > 0)
	{
		if (this->frameSources[i].frameData)
		{
			MemFree(this->frameSources[i].frameData);
			this->frameSources[i].frameData = 0;
		}
	}
}

OSInt Media::DDrawRenderer2::GetNextSurface(Int32 frameTime)
{
	if (!this->rendering)
		return -1;
	if (clk->GetCurrTime() > frameTime + 100 && !realtimeMode)
		return -1;
	while ((this->surfaceBuffEnd + 1) % FRAMEBUFFSIZE == this->surfaceBuffStart)
	{
		Sync::SimpleThread::Sleep(1);
	}
	this->surfaceBuff[this->surfaceBuffEnd].frameTime = frameTime;
	return this->surfaceBuffEnd;
}

void Media::DDrawRenderer2::PutNextSurface(OSInt index)
{
	if (index != this->surfaceBuffEnd)
		return;
	this->surfaceBuffEnd = (this->surfaceBuffEnd + 1) % FRAMEBUFFSIZE;
	this->surfaceBuffEvent.Set();
}

UInt32 __stdcall Media::DDrawRenderer2::ScnUpdater(void *obj)
{
	Media::DDrawRenderer2 *me = (Media::DDrawRenderer2*)obj;
	me->scnUpdRunning = true;
	while (!me->scnUpdToStop)
	{
		me->surfaceBuffEvent.Wait(1);
		if (me->scnUpdToStop)
			break;
		if (me->rendering)
		{
			while (me->surfaceBuffStart != me->surfaceBuffEnd)
			{
				if (me->clk->GetCurrTime() > me->surfaceBuff[me->surfaceBuffStart].frameTime)
				{
					Sync::MutexUsage mutUsage(&me->surfaceBuffMut);
					if (me->surfaceBuff[me->surfaceBuffStart].surface == 0)
					{
						break;
					}
					LPDIRECTDRAWSURFACE7 surface = (LPDIRECTDRAWSURFACE7)me->surfaceBuff[me->surfaceBuffStart].surface;
					RECT rc;
					RECT rcArea;
					rc.left = 0;
					rc.top = 0;
					rc.right = me->surfaceW;
					rc.bottom = me->surfaceH;
					me->GetDrawingRect(&rcArea);
					LPDIRECTDRAWSURFACE7 primarySurface = (LPDIRECTDRAWSURFACE7)me->pSurface;
					HRESULT res = primarySurface->Blt(&rcArea, surface, &rc, 0, 0);

					me->surfaceBuffStart = (me->surfaceBuffStart + 1) % FRAMEBUFFSIZE;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			me->surfaceBuffStart = me->surfaceBuffEnd;
		}
	}
	me->scnUpdRunning = false;
	return 0;
}

UInt32 __stdcall Media::DDrawRenderer2::FrameProcesser(void *obj)
{
	Media::DDrawRenderer2 *me = (Media::DDrawRenderer2*)obj;
	me->frameProcRunning = true;
	while (!me->frameProcToStop)
	{
		me->frameProcEvent.Wait(10);
		if (me->frameProcToStop)
			break;
		if (me->rendering)
		{
			while (me->frameProcStart != me->frameProcEnd)
			{
				me->csconv->ConvertV2(&me->frameSources[me->frameProcStart].frameData, me->frameRGBBuff, me->info.dispWidth, me->info.dispHeight, me->info.storeWidth, me->info.storeHeight, me->info.storeWidth << 2, Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);

				if (!me->firstFrame)
				{
					me->fieldInt = (me->frameSources[me->frameProcStart].frameTime - me->lastFrameTime) >> 1;
				}

				LPDIRECTDRAWSURFACE7 surface;
				OSInt i = me->GetNextSurface(me->frameSources[me->frameProcStart].frameTime);
				if (i != -1)
				{
					RECT rc;
					DDSURFACEDESC2 ddsd;

					ZeroMemory(&ddsd, sizeof(ddsd));
					ddsd.dwSize = sizeof(ddsd);

					Sync::MutexUsage mutUsage(&me->surfaceBuffMut);
					surface = (LPDIRECTDRAWSURFACE7)me->surfaceBuff[i].surface;
					if (surface)
					{
						Int32 outW = me->surfaceW;
						Int32 outH = me->surfaceH;
						UInt8 *srcPtr;
						UInt8 *destPtr;
						Int32 destW;
						Int32 destH;
						Int32 srcW;
						Int32 srcH;
						rc.left = 0;
						rc.top = 0;
						rc.right = me->surfaceW;
						rc.bottom = me->surfaceH;

						if (DD_OK == surface->Lock(&rc, &ddsd, DDLOCK_WAIT, 0))
						{
							destPtr = (UInt8*)ddsd.lpSurface;
							if (me->frameSources[me->frameProcStart].ftype == Media::FT_NON_INTERLACE)
							{
								srcW = me->info.dispWidth;
								srcH = me->info.dispHeight;
								srcPtr = me->frameRGBBuff;
							}
							else if (me->frameSources[me->frameProcStart].ftype == Media::FT_FIELD_TF)
							{
								srcW = me->info.dispWidth;
								srcH = me->info.dispHeight << 1;
								srcPtr = me->frameDIBuff;
								me->deinterlace->Deinterlace(me->frameRGBBuff, me->frameDIBuff, 0, me->info.dispWidth, me->info.storeWidth << 2);
							}
							else if (me->frameSources[me->frameProcStart].ftype == Media::FT_FIELD_BF)
							{
								srcW = me->info.dispWidth;
								srcH = me->info.dispHeight << 1;
								srcPtr = me->frameDIBuff;
								me->deinterlace->Deinterlace(me->frameRGBBuff, me->frameDIBuff, 1, me->info.dispWidth, me->info.storeWidth << 2);
							}
							else if (me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_TFF)
							{
								srcW = me->info.dispWidth;
								srcH = me->info.dispHeight;
								srcPtr = me->frameDIBuff;
								me->deinterlace->Deinterlace(me->frameRGBBuff, me->frameDIBuff, 0, me->info.dispWidth, me->info.storeWidth << 2);
							}
							else if (me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_BFF)
							{
								srcW = me->info.dispWidth;
								srcH = me->info.dispHeight;
								srcPtr = me->frameDIBuff;
								me->deinterlace->Deinterlace(me->frameRGBBuff + (me->info.storeWidth << 2), me->frameDIBuff, 1, me->info.dispWidth, me->info.storeWidth << 2);
							}
							else
							{
								srcW = me->info.dispWidth;
								srcH = me->info.dispHeight;
								srcPtr = me->frameRGBBuff;
							}

							if (srcW * me->info.par2 * outH > outW * (Double)srcH)
							{
								destW = outW;
								destH = Double2Int32(outW / me->info.par2 * srcH / srcW);
								destPtr = destPtr + ddsd.lPitch * ((outH - destH) >> 1);
							}
							else
							{
								destW = Double2Int32(outH * me->info.par2 * srcW / srcH);
								destH = outH;
								destPtr = destPtr + ((outW - destW) >> 1) * 4;
							}

							me->resizer->Resize(srcPtr, me->info.storeWidth << 2, me->info.dispWidth, me->info.dispHeight, 0, 0, destPtr, ddsd.lPitch, destW, destH);
							surface->Unlock(0);

							me->PutNextSurface(i);
							mutUsage.EndUse();

							if (me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_TFF || me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_BFF)
							{
								i = me->GetNextSurface(me->frameSources[me->frameProcStart].frameTime + me->fieldInt);
								if (i != -1)
								{
									if (me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_TFF)
									{
										srcW = me->info.dispWidth;
										srcH = me->info.dispHeight;
										srcPtr = me->frameDIBuff;
										me->deinterlace->Deinterlace(me->frameRGBBuff + (me->info.storeWidth << 2), me->frameDIBuff, 1, me->info.dispWidth, me->info.storeWidth << 2);
									}
									else if (me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_BFF)
									{
										srcW = me->info.dispWidth;
										srcH = me->info.dispHeight;
										srcPtr = me->frameDIBuff;
										me->deinterlace->Deinterlace(me->frameRGBBuff, me->frameDIBuff, 0, me->info.dispWidth, me->info.storeWidth << 2);
									}

									mutUsage.BeginUse();
									surface = (LPDIRECTDRAWSURFACE7)me->surfaceBuff[i].surface;
									if (surface)
									{
										if (DD_OK == surface->Lock(&rc, &ddsd, DDLOCK_WAIT, 0))
										{
											destPtr = (UInt8*)ddsd.lpSurface;

											if (srcW * me->info.par2 * outH > outW * (Double)srcH)
											{
												destW = outW;
												destH = Double2Int32(outW / me->info.par2 * srcH / srcW);
												destPtr = destPtr + ddsd.lPitch * ((outH - destH) >> 1);
											}
											else
											{
												destW = Double2Int32(outH * me->info.par2 * srcW / srcH);
												destH = outH;
												destPtr = destPtr + ((outW - destW) >> 1) * 4;
											}

											me->resizer->Resize(srcPtr, me->info.storeWidth << 2, me->info.dispWidth, me->info.dispHeight, 0, 0, destPtr, ddsd.lPitch, destW, destH);
											surface->Unlock(0);

											me->PutNextSurface(i);
										}
									}
									mutUsage.EndUse();
								}
							}
						}
					}
				}


				me->lastFrameTime = me->frameSources[me->frameProcStart].frameTime;
				me->firstFrame = false;
				me->frameProcStart = (me->frameProcStart + 1) % FRAMESRCSIZE;
			}
		}
		else
		{
			me->frameProcStart = me->frameProcEnd;
		}
	}
	me->frameProcRunning = false;
	return 0;
}

Media::DDrawRenderer2::DDrawRenderer2(void *hwnd, Media::ColorManager *colorMgr)
{
	this->hwnd = hwnd;
	this->colorMgr = colorMgr;
	this->clipper = 0;
	this->pSurface = 0;

	this->csconv = 0;
	this->frameSrcSize = 0;
	this->frameRGBBuff = 0;
	this->frameRGBSize = 0;
	this->fs = false;
	this->scnWidth = 0;
	this->scnHeight = 0;
	this->lastFrameTime = -34;
	this->fieldInt = 17;
	this->scnUpdRunning = false;
	this->scnUpdToStop = false;
	this->frameProcRunning = false;
	this->frameProcToStop = false;
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerH8_8(3, 3, Media::AT_NO_ALPHA));
	NEW_CLASS(this->deinterlace, Media::Deinterlace8(10, 10));
	this->surfaceBuff = MemAlloc(FrameBuffer, FRAMEBUFFSIZE);
	this->surfaceBuffStart = 0;
	this->surfaceBuffEnd = 0;
	OSInt i = FRAMEBUFFSIZE;
	while (i-- > 0)
	{
		this->surfaceBuff[i].surface = 0;
	}
	this->frameSources = MemAlloc(FrameSource, FRAMESRCSIZE);
	this->frameProcStart = 0;
	this->frameProcEnd = 0;
	i = FRAMESRCSIZE;
	while (i-- > 0)
	{
		this->frameSources[i].frameData = 0;
	}

	LPDIRECTDRAW7 lpDD;
	if (DirectDrawCreateEx( NULL, (VOID**)&lpDD, IID_IDirectDraw7, NULL ) != DD_OK )
	{
	}
	else
	{
		this->ddObj = lpDD;
		lpDD->SetCooperativeLevel((HWND)this->hwnd, DDSCL_NORMAL);
		LPDIRECTDRAWCLIPPER pcClipper;
		lpDD->CreateClipper( 0, &pcClipper, NULL );
		if (pcClipper->SetHWnd( 0, (HWND)this->hwnd ) != DD_OK)
		{
			DDDELETE(pcClipper)
			return;
		}
		this->clipper = pcClipper;

		CreateSurface();
		Sync::ThreadUtil::Create(ScnUpdater, this);
		Sync::ThreadUtil::Create(FrameProcesser, this);
	}
}

Media::DDrawRenderer2::~DDrawRenderer2()
{
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddObj;
	if (this->csconv)
	{
		DEL_CLASS(this->csconv);
	}
	this->ReleaseFrameSrcBuff();
	if (this->frameRGBBuff)
	{
		MemFree(this->frameRGBBuff);
	}
	if (this->frameDIBuff)
	{
		MemFree(this->frameDIBuff);
	}
	this->scnUpdToStop = true;
	this->frameProcToStop = true;
	this->surfaceBuffEvent.Set();
	this->frameProcEvent.Set();
	while (this->scnUpdRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	while (this->frameProcRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->ReleaseSurface();
	this->ReleaseSubSurface();
	MemFree(this->surfaceBuff);
	MemFree(this->frameSources);

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
	DEL_CLASS(this->resizer);
	DEL_CLASS(this->deinterlace);
}

void Media::DDrawRenderer2::OnSizeChanged()
{
	Sync::MutexUsage mutUsage(&this->surfaceBuffMut);
	this->ReleaseSubSurface();
	this->CreateSubSurface();
}

void Media::DDrawRenderer2::StartRender(Media::RefClock *clk, Bool realtimeMode)
{
	this->clk = clk;
	this->firstFrame = true;
	this->realtimeMode = realtimeMode;
	this->rendering = true;
}
void Media::DDrawRenderer2::StopRender()
{
	this->rendering = false;
}

void Media::DDrawRenderer2::ChangeFrameFormat(Media::FrameInfo *info)
{
	if (this->csconv)
	{
		DEL_CLASS(this->csconv);
	}
	this->ReleaseFrameSrcBuff();
	if (this->frameRGBBuff)
	{
		MemFree(this->frameRGBBuff);
	}
	if (this->frameDIBuff)
	{
		MemFree(this->frameDIBuff);
	}
	MemCopyNO(&this->info, info, sizeof(Media::FrameInfo));
	this->csconv = Media::CS::CSConverter::NewConverter(info->fourcc, info->storeBPP, info->pf, 0, 32, Media::CS::TRANT_VDISPLAY, this->info.rgbGamma, this->info.yuvType, this->colorMgr);
	if (this->csconv == 0)
	{
		this->frameSrcSize = 0;
		this->frameRGBSize = 0;
		this->frameRGBBuff = 0;
	}
	else
	{
		this->frameSrcSize = this->csconv->GetSrcFrameSize(info->dispWidth, info->dispHeight);
		this->CreateFrameSrcBuff();
		this->frameRGBSize = this->csconv->GetDestFrameSize(info->dispWidth, info->dispHeight);
		this->frameRGBBuff = MemAlloc(UInt8, frameRGBSize);
	}
	if (info->ftype == Media::FT_FIELD_TF || info->ftype == Media::FT_FIELD_BF)
	{
		this->frameDIBuff = MemAlloc(UInt8, frameRGBSize << 1);
		this->deinterlace->Reinit(this->info.dispWidth, this->info.storeWidth << 2);
	}
	else
	{
		this->frameDIBuff = MemAlloc(UInt8, frameRGBSize);
		this->deinterlace->Reinit(this->info.dispHeight >> 1, this->info.storeWidth << 3);
	}
}

void Media::DDrawRenderer2::AddFrame(const UInt8 *frameData, Int32 frameTime, Media::FrameType ftype)
{
	while (((this->frameProcEnd + 1) % FRAMESRCSIZE) == this->frameProcStart)
	{
		Sync::SimpleThread::Sleep(10);
	}
	MemCopyNO(this->frameSources[this->frameProcEnd].frameData, frameData, this->frameSrcSize);
	this->frameSources[this->frameProcEnd].frameTime = frameTime;
	this->frameSources[this->frameProcEnd].ftype = ftype;
	this->frameProcEnd = (this->frameProcEnd + 1) % FRAMESRCSIZE;
	this->frameProcEvent.Set();
}

void Media::DDrawRenderer2::RenderFrame()
{
}
