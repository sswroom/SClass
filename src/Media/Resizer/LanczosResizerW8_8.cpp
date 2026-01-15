#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Math/LanczosFilter.h"
#include "Media/RGBLUTGen.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/LanczosResizerW8_8.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

#define PI 3.141592653589793

extern "C"
{
	void LanczosResizerW8_8_horizontal_filter(const UInt8 *inPt, UInt8 *outPt,UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int16 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable, UIntOS swidth, UInt8 *tmpbuff);
	void LanczosResizerW8_8_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int16 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
	void LanczosResizerW8_8_expand(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
	void LanczosResizerW8_8_collapse(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
	void LanczosResizerW8_8_imgcopy(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
}

void Media::Resizer::LanczosResizerW8_8::SetupInterpolationParameterV(UIntOS nTap, Double source_length, IntOS source_max_pos, UIntOS result_length, NN<Media::Resizer::LanczosResizerW8_8::LRHPARAMETER> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double *work;
	Double  sum;
	Double  pos;
	Double dnTap = UIntOS2Double(nTap);
	Double posDiff = (dnTap / 2 - 0.5);//2.5);
	Double dresultLength = UIntOS2Double(result_length);
	Math::LanczosFilter lanczos(nTap);
	IntOS maxOfst = (source_max_pos - 1) * indexSep;

	out->length = result_length;
	out->tap = nTap;
	out->weight = MemAllocA(Int16, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);
	out->tap += out->tap & 1;

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) + 0.5) * source_length;
		pos = pos / dresultLength + offsetCorr;
		n = (IntOS)Math_Fix(pos - posDiff);
		pos = (IntOS2Double(n) + 0.5 - pos);
		sum = 0;
		for(j = 0; j < out->tap; j++)
		{
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= source_max_pos){
				out->index[i * out->tap + j] = maxOfst;
			}else{
				out->index[i * out->tap + j] = n * indexSep;
			}
			work[j] = lanczos.Weight(pos);
			sum += work[j];
			pos += 1;
			n += 1;
		}

		Double workRate = 32767.0 / sum;
		j = 0;
		while (j < out->tap)
		{
			Int16 v1 = (Int16)(0xffff & Double2Int32(work[j] * workRate));
			Int16 v2 = (Int16)(0xffff & Double2Int32(work[j + 1] * workRate));
			Int16 *tmpPtr = &out->weight[i * out->tap + j];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			j += 2;
		}
		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerW8_8::SetupDecimationParameterV(UIntOS nTap, Double source_length, IntOS source_max_pos, UIntOS result_length, NN<Media::Resizer::LanczosResizerW8_8::LRHPARAMETER> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	UIntOS ttap;
	Double *work;
	Double  sum;
	Double  pos, phase;
	Double dnTap = UIntOS2Double(nTap);
	Double rLength = UIntOS2Double(result_length);
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UIntOS)Math_Fix((dnTap * (source_length) + (rLength - 1)) / rLength);
	ttap = out->tap;
	out->tap += out->tap & 1;

	out->weight = MemAllocA(Int16, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) - Math_Fix(dnTap / 2) + 0.5) * source_length / rLength + 0.5;
		n = (IntOS)floor(pos + offsetCorr);
		sum = 0;
		j = 0;
		while (j < ttap)
		{
			phase = (IntOS2Double(n) + 0.5)*rLength;
			phase /= source_length;
			phase -= (UIntOS2Double(i)+0.5);
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if(n >= source_max_pos){
				out->index[i * out->tap + j] = (source_max_pos-1) * indexSep;
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

		Double workRate = 32767.0 / sum;
		j = 0;
		while (j < ttap)
		{
			Int16 v1 = (Int16)(0xffff & Double2Int32(work[j] * workRate));
			Int16 v2 = (Int16)(0xffff & Double2Int32(work[j + 1] * workRate));
			Int16 *tmpPtr = &out->weight[i * out->tap + j];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			j += 2;
		}
		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerW8_8::SetupInterpolationParameterH(UIntOS nTap, Double source_length, IntOS source_max_pos, UIntOS result_length, NN<Media::Resizer::LanczosResizerW8_8::LRHPARAMETER> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double *work;
	Double  sum;
	Double  pos;
	Double posDiff = UIntOS2Double(nTap / 2) - 0.5; //2.5
	Double dresultLength = UIntOS2Double(result_length);
	IntOS ind1;
	IntOS ind2;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = nTap;
	out->weight = MemAllocA(Int16, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) + 0.5) * source_length;
		pos = pos / dresultLength + offsetCorr;
		n = (IntOS)Math_Fix(pos - posDiff);
		pos = (IntOS2Double(n) + 0.5 - pos);
		sum = 0;
		j = 0;
		while (j < out->tap)
		{
			if (n < 0)
			{
				ind1 = 0;
				ind2 = 0;
				work[j] = lanczos.Weight(pos) + lanczos.Weight(pos + 1);
				work[j + 1] = 0;
				sum += work[j];
			}
			else if (n >= source_max_pos - 1)
			{
				ind1 = source_max_pos - 2;
				ind2 = ind1 + 1;
				work[j] = 0;
				work[j + 1] = lanczos.Weight(pos) + lanczos.Weight(pos + 1);
				sum += work[j + 1];
			}
			else
			{
				ind1 = n;
				ind2 = n + 1;
				work[j] = lanczos.Weight(pos);
				work[j + 1] = lanczos.Weight(pos + 1);
				sum += work[j] + work[j + 1];
			}
			out->index[i * out->tap + j] = ind1 * indexSep;
			out->index[i * out->tap + j + 1] = ind2 * indexSep;
			pos += 2;
			n += 2;
			j += 2;
		}

		Double workRate = 16384.0 / sum;
		j = 0;
		while (j < out->tap)
		{
			UInt16 v1;
			UInt16 v2;
			UInt16 *tmpPtr;
			v1 = (UInt16)(0xffff & Double2Int32(work[j] * workRate));
			v2 = (UInt16)(0xffff & Double2Int32(work[j + 1] * workRate));
			tmpPtr = (UInt16*)&out->weight[i * out->tap + j];
			tmpPtr[0] = v1;
			tmpPtr[1] = v2;
			j += 2;
		}
		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerW8_8::SetupDecimationParameterH(UIntOS nTap, Double source_length, IntOS source_max_pos, UIntOS result_length, NN<Media::Resizer::LanczosResizerW8_8::LRHPARAMETER> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double dn;
	Double di;
	UIntOS ttap;
	Double *work;
	Double  sum;
	Double  pos;
	Double phase;
	Double phase2;
	IntOS ind1;
	IntOS ind2;
	Double rLength = UIntOS2Double(result_length);
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UIntOS)Math_Fix((UIntOS2Double(nTap) * (source_length) + (rLength - 1)) / rLength);
	ttap = out->tap;
	out->tap += out->tap & 1;

	out->weight = MemAllocA(Int16, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		di = UIntOS2Double(i);
		pos = (di - UIntOS2Double(nTap / 2) + 0.5) * source_length / rLength + 0.5;
		dn = Math_Fix(pos + offsetCorr);
		n = (Int32)dn;
		sum = 0;
		j = 0;
		while (j < ttap)
		{
			dn = IntOS2Double(n);
			phase = ((dn + 0.5) * rLength / source_length) - (di + 0.5);
			phase2 = ((dn + 1.5) * rLength / source_length) - (di + 0.5);
			if (n < 0)
			{
				ind1 = 0;
				ind2 = 0;
				work[j] = lanczos.Weight(phase) + lanczos.Weight(phase2);
				work[j + 1] = 0;
				sum += work[j];
			}
			else if (n >= source_max_pos - 1)
			{
				ind1 = source_max_pos - 2;
				ind2 = ind1 + 1;
				work[j] = 0;
				work[j + 1] = lanczos.Weight(phase) + lanczos.Weight(phase2);
				sum += work[j + 1];
			}
			else
			{
				ind1 = n;
				ind2 = n + 1;
				work[j] = lanczos.Weight(phase);
				work[j + 1] = lanczos.Weight(phase2);
				sum += work[j] + work[j + 1];
			}
			out->index[i * out->tap + j] = ind1 * indexSep;
			out->index[i * out->tap + j + 1] = ind2 * indexSep;
			n += 2;
			j += 2;
		}

		j = 0;
		while (j < ttap)
		{
			Int32 v1 = Double2Int32((work[j] / sum) * 16384.0);
			Int32 v2 = Double2Int32((work[j + 1] / sum) * 16384.0);
			Int16 *tmpPtr = (Int16*)&out->weight[i * out->tap + j];
			tmpPtr[0] = (Int16)v1;
			tmpPtr[1] = (Int16)v2;
			j += 2;
		}

		i++;
	}

	MemFree(work);
}

void Media::Resizer::LanczosResizerW8_8::MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int16> weight, IntOS sstep, IntOS dstep, UIntOS swidth)
{
#if defined(VERBOSE)
	printf("LR8_C8: HFilter w = %d, h = %d, tap = %d\r\n", (UInt32)dwidth, (UInt32)height, (UInt32)tap);
#endif
	UIntOS currHeight;
	UIntOS lastHeight = height;
	FuncType funcType = FuncType::HFilter;
	UIntOS i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt + (IntOS)currHeight * sstep;
		this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
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
			this->params[i].funcType = funcType;
			this->ptask->AddTask(DoTask, &this->params[i]);
		}
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizerW8_8::MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int16> weight, IntOS sstep, IntOS dstep)
{
#if defined(VERBOSE)
	printf("LR8_C8: VFilter w = %d, h = %d, tap = %d\r\n", (UInt32)dwidth, (UInt32)height, (UInt32)tap);
#endif
	UIntOS currHeight;
	UIntOS lastHeight = height;
	FuncType funcType = FuncType::VFilter;
	UIntOS i = this->nThread;
	if (height < 16)
	{
		LanczosResizerW8_8_vertical_filter(inPt.Ptr(), outPt.Ptr(), dwidth, height, tap, index.Ptr(), weight.Ptr(), sstep, dstep, this->rgbTable.Ptr());
	}
	else
	{
		while (i-- > 0)
		{
			currHeight = MulDivUOS(i, height, this->nThread);
			this->params[i].inPt = inPt;
			this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
			this->params[i].dwidth = dwidth;
			this->params[i].height = lastHeight - currHeight;
			this->params[i].tap = tap;
			this->params[i].index = index + currHeight * tap;
			this->params[i].weight = weight + currHeight * tap;
			this->params[i].sstep = sstep;
			this->params[i].dstep = dstep;

			this->params[i].funcType = funcType;
			this->ptask->AddTask(DoTask, &this->params[i]);
			lastHeight = currHeight;
		}
		this->ptask->WaitForIdle();
	}
}

