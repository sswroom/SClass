#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/CSYUV420_RGB32C.h"
#include "Sync/Thread.h"
#include <float.h>

#define LANCZOS_NTAP 4

extern "C"
{
	void CSYUV420_RGB32C_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUPt, UInt8 *inVPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt tap, OSInt *index, Int64 *weight, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt ystep, OSInt dstep, Int64 *yuv2rgb14, Int64 *rgbGammaCorr);
	void CSYUV420_RGB32C_do_yv12rgb8(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
	void CSYUV420_RGB32C_do_yv12rgb2(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, OSInt isFirst, OSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, OSInt yBpl, OSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
}

Double Media::CS::CSYUV420_RGB32C::lanczos3_weight(Double phase)
{
	Double ret;
	Double aphase = Math::Abs(phase);
	
	if(aphase < DBL_EPSILON)
	{
		return 1.0;
	}

	if ((aphase * 2) >= LANCZOS_NTAP)
	{
		return 0.0;
	}

	ret = Math::Sin(Math::PI * phase) * Math::Sin(Math::PI * phase / LANCZOS_NTAP * 2) / (Math::PI * Math::PI * phase * phase / LANCZOS_NTAP * 2);

	return ret;
}

void Media::CS::CSYUV420_RGB32C::SetupInterpolationParameter(UOSInt source_length, UOSInt result_length, YVPARAMETER *out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double *work;
	Double  sum;
	Double  pos;

	out->length = result_length;
	out->tap = LANCZOS_NTAP;
#if LANCZOS_NTAP == 4
	Int32 *ind;
	out->weight = MemAllocA(Int64, out->length * 6);
	out->index = MemAllocA(OSInt, 1);
#else
	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);
#endif

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (i + 0.5) * source_length;
		pos /= result_length;
		n = (OSInt)Math::Fix(pos - (LANCZOS_NTAP / 2 - 0.5));//2.5);
		pos = (n + 0.5 - pos);
		sum = 0;
#if LANCZOS_NTAP == 4
		ind = (Int32*)&out->weight[i * 6];
		for(j = 0; j < out->tap; j++)
		{
			if(n < 0){
				ind[j] = 0;
			}else if(n >= (OSInt)source_length){
				ind[j] = (Int32)((source_length - 1) * indexSep);
			}else{
				ind[j] = (Int32)(n * indexSep);
			}
			work[j] = lanczos3_weight(pos + offsetCorr);
			sum += work[j];
			pos += 1;
			n += 1;
		}

		j = 0;
		while (j < out->tap)
		{
			UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[j] / sum) * 32767.0));
			UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[j + 1] / sum) * 32767.0));
			UInt16 *tmpPtr = (UInt16*)&out->weight[i * 6 + j + 2];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;
			j += 2;
		}
#else
		for(j = 0; j < out->tap; j++)
		{
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= (OSInt)source_length){
				out->index[i * out->tap + j] = (source_length - 1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos3_weight(pos + offsetCorr);
			sum += work[j];
			pos += 1;
			n += 1;
		}

		j = 0;
		while (j < out->tap)
		{
			UInt16 v1 = (UInt16)(0xffff & Math::Double2Int32((work[j] / sum) * 32767.0));
			UInt16 v2 = (UInt16)(0xffff & Math::Double2Int32((work[j + 1] / sum) * 32767.0));
			UInt16 *tmpPtr = (UInt16*)&out->weight[i * out->tap + j];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			tmpPtr[2] = v1;
			tmpPtr[3] = v2;
			tmpPtr[4] = v1;
			tmpPtr[5] = v2;
			tmpPtr[6] = v1;
			tmpPtr[7] = v2;
			j += 2;
		}
#endif
		i++;
	}

	MemFree(work);
}

