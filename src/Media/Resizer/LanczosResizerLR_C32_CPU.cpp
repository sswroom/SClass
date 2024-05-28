#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/Resizer/LanczosResizerLR_C32_CPU.h"
#include "Sync/Event.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void LanczosResizerLR_C32_CPU_horizontal_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerLR_C32_CPU_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerLR_C32_CPU_vertical_filter_na(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerLR_C32_CPU_hv_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt dheight, UOSInt swidth, UOSInt htap, OSInt *hindex, Int64 *hweight, UOSInt vtap, OSInt *vindex, Int64 *vweight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, UInt8 *buffPt);
	void LanczosResizerLR_C32_CPU_collapse(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerLR_C32_CPU_collapse_na(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
}

typedef struct
{
	UOSInt tap;
	OSInt *index;
	Int64 *weight;
} FilterParam;

void Media::Resizer::LanczosResizerLR_C32_CPU::mt_horizontal_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
{
	Manage::HiResClock clk;
	if (this->nThread == 1 || height < this->nThread)
	{
		LanczosResizerLR_C32_CPU_horizontal_filter(inPt, outPt, width, height, tap, index, weight, sstep, dstep, this->rgbTable);
	}
	else
	{
		UOSInt currHeight;
		UOSInt lastHeight = height;
		UOSInt i = this->nThread;
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, height, this->nThread);
			this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
			this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
			this->params[i].width = width;
			this->params[i].height = lastHeight - currHeight;
			this->params[i].tap = tap;
			this->params[i].index = index;
			this->params[i].weight = weight;
			this->params[i].sstep = sstep;
			this->params[i].dstep = dstep;

			this->params[i].funcType = 3;
			this->ptask->AddTask(DoTask, &this->params[i]);
			lastHeight = currHeight;
		}
		this->ptask->WaitForIdle();
	}
	this->hTotTime = clk.GetTimeDiff();
	this->hTotCount = 1;
}

void Media::Resizer::LanczosResizerLR_C32_CPU::mt_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType)
{
	Manage::HiResClock clk;
	if (this->nThread == 1 || height < this->nThread)
	{
		if (srcAlphaType == Media::AT_NO_ALPHA)
		{
			LanczosResizerLR_C32_CPU_vertical_filter_na(inPt, outPt, width, height, tap, index, weight, sstep, dstep, this->rgbTable);
		}
		else
		{
			LanczosResizerLR_C32_CPU_vertical_filter(inPt, outPt, width, height, tap, index, weight, sstep, dstep, this->rgbTable);
		}
	}
	else
	{
		UOSInt currHeight;
		UOSInt lastHeight = height;
		UOSInt i = this->nThread;
		UOSInt wTap;
		Int32 procStatus = 5;
		if (srcAlphaType == Media::AT_NO_ALPHA)
		{
			procStatus = 12;
		}
		if (tap == 6)
		{
			wTap = 10;
		}
		else
		{
			wTap = tap;
		}
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, height, this->nThread);
			this->params[i].inPt = inPt;
			this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
			this->params[i].width = width;
			this->params[i].height = lastHeight - currHeight;
			this->params[i].tap = tap;
			this->params[i].index = index + currHeight * tap;
			this->params[i].weight = weight + currHeight * wTap;
			this->params[i].sstep = sstep;
			this->params[i].dstep = dstep;

			this->params[i].funcType = procStatus;
			this->ptask->AddTask(DoTask, &this->params[i]);
			lastHeight = currHeight;
		}
		this->ptask->WaitForIdle();
	}
	this->vTotTime = clk.GetTimeDiff();
	this->vTotCount = 1;
}

void Media::Resizer::LanczosResizerLR_C32_CPU::mt_hv_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt dheight, UOSInt swidth, UOSInt htap, OSInt *hindex, Int64 *hweight, UOSInt vtap, OSInt *vindex, Int64 *vweight, OSInt sstep, OSInt dstep, UInt8 *buffPt, Media::AlphaType srcAlphaType)
{
	if (this->nThread == 1 || dheight < this->nThread)
	{
		LanczosResizerLR_C32_CPU_hv_filter(inPt, outPt, dwidth, dheight, swidth, htap, hindex, hweight, vtap, vindex, vweight, sstep, dstep, this->rgbTable, buffPt);
	}
	else
	{
		UOSInt currHeight;
		UOSInt lastHeight = dheight;
		UOSInt i = this->nThread;
		Int32 procStatus = 14;
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, dheight, this->nThread);
			this->params[i].inPt = inPt;
			this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
			this->params[i].width = dwidth;
			this->params[i].height = lastHeight - currHeight;
			this->params[i].width0 = swidth;
			this->params[i].tap = htap;
			this->params[i].index = hindex;
			this->params[i].weight = hweight;
			this->params[i].tap2 = vtap;
			this->params[i].index2 = vindex + currHeight * vtap;
			this->params[i].weight2 = vweight + currHeight * vtap;
			this->params[i].sstep = sstep;
			this->params[i].dstep = dstep;
			this->params[i].buffPt = buffPt + i * (swidth << 3);

			this->params[i].funcType = procStatus;
			this->ptask->AddTask(DoTask, &this->params[i]);
			lastHeight = currHeight;
		}
		this->ptask->WaitForIdle();
	}
}

