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
	void DeinterlaceLR_VerticalFilter(UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep);
	void DeinterlaceLR_VerticalFilterOdd(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep);
	void DeinterlaceLR_VerticalFilterEven(UInt8 *inPt, UInt8 *inPtCurr, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep);
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

void Media::DeinterlaceLR::SetupInterpolationParameter(UIntOS source_length, UIntOS result_length, NN<DIPARAMETER> out, UIntOS indexSep, Double offsetCorr)
{
	UInt32 i,j;
	Int32 n;
	UnsafeArray<Double> work;
	Double  sum;
	Double  pos;
	Double dslen = UIntOS2Double(source_length);
	Double drlen = UIntOS2Double(result_length);
	UnsafeArray<Int64> weight;
	UnsafeArray<IntOS> index;

	out->length = result_length;
	out->tap = LANCZOS_NTAP;
	out->weight = weight = MemAllocAArr(Int64, out->length * out->tap);
	out->index = index = MemAllocAArr(IntOS, out->length * out->tap);

	work = MemAllocArr(Double, out->tap);

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
				index[i * out->tap + j] = 0;
			}else if(n >= (IntOS)source_length){
				index[i * out->tap + j] = (IntOS)((source_length - 1) * indexSep);
			}else{
				index[i * out->tap + j] = (IntOS)((UInt32)n * indexSep);
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
			UInt16 *tmpPtr = (UInt16*)&weight[i * out->tap + j];
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

	MemFreeArr(work);
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
			DeinterlaceLR_VerticalFilter(stat->inPt.Ptr(), stat->outPt.Ptr(), stat->width, stat->height, stat->tap, stat->index.Ptr(), stat->weight.Ptr(), stat->sstep, stat->dstep);
			stat->status = 3;
			stat->evtMain->Set();
		}
		else if (stat->status == 5)
		{
			DeinterlaceLR_VerticalFilterOdd(stat->inPt.Ptr(), stat->inPtCurr.Ptr(), stat->outPt.Ptr(), stat->width, stat->height, stat->tap, stat->index.Ptr(), stat->weight.Ptr(), stat->sstep, stat->dstep);
			stat->status = 3;
			stat->evtMain->Set();
		}
		else if (stat->status == 6)
		{
			DeinterlaceLR_VerticalFilterEven(stat->inPt.Ptr(), stat->inPtCurr.Ptr(), stat->outPt.Ptr(), stat->width, stat->height, stat->tap, stat->index.Ptr(), stat->weight.Ptr(), stat->sstep, stat->dstep);
			stat->status = 3;
			stat->evtMain->Set();
		}
	}
	stat->status = 0;
	stat->evtMain->Set();
	return 0;
}

Media::DeinterlaceLR::DeinterlaceLR(UIntOS fieldCnt, UIntOS fieldSep)
{
	this->oddParam.index = nullptr;
	this->oddParam.weight = nullptr;
	this->evenParam.index = nullptr;
	this->evenParam.weight = nullptr;
	this->fieldCnt = 0;
	this->fieldSep = 0;
	Reinit(fieldCnt, fieldSep);

	this->nCore = Sync::ThreadUtil::GetThreadCnt();
	if (this->nCore > 4)
	{
		this->nCore = 4;
	}

	this->stats = MemAllocArr(DITHREADSTAT, this->nCore);
	UIntOS i = this->nCore;

	NEW_CLASSNN(this->evtMain, Sync::Event());
	while (i-- > 0)
	{
		NEW_CLASSNN(stats[i].evt, Sync::Event());
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
	UIntOS i = nCore;
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
		stats[i].evt.Delete();
	}
	this->evtMain.Delete();
	MemFreeArr(this->stats);
	UnsafeArray<Int64> weight;
	UnsafeArray<IntOS> index;
	if (this->oddParam.index.SetTo(index))
	{
		MemFreeAArr(index);
		this->oddParam.index = nullptr;
	}
	if (this->oddParam.weight.SetTo(weight))
	{
		MemFreeAArr(weight);
		this->oddParam.weight = nullptr;
	}
	if (this->evenParam.index.SetTo(index))
	{
		MemFreeAArr(index);
		this->evenParam.index = nullptr;
	}
	if (this->evenParam.weight.SetTo(weight))
	{
		MemFreeAArr(weight);
		this->evenParam.weight = nullptr;
	}
}

