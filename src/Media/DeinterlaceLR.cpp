#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/DeinterlaceLR.h"
#include "Sync/Event.h"
#include "Sync/ThreadUtil.h"
#include <float.h>
#include <math.h>

#define LANCZOS_NTAP 4
#define PI 3.141592653589793

extern "C"
{
	void DeinterlaceLR_VerticalFilter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
	void DeinterlaceLR_VerticalFilterOdd(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
	void DeinterlaceLR_VerticalFilterEven(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep);
}

Double Media::DeinterlaceLR::lanczos3_weight(Double phase)
{
	Double ret;
	
	if(fabs(phase) < DBL_EPSILON)
	{
		return 1.0;
	}

	if ((fabs(phase) * 2) >= LANCZOS_NTAP)
	{
		return 0.0;
	}

	ret = sin(PI * phase) * sin(PI * phase / LANCZOS_NTAP * 2) / (PI * PI * phase * phase / LANCZOS_NTAP * 2);

	return ret;
}

void Media::DeinterlaceLR::SetupInterpolationParameter(UOSInt source_length, UOSInt result_length, NN<DIPARAMETER> out, UOSInt indexSep, Double offsetCorr)
{
	UInt32 i,j;
	Int32 n;
	Double *work;
	Double  sum;
	Double  pos;
	Double dslen = UOSInt2Double(source_length);
	Double drlen = UOSInt2Double(result_length);

	out->length = result_length;
	out->tap = LANCZOS_NTAP;
	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (i + 0.5) * dslen;
		pos = pos / drlen + offsetCorr;
		n = (Int32)floor(pos - (LANCZOS_NTAP / 2 - 0.5));//2.5);
		pos = (n+0.5-pos);
		sum = 0;
		for(j = 0; j < out->tap; j++)
		{
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= (OSInt)source_length){
				out->index[i * out->tap + j] = (OSInt)((source_length - 1) * indexSep);
			}else{
				out->index[i * out->tap + j] = (OSInt)((UInt32)n * indexSep);
			}
			work[j] = lanczos3_weight(pos);
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
		i++;
	}

	MemFree(work);
}


UInt32 __stdcall Media::DeinterlaceLR::ProcThread(AnyType obj)
{
	NN<DITHREADSTAT> stat = obj.GetNN<DITHREADSTAT>();
	stat->status = 1;
	while (true)
	{
		stat->evt->Wait();
		if (stat->status == 4)
			break;

		if (stat->status == 2)
		{
			DeinterlaceLR_VerticalFilter(stat->inPt.Ptr(), stat->outPt.Ptr(), stat->width, stat->height, stat->tap, stat->index, stat->weight, stat->sstep, stat->dstep);
			stat->status = 3;
			stat->evtMain->Set();
		}
		else if (stat->status == 5)
		{
			DeinterlaceLR_VerticalFilterOdd(stat->inPt.Ptr(), stat->inPtCurr.Ptr(), stat->outPt.Ptr(), stat->width, stat->height, stat->tap, stat->index, stat->weight, stat->sstep, stat->dstep);
			stat->status = 3;
			stat->evtMain->Set();
		}
		else if (stat->status == 6)
		{
			DeinterlaceLR_VerticalFilterEven(stat->inPt.Ptr(), stat->inPtCurr.Ptr(), stat->outPt.Ptr(), stat->width, stat->height, stat->tap, stat->index, stat->weight, stat->sstep, stat->dstep);
			stat->status = 3;
			stat->evtMain->Set();
		}
	}
	stat->status = 0;
	stat->evtMain->Set();
	return 0;
}

Media::DeinterlaceLR::DeinterlaceLR(UOSInt fieldCnt, UOSInt fieldSep)
{
	this->oddParam.index = 0;
	this->oddParam.weight = 0;
	this->evenParam.index = 0;
	this->evenParam.weight = 0;
	this->fieldCnt = 0;
	this->fieldSep = 0;
	Reinit(fieldCnt, fieldSep);

	this->nCore = Sync::ThreadUtil::GetThreadCnt();
	if (this->nCore > 4)
	{
		this->nCore = 4;
	}

	this->stats = MemAlloc(DITHREADSTAT, this->nCore);
	UOSInt i = this->nCore;

	NEW_CLASS(evtMain, Sync::Event());
	while (i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event());
		stats[i].status = 0;
		stats[i].evtMain = this->evtMain;
	}

	i = nCore;
	while (i-- > 0)
	{
		Sync::ThreadUtil::Create(ProcThread, &stats[i]);
	}

}