void Media::Resizer::LanczosResizerLR_C32_CPU::mt_collapse(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType)
{
	if (this->nThread == 1 || height < this->nThread)
	{
		if (srcAlphaType == Media::AT_NO_ALPHA)
		{
			LanczosResizerLR_C32_CPU_collapse_na(inPt, outPt, width, height, sstep, dstep, this->rgbTable);
		}
		else
		{
			LanczosResizerLR_C32_CPU_collapse(inPt, outPt, width, height, sstep, dstep, this->rgbTable);
		}
	}
	else
	{
		UOSInt currHeight;
		UOSInt lastHeight = height;
		Int32 procStatus = 9;
		if (srcAlphaType == Media::AT_NO_ALPHA)
		{
			procStatus = 13;
		}
		UOSInt i = this->nThread;
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, height, this->nThread);
			this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
			this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
			this->params[i].width = width;
			this->params[i].height = lastHeight - currHeight;
			this->params[i].sstep = sstep;
			this->params[i].dstep = dstep;

			this->params[i].funcType = procStatus;
			this->ptask->AddTask(DoTask, &this->params[i]);
			lastHeight = currHeight;
		}
		this->ptask->WaitForIdle();
	}
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DoTask(AnyType obj)
{
	NN<TaskParam> ts = obj.GetNN<TaskParam>();
	if (ts->funcType == 3)
	{
		LanczosResizerLR_C32_CPU_horizontal_filter(ts->inPt, ts->outPt, ts->width, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
	else if (ts->funcType == 5)
	{
		LanczosResizerLR_C32_CPU_vertical_filter(ts->inPt, ts->outPt, ts->width, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
	else if (ts->funcType == 9)
	{
		LanczosResizerLR_C32_CPU_collapse(ts->inPt, ts->outPt, ts->width, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
	else if (ts->funcType == 12)
	{
		LanczosResizerLR_C32_CPU_vertical_filter_na(ts->inPt, ts->outPt, ts->width, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
	else if (ts->funcType == 13)
	{
		LanczosResizerLR_C32_CPU_collapse_na(ts->inPt, ts->outPt, ts->width, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
	else if (ts->funcType == 14)
	{
		LanczosResizerLR_C32_CPU_hv_filter(ts->inPt, ts->outPt, ts->width, ts->height, ts->width0, ts->tap, ts->index, ts->weight, ts->tap2, ts->index2, ts->weight2, ts->sstep, ts->dstep, ts->me->rgbTable, ts->buffPt);
	}
}

Media::Resizer::LanczosResizerLR_C32_CPU::LanczosResizerLR_C32_CPU()
{
	UOSInt i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 4)
	{
		this->nThread = 4;
	}
	this->hTotCount = 0;
	this->hTotTime = 0;
	this->vTotCount = 0;
	this->vTotTime = 0;

	this->params = MemAlloc(Media::Resizer::LanczosResizerLR_C32_CPU::TaskParam, this->nThread);
	MemClear(this->params, sizeof(Media::Resizer::LanczosResizerLR_C32_CPU::TaskParam) * this->nThread);
	i = nThread;
	while (i-- > 0)
	{
		this->params[i].me = this;
	}
	NEW_CLASS(this->ptask, Sync::ParallelTask(this->nThread, false));

	this->buffSize = 0;
	this->buffPtr = 0;
	this->rgbTable = 0;
}

Media::Resizer::LanczosResizerLR_C32_CPU::~LanczosResizerLR_C32_CPU()
{
	DEL_CLASS(this->ptask);
	MemFree(this->params);
	if (buffPtr)
	{
		MemFreeA64(buffPtr);
		buffPtr = 0;
	}
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DoHorizontalVerticalFilter(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt sheight, UOSInt dheight, NN<HoriFilter> hFilter, NN<VertFilter> vFilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType)
{
	NN<FilterParam> hparam = NN<FilterParam>::ConvertFrom(hFilter);
	NN<FilterParam> vparam = NN<FilterParam>::ConvertFrom(vFilter);

	if (this->buffSize < (sheight * dwidth << 3))
	{
		if (this->buffPtr)
		{
			MemFreeA64(this->buffPtr);
			this->buffPtr = 0;
		}
		this->buffSize = (sheight * dwidth << 3);
		this->buffPtr = MemAllocA64(UInt8, buffSize);
	}
	this->mt_horizontal_filter(inPt, this->buffPtr, dwidth, sheight, hparam->tap, hparam->index, hparam->weight, sstep, (OSInt)dwidth << 3);
	this->mt_vertical_filter(this->buffPtr, outPt, dwidth, dheight, vparam->tap, vparam->index, vparam->weight, (OSInt)dwidth << 3, dstep, srcAlphaType);
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DoHorizontalFilterCollapse(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt sheight, NN<HoriFilter> hFilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType)
{
	NN<FilterParam> hparam = NN<FilterParam>::ConvertFrom(hFilter);
	if (this->buffSize < (sheight * dwidth << 3))
	{
		if (this->buffPtr)
		{
			MemFreeA64(this->buffPtr);
			this->buffPtr = 0;
		}
		this->buffSize = (sheight * dwidth << 3);
		this->buffPtr = MemAllocA64(UInt8, buffSize);
	}
	this->mt_horizontal_filter(inPt, this->buffPtr, dwidth, sheight, hparam->tap, hparam->index, hparam->weight, sstep, (OSInt)dwidth << 3);
	this->mt_collapse(this->buffPtr, outPt, dwidth, sheight, (OSInt)dwidth << 3, dstep, srcAlphaType);
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DoVerticalFilter(const UInt8 *inPt, UInt8 *outPt, UOSInt swidth, UOSInt sheight, UOSInt dheight, NN<VertFilter> vFilter, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType)
{
	NN<FilterParam> vparam = NN<FilterParam>::ConvertFrom(vFilter);
	this->mt_vertical_filter(inPt, outPt, swidth, dheight, vparam->tap, vparam->index, vparam->weight, sstep, dstep, srcAlphaType);
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DoCollapse(const UInt8 *inPt, UInt8 *outPt, UOSInt swidth, UOSInt sheight, OSInt sstep, OSInt dstep, Media::AlphaType srcAlphaType)
{
	this->mt_collapse(inPt, outPt, swidth, sheight, sstep, dstep, srcAlphaType);
}

void Media::Resizer::LanczosResizerLR_C32_CPU::UpdateRGBTable(UInt8 *rgbTable)
{
	this->rgbTable = rgbTable;
}

NN<Media::Resizer::LanczosResizerLR_C32Action::HoriFilter> Media::Resizer::LanczosResizerLR_C32_CPU::CreateHoriFilter(UOSInt htap, OSInt *hIndex, Int64 *hWeight, UOSInt length)
{
	NN<FilterParam> param = MemAllocNN(FilterParam);
	param->tap = htap;
	param->index = hIndex;
	param->weight = hWeight;
	return NN<HoriFilter>::ConvertFrom(param);
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DestroyHoriFilter(NN<Media::Resizer::LanczosResizerLR_C32Action::HoriFilter> hfilter)
{
	NN<FilterParam> param = NN<FilterParam>::ConvertFrom(hfilter);
	MemFreeNN(param);
}

NN<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> Media::Resizer::LanczosResizerLR_C32_CPU::CreateVertFilter(UOSInt vtap, OSInt *vIndex, Int64 *vWeight, UOSInt length)
{
	NN<FilterParam> param = MemAllocNN(FilterParam);
	param->tap = vtap;
	param->index = vIndex;
	param->weight = vWeight;
	return NN<VertFilter>::ConvertFrom(param);
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DestroyVertFilter(NN<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> vfilter)
{
	NN<FilterParam> param = NN<FilterParam>::ConvertFrom(vfilter);
	MemFreeNN(param);
}

Double Media::Resizer::LanczosResizerLR_C32_CPU::GetHAvgTime()
{
	if (this->hTotCount <= 0)
		return 0;
	else
		return this->hTotTime / UOSInt2Double(this->hTotCount);
}

Double Media::Resizer::LanczosResizerLR_C32_CPU::GetVAvgTime()
{
	if (this->vTotCount <= 0)
		return 0;
	else
		return this->vTotTime / UOSInt2Double(this->vTotCount);
}
