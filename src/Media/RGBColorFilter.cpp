#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/CPUInst.h"
#include "Math/Math_C.h"
#include "Media/RGBColorFilter.h"
#include "Media/CS/TransferFunc.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void RGBColorFilter_ProcessImagePart(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt sAdd, OSInt dAdd, UInt8 *lut, UInt32 bpp);
	void RGBColorFilter_ProcessImageHDRPart(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt sAdd, OSInt dAdd, UInt8 *lut, UInt32 bpp);
	void RGBColorFilter_ProcessImageHDRDPart(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt sAdd, OSInt dAdd, UInt8 *lut, UInt32 bpp);
	void RGBColorFilter_ProcessImageHDRDLPart(UInt8 *srcPtr, UInt8 *destPtr, UOSInt width, UOSInt height, OSInt sAdd, OSInt dAdd, UInt8 *lut, UInt32 bpp, UInt32 hdrLev);
}

UInt32 __stdcall Media::RGBColorFilter::ProcessThread(AnyType userObj)
{
	NN<ThreadStat> tstat = userObj.GetNN<ThreadStat>();
	{
		Sync::Event evt;
		tstat->evt = &evt;
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
					RGBColorFilter_ProcessImageHDRDLPart(tstat->srcPtr.Ptr(), tstat->destPtr.Ptr(), tstat->width, tstat->height, tstat->sAdd, tstat->dAdd, tstat->me->lut, tstat->me->bpp, tstat->me->hdrLev);
				}
				else
				{
					RGBColorFilter_ProcessImagePart(tstat->srcPtr.Ptr(), tstat->destPtr.Ptr(), tstat->width, tstat->height, tstat->sAdd, tstat->dAdd, tstat->me->lut, tstat->me->bpp);
				}
				tstat->threadStat = 1;
				tstat->me->threadEvt.Set();
			}
			tstat->evt->Wait(10000);
		}
	}
	tstat->threadStat = 4;
	tstat->me->threadEvt.Set();
	return 0;
}

