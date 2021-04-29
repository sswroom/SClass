#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/CPUInfo.h"
#include "Math/Math.h"
#include "Media/RGBColorFilter.h"
#include "Media/CS/TransferFunc.h"
#include "Sync/Thread.h"

extern "C"
{
	void RGBColorFilter_ProcessImagePart(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt sbpl, OSInt dbpl, UInt8 *lut, Int32 bpp);
	void RGBColorFilter_ProcessImageHDRPart(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt sbpl, OSInt dbpl, UInt8 *lut, Int32 bpp);
	void RGBColorFilter_ProcessImageHDRDPart(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt sbpl, OSInt dbpl, UInt8 *lut, Int32 bpp);
	void RGBColorFilter_ProcessImageHDRDLPart(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt sbpl, OSInt dbpl, UInt8 *lut, Int32 bpp, Int32 hdrLev);
}

UInt32 __stdcall Media::RGBColorFilter::ProcessThread(void *userObj)
{
	ThreadStat *tstat = (ThreadStat *)userObj;
	tstat->threadStat = 1;
	while (true)
	{
		if (tstat->threadStat == 3)
		{
			break;
		}
		else if (tstat->threadStat == 2)
		{
			if (tstat->me->hdrLev != 0 && tstat->me->hasSSE41)
			{
				RGBColorFilter_ProcessImageHDRDLPart(tstat->srcPtr, tstat->destPtr, tstat->width, tstat->height, tstat->sbpl, tstat->dbpl, tstat->me->lut, tstat->me->bpp, tstat->me->hdrLev);
			}
			else
			{
				RGBColorFilter_ProcessImagePart(tstat->srcPtr, tstat->destPtr, tstat->width, tstat->height, tstat->sbpl, tstat->dbpl, tstat->me->lut, tstat->me->bpp);
			}
			tstat->threadStat = 1;
			tstat->me->threadEvt->Set();
		}
		tstat->evt->Wait(10000);
	}
	tstat->threadStat = 4;
	tstat->me->threadEvt->Set();
	return 0;
}

void Media::RGBColorFilter::WaitForThread(Int32 stat)
{
	UOSInt i;
	Bool found = true;
	while (found)
	{
		this->threadEvt->Wait(1000);
		found = false;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->threadStats[i].threadStat == stat)
			{
				found = true;
				break;
			}
		}
	}
}

Media::RGBColorFilter::RGBColorFilter(Media::ColorManager *colorMgr)
{
	this->colorMgr = colorMgr;
	this->lut = 0;
	this->nThread = Sync::Thread::GetThreadCnt();
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"Media.RGBColorFilter.threadEvt"));
	this->threadStats = MemAlloc(ThreadStat, this->nThread);
	this->hdrLev = 0;
	this->gammaParam = 0;
	this->gammaCnt = 0;
	this->pf = Media::PF_UNKNOWN;
#if defined(CPU_X86_64) || defined(CPU_X86_32)
	Manage::CPUInfo cpuInfo;
	this->hasSSE41 = cpuInfo.HasInstruction(Manage::CPUInfo::IT_SSE41);
#else
	this->hasSSE41 = true;
#endif
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		NEW_CLASS(this->threadStats[i].evt, Sync::Event(true, (const UTF8Char*)"Media.RGBColorFilter.threadStats.evt"));
		this->threadStats[i].threadStat = 0;
		this->threadStats[i].me = this;
		Sync::Thread::Create(ProcessThread, &this->threadStats[i]);
	}
	WaitForThread(0);
}

