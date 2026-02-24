#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/LanczosFilter.h"
#include "Math/Math_C.h"
#include "Media/RGBLUTGen.h"
#include "Media/Resizer/LanczosResizerLR_C16.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

#define PI 3.141592653589793

extern "C"
{
	void LanczosResizerLR_C16_horizontal_filter(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
	void LanczosResizerLR_C16_vertical_filter(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, UIntOS tap, IntOS *index, Int64 *weight, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
	void LanczosResizerLR_C16_collapse(const UInt8 *inPt, UInt8 *outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep, UInt8 *rgbTable);
}

void Media::Resizer::LanczosResizerLR_C16::SetupInterpolationParameter(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	Double *work;
	Double  sum;
	Double  pos;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = nTap;
	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);

	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) + 0.5)*source_length;
		pos = pos / UIntOS2Double(result_length) + offsetCorr;
		n = (Int32)Math_Fix(pos - (UIntOS2Double(nTap) / 2 - 0.5));//2.5);
		pos = (IntOS2Double(n) + 0.5-pos);
		sum = 0;
		j = 0;
		while (j < out->tap)
		{
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if((UIntOS)n >= source_max_pos){
				out->index[i * out->tap + j] = (IntOS)(source_max_pos - 1) * indexSep;
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

void Media::Resizer::LanczosResizerLR_C16::SetupDecimationParameter(UIntOS nTap, Double source_length, UIntOS source_max_pos, UIntOS result_length, NN<LRHPARAMETER> out, IntOS indexSep, Double offsetCorr)
{
	UIntOS i;
	UIntOS j;
	IntOS n;
	UIntOS ttap;
	Double *work;
	Double  sum;
	Double  pos, phase;
	Math::LanczosFilter lanczos(nTap);

	out->length = result_length;
	out->tap = (UIntOS)Math_Fix((UIntOS2Double(nTap) * (source_length) + UIntOS2Double(result_length - 1)) / UIntOS2Double(result_length));
	ttap = out->tap;
	out->tap += out->tap & 1;

	out->weight = MemAllocA(Int64, out->length * out->tap);
	out->index = MemAllocA(IntOS, out->length * out->tap);
	
	work = MemAlloc(Double, out->tap);

	i = 0;
	while (i < result_length)
	{
		pos = (UIntOS2Double(i) - UIntOS2Double(nTap / 2) + 0.5) * source_length / UIntOS2Double(result_length) + 0.5;
		n = (Int32)Math_Fix(pos + offsetCorr);
		sum = 0;
		j = 0;
		while (j < ttap)
		{
			phase = (IntOS2Double(n) + 0.5) * UIntOS2Double(result_length);
			phase /= source_length;
			phase -= (UIntOS2Double(i) + 0.5);
			if(n < 0){
				out->index[i * out->tap + j] = 0;
			}else if((UIntOS)n >= source_max_pos){
				out->index[i * out->tap + j] = (IntOS)(source_max_pos-1) * indexSep;
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

void Media::Resizer::LanczosResizerLR_C16::MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep)
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

void Media::Resizer::LanczosResizerLR_C16::MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep)
{
	UIntOS currHeight;
	UIntOS lastHeight = height;
	UIntOS i = this->nThread;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, height, this->nThread);
		this->params[i].inPt = inPt;
		this->params[i].outPt = outPt + (IntOS)currHeight * dstep;
		this->params[i].width = width;
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

void Media::Resizer::LanczosResizerLR_C16::MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep)
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
		this->params[i].sstep = sstep;
		this->params[i].dstep = dstep;

		this->params[i].funcType = 9;
		this->ptask->AddTask(DoTask, &this->params[i]);
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizerLR_C16::UpdateRGBTable()
{
	UnsafeArray<UInt8> rgbTable;
	if (!this->rgbTable.SetTo(rgbTable))
	{
		this->rgbTable = rgbTable = MemAlloc(UInt8, 65536 * 6);
	}
	Media::RGBLUTGen lutGen(this->colorSess);
	lutGen.GenLRGB_RGB16(rgbTable, this->destColor, 14, this->srcRefLuminance);
}

void __stdcall Media::Resizer::LanczosResizerLR_C16::DoTask(AnyType obj)
{
	NN<TaskParam> ts = obj.GetNN<TaskParam>();
	if (ts->funcType == 3)
	{
		LanczosResizerLR_C16_horizontal_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 5)
	{
		LanczosResizerLR_C16_vertical_filter(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
	else if (ts->funcType == 9)
	{
		LanczosResizerLR_C16_collapse(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->width, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
	}
}

void Media::Resizer::LanczosResizerLR_C16::DestoryHori()
{
	UnsafeArray<IntOS> hIndex;
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

void Media::Resizer::LanczosResizerLR_C16::DestoryVert()
{
	UnsafeArray<IntOS> vIndex;
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

Media::Resizer::LanczosResizerLR_C16::LanczosResizerLR_C16(UIntOS hnTap, UIntOS vnTap, NN<const Media::ColorProfile> destColor, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance) : Media::ImageResizer(srcAlphaType), destColor(destColor)
{
	UIntOS i;
	nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 4)
	{
		this->nThread = this->nThread >> 1;
	}

	this->hnTap = hnTap << 1;
	this->vnTap = vnTap << 1;
	this->rgbChanged = true;
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
	this->srcRefLuminance = srcRefLuminance;
	this->rgbTable = nullptr;

	this->params = MemAllocArr(TaskParam, this->nThread);
	MemClear(this->params.Ptr(), sizeof(TaskParam) * this->nThread);
	i = nThread;
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

Media::Resizer::LanczosResizerLR_C16::~LanczosResizerLR_C16()
{
	UnsafeArray<UInt8> buffPtr;
	UnsafeArray<UInt8> rgbTable;
	NN<Media::ColorManagerSess> nncolorSess;
	if (this->colorSess.SetTo(nncolorSess))
	{
		nncolorSess->RemoveHandler(*this);
	}
	this->ptask.Delete();
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

void Media::Resizer::LanczosResizerLR_C16::Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight)
{
	UnsafeArray<IntOS> hIndex;
	UnsafeArray<Int64> hWeight;
	UnsafeArray<IntOS> vIndex;
	UnsafeArray<Int64> vWeight;
	UnsafeArray<UInt8> buffPtr;
	LRHPARAMETER prm;
	Double w;
	Double h;
	UIntOS siWidth;
	UIntOS siHeight;

	if (dwidth < 16 || dheight < 16)
		return;

	w = xOfst + swidth;
	h = yOfst + sheight;
	siWidth = (UIntOS)w;
	siHeight = (UIntOS)h;
	w -= UIntOS2Double(siWidth);
	h -= UIntOS2Double(siHeight);
	if (w > 0)
		siWidth++;
	if (h > 0)
		siHeight++;

	if (siWidth <= 0 || siHeight <= 0)
		return;
	if (this->rgbChanged)
	{
		this->rgbChanged = false;
		UpdateRGBTable();
	}

	if (swidth != UIntOS2Double(dwidth) && sheight != UIntOS2Double(dheight))
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->hsSize != swidth || this->hdSize != dwidth || this->hsOfst != xOfst || !this->hIndex.SetTo(hIndex) || !this->hWeight.SetTo(hWeight))
		{
			DestoryHori();

			if (swidth > UIntOS2Double(dwidth))
			{
				SetupDecimationParameter(this->hnTap, swidth, siWidth, dwidth, prm, 8, xOfst);
			}
			else
			{
				SetupInterpolationParameter(this->hnTap, swidth, siWidth, dwidth, prm, 8, xOfst);
			}
			this->hsSize = swidth;
			this->hdSize = dwidth;
			this->hsOfst = xOfst;
			this->hIndex = hIndex = prm.index;
			this->hWeight = hWeight = prm.weight;
			this->hTap = prm.tap;
		}

		if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != (IntOS)(dwidth << 3) || this->vsOfst != yOfst || !this->vIndex.SetTo(vIndex) || !this->vWeight.SetTo(vWeight))
		{
			DestoryVert();

			if (sheight > UIntOS2Double(dheight))
			{
				SetupDecimationParameter(this->vnTap, sheight, siHeight, dheight, prm, (IntOS)dwidth << 3, yOfst);
			}
			else
			{
				SetupInterpolationParameter(this->vnTap, sheight, siHeight, dheight, prm, (IntOS)dwidth << 3, yOfst);
			}
			this->vsSize = sheight;
			this->vdSize = dheight;
			this->vsOfst = yOfst;
			this->vsStep = (IntOS)dwidth << 3;
			this->vIndex = vIndex = prm.index;
			this->vWeight = vWeight = prm.weight;
			this->vTap = prm.tap;
		}
		
		if (siHeight != this->buffH || (dwidth != this->buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			buffW = dwidth;
			buffH = siHeight;
			this->buffPtr = buffPtr = MemAllocA64(UInt8, buffW * buffH << 3);
		}
		MTHorizontalFilter(src, buffPtr, dwidth, siHeight, hTap,hIndex, hWeight, sbpl, (IntOS)dwidth << 3);
		MTVerticalFilter(buffPtr, dest, dwidth, dheight, vTap, vIndex, vWeight, (IntOS)dwidth << 3, dbpl);
	}
	else if (swidth != UIntOS2Double(dwidth))
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (hsSize != swidth || hdSize != dwidth || hsOfst != xOfst || !this->hIndex.SetTo(hIndex) || !this->hWeight.SetTo(hWeight))
		{
			DestoryHori();

			if (swidth > UIntOS2Double(dwidth))
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
			this->hTap = prm.tap;
		}
		if (dheight != this->buffH || (dwidth != this->buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			buffW = dwidth;
			buffH = dheight;
			this->buffPtr = buffPtr = MemAllocA64(UInt8, buffW * buffH << 3);
		}
		MTHorizontalFilter(src, buffPtr, dwidth, siHeight, hTap, hIndex, hWeight, sbpl, (IntOS)dwidth << 3);
		MTCollapse(buffPtr, dest, dwidth, dheight, (IntOS)dwidth << 3, dbpl);
	}
	else if (sheight != UIntOS2Double(dheight))
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != sbpl || this->vsOfst != yOfst || !this->vIndex.SetTo(vIndex) || !this->vWeight.SetTo(vWeight))
		{
			DestoryVert();

			if (sheight > UIntOS2Double(dheight))
			{
				SetupDecimationParameter(this->vnTap, sheight, siHeight, dheight, prm, sbpl, yOfst);
			}
			else
			{
				SetupInterpolationParameter(this->vnTap, sheight, siHeight, dheight, prm, sbpl, yOfst);
			}
			this->vsSize = sheight;
			this->vdSize = dheight;
			this->vsOfst = yOfst;
			this->vsStep = sbpl;
			this->vIndex = vIndex = prm.index;
			this->vWeight = vWeight = prm.weight;
			this->vTap = prm.tap;
		}
		if (dheight != this->buffH || (siWidth != this->buffW) || !this->buffPtr.SetTo(buffPtr))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = nullptr;
			}
			this->buffW = siWidth;
			this->buffH = dheight;
			this->buffPtr = buffPtr = MemAllocA64(UInt8, this->buffW * this->buffH << 3);
		}
		MTVerticalFilter(src, dest, siWidth, dheight, vTap, vIndex, vWeight, sbpl, dbpl);
	}
	else
	{
		Sync::MutexUsage mutUsage(this->mut);
		MTCollapse(src, dest, siWidth, dheight, (IntOS)siWidth << 3, dbpl);
	}
}

void Media::Resizer::LanczosResizerLR_C16::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void Media::Resizer::LanczosResizerLR_C16::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	this->rgbChanged = true;
}

void Media::Resizer::LanczosResizerLR_C16::SetSrcRefLuminance(Double srcRefLuminance)
{
	this->srcRefLuminance = srcRefLuminance;
	this->rgbChanged = true;
}

Bool Media::Resizer::LanczosResizerLR_C16::IsSupported(NN<const Media::FrameInfo> srcInfo)
{
	if (srcInfo->fourcc != *(UInt32*)"LRGB")
		return false;
	return true;
}

Optional<Media::StaticImage> Media::Resizer::LanczosResizerLR_C16::ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *img;
	if (srcImage->GetImageClass() != Media::RasterImage::ImageClass::StaticImage || !IsSupported(srcImage->info))
		return nullptr;
	Math::Size2D<UIntOS> targetSize = this->targetSize;
	if (targetSize.x == 0)
	{
		targetSize.x = (UIntOS)Double2Int32(srcBR.x - srcTL.x);//srcImage->info.width;
	}
	if (targetSize.y == 0)
	{
		targetSize.y = (UIntOS)Double2Int32(srcBR.y - srcTL.y);//srcImage->info.height;
	}
	CalOutputSize(srcImage->info, targetSize, destInfo, rar);
	destInfo.fourcc = 0;
	destInfo.storeBPP = 16;
	destInfo.pf = Media::PF_LE_R5G6B5;
	destInfo.color.GetPrimaries()->Set(srcImage->info.color.GetPrimariesRead());
	if (this->destColor.GetRTranParam()->GetTranType() != Media::CS::TRANT_VUNKNOWN && this->destColor.GetRTranParam()->GetTranType() != Media::CS::TRANT_PUNKNOWN)
	{
		destInfo.color.GetRTranParam()->Set(NN<const Media::CS::TransferParam>(this->destColor.GetRTranParam()));
		destInfo.color.GetGTranParam()->Set(NN<const Media::CS::TransferParam>(this->destColor.GetGTranParam()));
		destInfo.color.GetBTranParam()->Set(NN<const Media::CS::TransferParam>(this->destColor.GetBTranParam()));
	}
	NEW_CLASS(img, Media::StaticImage(destInfo));
	Int32 tlx = (Int32)srcTL.x;
	Int32 tly = (Int32)srcTL.y;
	Resize(((Media::StaticImage*)srcImage.Ptr())->data + (IntOS)srcImage->GetDataBpl() * tly + (tlx << 3), (IntOS)srcImage->GetDataBpl(), srcBR.x - srcTL.x, srcBR.y - srcTL.y, srcTL.x - tlx, srcTL.y - tly, img->data, (IntOS)img->GetDataBpl(), destInfo.dispSize.x, destInfo.dispSize.y);
	return img;
}
