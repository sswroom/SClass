#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/CSNV12_LRGBC.h"
#include "Sync/ThreadUtil.h"
#include <float.h>

#define LANCZOS_NTAP 4
#define YVADJ 0.25

extern "C"
{
	void CSNV12_LRGBC_VerticalFilterLRGB(UInt8 *inYPt, UInt8 *inUVPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, UOSInt isFirst, UOSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, UOSInt ystep, OSInt dstep, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
	void CSNV12_LRGBC_do_yv12rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, UOSInt isFirst, UOSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, UOSInt yBpl, UOSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
	void CSNV12_LRGBC_do_yv12rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, UOSInt isFirst, UOSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, UOSInt yBpl, UOSInt uvBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
}

Double Media::CS::CSNV12_LRGBC::lanczos3_weight(Double phase)
{
	Double ret;
	Double aphase = Math_Abs(phase);
	
	if(aphase < DBL_EPSILON)
	{
		return 1.0;
	}

	if ((aphase * 2) >= LANCZOS_NTAP)
	{
		return 0.0;
	}

	ret = Math_Sin(Math::PI * phase) * Math_Sin(Math::PI * phase / LANCZOS_NTAP * 2) / (Math::PI * Math::PI * phase * phase / LANCZOS_NTAP * 2);

	return ret;
}

void Media::CS::CSNV12_LRGBC::SetupInterpolationParameter(UOSInt source_length, UOSInt result_length, NN<YVPARAMETER> out, UOSInt indexSep, Double offsetCorr)
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
	out->index = MemAllocA(OSInt, out->length);
#else
	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);
#endif

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = ((UOSInt2Double(i) + 0.5) * UOSInt2Double(source_length));
		pos /= UOSInt2Double(result_length);
		n = (OSInt)Math_Fix(pos - (LANCZOS_NTAP / 2 - 0.5));//2.5);
		pos = (OSInt2Double(n) + 0.5 - pos);
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
				ind[j] = (Int32)(n * (OSInt)indexSep);
			}
			work[j] = lanczos3_weight(pos + offsetCorr);
			sum += work[j];
			pos += 1;
			n += 1;
		}

		j = 0;
		while (j < out->tap)
		{
			UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[j] / sum) * 32767.0));
			UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[j + 1] / sum) * 32767.0));
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
			}else if(n >= source_length){
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
			UInt16 v1 = (UInt16)(0xffff & Double2Int32((work[j] / sum) * 32767.0));
			UInt16 v2 = (UInt16)(0xffff & Double2Int32((work[j + 1] / sum) * 32767.0));
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

UInt32 Media::CS::CSNV12_LRGBC::WorkerThread(AnyType obj)
{
	NN<CSNV12_LRGBC> converter = obj.GetNN<CSNV12_LRGBC>();
	UOSInt threadId = converter->currId;
	THREADSTAT *ts = &converter->stats[threadId];

	ts->status = 1;
	converter->evtMain.Set();
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
				CSNV12_LRGBC_do_yv12rgb2(ts->yPtr.Ptr(), ts->uvPtr.Ptr(), ts->dest.Ptr(), ts->width, ts->height, ts->dbpl, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, ts->yBpl, ts->uvBpl, converter->yuv2rgb.Ptr(), converter->rgbGammaCorr.Ptr());
			}
			else
			{
				CSNV12_LRGBC_do_yv12rgb8(ts->yPtr.Ptr(), ts->uvPtr.Ptr(), ts->dest.Ptr(), ts->width, ts->height, ts->dbpl, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, ts->yBpl, ts->uvBpl, converter->yuv2rgb.Ptr(), converter->rgbGammaCorr.Ptr());
			}
			ts->status = 4;
			converter->evtMain.Set();
		}
		else if (ts->status == 11)
		{
#if LANCZOS_NTAP == 4
			CSNV12_LRGBC_VerticalFilterLRGB(ts->yPtr.Ptr(), ts->uvPtr.Ptr(), ts->dest.Ptr(), ts->width, ts->height, ts->yvParam->tap, ts->yvParam->index + ts->uvBpl, ts->yvParam->weight + ts->uvBpl * 6, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, ts->yBpl, ts->dbpl, converter->yuv2rgb14.Ptr(), converter->rgbGammaCorr.Ptr());
#else
			CSNV12_LRGBC_VerticalFilterLRGB(ts->yPtr.Ptr(), ts->uvPtr.Ptr(), ts->dest.Ptr(), ts->width, ts->height, ts->yvParam->tap, ts->yvParam->index + ts->uvBpl * LANCZOS_NTAP, ts->yvParam->weight + ts->uvBpl * LANCZOS_NTAP, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, ts->yBpl, ts->dbpl, converter->yuv2rgb14.Ptr(), converter->rgbGammaCorr.Ptr());
#endif
			ts->status = 4;
			converter->evtMain.Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain.Set();
	return 0;
}

