#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DebugWriter.h"
#include "Math/Math_C.h"
#include "Media/FrameInfo.h"
#include "Media/DDrawRendererLR.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
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

void Media::DDrawRendererLR::GetDrawingRect(void *rc)
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

void Media::DDrawRendererLR::CreateSurface()
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

void Media::DDrawRendererLR::ReleaseSurface()
{
	if (this->pSurface)
	{
		((LPDIRECTDRAWSURFACE7)this->pSurface)->Release();
		this->pSurface = 0;
	}
}

void Media::DDrawRendererLR::CreateSubSurface()
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
		IntOS i = FRAMEBUFFSIZE;
		while (i-- > 0)
		{
			HRESULT res = lpDD->CreateSurface( &ddsd, (LPDIRECTDRAWSURFACE7*)&surfaceBuff[i].surface, NULL );
		}
		surfaceBuffStart = 0;
		surfaceBuffEnd = 0;
	}
}

void Media::DDrawRendererLR::ReleaseSubSurface()
{
	IntOS i = FRAMEBUFFSIZE;
	while (i-- > 0)
	{
		if (this->surfaceBuff[i].surface)
		{
			((LPDIRECTDRAWSURFACE7)this->surfaceBuff[i].surface)->Release();
			this->surfaceBuff[i].surface = 0;
		}
	}
}

void Media::DDrawRendererLR::CreateFrameSrcBuff()
{
	IntOS i = FRAMESRCSIZE;
	while (i-- > 0)
	{
		this->frameSources[i].frameData = MemAlloc(UInt8, this->frameSrcSize);
	}
}

void Media::DDrawRendererLR::ReleaseFrameSrcBuff()
{
	IntOS i = FRAMESRCSIZE;
	while (i-- > 0)
	{
		if (this->frameSources[i].frameData)
		{
			MemFree(this->frameSources[i].frameData);
			this->frameSources[i].frameData = 0;
		}
	}
}

IntOS Media::DDrawRendererLR::GetNextSurface(Int32 frameTime)
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

void Media::DDrawRendererLR::PutNextSurface(IntOS index)
{
	if (index != this->surfaceBuffEnd)
		return;
	this->surfaceBuffEnd = (this->surfaceBuffEnd + 1) % FRAMEBUFFSIZE;
	this->surfaceBuffEvent->Set();
}

