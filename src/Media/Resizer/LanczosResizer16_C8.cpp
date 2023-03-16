#include "Stdafx.h"
#include <float.h>
#include "MyMemory.h"
#include "Math/LanczosFilter.h"
#include "Math/Math.h"
#include "Media/RGBLUTGen.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/LanczosResizer16_C8.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

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

void Media::Resizer::LanczosResizer16_C8::setup_interpolation_parameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, Media::Resizer::LanczosResizer16_C8::LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
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

void Media::Resizer::LanczosResizer16_C8::setup_decimation_parameter(UOSInt nTap, Double source_length, UOSInt source_max_pos, UOSInt result_length, Media::Resizer::LanczosResizer16_C8::LRHPARAMETER *out, OSInt indexSep, Double offsetCorr)
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

void Media::Resizer::LanczosResizer16_C8::mt_horizontal_filter_pa(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UOSInt swidth)
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

void Media::Resizer::LanczosResizer16_C8::mt_horizontal_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UOSInt swidth)
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

void Media::Resizer::LanczosResizer16_C8::mt_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep)
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

void Media::Resizer::LanczosResizer16_C8::mt_expand_pa(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
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

void Media::Resizer::LanczosResizer16_C8::mt_expand(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
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

void Media::Resizer::LanczosResizer16_C8::mt_collapse(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
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

void Media::Resizer::LanczosResizer16_C8::mt_copy_pa(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
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

void Media::Resizer::LanczosResizer16_C8::mt_copy(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
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
	if (this->rgbTable == 0)
	{
		this->rgbTable = MemAlloc(UInt8, 65536 * 4 + 65536 * 4 * 8);
	}
	Media::RGBLUTGen lutGen(this->colorSess);
	lutGen.GenLRGB_BGRA8(this->rgbTable, &this->destProfile, 14, Media::CS::TransferFunc::GetRefLuminance(&this->srcProfile.rtransfer));
	lutGen.GenRGB16_LRGBC((Int64*)&this->rgbTable[262144], &this->srcProfile, this->destProfile.GetPrimaries(), 14);
}

void __stdcall Media::Resizer::LanczosResizer16_C8::DoTask(void *obj)
{
	TaskParam *ts = (TaskParam*)obj;

	if (ts->funcType == 3)
	{
		if (ts->swidth != ts->tmpbuffSize)
		{
			if (ts->tmpbuff)
				MemFreeA(ts->tmpbuff);
			ts->tmpbuffSize = ts->swidth;
			ts->tmpbuff = MemAllocA(UInt8, ts->swidth << 3);
		}
		LanczosResizer16_C8_horizontal_filter(ts->inPt, ts->outPt, ts->dwidth, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep, ts->me->rgbTable, ts->swidth, ts->tmpbuff);
	}
	else if (ts->funcType == 5)
	{
		LanczosResizer16_C8_vertical_filter(ts->inPt, ts->outPt, ts->dwidth, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
	else if (ts->funcType == 7)
	{
		LanczosResizer16_C8_expand(ts->inPt, ts->outPt, ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
	else if (ts->funcType == 9)
	{
		LanczosResizer16_C8_collapse(ts->inPt, ts->outPt, ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
	else if (ts->funcType == 11)
	{
		LanczosResizer16_C8_imgcopy(ts->inPt, ts->outPt, ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
	else if (ts->funcType == 12)
	{
		if (ts->swidth != ts->tmpbuffSize)
		{
			if (ts->tmpbuff)
				MemFreeA(ts->tmpbuff);
			ts->tmpbuffSize = ts->swidth;
			ts->tmpbuff = MemAllocA(UInt8, ts->swidth << 3);
		}
		LanczosResizer16_C8_horizontal_filter_pa(ts->inPt, ts->outPt, ts->dwidth, ts->height, ts->tap, ts->index, ts->weight, ts->sstep, ts->dstep, ts->me->rgbTable, ts->swidth, ts->tmpbuff);
	}
	else if (ts->funcType == 13)
	{
		LanczosResizer16_C8_expand_pa(ts->inPt, ts->outPt, ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
	else if (ts->funcType == 14)
	{
		LanczosResizer16_C8_imgcopy_pa(ts->inPt, ts->outPt, ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable);
	}
}

void Media::Resizer::LanczosResizer16_C8::DestoryHori()
{
	if (hIndex)
	{
		MemFreeA(hIndex);
		hIndex = 0;
	}
	if (hWeight)
	{
		MemFreeA(hWeight);
		hWeight = 0;
	}
	hsSize = 0;
}

void Media::Resizer::LanczosResizer16_C8::DestoryVert()
{
	if (vIndex)
	{
		MemFreeA(vIndex);
		vIndex = 0;
	}
	if (vWeight)
	{
		MemFreeA(vWeight);
		vWeight = 0;
	}
	vsSize = 0;
	vsStep = 0;
}

Media::Resizer::LanczosResizer16_C8::LanczosResizer16_C8(UOSInt hnTap, UOSInt vnTap, const Media::ColorProfile *srcProfile, const Media::ColorProfile *destProfile, Media::ColorManagerSess *colorSess, Media::AlphaType srcAlphaType) : Media::IImgResizer(srcAlphaType), srcProfile(srcProfile), destProfile(destProfile)
{
	this->nThread = Sync::Thread::GetThreadCnt();
	if (this->nThread > 6)
	{
		this->nThread = 6;
	}

	this->hnTap = hnTap << 1;
	this->vnTap = vnTap << 1;
	this->rgbChanged = true;
	this->rgbTable = 0;
	if (colorSess)
	{
		this->colorSess = colorSess;
		this->colorSess->AddHandler(this);
	}
	else
	{
		this->colorSess = 0;
	}

	this->params = MemAlloc(TaskParam, this->nThread);
	MemClear(this->params, sizeof(TaskParam) * this->nThread);
	UOSInt i = this->nThread;
	while (i-- > 0)
	{
		this->params[i].me = this;
	}
	NEW_CLASS(this->ptask, Sync::ParallelTask(this->nThread, false));

	hsSize = 0;
	hsOfst = 0;
	hdSize = 0;
	hIndex = 0;
	hWeight = 0;
	hTap = 0;

	vsSize = 0;
	vsOfst = 0;
	vdSize = 0;
	vsStep = 0;
	vIndex = 0;
	vWeight = 0;
	vTap = 0;

	buffW = 0;
	buffH = 0;
	buffPtr = 0;
}

Media::Resizer::LanczosResizer16_C8::~LanczosResizer16_C8()
{
	UOSInt i;
	if (this->colorSess)
	{
		this->colorSess->RemoveHandler(this);
	}
	DEL_CLASS(this->ptask);
	i = nThread;
	while (i-- > 0)
	{
		if (this->params[i].tmpbuff)
		{
			MemFreeA(this->params[i].tmpbuff);
			this->params[i].tmpbuff = 0;
			this->params[i].tmpbuffSize = 0;
		}
	}
	MemFree(this->params);

	DestoryHori();
	DestoryVert();
	if (buffPtr)
	{
		MemFreeA(buffPtr);
		buffPtr = 0;
	}
	if (this->rgbTable)
	{
		MemFree(this->rgbTable);
	}
}

void Media::Resizer::LanczosResizer16_C8::Resize(const UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
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
		Sync::MutexUsage mutUsage(&this->mut);
		if (this->hsSize != swidth || this->hdSize != dwidth || this->hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > UOSInt2Double(dwidth))
			{
				setup_decimation_parameter(this->hnTap, swidth, siWidth, dwidth, &prm, 8, xOfst);
			}
			else
			{
				setup_interpolation_parameter(this->hnTap, swidth, siWidth, dwidth,&prm, 8, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
		}

		if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != (OSInt)hdSize * 8 || this->vsOfst != yOfst)
		{
			DestoryVert();
			vsStep = (OSInt)hdSize * 8;

			if (sheight > UOSInt2Double(dheight))
			{
				setup_decimation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, vsStep, yOfst);
			}
			else
			{
				setup_interpolation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, vsStep, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
		}
		
		if ((UOSInt)siHeight != buffH || (dwidth != buffW))
		{
			if (buffPtr)
			{
				MemFreeA(buffPtr);
				buffPtr = 0;
			}
			buffW = dwidth;
			buffH = siHeight;
			buffPtr = MemAllocA(UInt8, buffW * buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			mt_horizontal_filter_pa(src, buffPtr, dwidth, siHeight, hTap,hIndex, hWeight, sbpl, (OSInt)dwidth << 3, siWidth);
//			horizontal_filter_pa(src, buffPtr, dwidth, sheight, hTap,hIndex, hWeight, sbpl, (OSInt)dwidth << 3, this->rgbTable, swidth, );
		}
		else
		{
			mt_horizontal_filter(src, buffPtr, dwidth, siHeight, hTap,hIndex, hWeight, sbpl, (OSInt)dwidth << 3, siWidth);
//			horizontal_filter(src, buffPtr, dwidth, sheight, hTap,hIndex, hWeight, sbpl, (OSInt)dwidth << 3, this->rgbTable, swidth, );
		}
		mt_vertical_filter(buffPtr, dest, dwidth, dheight, vTap, vIndex, vWeight, (OSInt)dwidth << 3, dbpl);
//		vertical_filter(buffPtr, dest, dwidth, dheight, vTap, vIndex, vWeight, (OSInt)dwidth << 3, dbpl, this->rgbTable);
		mutUsage.EndUse();
	}
	else if ((UOSInt)siWidth != dwidth)
	{
		Sync::MutexUsage mutUsage(&this->mut);
		if (hsSize != swidth || hdSize != dwidth || hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > UOSInt2Double(dwidth))
			{
				setup_decimation_parameter(this->hnTap, swidth, siWidth, dwidth, &prm, 8, xOfst);
			}
			else
			{
				setup_interpolation_parameter(this->hnTap, swidth, siWidth, dwidth, &prm, 8, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			hIndex = prm.index;
			hWeight = prm.weight;
			hTap = prm.tap;
		}
		if ((UOSInt)siHeight != buffH || (dwidth != buffW))
		{
			if (buffPtr)
			{
				MemFreeA(buffPtr);
				buffPtr = 0;
			}
			buffW = dwidth;
			buffH = siHeight;
			buffPtr = MemAllocA(UInt8, buffW * buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			mt_horizontal_filter_pa(src, buffPtr, dwidth, siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, siWidth);
		}
		else
		{
			mt_horizontal_filter(src, buffPtr, dwidth, siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, siWidth);
		}
		mt_collapse(buffPtr, dest, dwidth, dheight, (OSInt)dwidth << 3, dbpl);
		mutUsage.EndUse();
	}
	else if (siHeight != dheight)
	{
		Sync::MutexUsage mutUsage(&this->mut);
		if (vsSize != sheight || vdSize != dheight || vsStep != (OSInt)siWidth * 8 || vsOfst != yOfst)
		{
			DestoryVert();
			vsStep = (OSInt)siWidth * 8;

			if (sheight > UOSInt2Double(dheight))
			{
				setup_decimation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, vsStep, yOfst);
			}
			else
			{
				setup_interpolation_parameter(this->vnTap, sheight, siHeight, dheight, &prm, vsStep, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			vIndex = prm.index;
			vWeight = prm.weight;
			vTap = prm.tap;
		}
		if ((UOSInt)siHeight != buffH || ((UOSInt)siWidth != buffW))
		{
			if (buffPtr)
			{
				MemFreeA(buffPtr);
				buffPtr = 0;
			}
			buffW = siWidth;
			buffH = siHeight;
			buffPtr = MemAllocA(UInt8, buffW * buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			mt_expand_pa(src, buffPtr, siWidth, siHeight, sbpl, (OSInt)siWidth << 3);
		}
		else
		{
			mt_expand(src, buffPtr, siWidth, siHeight, sbpl, (OSInt)siWidth << 3);
		}
		mt_vertical_filter(buffPtr, dest, siWidth, dheight, vTap, vIndex, vWeight, (OSInt)siWidth << 3, dbpl);
		mutUsage.EndUse();
	}
	else
	{
		Sync::MutexUsage mutUsage(&this->mut);
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			mt_copy_pa(src, dest, (UOSInt)siWidth, dheight, sbpl, dbpl);
		}
		else
		{
			mt_copy(src, dest, (UOSInt)siWidth, dheight, sbpl, dbpl);
		}
		mutUsage.EndUse();
	}
}

Bool Media::Resizer::LanczosResizer16_C8::Resize(Media::StaticImage *srcImg, Media::StaticImage *destImg)
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
		Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(srcImg->info.dispWidth), UOSInt2Double(srcImg->info.dispHeight), 0, 0, destImg->data, (OSInt)destImg->GetDataBpl(), destImg->info.dispWidth, destImg->info.dispHeight);
		return true;
	}
	else
	{
		OSInt dbpl = (OSInt)destImg->GetDataBpl();
		Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(srcImg->info.dispWidth), UOSInt2Double(srcImg->info.dispHeight), 0, 0, destImg->data + (OSInt)(destImg->info.storeHeight - 1) * dbpl, -dbpl, destImg->info.dispWidth, destImg->info.dispHeight);
		return true;
	}
}

void Media::Resizer::LanczosResizer16_C8::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void Media::Resizer::LanczosResizer16_C8::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	this->rgbChanged = true;
}

void Media::Resizer::LanczosResizer16_C8::SetSrcProfile(const Media::ColorProfile *srcProfile)
{
	if (!this->srcProfile.Equals(srcProfile))
	{
		this->srcProfile.Set(srcProfile);
		this->rgbChanged = true;
	}
}

void Media::Resizer::LanczosResizer16_C8::SetDestProfile(const Media::ColorProfile *destProfile)
{
	if (!this->destProfile.Equals(destProfile))
	{
		this->destProfile.Set(destProfile);
		this->rgbChanged = true;
	}
}

Media::AlphaType Media::Resizer::LanczosResizer16_C8::GetDestAlphaType()
{
	if (this->srcAlphaType == Media::AT_NO_ALPHA)
	{
		return Media::AT_NO_ALPHA;
	}
	else
	{
		return Media::AT_PREMUL_ALPHA;
	}
}

Bool Media::Resizer::LanczosResizer16_C8::IsSupported(const Media::FrameInfo *srcInfo)
{
	if (srcInfo->fourcc != 0)
		return false;
	if (srcInfo->pf != Media::PF_LE_B16G16R16A16)
		return false;
	return true;
}

Media::StaticImage *Media::Resizer::LanczosResizer16_C8::ProcessToNewPartial(const Media::Image *srcImage, Double srcX1, Double srcY1, Double srcX2, Double srcY2)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *newImage;
	if (srcImage->GetImageType() != Media::Image::ImageType::Static || !IsSupported(&srcImage->info))
	{
		return 0;
	}
	UOSInt targetWidth = this->targetWidth;
	UOSInt targetHeight = this->targetHeight;
	if (targetWidth == 0)
	{
		targetWidth = (UOSInt)Double2Int32(srcX2 - srcX1);
	}
	if (targetHeight == 0)
	{
		targetHeight = (UOSInt)Double2Int32(srcY2 - srcY1);
	}
	CalOutputSize(&srcImage->info, targetWidth, targetHeight, &destInfo, this->rar);
	destInfo.storeBPP = 32;
	destInfo.pf = Media::PF_B8G8R8A8;
	this->SetSrcProfile(srcImage->info.color);
	this->SetSrcAlphaType(srcImage->info.atype);
	if (this->destProfile.GetRTranParam()->GetTranType() != Media::CS::TRANT_VUNKNOWN && this->destProfile.GetRTranParam()->GetTranType() != Media::CS::TRANT_PUNKNOWN)
	{
		destInfo.color->GetRTranParam()->Set(this->destProfile.GetRTranParam());
		destInfo.color->GetGTranParam()->Set(this->destProfile.GetGTranParam());
		destInfo.color->GetBTranParam()->Set(this->destProfile.GetBTranParam());
	}
	destInfo.color->GetPrimaries()->Set(this->destProfile.GetPrimaries());
	destInfo.atype = this->GetDestAlphaType();
	NEW_CLASS(newImage, Media::StaticImage(&destInfo));
	Int32 tlx = (Int32)srcX1;
	Int32 tly = (Int32)srcY1;
	Resize(((Media::StaticImage*)srcImage)->data + (tlx << 2) + tly * (OSInt)srcImage->GetDataBpl(), (OSInt)srcImage->GetDataBpl(), srcX2 - srcX1, srcY2 - srcY1, srcX1 - tlx, srcY1 - tly, newImage->data, (OSInt)newImage->GetDataBpl(), newImage->info.dispWidth, newImage->info.dispHeight);
	return newImage;
}