void Media::CS::CSNV12_LRGBC::WaitForWorker(Int32 jobStatus)
{
	UOSInt i;
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
		this->evtMain.Wait();
	}
}

Media::CS::CSNV12_LRGBC::CSNV12_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess) : Media::CS::CSYUV_LRGBC(srcProfile, destProfile, yuvType, colorSess)
{
	UOSInt i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
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

	stats = MemAllocArr(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASSNN(stats[i].evt, Sync::Event());
		stats[i].status = 0;
		stats[i].csLineSize = 0;
		stats[i].csLineBuff = 0;
		stats[i].csLineBuff2 = 0;

		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this, 65536);
		while (stats[i].status == 0)
		{
			this->evtMain.Wait();
		}
	}
}

Media::CS::CSNV12_LRGBC::~CSNV12_LRGBC()
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

		this->evtMain.Wait(100);
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
		stats[i].evt.Delete();
	}
	MemFreeArr(stats);

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

void Media::CS::CSNV12_LRGBC::ConvertV2(UnsafeArray<UnsafeArray<UInt8>> srcPtr2, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UInt32 isLast = 1;
	UInt32 isFirst = 0;
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;
	UOSInt cSize = dispWidth << 4;
	UnsafeArray<UInt8> srcPtr = srcPtr2[0];
	if (srcStoreWidth & 1)
	{
		srcStoreWidth = srcStoreWidth + 1;
	}
	if (srcStoreHeight & 1)
	{
		srcStoreHeight = srcStoreHeight + 1;
	}
	
	if (ftype == Media::FT_MERGED_TF || ftype == Media::FT_MERGED_BF)
	{
		UnsafeArray<UInt8> uvStart = srcPtr + srcStoreWidth * srcStoreHeight;

		if (ftype == Media::FT_MERGED_TF)
		{
		}
		else
		{
			uvStart += srcStoreWidth;
			srcPtr += srcStoreWidth;
		}

		if ((ycOfst == Media::YCOFST_C_CENTER_LEFT || ycOfst == Media::YCOFST_C_CENTER_CENTER) && (srcStoreWidth & 7) == 0)
		{
			if (ftype == Media::FT_MERGED_TF)
			{
				if (this->yvParamO.index == 0 || (this->yvParamO.length != dispHeight >> 1) || this->yvStepO != (srcStoreWidth << 1))
				{
					if (this->yvParamO.index)
					{
						MemFreeA(this->yvParamO.weight);
						MemFreeA(this->yvParamO.index);
						this->yvParamO.weight = 0;
						this->yvParamO.index = 0;
					}
					this->yvStepO = srcStoreWidth << 1;
					SetupInterpolationParameter(dispHeight >> 2, dispHeight >> 1, this->yvParamO, this->yvStepO, 0);
				}
			}
			else
			{
				if (this->yvParamE.index == 0 || (this->yvParamE.length != dispHeight >> 1) || this->yvStepE != (srcStoreWidth << 1))
				{
					if (this->yvParamE.index)
					{
						MemFreeA(this->yvParamE.weight);
						MemFreeA(this->yvParamE.index);
						this->yvParamE.weight = 0;
						this->yvParamE.index = 0;
					}
					this->yvStepE = srcStoreWidth << 1;
					SetupInterpolationParameter(dispHeight >> 2, dispHeight >> 1, this->yvParamE, this->yvStepE, YVADJ);
				}
			}

			i = this->nThread;
			isLast = 1;
			lastHeight = dispHeight >> 1;
			while (i-- > 0)
			{
				if (i == 0)
					isFirst = 1;
				currHeight = MulDivUOS(i, dispHeight >> 1, this->nThread) & (UOSInt)~1;

				if (ftype == Media::FT_MERGED_TF)
				{
					stats[i].yPtr = srcPtr + (srcStoreWidth * currHeight << 1);
					stats[i].uvPtr = uvStart;
					stats[i].yvParam = this->yvParamO;
				}
				else
				{
					stats[i].yPtr = srcPtr + (srcStoreWidth * (currHeight << 1));
					stats[i].uvPtr = uvStart;
					stats[i].yvParam = this->yvParamE;
				}
				stats[i].yBpl = srcStoreWidth << 1;
				stats[i].uvBpl = currHeight;
				stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
				stats[i].isFirst = isFirst;
				stats[i].isLast = isLast;
				stats[i].ycOfst = ycOfst;
				isLast = 0;
				stats[i].width = dispWidth;
				stats[i].height = lastHeight - currHeight;
				stats[i].dbpl = destRGBBpl;

				if (stats[i].csLineSize < dispWidth)
				{
					if (stats[i].csLineBuff)
						MemFreeA64(stats[i].csLineBuff);
					if (stats[i].csLineBuff2)
						MemFreeA64(stats[i].csLineBuff2);
					stats[i].csLineSize = dispWidth;
					stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
					stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
				}
				stats[i].status = 11;
				stats[i].evt->Set();
				lastHeight = currHeight;
			}
			WaitForWorker(11);

		}
		else
		{
			lastHeight = dispHeight >> 1;
			while (i-- > 0)
			{
				if (i == 0)
					isFirst = 1;
				currHeight = MulDivUOS(i, dispHeight >> 1, nThread) & (UOSInt)~1;

				stats[i].yPtr = srcPtr + srcStoreWidth * (currHeight << 1);
				stats[i].yBpl = srcStoreWidth << 1;
				stats[i].uvPtr = uvStart + (srcStoreWidth * currHeight);
				stats[i].uvBpl = srcStoreWidth << 1;
				stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
				stats[i].isFirst = isFirst;
				stats[i].isLast = isLast;
				stats[i].ycOfst = ycOfst;
				isLast = 0;
				stats[i].width = dispWidth;
				stats[i].height = lastHeight - currHeight;
				stats[i].dbpl = destRGBBpl;

				if (stats[i].csLineSize < dispWidth)
				{
					if (stats[i].csLineBuff)
						MemFreeA64(stats[i].csLineBuff);
					if (stats[i].csLineBuff2)
						MemFreeA64(stats[i].csLineBuff2);
					stats[i].csLineSize = dispWidth;
					stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1) + 15);
					stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1) + 15);
				}
				stats[i].status = 3;
				stats[i].evt->Set();
				lastHeight = currHeight;
			}
			WaitForWorker(3);
		}
	}
	else if (ftype == Media::FT_INTERLACED_BFF || ftype == Media::FT_INTERLACED_TFF || ftype == Media::FT_INTERLACED_NODEINT)
	{
		UOSInt j;
		if (i & 1)
		{
			
		}
		else
		{
			if ((ycOfst == Media::YCOFST_C_CENTER_LEFT || ycOfst == Media::YCOFST_C_CENTER_CENTER) && (srcStoreWidth & 3) == 0)
			{
				UnsafeArray<UInt8> uvStart = srcPtr + srcStoreWidth * srcStoreHeight;

				if (this->yvParamO.index == 0 || this->yvParamO.length != (dispHeight >> 1) || this->yvStepO != (srcStoreWidth << 1))
				{
					if (this->yvParamO.index)
					{
						MemFreeA(this->yvParamO.weight);
						MemFreeA(this->yvParamO.index);
						this->yvParamO.weight = 0;
						this->yvParamO.index = 0;
					}
					this->yvStepO = srcStoreWidth << 1;
					SetupInterpolationParameter(dispHeight >> 2, dispHeight >> 1, this->yvParamO, this->yvStepO, 0);
				}
				if (this->yvParamE.index == 0 || this->yvParamE.length != (dispHeight >> 1) || this->yvStepE != (srcStoreWidth << 1))
				{
					if (this->yvParamE.index)
					{
						MemFreeA(this->yvParamE.weight);
						MemFreeA(this->yvParamE.index);
						this->yvParamE.weight = 0;
						this->yvParamE.index = 0;
					}
					this->yvStepE = srcStoreWidth << 1;
					SetupInterpolationParameter(dispHeight >> 2, dispHeight >> 1, this->yvParamE, this->yvStepE, YVADJ);
				}

				j = this->nThread >> 1;
				isLast = 1;
				isFirst = 0;
				lastHeight = dispHeight;
				while (i-- > j)
				{
					if (i == j)
						isFirst = 1;
					currHeight = MulDivUOS(i - j, dispHeight, j) & (UOSInt)~3;

					stats[i].yPtr = srcPtr + srcStoreWidth * currHeight;
					stats[i].uvPtr = uvStart;
					stats[i].yBpl = srcStoreWidth << 1;
					stats[i].uvBpl = currHeight >> 1;
					stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
					stats[i].isFirst = isFirst;
					stats[i].isLast = isLast;
					stats[i].ycOfst = ycOfst;
					isLast = 0;
					stats[i].width = dispWidth;
					stats[i].height = (lastHeight - currHeight) >> 1;
					stats[i].dbpl = destRGBBpl << 1;
					stats[i].yvParam = this->yvParamO;

					if (stats[i].csLineSize < dispWidth)
					{
						if (stats[i].csLineBuff)
							MemFreeA64(stats[i].csLineBuff);
						if (stats[i].csLineBuff2)
							MemFreeA64(stats[i].csLineBuff2);
						stats[i].csLineSize = dispWidth;
						stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
						stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
					}
					stats[i].status = 11;
					stats[i].evt->Set();
					lastHeight = currHeight;
				}
				i = j;
				isLast = 1;
				isFirst = 0;
				lastHeight = dispHeight + 1;
				while (i-- > 0)
				{
					if (i == 0)
						isFirst = 1;
					currHeight = (MulDivUOS(i, dispHeight, j) & (UOSInt)~3) + 1;

					stats[i].yPtr = srcPtr + srcStoreWidth * currHeight;
					stats[i].uvPtr = uvStart + srcStoreWidth;
					stats[i].yBpl = srcStoreWidth << 1;
					stats[i].uvBpl = currHeight >> 1;
					stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
					stats[i].isFirst = isFirst;
					stats[i].isLast = isLast;
					stats[i].ycOfst = ycOfst;
					isLast = 0;
					stats[i].width = dispWidth;
					stats[i].height = (lastHeight - currHeight) >> 1;
					stats[i].dbpl = destRGBBpl << 1;
					stats[i].yvParam = this->yvParamE;

					if (stats[i].csLineSize < dispWidth)
					{
						if (stats[i].csLineBuff)
							MemFreeA64(stats[i].csLineBuff);
						if (stats[i].csLineBuff2)
							MemFreeA64(stats[i].csLineBuff2);
						stats[i].csLineSize = dispWidth;
						stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
						stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
					}
					stats[i].status = 11;
					stats[i].evt->Set();
					lastHeight = currHeight;
				}
				WaitForWorker(11);
			}
			else
			{
				j = this->nThread >> 1;

				isLast = 1;
				while (i-- > j)
				{
					if (i == j)
						isFirst = 1;
					currHeight = MulDivUOS(i - j, dispHeight, j) & (UOSInt)~3;

					stats[i].yPtr = srcPtr + srcStoreWidth * currHeight;
					stats[i].yBpl = srcStoreWidth << 1;
					stats[i].uvPtr = srcPtr + srcStoreWidth * srcStoreHeight + (srcStoreWidth * (currHeight >> 1));
					stats[i].uvBpl = srcStoreWidth << 1;
					stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
					stats[i].isFirst = isFirst;
					stats[i].isLast = isLast;
					stats[i].ycOfst = ycOfst;
					isLast = 0;
					stats[i].width = dispWidth;
					stats[i].height = (lastHeight - currHeight) >> 1;
					stats[i].dbpl = destRGBBpl << 1;

					if (stats[i].csLineSize < dispWidth)
					{
						if (stats[i].csLineBuff)
							MemFreeA64(stats[i].csLineBuff);
						if (stats[i].csLineBuff2)
							MemFreeA64(stats[i].csLineBuff2);
						stats[i].csLineSize = dispWidth;
						stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
						stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
					}
					stats[i].status = 3;
					stats[i].evt->Set();
					lastHeight = currHeight;
				}

				i = j;
				isLast = 1;
				isFirst = 0;
				lastHeight = dispHeight + 1;
				while (i-- > 0)
				{
					if (i == 0)
						isFirst = 1;
					currHeight = (MulDivUOS(i, dispHeight, j) & (UOSInt)~3) + 1;

					stats[i].yPtr = srcPtr + srcStoreWidth * currHeight;
					stats[i].yBpl = srcStoreWidth << 1;
					stats[i].uvPtr = srcPtr + srcStoreWidth * srcStoreHeight + (srcStoreWidth * ((currHeight >> 1) + 1));
					stats[i].uvBpl = srcStoreWidth << 1;
					stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
					stats[i].isFirst = isFirst;
					stats[i].isLast = isLast;
					stats[i].ycOfst = ycOfst;
					isLast = 0;
					stats[i].width = dispWidth;
					stats[i].height = (lastHeight - currHeight) >> 1;
					stats[i].dbpl = destRGBBpl << 1;

					if (stats[i].csLineSize < dispWidth)
					{
						if (stats[i].csLineBuff)
							MemFreeA64(stats[i].csLineBuff);
						if (stats[i].csLineBuff2)
							MemFreeA64(stats[i].csLineBuff2);
						stats[i].csLineSize = dispWidth;
						stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
						stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
					}
					stats[i].status = 3;
					stats[i].evt->Set();
					lastHeight = currHeight;
				}
				WaitForWorker(3);
			}
		}
	}
	else
	{
		if ((ycOfst == Media::YCOFST_C_CENTER_LEFT || ycOfst == Media::YCOFST_C_CENTER_CENTER))
		{
			UnsafeArray<UInt8> uvStart = srcPtr + srcStoreWidth * srcStoreHeight;

			if (this->yvParamO.index == 0 || this->yvParamO.length != dispHeight || this->yvStepO != srcStoreWidth)
			{
				if (this->yvParamO.index)
				{
					MemFreeA(this->yvParamO.weight);
					MemFreeA(this->yvParamO.index);
					this->yvParamO.weight = 0;
					this->yvParamO.index = 0;
				}
				this->yvStepO = srcStoreWidth;
				SetupInterpolationParameter(dispHeight >> 1, dispHeight, this->yvParamO, this->yvStepO, 0);
			}

			i = this->nThread;
			isLast = 1;
			lastHeight = dispHeight;
			while (i-- > 0)
			{
				if (i == 0)
					isFirst = 1;
				currHeight = MulDivUOS(i, dispHeight, this->nThread);

				stats[i].yPtr = srcPtr + srcStoreWidth * currHeight;
				stats[i].uvPtr = uvStart;
				stats[i].yBpl = srcStoreWidth;
				stats[i].uvBpl = currHeight;
				stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
				stats[i].isFirst = isFirst;
				stats[i].isLast = isLast;
				stats[i].ycOfst = ycOfst;
				isLast = 0;
				stats[i].width = dispWidth;
				stats[i].height = lastHeight - currHeight;
				stats[i].dbpl = destRGBBpl;
				stats[i].yvParam = this->yvParamO;

				if (stats[i].csLineSize < dispWidth)
				{
					if (stats[i].csLineBuff)
						MemFreeA64(stats[i].csLineBuff);
					if (stats[i].csLineBuff2)
						MemFreeA64(stats[i].csLineBuff2);
					stats[i].csLineSize = dispWidth;
					stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
					stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
				}
				stats[i].status = 11;
				stats[i].evt->Set();
				lastHeight = currHeight;
			}
			WaitForWorker(11);
		}
		else
		{
			while (i-- > 0)
			{
				if (i == 0)
					isFirst = 1;
				currHeight = MulDivUOS(i, dispHeight, nThread) & (UOSInt)~1;

				stats[i].yPtr = srcPtr + srcStoreWidth * currHeight;
				stats[i].yBpl = srcStoreWidth;
				stats[i].uvPtr = srcPtr + srcStoreWidth * srcStoreHeight + ((srcStoreWidth * currHeight) >> 1);
				stats[i].uvBpl = srcStoreWidth;
				stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
				stats[i].isFirst = isFirst;
				stats[i].isLast = isLast;
				stats[i].ycOfst = ycOfst;
				isLast = 0;
				stats[i].width = dispWidth;
				stats[i].height = lastHeight - currHeight;
				stats[i].dbpl = destRGBBpl;

				if (stats[i].csLineSize < dispWidth)
				{
					if (stats[i].csLineBuff)
						MemFreeA64(stats[i].csLineBuff);
					if (stats[i].csLineBuff2)
						MemFreeA64(stats[i].csLineBuff2);
					stats[i].csLineSize = dispWidth;
					stats[i].csLineBuff = MemAllocA64(UInt8, (cSize << 1));
					stats[i].csLineBuff2 = MemAllocA64(UInt8, (cSize << 1));
				}
				stats[i].status = 3;
				stats[i].evt->Set();
				lastHeight = currHeight;
			}
			WaitForWorker(3);
		}
	}
}

UOSInt Media::CS::CSNV12_LRGBC::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return (width * height * 3) >> 1;
}