void Media::RGBColorFilter::WaitForThread(Int32 stat)
{
	UOSInt i;
	Bool found = true;
	while (found)
	{
		this->threadEvt.Wait(1000);
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

Media::RGBColorFilter::RGBColorFilter(NN<Media::ColorManager> colorMgr)
{
	this->colorMgr = colorMgr;
	this->lut = 0;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	this->threadStats = MemAllocArr(ThreadStat, this->nThread);
	this->hdrLev = 0;
	this->gammaParam = 0;
	this->gammaCnt = 0;
	this->pf = Media::PF_UNKNOWN;
#if defined(CPU_X86_64) || defined(CPU_X86_32)
	this->hasSSE41 = Manage::CPUInst::HasInstruction(Manage::CPUInst::IT_SSE41);
#else
	this->hasSSE41 = true;
#endif
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		this->threadStats[i].threadStat = 0;
		this->threadStats[i].me = *this;
		Sync::ThreadUtil::Create(ProcessThread, &this->threadStats[i]);
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
		this->threadEvt.Wait(1000);
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
	MemFreeArr(this->threadStats);
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

void Media::RGBColorFilter::SetGammaCorr(UnsafeArrayOpt<Double> gammaParam, UOSInt gammaCnt)
{
	if (this->gammaParam)
	{
		MemFree(this->gammaParam);
	}
	UnsafeArray<Double> nngammaParam;
	if (gammaParam.SetTo(nngammaParam) && gammaCnt > 0)
	{
		this->gammaCnt = gammaCnt;
		this->gammaParam = MemAlloc(Double, gammaCnt);
		MemCopyNO(this->gammaParam, nngammaParam.Ptr(), sizeof(Double) * gammaCnt);
	}
	else
	{
		this->gammaCnt = 0;
		this->gammaParam = 0;
	}
}

void Media::RGBColorFilter::SetParameter(Double brightness, Double contrast, Double gamma, NN<const Media::ColorProfile> color, UInt32 bpp, Media::PixelFormat pf, UInt32 hdrLev)
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

	UOSInt i;
	NN<Media::CS::TransferFunc> rtFunc = Media::CS::TransferFunc::CreateFunc(color->GetRTranParamRead());
	NN<Media::CS::TransferFunc> gtFunc = Media::CS::TransferFunc::CreateFunc(color->GetGTranParamRead());
	NN<Media::CS::TransferFunc> btFunc = Media::CS::TransferFunc::CreateFunc(color->GetBTranParamRead());
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
		UOSInt j;
		if (this->gammaParam != 0 && this->gammaCnt > 0)
		{
			Double gammaCntM1 = UOSInt2Double(this->gammaCnt - 1);
			i = 256;
			while (i-- > 0)
			{
				lVal = rtFunc->InverseTransfer(UOSInt2Double(i) * tmp);
				if (lVal >= 0 && lVal < 1)
				{
					j = (UInt32)(lVal * gammaCntM1);
					lVal = this->gammaParam[j] + (this->gammaParam[j + 1] - this->gammaParam[j]) * (lVal - UOSInt2Double(j) * (1.0 / gammaCntM1)) * gammaCntM1;
				}
				v = Double2Int32((Math_Pow(lVal, this->gamma) * this->contrast + this->brightness) * 8192);
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
				lVal = rtFunc->InverseTransfer(UOSInt2Double(i) * tmp);
				v = Double2Int32((Math_Pow(lVal, this->gamma) * this->contrast + this->brightness) * 8192);
				srcLUT32[i] = v;
				if (v >= 32768)
					v = 32767;
				srcLUT[i] = (Int16)v;
			}
		}

		i = 65536;
		while (i-- > 32768)
		{
			v = Double2Int32(rtFunc->ForwardTransfer(OSInt2Double((OSInt)i - 65536) * tmpD) * 255.0);
			if (v >= 256)
				v = 255;
			else if (v < 0)
				v = 0;
			destLUT[i] = (UInt8)v;
		}
		i = 32768;
		while (i-- > 0)
		{
			v = Double2Int32(rtFunc->ForwardTransfer(UOSInt2Double(i) * tmpD) * 255.0);
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
			Double gammaCntM1 = UOSInt2Double(this->gammaCnt - 1);
			i = 65536;
			while (i-- > 0)
			{
				lVal = rtFunc->InverseTransfer(UOSInt2Double(i) * tmp);
				if (lVal >= 0 && lVal < 1)
				{
					j = (Int32)(lVal * gammaCntM1);
					lVal = this->gammaParam[j] + (this->gammaParam[j + 1] - this->gammaParam[j]) * (lVal - OSInt2Double(j) * (1.0 / gammaCntM1)) * gammaCntM1;
				}
				v = Double2Int32((Math_Pow(lVal, this->gamma) * this->contrast + this->brightness) * 8192);
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
				v = Double2Int32((Math_Pow(rtFunc->InverseTransfer(UOSInt2Double(i) * tmp), this->gamma) * this->contrast + this->brightness) * 8192);
				srcLUT32[i] = v;
				if (v >= 32768)
					v = 32767;
				srcLUT[i] = (Int16)v;
			}
		}

		i = 65536;
		while (i-- > 32768)
		{
			destLUT[i] = Math::SDouble2UInt16(rtFunc->ForwardTransfer(OSInt2Double((OSInt)i - 65536) * tmpD) * 65535.0);
		}
		i = 32768;
		while (i-- > 0)
		{
			destLUT[i] = Math::SDouble2UInt16(rtFunc->ForwardTransfer(UOSInt2Double(i) * tmpD) * 65535.0);
		}
	}
	rtFunc.Delete();
	gtFunc.Delete();
	btFunc.Delete();
}

void Media::RGBColorFilter::ProcessImage(UnsafeArray<UInt8> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt width, UOSInt height, UOSInt sbpl, UOSInt dbpl, Bool upsideDown)
{
	if (this->lut == 0)
		return;
	if (this->bpp == 32 || this->bpp == 48)
	{
		OSInt sAdd = (OSInt)sbpl;
		OSInt dAdd;
		if (upsideDown)
		{
			destPtr += dbpl * (height - 1);
			dAdd = -(OSInt)dbpl;
		}
		else
		{
			dAdd = (OSInt)dbpl;
		}
		UOSInt lastHeight = height;
		UOSInt currHeight;
		UOSInt i = this->nThread;
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, height, this->nThread);
			this->threadStats[i].srcPtr = srcPtr + (OSInt)currHeight * sAdd;
			this->threadStats[i].destPtr = destPtr + (OSInt)currHeight * dAdd;
			this->threadStats[i].width = width;
			this->threadStats[i].height = lastHeight - currHeight;
			this->threadStats[i].sAdd = sAdd;
			this->threadStats[i].dAdd = dAdd;
			this->threadStats[i].threadStat = 2;
			this->threadStats[i].evt->Set();
			lastHeight = currHeight;
		}
		WaitForThread(2);
	}
}
