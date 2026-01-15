#include "Stdafx.h"
#include <float.h>
#include "MyMemory.h"
#include "Math/LanczosFilter.h"
#include "Math/Math_C.h"
#include "Media/RGBLUTGen.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/LanczosResizer16_C8.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

#define PI 3.141592653589793

extern "C"
{
	void LanczosResizer16_C8_horizontal_filter_pa(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizer16_C8_horizontal_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizer16_C8_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizer16_C8_expand(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizer16_C8_expand_pa(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizer16_C8_collapse(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizer16_C8_imgcopy(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizer16_C8_imgcopy_pa(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
}

void Media::Resizer::LanczosResizer16_C8::SetupInterpolationParameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<Media::Resizer::LanczosResizer16_C8::LRHPARAMETER> out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	Double *work;
	Double  sum;
	Double  pos;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = nTap;
	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UOSInt2Double(i) + 0.5) * source_length;
		pos = pos / UOSInt2Double(result_length) + offsetCorr;
		n = (Int32)Math_Fix(pos - (UOSInt2Double(nTap / 2) - 0.5));//2.5);
		pos = (OSInt2Double(n) + 0.5 - pos);
		sum = 0;
		j = 0;
		while (j < out->tap)
		{
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if((UOSInt)n >= source_max_pos){
				out->index[i * out->tap + j] = (OSInt)(source_max_pos - 1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos.Weight(pos);
			sum += work[j];
			pos += 1;
			n += 1;
			j++;
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

void Media::Resizer::LanczosResizer16_C8::SetupDecimationParameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, NN<Media::Resizer::LanczosResizer16_C8::LRHPARAMETER> out, OSInt indexSep, Double offsetCorr)
{
	UOSInt i;
	UOSInt j;
	OSInt n;
	UOSInt ttap;
	Double *work;
	Double  sum;
	Double  pos, phase;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UOSInt)Math_Fix((UOSInt2Double(nTap) * (source_length) + UOSInt2Double(result_length - 1)) / UOSInt2Double(result_length));
	ttap = out->tap;
	out->tap += out->tap & 1;

	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(OSInt, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UOSInt2Double(i) - UOSInt2Double(nTap / 2) + 0.5) * source_length / UOSInt2Double(result_length) + 0.5;
		n = (Int32)floor(pos + offsetCorr);
		sum = 0;
		j = 0;
		while (j < ttap)
		{
			phase = (OSInt2Double(n) + 0.5) * UOSInt2Double(result_length);
			phase /= source_length;
			phase -= (UOSInt2Double(i) + 0.5);
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if((UOSInt)n >= source_max_pos){
				out->index[i * out->tap + j] = (OSInt)(source_max_pos-1) * indexSep;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos.Weight(phase);
			sum += work[j];
			n += 1;
			j++;
		}
		if (ttap & 1)
		{
			work[ttap] = 0;
			out->index[i * out->tap + ttap] = out->index[i * out->tap + ttap - 1];
		}

		j = 0;
		while (j < ttap)
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

void Media::Resizer::LanczosResizer16_C8::MTHorizontalFilterPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep, UOSInt swidth)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].dwidth = dwidth;
		this->params[i].swidth = swidth;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].tap = tap;
		this->params[i].index = index;
		this->params[i].weight = weight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		if (this->params[i].swidth != 0 && this->params[i].height != 0)
		{
			this->params[i].funcType = 12;
			this->ptask->AddTask(DoTask, &this->params[i]);
		}
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizer16_C8::MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep, UOSInt swidth)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].dwidth = dwidth;
		this->params[i].swidth = swidth;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].tap = tap;
		this->params[i].index = index;
		this->params[i].weight = weight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		if (this->params[i].swidth != 0 && this->params[i].height != 0)
		{
			this->params[i].funcType = 3;
			this->ptask->AddTask(DoTask, &this->params[i]);
		}
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizer16_C8::MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].dwidth = dwidth;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].tap = tap;
		this->params[i].index = index + currHeight * tap;
		this->params[i].weight = weight + currHeight * tap;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 5;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizer16_C8::MTExpandPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].dwidth = dwidth;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 13;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizer16_C8::MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].dwidth = dwidth;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 7;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizer16_C8::MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].dwidth = dwidth;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 9;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizer16_C8::MTCopyPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].dwidth = dwidth;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 14;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizer16_C8::MTCopy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
	UOSInt currHeight;
	UOSInt lastHeight = height;
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (OSInt)currHeight * sstep;
		this->params[i].outPt = outPt + (OSInt)currHeight * dstep;
		this->params[i].dwidth = dwidth;
		this->params[i].height = lastHeight - currHeight;
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 11;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizer16_C8::UpdateRGBTable()
{
	UnsafeArray<UInt8> rgbTable;
	if (!this->rgbTable.SetTo(rgbTable))
	{
		this->rgbTable = rgbTable = MemAlloc(UInt8, 65536 * 4 + 65536 * 4 * 8);
	}
	Media::RGBLUTGen lutGen(this->colorSess);
	lutGen.GenLRGB_BGRA8(rgbTable, this->destProfile, 14, Media::CS::TransferFunc::GetRefLuminance(this->srcProfile.rtransfer));
	lutGen.GenRGB16_LRGBC((Int64*)&rgbTable[262144], this->srcProfile, this->destProfile.GetPrimaries(), 14);
}