UInt32 __stdcall Media::DDrawRendererLR::ScnUpdater(void *obj)
{
	Media::DDrawRendererLR *me = (Media::DDrawRendererLR*)obj;
	me->scnUpdRunning = true;
	while (!me->scnUpdToStop)
	{
		me->surfaceBuffEvent->Wait(1);
		if (me->scnUpdToStop)
			break;
		if (me->rendering)
		{
			while (me->surfaceBuffStart != me->surfaceBuffEnd)
			{
				if (me->clk->GetCurrTime() > me->surfaceBuff[me->surfaceBuffStart].frameTime)
				{
					Sync::MutexUsage mutUsage(me->surfaceBuffMut);
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

UInt32 __stdcall Media::DDrawRendererLR::FrameProcesser(void *obj)
{
	Media::DDrawRendererLR *me = (Media::DDrawRendererLR*)obj;
	me->frameProcRunning = true;
	while (!me->frameProcToStop)
	{
		me->frameProcEvent->Wait(10);
		if (me->frameProcToStop)
			break;
		if (me->rendering)
		{
			while (me->frameProcStart != me->frameProcEnd)
			{
				me->frameProcMut->Lock();
				me->csconv->ConvertV2(&me->frameSources[me->frameProcStart].frameData, me->frameRGBBuff, me->info.dispSize.x, me->info.dispSize.y, me->info.storeSize.x * (me->info.storeBPP >> 3), me->info.storeSize.x << 3);

				if (!me->firstFrame)
				{
					me->fieldInt = (me->frameSources[me->frameProcStart].frameTime - me->lastFrameTime) >> 1;
				}

				LPDIRECTDRAWSURFACE7 surface;
				IntOS i = me->GetNextSurface(me->frameSources[me->frameProcStart].frameTime);
				if (i != -1)
				{
					RECT rc;
					DDSURFACEDESC2 ddsd;

					ZeroMemory(&ddsd, sizeof(ddsd));
					ddsd.dwSize = sizeof(ddsd);

					me->surfaceBuffMut->Lock();
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
								srcW = me->info.dispSize.x;
								srcH = me->info.dispSize.y;
								srcPtr = me->frameRGBBuff;
							}
							else if (me->frameSources[me->frameProcStart].ftype == Media::FT_FIELD_TF)
							{
								srcW = me->info.dispSize.x;
								srcH = me->info.dispSize.y << 1;
								srcPtr = me->frameDIBuff;
								me->deinterlace->Deinterlace(me->frameRGBBuff, me->frameDIBuff, 0, me->info.dispSize.x, me->info.storeSize.x << 3);
							}
							else if (me->frameSources[me->frameProcStart].ftype == Media::FT_FIELD_BF)
							{
								srcW = me->info.dispSize.x;
								srcH = me->info.dispSize.y << 1;
								srcPtr = me->frameDIBuff;
								me->deinterlace->Deinterlace(me->frameRGBBuff, me->frameDIBuff, 1, me->info.dispSize.x, me->info.storeSize.x << 3);
							}
							else if (me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_TFF)
							{
								srcW = me->info.dispSize.x;
								srcH = me->info.dispSize.y;
								srcPtr = me->frameDIBuff;
								me->deinterlace->Deinterlace(me->frameRGBBuff, me->frameDIBuff, 0, me->info.dispSize.x, me->info.storeSize.x << 3);
							}
							else if (me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_BFF)
							{
								srcW = me->info.dispSize.x;
								srcH = me->info.dispSize.y;
								srcPtr = me->frameDIBuff;
								me->deinterlace->Deinterlace(me->frameRGBBuff + (me->info.storeSize.x << 3), me->frameDIBuff, 1, me->info.dispSize.x, me->info.storeSize.x << 3);
							}
							else
							{
								srcW = me->info.dispSize.x;
								srcH = me->info.dispSize.y;
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

							me->resizer->Resize(srcPtr, me->info.storeSize.x << 3, me->info.dispSize.x, me->info.dispSize.y, 0, 0, destPtr, ddsd.lPitch, destW, destH);
							surface->Unlock(0);

							me->PutNextSurface(i);
							me->surfaceBuffMut->Unlock();


							if (me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_TFF || me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_BFF)
							{
								i = me->GetNextSurface(me->frameSources[me->frameProcStart].frameTime + me->fieldInt);
								if (i != -1)
								{
									if (me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_TFF)
									{
										srcW = me->info.dispSize.x;
										srcH = me->info.dispSize.y;
										srcPtr = me->frameDIBuff;
										me->deinterlace->Deinterlace(me->frameRGBBuff + (me->info.storeSize.x << 3), me->frameDIBuff, 1, me->info.dispSize.x, me->info.storeSize.x << 3);
									}
									else if (me->frameSources[me->frameProcStart].ftype == Media::FT_INTERLACED_BFF)
									{
										srcW = me->info.dispSize.x;
										srcH = me->info.dispSize.y;
										srcPtr = me->frameDIBuff;
										me->deinterlace->Deinterlace(me->frameRGBBuff, me->frameDIBuff, 0, me->info.dispSize.x, me->info.storeSize.x << 3);
									}

									me->surfaceBuffMut->Lock();
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

											me->resizer->Resize(srcPtr, me->info.storeSize.x << 3, me->info.dispSize.x, me->info.dispSize.y, 0, 0, destPtr, ddsd.lPitch, destW, destH);
											surface->Unlock(0);

											me->PutNextSurface(i);
										}
									}
									me->surfaceBuffMut->Unlock();
								}
							}
						}
					}
					else
					{
						me->surfaceBuffMut->Unlock();
					}
				}


				me->lastFrameTime = me->frameSources[me->frameProcStart].frameTime;
				me->firstFrame = false;
				me->frameProcStart = (me->frameProcStart + 1) % FRAMESRCSIZE;
				me->frameProcMut->Unlock();
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

Media::DDrawRendererLR::DDrawRendererLR(void *hwnd, Media::ColorManager *colorMgr)
{
	this->hwnd = hwnd;
	this->colorMgr = colorMgr;
	this->clipper = 0;
	this->pSurface = 0;

	this->csconv = 0;
	this->frameSrcSize = 0;
	this->frameRGBBuff = 0;
	this->frameDIBuff = 0;
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
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerLR_C8(3, 3, Media::CS::TRANT_DISPLAY, 2.2, colorMgr));
	NEW_CLASS(this->deinterlace, Media::DeinterlaceLR(10, 10));
	this->surfaceBuff = MemAlloc(FrameBuffer, FRAMEBUFFSIZE);
	this->surfaceBuffStart = 0;
	this->surfaceBuffEnd = 0;
	IntOS i = FRAMEBUFFSIZE;
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
	NEW_CLASS(this->surfaceBuffEvent, Sync::Event(L"Media.DDrawRenderer2.surfaceBuffEvt"));
	NEW_CLASS(this->surfaceBuffMut, Sync::Mutex());
	NEW_CLASS(this->frameProcEvent, Sync::Event(L"Media.DDrawRenderer2.frameProcEvent"));
	NEW_CLASS(this->frameProcMut, Sync::Mutex());

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

Media::DDrawRendererLR::~DDrawRendererLR()
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
	this->surfaceBuffEvent->Set();
	this->frameProcEvent->Set();
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
	DEL_CLASS(this->surfaceBuffEvent);
	DEL_CLASS(this->surfaceBuffMut);
	DEL_CLASS(this->frameProcEvent);
	DEL_CLASS(this->frameProcMut);

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

void Media::DDrawRendererLR::OnSizeChanged()
{
	this->surfaceBuffMut->Lock();
	this->ReleaseSubSurface();
	this->CreateSubSurface();
	this->surfaceBuffMut->Unlock();
}

void Media::DDrawRendererLR::StartRender(Media::RefClock *clk, Bool realtimeMode)
{
	this->clk = clk;
	this->firstFrame = true;
	this->realtimeMode = realtimeMode;
	this->rendering = true;
}
void Media::DDrawRendererLR::StopRender()
{
	this->rendering = false;
}

void Media::DDrawRendererLR::ChangeFrameFormat(Media::FrameInfo *info)
{
	Sync::MutexUsage mutUsage(this->frameProcMut);
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
	this->info.Set(info);
	Media::ColorProfile color(Media::ColorProfile::CPT_VDISPLAY);
	this->csconv = Media::CS::CSConverter::NewConverter(info->fourcc, info->storeBPP, info->pf, info->color, *(Int32*)"LRGB", 64, Media::PF_UNKNOWN, &color, info->yuvType, this->colorMgr);
	if (this->csconv == 0)
	{
		this->frameSrcSize = 0;
		this->frameRGBSize = 0;
		this->frameRGBBuff = 0;
	}
	else
	{
		this->frameSrcSize = this->csconv->GetSrcFrameSize(info->dispSize.x, info->dispHeight);
		this->CreateFrameSrcBuff();
		this->frameRGBSize = this->csconv->GetDestFrameSize(info->dispSize.x, info->dispHeight);
		this->frameRGBBuff = MemAlloc(UInt8, frameRGBSize);
	}
	if (info->ftype == Media::FT_FIELD_TF || info->ftype == Media::FT_FIELD_BF)
	{
		this->frameDIBuff = MemAlloc(UInt8, frameRGBSize << 1);
		this->deinterlace->Reinit(this->info.dispSize.y, this->info.storeSize.x << 3);
	}
	else
	{
		this->frameDIBuff = MemAlloc(UInt8, frameRGBSize);
		this->deinterlace->Reinit(this->info.dispSize.y >> 1, this->info.storeSize.x << 4);
	}
}

void Media::DDrawRendererLR::AddFrame(const UInt8 *frameData, Int32 frameTime, Media::FrameType ftype)
{
	while (((this->frameProcEnd + 1) % FRAMESRCSIZE) == this->frameProcStart)
	{
		Sync::SimpleThread::Sleep(10);
		if (this->rendering == false)
			return;
	}
	MemCopyNO(this->frameSources[this->frameProcEnd].frameData, frameData, this->frameSrcSize);
	this->frameSources[this->frameProcEnd].frameTime = frameTime;
	this->frameSources[this->frameProcEnd].ftype = ftype;
	this->frameProcEnd = (this->frameProcEnd + 1) % FRAMESRCSIZE;
	this->frameProcEvent->Set();
}

void Media::DDrawRendererLR::RenderFrame()
{
}