Media::RGBColorFilter::~RGBColorFilter()
{
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		this->threadStats[i].threadStat = 3;
		this->threadStats[i].evt->Set();
	}
	Bool found = true;
	while (found)
	{
		this->threadEvt->Wait(1000);
		found = false;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->threadStats[i].threadStat == 3)
			{
				found = true;
				break;
			}
			else if (this->threadStats[i].threadStat != 4)
			{
				this->threadStats[i].threadStat = 3;
				this->threadStats[i].evt->Set();
			}
		}
	}
	i = this->nThread;
	while (i-- > 0)
	{
		DEL_CLASS(this->threadStats[i].evt);
	}
	MemFree(this->threadStats);
	DEL_CLASS(this->threadEvt);
	if (this->lut)
	{
		MemFree(this->lut);
		this->lut = 0;
	}
	if (this->gammaParam)
	{
		MemFree(this->gammaParam);
		this->gammaParam = 0;
	}
}

void Media::RGBColorFilter::SetGammaCorr(Double *gammaParam, UOSInt gammaCnt)
{
	if (this->gammaParam)
	{
		MemFree(this->gammaParam);
	}
	if (gammaParam != 0 && gammaCnt > 0)
	{
		this->gammaCnt = gammaCnt;
		this->gammaParam = MemAlloc(Double, gammaCnt);
		MemCopyNO(this->gammaParam, gammaParam, sizeof(Double) * gammaCnt);
	}
	else
	{
		this->gammaCnt = 0;
		this->gammaParam = 0;
	}
}