void Media::Resizer::LanczosResizerW8_8::MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS height, IntOS sstep, IntOS dstep)
{
	LanczosResizerW8_8_expand(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr());
}

void Media::Resizer::LanczosResizerW8_8::MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS height, IntOS sstep, IntOS dstep)
{
	LanczosResizerW8_8_collapse(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr());
}

void Media::Resizer::LanczosResizerW8_8::MTCopy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS dwidth, UIntOS height, IntOS sstep, IntOS dstep)
{
	LanczosResizerW8_8_imgcopy(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr());
}

void Media::Resizer::LanczosResizerW8_8::UpdateRGBTable()
{
	UnsafeArray<UInt8> rgbTable;
	if (!this->rgbTable.SetTo(rgbTable))
	{
		this->rgbTable = rgbTable = MemAllocArr(UInt8, 65536 + 256 * 2);
	}
	Media::RGBLUTGen lutGen(this->colorSess);
	lutGen.GenL_W8(rgbTable, this->destTran, 14, Media::CS::TransferFunc::GetRefLuminance(this->srcTran));
	lutGen.GenW8_L((UInt16*)&rgbTable[65536], this->srcTran, 14);
}

void __stdcall Media::Resizer::LanczosResizerW8_8::DoTask(AnyType obj)
{
	UnsafeArray<UInt8> tmpbuff;
	NN<TaskParam> ts = obj.GetNN<TaskParam>();
	switch (ts->funcType)
	{
	case FuncType::HFilter:
		if (ts->swidth != ts->tmpbuffSize || !ts->tmpbuff.SetTo(tmpbuff))
		{
			if (ts->tmpbuff.SetTo(tmpbuff))
				MemFreeAArr(tmpbuff);
			ts->tmpbuffSize = ts->swidth;
			ts->tmpbuff = tmpbuff = MemAllocAArr(UInt8, ts->swidth << 3);
		}
		LanczosResizerW8_8_horizontal_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->swidth, tmpbuff.Ptr());
		break;
	case FuncType::Expand:
		LanczosResizerW8_8_expand(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		break;
	case FuncType::VFilter:
		LanczosResizerW8_8_vertical_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		break;
	case FuncType::Collapse:
		LanczosResizerW8_8_collapse(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		break;
	case FuncType::ImgCopy:
		LanczosResizerW8_8_imgcopy(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		break;
	default:
	case FuncType::NoFunction:
		break;
	}
}

void Media::Resizer::LanczosResizerW8_8::DestoryHori()
{
	UnsafeArray<IntOS> hIndex;
	UnsafeArray<Int16> hWeight;
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
	hsSize = 0;
}

void Media::Resizer::LanczosResizerW8_8::DestoryVert()
{
	UnsafeArray<IntOS> vIndex;
	UnsafeArray<Int16> vWeight;
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
	vsSize = 0;
	vsStep = 0;
}

Media::Resizer::LanczosResizerW8_8::LanczosResizerW8_8(UIntOS hnTap, UIntOS vnTap, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess) : Media::ImageResizer(Media::AT_ALPHA_ALL_FF)
{
	UIntOS i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 6)
	{
		this->nThread = 6;
	}

	this->hnTap = hnTap << 1;
	this->vnTap = vnTap << 1;
	this->rgbChanged = true;
	this->rgbTable = nullptr;
	this->srcPF = Media::PF_B8G8R8A8;
	this->srcPal = nullptr;
	NN<Media::ColorManagerSess> nncolorSess;
	if (colorSess.SetTo(nncolorSess))
	{
		this->colorSess = nncolorSess;
		nncolorSess->AddHandler(*this);
	}
	else
	{
		this->colorSess = nullptr;
	}

	this->params = MemAllocArr(Media::Resizer::LanczosResizerW8_8::TaskParam, this->nThread);
	MemClear(this->params.Ptr(), sizeof(Media::Resizer::LanczosResizerW8_8::TaskParam) * this->nThread);
	i = this->nThread;
	while(i-- > 0)
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

Media::Resizer::LanczosResizerW8_8::~LanczosResizerW8_8()
{
	UnsafeArray<UInt8> buffPtr;
	UIntOS i;
	NN<Media::ColorManagerSess> nncolorSess;
	UnsafeArray<UInt8> rgbTable;
	UnsafeArray<UInt8> tmpbuff;
	if (this->colorSess.SetTo(nncolorSess))
	{
		nncolorSess->RemoveHandler(*this);
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

void Media::Resizer::LanczosResizerW8_8::Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight)
{
	UnsafeArray<IntOS> hIndex;
	UnsafeArray<Int16> hWeight;
	UnsafeArray<IntOS> vIndex;
	UnsafeArray<Int16> vWeight;
	UnsafeArray<UInt8> buffPtr;
	Media::Resizer::LanczosResizerW8_8::LRHPARAMETER prm;
	Double w;
	Double h;
	IntOS siWidth;
	IntOS siHeight;
	if (dwidth < this->hnTap || dheight < this->vnTap)
		return;

#if defined(VERBOSE)
	printf("LR8_C8: Resize %lf x %lf -> %d x %d\r\n", swidth, sheight, (UInt32)dwidth, (UInt32)dheight);
#endif
	w = xOfst + swidth;
	h = yOfst + sheight;
	siWidth = (IntOS)w;
	siHeight = (IntOS)h;
	w -= IntOS2Double(siWidth);
	h -= IntOS2Double(siHeight);
	if (w > 0)
		siWidth++;
	if (h > 0)
		siHeight++;

	if (siWidth < (IntOS)this->hnTap || siHeight < (IntOS)this->vnTap)
		return;

	if (this->rgbChanged)
	{
		this->rgbChanged = false;
		UpdateRGBTable();
	}

	if (siWidth != (IntOS)dwidth && siHeight != (IntOS)dheight)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->hsSize != swidth || this->hdSize != dwidth || this->hsOfst != xOfst || !this->hIndex.SetTo(hIndex) || !this->hWeight.SetTo(hWeight))
		{
			DestoryHori();

			if (swidth > UIntOS2Double(dwidth))
			{
				SetupDecimationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 2, xOfst);
			}
			else
			{
				SetupInterpolationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 2, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			this->hIndex = hIndex = prm.index;
			this->hWeight = hWeight = prm.weight;
			hTap = prm.tap;
		}

		if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != (IntOS)hdSize * 2 || this->vsOfst != yOfst || !this->vIndex.SetTo(vIndex) || !this->vWeight.SetTo(vWeight))
		{
			DestoryVert();
			vsStep = (IntOS)hdSize * 2;

			if (sheight > UIntOS2Double(dheight))
			{
				SetupDecimationParameterV(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			else
			{
				SetupInterpolationParameterV(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			this->vIndex = vIndex = prm.index;
			this->vWeight = vWeight = prm.weight;
			vTap = prm.tap;
		}
		
		if (siHeight != buffH || (dwidth != buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			buffW = dwidth;
			buffH = siHeight;
			this->buffPtr = buffPtr = MemAllocA(UInt8, buffW * (UIntOS)buffH << 3);
		}
		MTHorizontalFilter(src, buffPtr, dwidth, (UIntOS)siHeight, hTap, hIndex, hWeight, sbpl, (IntOS)dwidth << 1, (UIntOS)siWidth);
		MTVerticalFilter(buffPtr, dest, dwidth, dheight, vTap, vIndex, vWeight, (IntOS)dwidth << 1, dbpl);
	}
	else if (siWidth != (IntOS)dwidth)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (hsSize != swidth || hdSize != dwidth || hsOfst != xOfst || !this->hIndex.SetTo(hIndex) || !this->hWeight.SetTo(hWeight))
		{
			DestoryHori();

			if (swidth > UIntOS2Double(dwidth))
			{
				SetupDecimationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 2, xOfst);
			}
			else
			{
				SetupInterpolationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 2, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			this->hIndex = hIndex = prm.index;
			this->hWeight = hWeight = prm.weight;
			hTap = prm.tap;
		}
		if (siHeight != buffH || (dwidth != buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			buffW = dwidth;
			buffH = siHeight;
			this->buffPtr = buffPtr = MemAllocAArr(UInt8, buffW * (UIntOS)buffH << 3);
		}
		MTHorizontalFilter(src, buffPtr, dwidth, (UIntOS)siHeight, hTap, hIndex, hWeight, sbpl, (IntOS)dwidth << 1, (UIntOS)siWidth);
		MTCollapse(buffPtr, dest, dwidth, dheight, (IntOS)dwidth << 1, dbpl);
	}
	else if (siHeight != (IntOS)dheight)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (vsSize != sheight || vdSize != dheight || vsStep != Double2Int32(swidth) * 2 || vsOfst != yOfst || !this->vIndex.SetTo(vIndex) || !this->vWeight.SetTo(vWeight))
		{
			DestoryVert();
			vsStep = siWidth * 2;

			if (sheight > UIntOS2Double(dheight))
			{
				SetupDecimationParameterV(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			else
			{
				SetupInterpolationParameterV(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			this->vIndex = vIndex = prm.index;
			this->vWeight = vWeight = prm.weight;
			vTap = prm.tap;
		}
		if (siHeight != buffH || (siWidth != (IntOS)buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			buffW = (UIntOS)siWidth;
			buffH = siHeight;
			this->buffPtr = buffPtr = MemAllocA(UInt8, buffW * (UIntOS)buffH << 3);
		}
		MTExpand(src, buffPtr, (UIntOS)siWidth, (UIntOS)siHeight, sbpl, siWidth << 1);
		MTVerticalFilter(buffPtr, dest, (UIntOS)siWidth, dheight, vTap, vIndex, vWeight, siWidth << 1, dbpl);
	}
	else
	{
		Sync::MutexUsage mutUsage(this->mut);
		MTCopy(src, dest, (UIntOS)siWidth, dheight, sbpl, dbpl);
	}
}

Bool Media::Resizer::LanczosResizerW8_8::Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg)
{
	if (srcImg->info.fourcc != 0 && srcImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (destImg->info.fourcc != 0 && destImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (srcImg->info.pf != Media::PF_PAL_W8 || destImg->info.pf != Media::PF_PAL_W8)
		return false;
	//destImg->info.color.rgbGamma = srcImg->info.color.rgbGamma;
	SetDestTransfer(destImg->info.color.rtransfer);
	SetSrcTransfer(srcImg->info.color.rtransfer);
	if (srcImg->info.fourcc == destImg->info.fourcc)
	{
		Resize(srcImg->data.Ptr(), (IntOS)srcImg->GetDataBpl(), UIntOS2Double(srcImg->info.dispSize.x), UIntOS2Double(srcImg->info.dispSize.y), 0, 0, destImg->data.Ptr(), (IntOS)destImg->GetDataBpl(), destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
	else
	{
		IntOS dbpl = (IntOS)destImg->GetDataBpl();
		Resize(srcImg->data.Ptr(), (IntOS)srcImg->GetDataBpl(), UIntOS2Double(srcImg->info.dispSize.x), UIntOS2Double(srcImg->info.dispSize.y), 0, 0, destImg->data.Ptr() + (IntOS)(destImg->info.storeSize.y - 1) * dbpl, -dbpl, destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
}

void Media::Resizer::LanczosResizerW8_8::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void Media::Resizer::LanczosResizerW8_8::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	this->rgbChanged = true;
}

void Media::Resizer::LanczosResizerW8_8::SetSrcTransfer(NN<const Media::CS::TransferParam> srcTran)
{
	if (!this->srcTran.Equals(srcTran))
	{
		this->srcTran.Set(srcTran);
		this->rgbChanged = true;
	}
}

void Media::Resizer::LanczosResizerW8_8::SetDestTransfer(NN<const Media::CS::TransferParam> destTran)
{
	if (!this->destTran.Equals(destTran))
	{
		this->destTran.Set(destTran);
		this->rgbChanged = true;
	}
}

Bool Media::Resizer::LanczosResizerW8_8::IsSupported(NN<const Media::FrameInfo> srcInfo)
{
	if (srcInfo->fourcc != 0)
		return false;
	if (srcInfo->pf != Media::PF_PAL_W8)
		return false;
	return true;
}

Optional<Media::StaticImage> Media::Resizer::LanczosResizerW8_8::ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *newImage;
	if (srcImage->GetImageType() != Media::RasterImage::ImageType::Static || !IsSupported(srcImage->info))
	{
		return nullptr;
	}
	Math::Size2D<UIntOS> targetSize = this->targetSize;
	if (targetSize.x == 0)
	{
		targetSize.x = (UIntOS)Double2Int32(srcBR.x - srcTL.x);
	}
	if (targetSize.y == 0)
	{
		targetSize.y = (UIntOS)Double2Int32(srcBR.y - srcTL.y);
	}
	CalOutputSize(srcImage->info, targetSize, destInfo, this->rar);
	this->SetSrcTransfer(srcImage->info.color.rtransfer);
	this->SetSrcAlphaType(srcImage->info.atype);
	if (this->destTran.GetTranType() != Media::CS::TRANT_VUNKNOWN && this->destTran.GetTranType() != Media::CS::TRANT_PUNKNOWN)
	{
		destInfo.color.GetRTranParam()->Set(NN<const Media::CS::TransferParam>(this->destTran));
		destInfo.color.GetGTranParam()->Set(NN<const Media::CS::TransferParam>(this->destTran));
		destInfo.color.GetBTranParam()->Set(NN<const Media::CS::TransferParam>(this->destTran));
	}
	destInfo.color.GetPrimaries()->Set(srcImage->info.color.GetPrimariesRead());
	destInfo.atype = Media::AT_ALPHA_ALL_FF;
	destInfo.pf = Media::PF_PAL_W8;
	destInfo.storeBPP = 8;
	NEW_CLASS(newImage, Media::StaticImage(destInfo));
	UnsafeArray<UInt8> pal;
	if (newImage->pal.SetTo(pal))
	{
		UIntOS i = 0;
		UIntOS j = 256;
		while (i < j)
		{
			pal[0] = (UInt8)i;
			pal[1] = (UInt8)i;
			pal[2] = (UInt8)i;
			pal[3] = (UInt8)0xff;
			pal += 4;
			i++;
		}
	}
	Int32 tlx = (Int32)srcTL.x;
	Int32 tly = (Int32)srcTL.y;
	IntOS bpp = srcImage->info.storeBPP >> 3;
	Resize(NN<Media::StaticImage>::ConvertFrom(srcImage)->data.Ptr() + (tlx * bpp) + tly * (IntOS)srcImage->GetDataBpl(), (IntOS)srcImage->GetDataBpl(), srcBR.x - srcTL.x, srcBR.y - srcTL.y, srcTL.x - tlx, srcTL.y - tly, newImage->data.Ptr(), (IntOS)newImage->GetDataBpl(), newImage->info.dispSize.x, newImage->info.dispSize.y);
	return newImage;
}