UInt32 Media::CS::CSYUV420_RGB32C::WorkerThread(void *obj)
{
	CSYUV420_RGB32C *converter = (CSYUV420_RGB32C*)obj;
	OSInt threadId = converter->currId;
	THREADSTAT *ts = &converter->stats[threadId];

	ts->status = 1;
	converter->evtMain->Set();
	while (true)
	{
		ts->evt->Wait();
		if (ts->status == 2)
		{
			break;
		}
		else if (ts->status == 3)
		{
			if (ts->width & 3)
			{
				CSYUV420_RGB32C_do_yv12rgb2(ts->yPtr, ts->uPtr, ts->vPtr, ts->dest, ts->width, ts->height, ts->dbpl, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, ts->yBpl, ts->uvBpl, converter->yuv2rgb, converter->rgbGammaCorr);
			}
			else
			{
				CSYUV420_RGB32C_do_yv12rgb8(ts->yPtr, ts->uPtr, ts->vPtr, ts->dest, ts->width, ts->height, ts->dbpl, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, ts->yBpl, ts->uvBpl, converter->yuv2rgb, converter->rgbGammaCorr);
			}
			ts->status = 4;
			converter->evtMain->Set();
		}
		else if (ts->status == 11)
		{
#if LANCZOS_NTAP == 4
			CSYUV420_RGB32C_VerticalFilterLRGB(ts->yPtr, ts->uPtr, ts->vPtr, ts->dest, ts->width, ts->height, ts->yvParam->tap, ts->yvParam->index + ts->uvBpl, ts->yvParam->weight + ts->uvBpl * 6, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, ts->yBpl, ts->dbpl, converter->yuv2rgb14, converter->rgbGammaCorr);
#else
			CSYUV420_RGB32C_VerticalFilterLRGB(ts->yPtr, ts->uPtr, ts->vPtr, ts->dest, ts->width, ts->height, ts->yvParam->tap, ts->yvParam->index + ts->uvBpl * LANCZOS_NTAP, ts->yvParam->weight + ts->uvBpl * LANCZOS_NTAP, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, ts->yBpl, ts->dbpl, converter->yuv2rgb14, converter->rgbGammaCorr);
#endif
			ts->status = 4;
			converter->evtMain->Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain->Set();
	return 0;
}

void Media::CS::CSYUV420_RGB32C::WaitForWorker(Int32 jobStatus)
{
	OSInt i;
	Bool exited;
	while (true)
	{
		exited = true;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->stats[i].status == jobStatus)
			{
				this->stats[i].evt->Set();
				exited = false;
				break;
			}
		}
		if (exited)
			break;
		this->evtMain->Wait();
	}
}

Media::CS::CSYUV420_RGB32C::CSYUV420_RGB32C(const Media::ColorProfile *srcProfile, const Media::ColorProfile *destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess, Media::PixelFormat destPF) : Media::CS::CSYUV_RGB32C(srcProfile, destProfile, yuvType, colorSess, destPF)
{
	UOSInt i;
	this->nThread = Sync::Thread::GetThreadCnt();
	if (this->nThread > 4)
	{
		this->nThread = this->nThread >> 1;
	}
	this->yvParamO.weight = 0;
	this->yvParamO.index = 0;
	this->yvParamE.weight = 0;
	this->yvParamE.index = 0;
	this->uBuff = 0;
	this->vBuff = 0;
	this->yvBuffSize = 0;
	this->uvBuff = 0;
	this->uvBuffSize = 0;

	NEW_CLASS(evtMain, Sync::Event((const UTF8Char*)"Media.CS.CSYV12_LRGB.evtMain"));
	stats = MemAlloc(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event((const UTF8Char*)"Media.CS.CSYV12_LRGB.stats.evt"));
		stats[i].status = 0;
		stats[i].csLineSize = 0;
		stats[i].csLineBuff = 0;
		stats[i].csLineBuff2 = 0;

		currId = i;
		Sync::Thread::Create(WorkerThread, this, 65536);
		while (stats[i].status == 0)
		{
			evtMain->Wait();
		}
	}
}

Media::CS::CSYUV420_RGB32C::~CSYUV420_RGB32C()
{
	UOSInt i = nThread;
	Bool exited;
	while (i-- > 0)
	{
		if (stats[i].status != 0)
		{
			stats[i].status = 2;
			stats[i].evt->Set();
		}
	}
	while (true)
	{
		exited = true;
		i = nThread;
		while (i-- > 0)
		{
			if (stats[i].status != 0)
			{
				if (stats[i].status == 2)
				{
					stats[i].evt->Set();
					exited = false;
					break;
				}
				else if (stats[i].status > 0)
				{
					stats[i].status = 2;
					stats[i].evt->Set();
					exited = false;
					break;
				}
				else
				{
					stats[i].evt->Set();
					exited = false;
					break;
				}
			}
		}
		if (exited)
			break;

		evtMain->Wait(100);
	}
	i = nThread;
	while (i-- > 0)
	{
		if (stats[i].csLineBuff)
		{
			MemFreeA(stats[i].csLineBuff);
			stats[i].csLineBuff = 0;
		}
		if (stats[i].csLineBuff2)
		{
			MemFreeA(stats[i].csLineBuff2);
			stats[i].csLineBuff2 = 0;
		}
		DEL_CLASS(stats[i].evt);
	}
	DEL_CLASS(evtMain);
	MemFree(stats);

	if (this->yvParamO.index)
	{
		MemFreeA(this->yvParamO.weight);
		MemFreeA(this->yvParamO.index);
		this->yvParamO.weight = 0;
		this->yvParamO.index = 0;
	}
	if (this->yvParamE.index)
	{
		MemFreeA(this->yvParamE.weight);
		MemFreeA(this->yvParamE.index);
		this->yvParamE.weight = 0;
		this->yvParamE.index = 0;
	}
	if (this->uBuff)
	{
		MemFreeA64(this->uBuff);
		MemFreeA64(this->vBuff);
		this->uBuff = 0;
		this->vBuff = 0;
		this->yvBuffSize = 0;
	}
	if (this->uvBuff)
	{
		MemFreeA64(this->uvBuff);
		this->uvBuff = 0;
		this->uvBuffSize = 0;
	}
}

UOSInt Media::CS::CSYUV420_RGB32C::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return (width * height * 3) >> 1;
}