void Media::DeinterlaceLR::Reinit(UIntOS fieldCnt, UIntOS fieldSep)
{
	if (fieldCnt == this->fieldCnt && fieldSep == this->fieldSep)
		return;

	UnsafeArray<Int64> weight;
	UnsafeArray<IntOS> index;
	if (this->oddParam.index.SetTo(index))
	{
		MemFreeAArr(index);
		this->oddParam.index = nullptr;
	}
	if (this->oddParam.weight.SetTo(weight))
	{
		MemFreeAArr(weight);
		this->oddParam.weight = nullptr;
	}
	if (this->evenParam.index.SetTo(index))
	{
		MemFreeAArr(index);
		this->evenParam.index = nullptr;
	}
	if (this->evenParam.weight.SetTo(weight))
	{
		MemFreeAArr(weight);
		this->evenParam.weight = nullptr;
	}
	Media::DeinterlaceLR::SetupInterpolationParameter(fieldCnt, fieldCnt << 1, oddParam, fieldSep, 0.25);
	Media::DeinterlaceLR::SetupInterpolationParameter(fieldCnt, fieldCnt << 1, evenParam, fieldSep, -0.25);
	this->fieldCnt = fieldCnt;
	this->fieldSep = fieldSep;
}

void Media::DeinterlaceLR::Deinterlace(UnsafeArray<UInt8> src, UnsafeArray<UInt8> dest, Bool bottomField, UIntOS width, IntOS dstep)
{
	UnsafeArray<Int64> weight;
	UnsafeArray<IntOS> index;
	if (!bottomField)
	{
		if (!this->oddParam.index.SetTo(index) || !this->oddParam.weight.SetTo(weight))
		{
			return;
		}
		UIntOS imgHeight = oddParam.length >> 1;

		UIntOS thisLine;
		UIntOS lastLine = imgHeight << 1;
		UIntOS i = nCore;
		while (i-- > 0)
		{
			thisLine = MulDivUOS(imgHeight, i, nCore) * 2;
			stats[i].inPt = src;
			stats[i].inPtCurr = src + (this->fieldSep * thisLine >> 1);
			stats[i].outPt = dest + dstep * (IntOS)thisLine;
			stats[i].width = width;
			stats[i].height = lastLine - thisLine;
			stats[i].tap = oddParam.tap;
			stats[i].index = index + thisLine * oddParam.tap;
			stats[i].weight = weight + thisLine * oddParam.tap;
			stats[i].sstep = (IntOS)this->fieldSep;
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
		if (!this->evenParam.index.SetTo(index) || !this->evenParam.weight.SetTo(weight))
		{
			return;
		}
		UIntOS imgHeight = evenParam.length >> 1;

		UIntOS thisLine;
		UIntOS lastLine = imgHeight << 1;
		UIntOS i = nCore;
		while (i-- > 0)
		{
			thisLine = MulDivUOS(imgHeight, i, nCore) * 2;
			stats[i].inPt = src;
			stats[i].inPtCurr = src + (this->fieldSep * thisLine >> 1);
			stats[i].outPt = dest + dstep * (IntOS)thisLine;
			stats[i].width = width;
			stats[i].height = lastLine - thisLine;
			stats[i].tap = evenParam.tap;
			stats[i].index = index + thisLine * evenParam.tap;
			stats[i].weight = weight + thisLine * evenParam.tap;
			stats[i].sstep = (IntOS)this->fieldSep;
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