void Media::RGBColorFilter::SetParameter(Double brightness, Double contrast, Double gamma, Media::ColorProfile *color, Int32 bpp, Media::PixelFormat pf, UInt32 hdrLev)
{
	this->brightness = brightness;
	this->contrast = contrast;
	this->gamma = gamma;
	this->bpp = bpp;
	this->pf = pf;
	this->hdrLev = hdrLev;

	if (this->lut)
	{
		MemFree(this->lut);
		this->lut = 0;
	}

	OSInt i;
	Media::CS::TransferFunc *rtFunc = Media::CS::TransferFunc::CreateFunc(color->GetRTranParam());
	Media::CS::TransferFunc *gtFunc = Media::CS::TransferFunc::CreateFunc(color->GetGTranParam());
	Media::CS::TransferFunc *btFunc = Media::CS::TransferFunc::CreateFunc(color->GetBTranParam());
	if (bpp == 24 || bpp == 32)
	{
		Int16 *srcLUT;
		Int32 *srcLUT32;
		UInt8 *destLUT;
		lut = MemAlloc(UInt8, 512 + 65536 + 1024);
		srcLUT = (Int16*)lut;
		srcLUT32 = (Int32*)(lut + 512 + 65536);
		destLUT = (UInt8*)(srcLUT + 256);

		Double tmp = 1 / 255.0;
		Double tmpD = 1 / 8192.0;
		Int32 v;
		Double lVal;
		OSInt j;
		if (this->gammaParam != 0 && this->gammaCnt > 0)
		{
			i = 256;
			while (i-- > 0)
			{
				lVal = rtFunc->InverseTransfer(i * tmp);
				if (lVal >= 0 && lVal < 1)
				{
					j = (Int32)(lVal * (this->gammaCnt - 1));
					lVal = this->gammaParam[j] + (this->gammaParam[j + 1] - this->gammaParam[j]) * (lVal - j * (1.0 / (this->gammaCnt - 1))) * (this->gammaCnt - 1);
				}
				v = Math::Double2Int32((Math::Pow(lVal, this->gamma) * this->contrast + this->brightness) * 8192);
				srcLUT32[i] = v;
				if (v >= 32768)
					v = 32767;
				srcLUT[i] = (Int16)v;
			}
		}
		else
		{
			i = 256;
			while (i-- > 0)
			{
				lVal = rtFunc->InverseTransfer(i * tmp);
				v = Math::Double2Int32((Math::Pow(lVal, this->gamma) * this->contrast + this->brightness) * 8192);
				srcLUT32[i] = v;
				if (v >= 32768)
					v = 32767;
				srcLUT[i] = (Int16)v;
			}
		}

		i = 65536;
		while (i-- > 32768)
		{
			v = Math::Double2Int32(rtFunc->ForwardTransfer((i - 65536) * tmpD) * 255.0);
			if (v >= 256)
				v = 255;
			else if (v < 0)
				v = 0;
			destLUT[i] = (UInt8)v;
		}
		i = 32768;
		while (i-- > 0)
		{
			v = Math::Double2Int32(rtFunc->ForwardTransfer(i * tmpD) * 255.0);
			if (v >= 256)
				v = 255;
			else if (v < 0)
				v = 0;
			destLUT[i] = (UInt8)v;
		}
	}
	else if (bpp == 48 || bpp == 64 || (bpp == 16 && pf == Media::PF_LE_W16))
	{
		lut = MemAlloc(UInt8, 65536 * 8);
		Int16 *srcLUT = (Int16*)lut;
		Int32 *srcLUT32 = (Int32*)(lut + (65536 * 4));
		UInt16 *destLUT = (UInt16*)(srcLUT + 65536);

		Double tmp = 1 / 65535.0;
		Double tmpD = 1 / 8192.0;
		Int32 v;

		Double lVal;
		OSInt j;
		if (this->gammaParam != 0 && this->gammaCnt > 0)
		{
			i = 65536;
			while (i-- > 0)
			{
				lVal = rtFunc->InverseTransfer(i * tmp);
				if (lVal >= 0 && lVal < 1)
				{
					j = (Int32)(lVal * (this->gammaCnt - 1));
					lVal = this->gammaParam[j] + (this->gammaParam[j + 1] - this->gammaParam[j]) * (lVal - j * (1.0 / (this->gammaCnt - 1))) * (this->gammaCnt - 1);
				}
				v = Math::Double2Int32((Math::Pow(lVal, this->gamma) * this->contrast + this->brightness) * 8192);
				srcLUT32[i] = v;
				if (v >= 32768)
					v = 32767;
				srcLUT[i] = (Int16)v;
			}
		}
		else
		{
			i = 65536;
			while (i-- > 0)
			{
				v = Math::Double2Int32((Math::Pow(rtFunc->InverseTransfer(i * tmp), this->gamma) * this->contrast + this->brightness) * 8192);
				srcLUT32[i] = v;
				if (v >= 32768)
					v = 32767;
				srcLUT[i] = (Int16)v;
			}
		}

		i = 65536;
		while (i-- > 32768)
		{
			v = Math::Double2Int32(rtFunc->ForwardTransfer((i - 65536) * tmpD) * 65535.0);
			if (v >= 65536)
				v = 65535;
			else if (v < 0)
				v = 0;
			destLUT[i] = (Int16)v;
		}
		i = 32768;
		while (i-- > 0)
		{
			v = Math::Double2Int32(rtFunc->ForwardTransfer(i * tmpD) * 65535.0);
			if (v >= 65536)
				v = 65535;
			else if (v < 0)
				v = 0;
			destLUT[i] = (Int16)v;
		}
	}
	DEL_CLASS(rtFunc);
	DEL_CLASS(gtFunc);
	DEL_CLASS(btFunc);
}

void Media::RGBColorFilter::ProcessImage(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt sbpl, OSInt dbpl)
{
	if (this->lut == 0)
		return;
	if (this->bpp == 32 || this->bpp == 48)
	{
		UOSInt lastHeight = height;
		UOSInt currHeight;
		UOSInt i = this->nThread;
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, height, this->nThread);
			this->threadStats[i].srcPtr = srcPtr + currHeight * sbpl;
			this->threadStats[i].destPtr = destPtr + currHeight * dbpl;
			this->threadStats[i].width = width;
			this->threadStats[i].height = lastHeight - currHeight;
			this->threadStats[i].sbpl = sbpl;
			this->threadStats[i].dbpl = dbpl;
			this->threadStats[i].threadStat = 2;
			this->threadStats[i].evt->Set();
			lastHeight = currHeight;
		}
		WaitForThread(2);
	}
}
