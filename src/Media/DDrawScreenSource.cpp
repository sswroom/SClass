#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/DDrawScreenSource.h"
#include "Sync/Thread.h"
#include <windows.h>
#include <ddraw.h>

UInt32 __stdcall Media::DDrawScreenSource::CaptureThread(void *userObj)
{
	Media::DDrawScreenSource *me = (Media::DDrawScreenSource*)userObj;
	LPDIRECTDRAWSURFACE7 surface;
	DDSURFACEDESC2 ddsd;
	OSInt sizeNeeded;
	Manage::HiResClock *clk;
	UInt32 frameNum;
	Double t1;
	Double t2;

	me->captureRunning = true;
	frameNum = 0;
	NEW_CLASS(clk, Manage::HiResClock());
	while (!me->captureToStop)
	{
		surface = (LPDIRECTDRAWSURFACE7)me->primarySurface;
		if (surface)
		{
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);

			t1 = clk->GetTimeDiff();
			if (surface->Lock(0, &ddsd, DDLOCK_READONLY | DDLOCK_WAIT, 0) == DD_OK)
			{
				if (frameNum == 0)
				{
					clk->Start();
				}
				sizeNeeded = ddsd.dwWidth * ddsd.dwHeight * 4;
				if (me->scnBuffSize < sizeNeeded)
				{
					me->scnBuffSize = sizeNeeded;
					if (me->scnBuff)
					{
						MemFree(me->scnBuff);
					}
					me->scnBuff = MemAlloc(UInt8, me->scnBuffSize + 16);
					if (15 & (OSInt)me->scnBuff)
					{
						me->scnABuff = me->scnBuff + 16 - (15 & (OSInt)me->scnBuff);
					}
					else
					{
						me->scnABuff = me->scnBuff;
					}
				}

				Int32 w = ddsd.dwWidth;
				Int32 h = ddsd.dwHeight;
				UInt8 *srcPtr = (UInt8*)ddsd.lpSurface;
				Int32 sbpl = ddsd.lPitch;
				UInt8 *destPtr = me->scnABuff;
				Int32 dbpl = w * 4;

#ifdef HAS_ASM32
				_asm
				{
					mov eax,0xff000000
					movd xmm2,eax
					punpckldq xmm2, xmm2
					punpckldq xmm2, xmm2
					mov esi,srcPtr
					mov edi,destPtr
					mov edx,w
					shr edx,2
ctlop:
					push esi
					push edi
					mov ecx,edx
ctlop2:
					movdqu xmm0,mmword ptr [esi]
					por xmm0,xmm2
					movdqa mmword ptr [edi],xmm0
					add esi,16
					add edi,16
					dec ecx
					jnz ctlop2
					pop edi
					pop esi
					add esi,sbpl
					add edi,dbpl
					dec h
					jnz ctlop
				}
#else
				while (h-- > 0)
				{
					memcpy(destPtr, srcPtr, dbpl);
					srcPtr += sbpl;
					destPtr += dbpl;
				}
#endif

				surface->Unlock(0);
				t2 = clk->GetTimeDiff();
				t1 = t2 - t1;

				me->captureCb((UInt32)Double2Int32(clk->GetTimeDiff() * 1000), frameNum, &me->scnABuff, sizeNeeded, Media::IVideoSource::FS_I, me->captureCbData, Media::FT_NON_INTERLACE, (frameNum == 0)?Media::IVideoSource::FF_FORCEDISP:0, Media::YCOFST_C_CENTER_LEFT);
				frameNum++;
			}
			else
			{
				me->captureEvt->Wait(1000);
			}
		}
		else
		{
			me->captureEvt->Wait(1000);
		}
	}
	DEL_CLASS(clk);
	me->captureRunning = false;
	return 0;
}

void Media::DDrawScreenSource::CreateSurface()
{
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAW7 lpDD = (LPDIRECTDRAW7)ddObj;
	this->ReleaseSurface();

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags        = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	lpDD->CreateSurface( &ddsd, (LPDIRECTDRAWSURFACE7*)&primarySurface, NULL );
}

void Media::DDrawScreenSource::ReleaseSurface()
{
	if (this->primarySurface)
	{
		((LPDIRECTDRAWSURFACE7)this->primarySurface)->Release();
		this->primarySurface = 0;
	}
}

Media::DDrawScreenSource::DDrawScreenSource()
{
	this->captureToStop = false;
	this->captureRunning = false;
	NEW_CLASS(this->captureEvt, Sync::Event(true));

	this->ddObj = 0;
	this->primarySurface = 0;
	this->scnBuff = 0;
	this->scnBuffSize = 0;

	LPDIRECTDRAW7 lpDD;
	if (DirectDrawCreateEx( NULL, (VOID**)&lpDD, IID_IDirectDraw7, NULL ) != DD_OK )
	{
	}
	else
	{
		this->ddObj = lpDD;
		lpDD->SetCooperativeLevel(0, DDSCL_NORMAL);
		this->CreateSurface();
	}
}

Media::DDrawScreenSource::~DDrawScreenSource()
{
	Stop();
	this->ReleaseSurface();
	if (this->ddObj)
	{
		((LPDIRECTDRAW7)ddObj)->Release();
		this->ddObj = 0;
	}
	DEL_CLASS(this->captureEvt);
	if (this->scnBuff)
	{
		MemFree(this->scnBuff);
	}
}

WChar *Media::DDrawScreenSource::GetName(WChar *buff)
{
	return Text::StrConcat(buff, L"screen://");
}

Bool Media::DDrawScreenSource::GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, Int32 *maxFrameSize)
{
	*frameRateNorm = 30;
	*frameRateDenorm = 1;
	if (this->primarySurface == 0)
	{
		*maxFrameSize = 0;
		return false;
	}
	LPDIRECTDRAWSURFACE7 surface = (LPDIRECTDRAWSURFACE7)this->primarySurface;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	if (surface->GetSurfaceDesc(&ddsd) != DD_OK)
	{
		*maxFrameSize = 0;
		return false;
	}

	info->storeWidth = ddsd.dwWidth;
	info->storeHeight = ddsd.dwHeight;
	info->dispWidth = info->storeWidth;
	info->dispHeight = info->storeHeight;
	info->fourcc = 0;
	info->storeBPP = ddsd.ddpfPixelFormat.dwRGBBitCount;
	info->byteSize = 0;
	info->par2 = 1;
	info->ftype = Media::FT_NON_INTERLACE;
	info->atype = Media::AT_NO_ALPHA;
	info->color->SetCommonProfile(Media::ColorProfile::CPT_SRGB);
	info->yuvType = Media::ColorProfile::YUVT_BT709;
	if (info->storeBPP == 32)
	{
		*maxFrameSize = info->storeWidth * info->storeHeight << 2;
	}

	return true;
}

Bool Media::DDrawScreenSource::Start(FrameCallback cb, void *userData)
{
	if (this->captureRunning)
		return false;
	this->captureCb = cb;
	this->captureCbData = userData;
	this->captureToStop = false;
	Sync::Thread::Create(CaptureThread, this);
	while (!this->captureRunning)
	{
		Sync::Thread::Sleep(10);
	}
	return true;
}

void Media::DDrawScreenSource::Stop()
{
	if (this->captureRunning)
	{
		this->captureToStop = true;
		this->captureEvt->Set();
		while (this->captureRunning)
		{
			Sync::Thread::Sleep(10);
		}
		this->captureToStop = true;
	}
}

Bool Media::DDrawScreenSource::IsRunning()
{
	return this->captureRunning;
}
