#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Math/Math_C.h"
#include "Media/Resizer/LanczosResizerLR_C32_CPU.h"
#include "Sync/Event.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void LanczosResizerLR_C32_CPU_horizontal_filter(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
	void LanczosResizerLR_C32_CPU_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
	void LanczosResizerLR_C32_CPU_vertical_filter_na(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
	void LanczosResizerLR_C32_CPU_hv_filter(const UInt8 *inPt, UInt8 *outPt, UIntOS dwidth, UIntOS dheight, UIntOS swidth, UIntOS htap, IntOS *hindex, Int64 *hweight, UIntOS vtap, IntOS *vindex, Int64 *vweight, IntOS sstep, IntOS dstep, UInt8 *rgbTable, UInt8 *buffPt);
	void LanczosResizerLR_C32_CPU_collapse(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
	void LanczosResizerLR_C32_CPU_collapse_na(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
}

typedef struct
{
	UIntOS tap;
	UnsafeArray<IntOS> index;
	UnsafeArray<Int64> weight;
} FilterParam;

void Media::Resizer::LanczosResizerLR_C32_CPU::mt_horizontal_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep)
{
	Manage::HiResClock clk;
	if (this->nThread == 1 || height < this->nThread)
	{
		LanczosResizerLR_C32_CPU_horizontal_filter(inPt.Ptr(), outPt.Ptr(), width, height, tap, index.Ptr(), weight.Ptr(), sstep, dstep, this->rgbTable.Ptr());
	}
	else
	{
		UIntOS currHeight;
		UIntOS lastHeight = height;
		UIntOS i = this->nThread;
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, height, this->nThread);
			this->params[i].inPt = inPt + (IntOS)currHeight * sstep;
			this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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

void Media::Resizer::LanczosResizerLR_C32_CPU::mt_vertical_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType)
{
	Manage::HiResClock clk;
	if (this->nThread == 1 || height < this->nThread)
	{
		if (srcAlphaType == Media::AT_IGNORE_ALPHA || srcAlphaType == Media::AT_ALPHA_ALL_FF)
		{
			LanczosResizerLR_C32_CPU_vertical_filter_na(inPt.Ptr(), outPt.Ptr(), width, height, tap, index.Ptr(), weight.Ptr(), sstep, dstep, this->rgbTable.Ptr());
		}
		else
		{
			LanczosResizerLR_C32_CPU_vertical_filter(inPt.Ptr(), outPt.Ptr(), width, height, tap, index.Ptr(), weight.Ptr(), sstep, dstep, this->rgbTable.Ptr());
		}
	}
	else
	{
		UIntOS currHeight;
		UIntOS lastHeight = height;
		UIntOS i = this->nThread;
		UIntOS wTap;
		Int32 procStatus = 5;
		if (srcAlphaType == Media::AT_IGNORE_ALPHA || srcAlphaType == Media::AT_ALPHA_ALL_FF)
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
			this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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

void Media::Resizer::LanczosResizerLR_C32_CPU::mt_hv_filter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS dheight, UIntOS swidth, UIntOS htap, UnsafeArray<IntOS> hindex, UnsafeArray<Int64> hweight, UIntOS vtap, UnsafeArray<IntOS> vindex, UnsafeArray<Int64> vweight, IntOS sstep, IntOS dstep, UnsafeArray<UInt8> buffPt, Media::AlphaType srcAlphaType)
{
	if (this->nThread == 1 || dheight < this->nThread)
	{
		LanczosResizerLR_C32_CPU_hv_filter(inPt.Ptr(), outPt.Ptr(), dwidth, dheight, swidth, htap, hindex.Ptr(), hweight.Ptr(), vtap, vindex.Ptr(), vweight.Ptr(), sstep, dstep, this->rgbTable.Ptr(), buffPt.Ptr());
	}
	else
	{
		UIntOS currHeight;
		UIntOS lastHeight = dheight;
		UIntOS i = this->nThread;
		Int32 procStatus = 14;
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, dheight, this->nThread);
			this->params[i].inPt = inPt;
			this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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

void Media::Resizer::LanczosResizerLR_C32_CPU::mt_collapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType)
{
	if (this->nThread == 1 || height < this->nThread)
	{
		if (srcAlphaType == Media::AT_IGNORE_ALPHA || srcAlphaType == Media::AT_ALPHA_ALL_FF)
		{
			LanczosResizerLR_C32_CPU_collapse_na(inPt.Ptr(), outPt.Ptr(), width, height, sstep, dstep, this->rgbTable.Ptr());
		}
		else
		{
			LanczosResizerLR_C32_CPU_collapse(inPt.Ptr(), outPt.Ptr(), width, height, sstep, dstep, this->rgbTable.Ptr());
		}
	}
	else
	{
		UIntOS currHeight;
		UIntOS lastHeight = height;
		Int32 procStatus = 9;
		if (srcAlphaType == Media::AT_IGNORE_ALPHA || srcAlphaType == Media::AT_ALPHA_ALL_FF)
		{
			procStatus = 13;
		}
		UIntOS i = this->nThread;
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, height, this->nThread);
			this->params[i].inPt = inPt + (IntOS)currHeight * sstep;
			this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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
		LanczosResizerLR_C32_CPU_horizontal_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 5)
	{
		LanczosResizerLR_C32_CPU_vertical_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 9)
	{
		LanczosResizerLR_C32_CPU_collapse(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 12)
	{
		LanczosResizerLR_C32_CPU_vertical_filter_na(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 13)
	{
		LanczosResizerLR_C32_CPU_collapse_na(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 14)
	{
		LanczosResizerLR_C32_CPU_hv_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->width0, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->tap2, ts->index2.Ptr(), ts->weight2.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->buffPt.Ptr());
	}
}

Media::Resizer::LanczosResizerLR_C32_CPU::LanczosResizerLR_C32_CPU()
{
	UIntOS i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 4)
	{
		this->nThread = 4;
	}
	this->hTotCount = 0;
	this->hTotTime = 0;
	this->vTotCount = 0;
	this->vTotTime = 0;

	this->params = MemAllocArr(Media::Resizer::LanczosResizerLR_C32_CPU::TaskParam, this->nThread);
	MemClear(this->params.Ptr(), sizeof(Media::Resizer::LanczosResizerLR_C32_CPU::TaskParam) * this->nThread);
	i = nThread;
	while (i-- > 0)
	{
		this->params[i].me = *this;
	}
	NEW_CLASSNN(this->ptask, Sync::ParallelTask(this->nThread, false));

	this->buffSize = 0;
	this->buffPtr = nullptr;
	this->rgbTable = nullptr;
}

Media::Resizer::LanczosResizerLR_C32_CPU::~LanczosResizerLR_C32_CPU()
{
	UnsafeArray<UInt8> buffPtr;
	this->ptask.Delete();
	MemFreeArr(this->params);
	if (this->buffPtr.SetTo(buffPtr))
	{
		MemFreeAArr(buffPtr);
		this->buffPtr = nullptr;
	}
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DoHorizontalVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS sheight, UIntOS dheight, NN<HoriFilter> hFilter, NN<VertFilter> vFilter, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType)
{
	NN<FilterParam> hparam = NN<FilterParam>::ConvertFrom(hFilter);
	NN<FilterParam> vparam = NN<FilterParam>::ConvertFrom(vFilter);
	UnsafeArray<UInt8> buffPtr;

	if (this->buffSize < (sheight * dwidth << 3) || !this->buffPtr.SetTo(buffPtr))
	{
		if (this->buffPtr.SetTo(buffPtr))
		{
			MemFreeAArr(buffPtr);
			this->buffPtr = nullptr;
		}
		this->buffSize = (sheight * dwidth << 3);
		this->buffPtr = buffPtr = MemAllocA64(UInt8, buffSize);
	}
	this->mt_horizontal_filter(inPt, buffPtr, dwidth, sheight, hparam->tap, hparam->index, hparam->weight, sstep, (IntOS)dwidth << 3);
	this->mt_vertical_filter(buffPtr, outPt, dwidth, dheight, vparam->tap, vparam->index, vparam->weight, (IntOS)dwidth << 3, dstep, srcAlphaType);
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DoHorizontalFilterCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS sheight, NN<HoriFilter> hFilter, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType)
{
	NN<FilterParam> hparam = NN<FilterParam>::ConvertFrom(hFilter);
	UnsafeArray<UInt8> buffPtr;
	if (this->buffSize < (sheight * dwidth << 3) || !this->buffPtr.SetTo(buffPtr))
	{
		if (this->buffPtr.SetTo(buffPtr))
		{
			MemFreeAArr(buffPtr);
			this->buffPtr = nullptr;
		}
		this->buffSize = (sheight * dwidth << 3);
		this->buffPtr = buffPtr = MemAllocA64(UInt8, buffSize);
	}
	this->mt_horizontal_filter(inPt, buffPtr, dwidth, sheight, hparam->tap, hparam->index, hparam->weight, sstep, (IntOS)dwidth << 3);
	this->mt_collapse(buffPtr, outPt, dwidth, sheight, (IntOS)dwidth << 3, dstep, srcAlphaType);
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DoVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS swidth, UIntOS sheight, UIntOS dheight, NN<VertFilter> vFilter, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType)
{
	NN<FilterParam> vparam = NN<FilterParam>::ConvertFrom(vFilter);
	this->mt_vertical_filter(inPt, outPt, swidth, dheight, vparam->tap, vparam->index, vparam->weight, sstep, dstep, srcAlphaType);
}

void Media::Resizer::LanczosResizerLR_C32_CPU::DoCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS swidth, UIntOS sheight, IntOS sstep, IntOS dstep, Media::AlphaType srcAlphaType)
{
	this->mt_collapse(inPt, outPt, swidth, sheight, sstep, dstep, srcAlphaType);
}

void Media::Resizer::LanczosResizerLR_C32_CPU::UpdateRGBTable(UnsafeArray<UInt8> rgbTable)
{
	this->rgbTable = rgbTable;
}

NN<Media::Resizer::LanczosResizerLR_C32Action::HoriFilter> Media::Resizer::LanczosResizerLR_C32_CPU::CreateHoriFilter(UIntOS htap, UnsafeArray<IntOS> hIndex, UnsafeArray<Int64> hWeight, UIntOS length)
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

NN<Media::Resizer::LanczosResizerLR_C32Action::VertFilter> Media::Resizer::LanczosResizerLR_C32_CPU::CreateVertFilter(UIntOS vtap, UnsafeArray<IntOS> vIndex, UnsafeArray<Int64> vWeight, UIntOS length)
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
		return this->hTotTime / UIntOS2Double(this->hTotCount);
}

Double Media::Resizer::LanczosResizerLR_C32_CPU::GetVAvgTime()
{
	if (this->vTotCount <= 0)
		return 0;
	else
		return this->vTotTime / UIntOS2Double(this->vTotCount);
}