Media::DeinterlaceLR::~DeinterlaceLR()
{
	UOSInt i = nCore;
	while (i-- > 0)
	{
		stats[i].status = 4;
		stats[i].evt->Set();
	}
	Bool allExit;
	while (true)
	{
		evtMain->Wait(100);
		allExit = true;
		i = nCore;
		while (i-- > 0)
		{
			if (stats[i].status != 0)
			{
				allExit = false;
				break;
			}
		}
		if (allExit)
			break;
	}

	i = nCore;
	while (i-- > 0)
	{
		DEL_CLASS(stats[i].evt);
	}
	DEL_CLASS(evtMain);
	MemFree(this->stats);
	if (this->oddParam.index)
	{
		MemFreeA(this->oddParam.weight);
		MemFreeA(this->oddParam.index);
		this->oddParam.weight = 0;
		this->oddParam.index = 0;
	}
	if (this->evenParam.index)
	{
		MemFreeA(this->evenParam.weight);
		MemFreeA(this->evenParam.index);
		this->evenParam.weight = 0;
		this->evenParam.index = 0;
	}
}

void Media::DeinterlaceLR::Reinit(UOSInt fieldCnt, UOSInt fieldSep)
{
	if (fieldCnt == this->fieldCnt && fieldSep == this->fieldSep)
		return;

	if (this->oddParam.index)
	{
		MemFreeA(this->oddParam.weight);
		MemFreeA(this->oddParam.index);
		this->oddParam.weight = 0;
		this->oddParam.index = 0;
	}
	if (this->evenParam.index)
	{
		MemFreeA(this->evenParam.weight);
		MemFreeA(this->evenParam.index);
		this->evenParam.weight = 0;
		this->evenParam.index = 0;
	}
	Media::DeinterlaceLR::SetupInterpolationParameter(fieldCnt, fieldCnt << 1, oddParam, fieldSep, 0.25);
	Media::DeinterlaceLR::SetupInterpolationParameter(fieldCnt, fieldCnt << 1, evenParam, fieldSep, -0.25);
	this->fieldCnt = fieldCnt;
	this->fieldSep = fieldSep;
}

void Media::DeinterlaceLR::Deinterlace(UnsafeArray<UInt8> src, UnsafeArray<UInt8> dest, Bool bottomField, UOSInt width, OSInt dstep)
{
	if (!bottomField)
	{
		UOSInt imgHeight = oddParam.length >> 1;

		UOSInt thisLine;
		UOSInt lastLine = imgHeight << 1;
		UOSInt i = nCore;
		while (i-- > 0)
		{
			thisLine = MulDivUOS(imgHeight, i, nCore) * 2;
			stats[i].inPt = src;
			stats[i].inPtCurr = src + (this->fieldSep * thisLine >> 1);
			stats[i].outPt = dest + dstep * (OSInt)thisLine;
			stats[i].width = width;
			stats[i].height = lastLine - thisLine;
			stats[i].tap = oddParam.tap;
			stats[i].index = oddParam.index + thisLine * oddParam.tap;
			stats[i].weight = oddParam.weight + thisLine * oddParam.tap;
			stats[i].sstep = (OSInt)this->fieldSep;
			stats[i].dstep = dstep;

			stats[i].status = 5;
			stats[i].evt->Set();
			lastLine = thisLine;
		}

		while (true)
		{
			evtMain->Wait(100);
			Bool allFin;
			allFin = true;
			i = nCore;
			while (i-- > 0)
			{
				if (stats[i].status == 5)
				{
					allFin = false;
					break;
				}
			}
			if (allFin)
				break;
		}
	}
	else
	{
		UOSInt imgHeight = evenParam.length >> 1;

		UOSInt thisLine;
		UOSInt lastLine = imgHeight << 1;
		UOSInt i = nCore;
		while (i-- > 0)
		{
			thisLine = MulDivUOS(imgHeight, i, nCore) * 2;
			stats[i].inPt = src;
			stats[i].inPtCurr = src + (this->fieldSep * thisLine >> 1);
			stats[i].outPt = dest + dstep * (OSInt)thisLine;
			stats[i].width = width;
			stats[i].height = lastLine - thisLine;
			stats[i].tap = evenParam.tap;
			stats[i].index = evenParam.index + thisLine * evenParam.tap;
			stats[i].weight = evenParam.weight + thisLine * evenParam.tap;
			stats[i].sstep = (OSInt)this->fieldSep;
			stats[i].dstep = dstep;

			stats[i].status = 6;
			stats[i].evt->Set();
			lastLine = thisLine;
		}

		while (true)
		{
			evtMain->Wait(100);
			Bool allFin;
			allFin = true;
			i = nCore;
			while (i-- > 0)
			{
				if (stats[i].status == 6)
				{
					allFin = false;
					break;
				}
			}
			if (allFin)
				break;
		}
	}
}