void __stdcall Media::Resizer::LanczosResizer16_C8::DoTask(AnyType obj)
{
	UnsafeArray<UInt8> tmpbuff;
	NN<TaskParam> ts = obj.GetNN<TaskParam>();

	if (ts->funcType == 3)
	{
		if (ts->swidth != ts->tmpbuffSize || !ts->tmpbuff.SetTo(tmpbuff))
		{
			if (ts->tmpbuff.SetTo(tmpbuff))
				MemFreeAArr(tmpbuff);
			ts->tmpbuffSize = ts->swidth;
			ts->tmpbuff = tmpbuff = MemAllocA(UInt8, ts->swidth << 3);
		}
		LanczosResizer16_C8_horizontal_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->swidth, tmpbuff.Ptr());
	}
	else if (ts->funcType == 5)
	{
		LanczosResizer16_C8_vertical_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 7)
	{
		LanczosResizer16_C8_expand(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 9)
	{
		LanczosResizer16_C8_collapse(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 11)
	{
		LanczosResizer16_C8_imgcopy(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 12)
	{
		if (ts->swidth != ts->tmpbuffSize || !ts->tmpbuff.SetTo(tmpbuff))
		{
			if (ts->tmpbuff.SetTo(tmpbuff))
				MemFreeAArr(tmpbuff);
			ts->tmpbuffSize = ts->swidth;
			ts->tmpbuff = tmpbuff = MemAllocA(UInt8, ts->swidth << 3);
		}
		LanczosResizer16_C8_horizontal_filter_pa(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->swidth, tmpbuff.Ptr());
	}
	else if (ts->funcType == 13)
	{
		LanczosResizer16_C8_expand_pa(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 14)
	{
		LanczosResizer16_C8_imgcopy_pa(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
}

void Media::Resizer::LanczosResizer16_C8::DestoryHori()
{
	UnsafeArray<OSInt> hIndex;
	UnsafeArray<Int64> hWeight;
	if (this->hIndex.SetTo(hIndex))
	{
		MemFreeAArr(hIndex);
		this->hIndex = nullptr;
	}
	if (this->hWeight.SetTo(hWeight))
	{
		MemFreeAArr(hWeight);
		this->hWeight = nullptr;
	}
	this->hsSize = 0;
}

void Media::Resizer::LanczosResizer16_C8::DestoryVert()
{
	UnsafeArray<OSInt> vIndex;
	UnsafeArray<Int64> vWeight;
	if (this->vIndex.SetTo(vIndex))
	{
		MemFreeAArr(vIndex);
		this->vIndex = nullptr;
	}
	if (this->vWeight.SetTo(vWeight))
	{
		MemFreeAArr(vWeight);
		this->vWeight = nullptr;
	}
	this->vsSize = 0;
	this->vsStep = 0;
}

Media::Resizer::LanczosResizer16_C8::LanczosResizer16_C8(UOSInt hnTap, UOSInt vnTap, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType) : Media::ImageResizer(srcAlphaType), srcProfile(srcProfile), destProfile(destProfile)
{
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 6)
	{
		this->nThread = 6;
	}

	this->hnTap = hnTap << 1;
	this->vnTap = vnTap << 1;
	this->rgbChanged = true;
	this->rgbTable = nullptr;
	NN<Media::ColorManagerSess> nncolorSess;
	if (colorSess.SetTo(nncolorSess))
	{
		this->colorSess = colorSess;
		nncolorSess->AddHandler(*this);
	}
	else
	{
		this->colorSess = nullptr;
	}

	this->params = MemAllocArr(TaskParam, this->nThread);
	MemClear(this->params.Ptr(), sizeof(TaskParam) * this->nThread);
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		this->params[i].me = *this;
	}
	NEW_CLASSNN(this->ptask, Sync::ParallelTask(this->nThread, false));

	this->hsSize = 0;
	this->hsOfst = 0;
	this->hdSize = 0;
	this->hIndex = nullptr;
	this->hWeight = nullptr;
	this->hTap = 0;

	this->vsSize = 0;
	this->vsOfst = 0;
	this->vdSize = 0;
	this->vsStep = 0;
	this->vIndex = nullptr;
	this->vWeight = nullptr;
	this->vTap = 0;

	this->buffW = 0;
	this->buffH = 0;
	this->buffPtr = nullptr;
}

Media::Resizer::LanczosResizer16_C8::~LanczosResizer16_C8()
{
	UOSInt i;
	UnsafeArray<UInt8> tmpbuff;
	NN<Media::ColorManagerSess> colorSess;
	UnsafeArray<UInt8> buffPtr;
	UnsafeArray<UInt8> rgbTable;
	if (this->colorSess.SetTo(colorSess))
	{
		colorSess->RemoveHandler(*this);
	}
	this->ptask.Delete();
	i = this->nThread;
	while (i-- > 0)
	{
		if (this->params[i].tmpbuff.SetTo(tmpbuff))
		{
			MemFreeAArr(tmpbuff);
			this->params[i].tmpbuff = nullptr;
			this->params[i].tmpbuffSize = 0;
		}
	}
	MemFreeArr(this->params);

	DestoryHori();
	DestoryVert();
	if (this->buffPtr.SetTo(buffPtr))
	{
		MemFreeAArr(buffPtr);
		this->buffPtr = nullptr;
	}
	if (this->rgbTable.SetTo(rgbTable))
	{
		MemFreeArr(rgbTable);
	}
}

void Media::Resizer::LanczosResizer16_C8::Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
	UnsafeArray<OSInt> hIndex;
	UnsafeArray<Int64> hWeight;
	UnsafeArray<OSInt> vIndex;
	UnsafeArray<Int64> vWeight;
	UnsafeArray<UInt8> buffPtr;
	Media::Resizer::LanczosResizer16_C8::LRHPARAMETER prm;
	Double w;
	Double h;
	UOSInt siWidth;
	UOSInt siHeight;
	if (dwidth < 16 || dheight < 16)
		return;

	w = xOfst + swidth;
	h = yOfst + sheight;
	siWidth = (UOSInt)w;
	siHeight = (UOSInt)h;
	w -= UOSInt2Double(siWidth);
	h -= UOSInt2Double(siHeight);
	if (w > 0)
		siWidth++;
	if (h > 0)
		siHeight++;

	if (this->rgbChanged)
	{
		this->rgbChanged = false;
		UpdateRGBTable();
	}

	if (siWidth != dwidth && siHeight != dheight)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->hsSize != swidth || this->hdSize != dwidth || this->hsOfst != xOfst || !this->hIndex.SetTo(hIndex) || !this->hWeight.SetTo(hWeight))
		{
			DestoryHori();

			if (swidth > UOSInt2Double(dwidth))
			{
				SetupDecimationParameter(this->hnTap, swidth, siWidth, dwidth, prm, 8, xOfst);
			}
			else
			{
				SetupInterpolationParameter(this->hnTap, swidth, siWidth, dwidth, prm, 8, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			this->hIndex = hIndex = prm.index;
			this->hWeight = hWeight = prm.weight;
			hTap = prm.tap;
		}

		if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != (OSInt)hdSize * 8 || this->vsOfst != yOfst || !this->vIndex.SetTo(vIndex) || !this->vWeight.SetTo(vWeight))
		{
			DestoryVert();
			vsStep = (OSInt)hdSize * 8;

			if (sheight > UOSInt2Double(dheight))
			{
				SetupDecimationParameter(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			else
			{
				SetupInterpolationParameter(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			this->vIndex = vIndex = prm.index;
			this->vWeight = vWeight = prm.weight;
			vTap = prm.tap;
		}
		
		if ((UOSInt)siHeight != buffH || (dwidth != buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			buffW = dwidth;
			buffH = siHeight;
			this->buffPtr = buffPtr = MemAllocA(UInt8, buffW * buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			MTHorizontalFilterPA(src, buffPtr, dwidth, siHeight, hTap,hIndex, hWeight, sbpl, (OSInt)dwidth << 3, siWidth);
//			horizontal_filter_pa(src, buffPtr, dwidth, sheight, hTap,hIndex, hWeight, sbpl, (OSInt)dwidth << 3, this->rgbTable, swidth, );
		}
		else
		{
			MTHorizontalFilter(src, buffPtr, dwidth, siHeight, hTap,hIndex, hWeight, sbpl, (OSInt)dwidth << 3, siWidth);
//			horizontal_filter(src, buffPtr, dwidth, sheight, hTap,hIndex, hWeight, sbpl, (OSInt)dwidth << 3, this->rgbTable, swidth, );
		}
		MTVerticalFilter(buffPtr, dest, dwidth, dheight, vTap, vIndex, vWeight, (OSInt)dwidth << 3, dbpl);
//		vertical_filter(buffPtr, dest, dwidth, dheight, vTap, vIndex, vWeight, (OSInt)dwidth << 3, dbpl, this->rgbTable);
	}
	else if ((UOSInt)siWidth != dwidth)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (hsSize != swidth || hdSize != dwidth || hsOfst != xOfst || !this->hIndex.SetTo(hIndex) || !this->hWeight.SetTo(hWeight))
		{
			DestoryHori();

			if (swidth > UOSInt2Double(dwidth))
			{
				SetupDecimationParameter(this->hnTap, swidth, siWidth, dwidth, prm, 8, xOfst);
			}
			else
			{
				SetupInterpolationParameter(this->hnTap, swidth, siWidth, dwidth, prm, 8, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
		}
		if ((UOSInt)siHeight != buffH || (dwidth != buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			this->buffW = dwidth;
			this->buffH = siHeight;
			this->buffPtr = buffPtr = MemAllocA(UInt8, this->buffW * this->buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			MTHorizontalFilterPA(src, buffPtr, dwidth, siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, siWidth);
		}
		else
		{
			MTHorizontalFilter(src, buffPtr, dwidth, siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, siWidth);
		}
		MTCollapse(buffPtr, dest, dwidth, dheight, (OSInt)dwidth << 3, dbpl);
	}
	else if (siHeight != dheight)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (vsSize != sheight || vdSize != dheight || vsStep != (OSInt)siWidth * 8 || vsOfst != yOfst || !this->vIndex.SetTo(vIndex) || !this->vWeight.SetTo(vWeight))
		{
			DestoryVert();
			vsStep = (OSInt)siWidth * 8;

			if (sheight > UOSInt2Double(dheight))
			{
				SetupDecimationParameter(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			else
			{
				SetupDecimationParameter(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			this->vIndex = vIndex = prm.index;
			this->vWeight = vWeight = prm.weight;
			vTap = prm.tap;
		}
		if ((UOSInt)siHeight != this->buffH || ((UOSInt)siWidth != this->buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			this->buffW = siWidth;
			this->buffH = siHeight;
			this->buffPtr = buffPtr = MemAllocA(UInt8, this->buffW * this->buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			MTExpandPA(src, buffPtr, siWidth, siHeight, sbpl, (OSInt)siWidth << 3);
		}
		else
		{
			MTExpand(src, buffPtr, siWidth, siHeight, sbpl, (OSInt)siWidth << 3);
		}
		MTVerticalFilter(buffPtr, dest, siWidth, dheight, vTap, vIndex, vWeight, (OSInt)siWidth << 3, dbpl);
		mutUsage.EndUse();
	}
	else
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			MTCopyPA(src, dest, (UOSInt)siWidth, dheight, sbpl, dbpl);
		}
		else
		{
			MTCopy(src, dest, (UOSInt)siWidth, dheight, sbpl, dbpl);
		}
		mutUsage.EndUse();
	}
}

Bool Media::Resizer::LanczosResizer16_C8::Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg)
{
	if (srcImg->info.fourcc != 0 && srcImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (destImg->info.fourcc != 0 && destImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (srcImg->info.pf != Media::PF_LE_B16G16R16A16 || destImg->info.pf != Media::PF_B8G8R8A8)
		return false;
	//destImg->info.color.rgbGamma = srcImg->info.color.rgbGamma;
	this->SetDestProfile(destImg->info.color);
	this->SetSrcProfile(srcImg->info.color);
	if (srcImg->info.fourcc == destImg->info.fourcc)
	{
		Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(srcImg->info.dispSize.x), UOSInt2Double(srcImg->info.dispSize.y), 0, 0, destImg->data, (OSInt)destImg->GetDataBpl(), destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
	else
	{
		OSInt dbpl = (OSInt)destImg->GetDataBpl();
		Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(srcImg->info.dispSize.x), UOSInt2Double(srcImg->info.dispSize.y), 0, 0, destImg->data + (OSInt)(destImg->info.storeSize.y - 1) * dbpl, -dbpl, destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
}

void Media::Resizer::LanczosResizer16_C8::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void Media::Resizer::LanczosResizer16_C8::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	this->rgbChanged = true;
}

void Media::Resizer::LanczosResizer16_C8::SetSrcProfile(NN<const Media::ColorProfile> srcProfile)
{
	if (!this->srcProfile.Equals(srcProfile))
	{
		this->srcProfile.Set(srcProfile);
		this->rgbChanged = true;
	}
}

void Media::Resizer::LanczosResizer16_C8::SetDestProfile(NN<const Media::ColorProfile> destProfile)
{
	if (!this->destProfile.Equals(destProfile))
	{
		this->destProfile.Set(destProfile);
		this->rgbChanged = true;
	}
}

Media::AlphaType Media::Resizer::LanczosResizer16_C8::GetDestAlphaType()
{
	if (this->srcAlphaType == Media::AT_ALPHA_ALL_FF)
	{
		return Media::AT_ALPHA_ALL_FF;
	}
	else if (this->srcAlphaType == Media::AT_IGNORE_ALPHA)
	{
		return Media::AT_ALPHA_ALL_FF;
	}
	else
	{
		return Media::AT_PREMUL_ALPHA;
	}
}

Bool Media::Resizer::LanczosResizer16_C8::IsSupported(NN<const Media::FrameInfo> srcInfo)
{
	if (srcInfo->fourcc != 0)
		return false;
	if (srcInfo->pf != Media::PF_LE_B16G16R16A16)
		return false;
	return true;
}

Optional<Media::StaticImage> Media::Resizer::LanczosResizer16_C8::ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *newImage;
	if (srcImage->GetImageType() != Media::RasterImage::ImageType::Static || !IsSupported(srcImage->info))
	{
		return nullptr;
	}
	Math::Size2D<UOSInt> targetSize = this->targetSize;
	if (targetSize.x == 0)
	{
		targetSize.x = (UOSInt)Double2Int32(srcBR.x - srcTL.x);
	}
	if (targetSize.y == 0)
	{
		targetSize.y = (UOSInt)Double2Int32(srcBR.y - srcTL.y);
	}
	CalOutputSize(srcImage->info, targetSize, destInfo, this->rar);
	destInfo.storeBPP = 32;
	destInfo.pf = Media::PF_B8G8R8A8;
	this->SetSrcProfile(srcImage->info.color);
	this->SetSrcAlphaType(srcImage->info.atype);
	if (this->destProfile.GetRTranParam()->GetTranType() != Media::CS::TRANT_VUNKNOWN && this->destProfile.GetRTranParam()->GetTranType() != Media::CS::TRANT_PUNKNOWN)
	{
		destInfo.color.GetRTranParam()->Set(NN<const Media::CS::TransferParam>(this->destProfile.GetRTranParam()));
		destInfo.color.GetGTranParam()->Set(NN<const Media::CS::TransferParam>(this->destProfile.GetGTranParam()));
		destInfo.color.GetBTranParam()->Set(NN<const Media::CS::TransferParam>(this->destProfile.GetBTranParam()));
	}
	destInfo.color.GetPrimaries()->Set(this->destProfile.GetPrimaries());
	destInfo.atype = this->GetDestAlphaType();
	NEW_CLASS(newImage, Media::StaticImage(destInfo));
	Int32 tlx = (Int32)srcTL.x;
	Int32 tly = (Int32)srcTL.y;
	Resize(((Media::StaticImage*)srcImage.Ptr())->data + (tlx << 2) + tly * (OSInt)srcImage->GetDataBpl(), (OSInt)srcImage->GetDataBpl(), srcBR.x - srcTL.x, srcBR.y - srcTL.y, srcTL.x - tlx, srcTL.y - tly, newImage->data, (OSInt)newImage->GetDataBpl(), newImage->info.dispSize.x, newImage->info.dispSize.y);
	return newImage;
}
