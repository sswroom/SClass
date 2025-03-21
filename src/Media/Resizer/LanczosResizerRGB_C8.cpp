#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/LanczosFilter.h"
#include "Media/RGBLUTGen.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/LanczosResizerFunc.h"
#include "Media/Resizer/LanczosResizerFuncC.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void Media::Resizer::LanczosResizerRGB_C8::MTHorizontalFilterPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep, UOSInt swidth)
{
#if defined(VERBOSE)
	printf("LanczosResizerRGB_C8: HFilterPA w = %d, h = %d, tap = %d, srcPF = %s\r\n", (UInt32)dwidth, (UInt32)height, (UInt32)tap, Media::PixelFormatGetName(this->srcPF).v.Ptr());
#endif
	if (this->IsSrcUInt16())
	{
		if (this->rgb16Changed || this->rgb16Table.IsNull())
		{
			this->UpdateRGB16Table();
		}
	}
	UOSInt currHeight;
	UOSInt lastHeight = height;
	FuncType funcType = FuncType::HFilterPA;
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
		this->params[i].srcPF = this->srcPF;
		this->params[i].destPF = this->destPF;

		if (this->params[i].swidth != 0 && this->params[i].height != 0)
		{
			this->params[i].funcType = funcType;
			this->ptask->AddTask(DoTask, &this->params[i]);
		}
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizerRGB_C8::MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep, UOSInt swidth)
{
#if defined(VERBOSE)
	printf("LanczosResizerRGB_C8: HFilter w = %d, h = %d, tap = %d, srcPF = %s\r\n", (UInt32)dwidth, (UInt32)height, (UInt32)tap, Media::PixelFormatGetName(this->srcPF).v.Ptr());
#endif
	if (this->IsSrcUInt16())
	{
		if (this->rgb16Changed || this->rgb16Table.IsNull())
		{
			this->UpdateRGB16Table();
		}
	}
	UOSInt currHeight;
	UOSInt lastHeight = height;
	FuncType funcType = FuncType::HFilter;
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
		this->params[i].srcPF = this->srcPF;
		this->params[i].destPF = this->destPF;

		if (this->params[i].swidth != 0 && this->params[i].height != 0)
		{
			this->params[i].funcType = funcType;
			this->ptask->AddTask(DoTask, &this->params[i]);
		}
		lastHeight = currHeight;
	}
	this->ptask->WaitForIdle();
}

void Media::Resizer::LanczosResizerRGB_C8::MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, UOSInt tap, UnsafeArray<OSInt> index, UnsafeArray<Int64> weight, OSInt sstep, OSInt dstep)
{
#if defined(VERBOSE)
	printf("LanczosResizerRGB_C8: VFilter w = %d, h = %d, tap = %d, destPF = %s\r\n", (UInt32)dwidth, (UInt32)height, (UInt32)tap, Media::PixelFormatGetName(this->destPF).v.Ptr());
#endif
	UOSInt currHeight;
	UOSInt lastHeight = height;
	FuncType funcType = FuncType::VFilter;
	UOSInt i = this->nThread;
	if (height < 16)
	{
		if (this->destPF == Media::PF_B8G8R8)
		{
			LanczosResizerFunc_VerticalFilterB8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, tap, index.Ptr(), weight.Ptr(), sstep, dstep, this->rgbTable.Ptr());
		}
		else
		{
			LanczosResizerFunc_VerticalFilterB8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, tap, index.Ptr(), weight.Ptr(), sstep, dstep, this->rgbTable.Ptr());
		}
	}
	else
	{
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
			this->params[i].srcPF = this->srcPF;
			this->params[i].destPF = this->destPF;
	
			this->params[i].funcType = funcType;
			this->ptask->AddTask(DoTask, &this->params[i]);
			lastHeight = currHeight;
		}
		this->ptask->WaitForIdle();
	}
}

void Media::Resizer::LanczosResizerRGB_C8::MTExpandPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
#if defined(VERBOSE)
	printf("LanczosResizerRGB_C8: ExpandPA w = %d, h = %d, srcPF = %s\r\n", (UInt32)dwidth, (UInt32)height, Media::PixelFormatGetName(this->srcPF).v.Ptr());
#endif
	if (this->srcPF == PF_PAL_8 || this->srcPF == PF_PAL_W8)
		LanczosResizerFunc_ExpandPal8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr() + 270336);
	else if (this->srcPF == PF_B8G8R8)
		LanczosResizerFunc_ExpandB8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr() + 262144);
	else if (this->srcPF == PF_LE_R16G16B16)
	{
		UnsafeArray<UInt8> rgbTable;
		if (this->rgb16Changed || !this->rgb16Table.SetTo(rgbTable))
		{
			rgbTable = this->UpdateRGB16Table();
		}
		LanczosResizerFunc_ExpandR16G16B16(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, rgbTable.Ptr());
	}
	else
		LanczosResizerFunc_ExpandB8G8R8A8PA(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr() + 262144);
}

void Media::Resizer::LanczosResizerRGB_C8::MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
#if defined(VERBOSE)
	printf("LanczosResizerRGB_C8: Expand w = %d, h = %d, srcPF = %s\r\n", (UInt32)dwidth, (UInt32)height, Media::PixelFormatGetName(this->srcPF).v.Ptr());
#endif
	if (this->srcPF == PF_PAL_8 || this->srcPF == PF_PAL_W8)
		LanczosResizerFunc_ExpandPal8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr() + 270336);
	else if (this->srcPF == PF_B8G8R8)
		LanczosResizerFunc_ExpandB8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr() + 262144);
	else if (this->srcPF == PF_LE_R16G16B16)
	{
		UnsafeArray<UInt8> rgbTable;
		if (this->rgb16Changed || !this->rgb16Table.SetTo(rgbTable))
		{
			rgbTable = this->UpdateRGB16Table();
		}
		LanczosResizerFunc_ExpandR16G16B16(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, rgbTable.Ptr());
	}
	else
		LanczosResizerFunc_ExpandB8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr() + 262144);
}

void Media::Resizer::LanczosResizerRGB_C8::MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
#if defined(VERBOSE)
	printf("LanczosResizerRGB_C8: Collapse w = %d, h = %d, srcPF = %s\r\n", (UInt32)dwidth, (UInt32)height, Media::PixelFormatGetName(this->srcPF).v.Ptr());
#endif
	if (this->destPF == Media::PF_B8G8R8)
	{
		LanczosResizerFunc_CollapseB8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr());
	}
	else
	{
		LanczosResizerFunc_CollapseB8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr());
	}
}

void Media::Resizer::LanczosResizerRGB_C8::MTCopyPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
#if defined(VERBOSE)
	printf("LanczosResizerRGB_C8: CopyPA w = %d, h = %d, srcPF = %s, destPF = %s\r\n", (UInt32)dwidth, (UInt32)height, Media::PixelFormatGetName(this->srcPF).v.Ptr(), Media::PixelFormatGetName(this->destPF).v.Ptr());
#endif
	if (this->destPF == Media::PF_B8G8R8)
	{
		if (this->srcPF == PF_PAL_8 || this->srcPF == PF_PAL_W8)
			LanczosResizerFunc_ImgCopyPal8_B8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 270336);
		else if (this->srcPF == PF_B8G8R8)
			LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 262144);
		else if (this->srcPF == PF_LE_R16G16B16)
		{
			UnsafeArray<UInt8> rgbTable;
			if (this->rgb16Changed || !this->rgb16Table.SetTo(rgbTable))
			{
				rgbTable = this->UpdateRGB16Table();
			}
			LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), rgbTable.Ptr());
		}
		else
			LanczosResizerFunc_ImgCopyB8G8R8A8PA_B8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 262144);
	}
	else
	{
		if (this->srcPF == PF_PAL_8 || this->srcPF == PF_PAL_W8)
			LanczosResizerFunc_ImgCopyPal8_B8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 270336);
		else if (this->srcPF == PF_B8G8R8)
			LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 262144);
		else if (this->srcPF == PF_LE_R16G16B16)
		{
			UnsafeArray<UInt8> rgbTable;
			if (this->rgb16Changed || !this->rgb16Table.SetTo(rgbTable))
			{
				rgbTable = this->UpdateRGB16Table();
			}
			LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), rgbTable.Ptr());
		}
		else
			LanczosResizerFunc_ImgCopyB8G8R8A8PA_B8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 262144);
#if defined(VERBOSE)
	printf("LanczosResizerRGB_C8: Alpha = %x\r\n", ReadNInt32(&outPt[0]));
#endif
	}
}

void Media::Resizer::LanczosResizerRGB_C8::MTCopy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UOSInt dwidth, UOSInt height, OSInt sstep, OSInt dstep)
{
#if defined(VERBOSE)
	printf("LanczosResizerRGB_C8: Copy w = %d, h = %d, srcPF = %s, destPF = %s\r\n", (UInt32)dwidth, (UInt32)height, Media::PixelFormatGetName(this->srcPF).v.Ptr(), Media::PixelFormatGetName(this->destPF).v.Ptr());
#endif
	if (this->destPF == Media::PF_B8G8R8)
	{
		if (this->srcPF == PF_PAL_8 || this->srcPF == PF_PAL_W8)
			LanczosResizerFunc_ImgCopyPal8_B8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 270336);
		else if (this->srcPF == PF_B8G8R8)
			LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 262144);
		else if (this->srcPF == PF_LE_R16G16B16)
		{
			UnsafeArray<UInt8> rgbTable;
			if (this->rgb16Changed || !this->rgb16Table.SetTo(rgbTable))
			{
				rgbTable = this->UpdateRGB16Table();
			}
			LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), rgbTable.Ptr());
		}
		else
			LanczosResizerFunc_ImgCopyB8G8R8A8_B8G8R8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 262144);
	}
	else
	{
		if (this->srcPF == PF_PAL_8 || this->srcPF == PF_PAL_W8)
			LanczosResizerFunc_ImgCopyPal8_B8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 270336);
		else if (this->srcPF == PF_B8G8R8)
			LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 262144);
		else if (this->srcPF == PF_LE_R16G16B16)
		{
			UnsafeArray<UInt8> rgbTable;
			if (this->rgb16Changed || !this->rgb16Table.SetTo(rgbTable))
			{
				rgbTable = this->UpdateRGB16Table();
			}
			LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), rgbTable.Ptr());
		}
		else
			LanczosResizerFunc_ImgCopyB8G8R8A8_B8G8R8A8(inPt.Ptr(), outPt.Ptr(), dwidth, height, sstep, dstep, this->rgbTable.Ptr(), this->rgbTable.Ptr() + 262144);
#if defined(VERBOSE)
		printf("LanczosResizerRGB_C8: Alpha = %x\r\n", ReadNInt32(&outPt[0]));
#endif
		}
}

void Media::Resizer::LanczosResizerRGB_C8::UpdateRGBTable()
{
	UnsafeArray<UInt8> rgbTable;
	if (!this->rgbTable.SetTo(rgbTable))
	{
		this->rgbTable = rgbTable = MemAllocArr(UInt8, 65536 * 4 + 256 * 4 * 8 + 256 * 8);
	}
	Media::RGBLUTGen lutGen(this->colorSess);
	lutGen.GenLRGB_BGRA8(rgbTable, this->destProfile, 14, Media::CS::TransferFunc::GetRefLuminance(this->srcProfile.rtransfer));
	lutGen.GenRGBA8_LRGBC((Int64*)&rgbTable[262144], this->srcProfile, this->destProfile.GetPrimaries(), 14);
	this->UpdatePalTable();
}

UnsafeArray<UInt8> Media::Resizer::LanczosResizerRGB_C8::UpdateRGB16Table()
{
	UnsafeArray<UInt8> rgbTable;
	if (!this->rgb16Table.SetTo(rgbTable))
	{
		this->rgb16Table = rgbTable = MemAllocArr(UInt8, 2097152);
	}
	this->rgb16Changed = false;
	Media::RGBLUTGen lutGen(this->colorSess);
	lutGen.GenRGB16_LRGBC((Int64*)&rgbTable[0], this->srcProfile, this->destProfile.GetPrimaries(), 14);
	return rgbTable;
}

void Media::Resizer::LanczosResizerRGB_C8::UpdatePalTable()
{
	UnsafeArray<const UInt8> srcPal;
	UnsafeArray<UInt8> rgbTable;
	if (this->rgbTable.SetTo(rgbTable) && this->srcPal.SetTo(srcPal))
	{
		UOSInt nColor;
		if (this->srcPF == PF_PAL_8 || this->srcPF == PF_PAL_W8)
		{
			nColor = 256;
		}
		else
		{
			nColor = 0;
		}
		UOSInt i = 0;
		UInt8 c;
		Int32 a;
		Int32 r;
		Int32 g;
		Int32 b;
		while (i < nColor)
		{
			c = srcPal[i * 4 + 2];
			b = *(Int16*)&rgbTable[262144 + c * 8];
			g = *(Int16*)&rgbTable[262144 + c * 8 + 2];
			r = *(Int16*)&rgbTable[262144 + c * 8 + 4];
			a = *(Int16*)&rgbTable[262144 + c * 8 + 6];
			c = srcPal[i * 4 + 1];
			b += *(Int16*)&rgbTable[262144 + 2048 + c * 8];
			g += *(Int16*)&rgbTable[262144 + 2048 + c * 8 + 2];
			r += *(Int16*)&rgbTable[262144 + 2048 + c * 8 + 4];
			a += *(Int16*)&rgbTable[262144 + 2048 + c * 8 + 6];
			c = srcPal[i * 4 + 0];
			b += *(Int16*)&rgbTable[262144 + 4096 + c * 8];
			g += *(Int16*)&rgbTable[262144 + 4096 + c * 8 + 2];
			r += *(Int16*)&rgbTable[262144 + 4096 + c * 8 + 4];
			a += *(Int16*)&rgbTable[262144 + 4096 + c * 8 + 6];
			c = srcPal[i * 4 + 3];
			b += *(Int16*)&rgbTable[262144 + 6144 + c * 8];
			g += *(Int16*)&rgbTable[262144 + 6144 + c * 8 + 2];
			r += *(Int16*)&rgbTable[262144 + 6144 + c * 8 + 4];
			a += *(Int16*)&rgbTable[262144 + 6144 + c * 8 + 6];
			if (b >= 32768)
				b = 32767;
			else if (b < -32768)
				b = -32768;
			if (g >= 32768)
				g = 32767;
			else if (g < -32768)
				g = -32768;
			if (r >= 32768)
				r = 32767;
			else if (r < -32768)
				r = -32768;
			if (a >= 32768)
				a = 32767;
			else if (a < -32768)
				a = -32768;
			*(Int16*)&rgbTable[262144 + 8192 + i * 8 + 0] = (Int16)b;
			*(Int16*)&rgbTable[262144 + 8192 + i * 8 + 2] = (Int16)g;
			*(Int16*)&rgbTable[262144 + 8192 + i * 8 + 4] = (Int16)r;
			*(Int16*)&rgbTable[262144 + 8192 + i * 8 + 6] = (Int16)a;
			i++;
		}
	}
}

void __stdcall Media::Resizer::LanczosResizerRGB_C8::DoTask(AnyType obj)
{
	NN<TaskParam> ts = obj.GetNN<TaskParam>();
	UnsafeArray<UInt8> tmpBuff;
	switch (ts->funcType)
	{
	case FuncType::HFilter:
		if (ts->swidth != ts->tmpbuffSize)
		{
			if (ts->tmpbuff.SetTo(tmpBuff))
				MemFreeAArr(tmpBuff);
			ts->tmpbuffSize = ts->swidth;
			ts->tmpbuff = MemAllocAArr(UInt8, ts->swidth << 3);
		}
		if (ts->srcPF == Media::PF_PAL_8 || ts->srcPF == Media::PF_PAL_W8)
		{
			LanczosResizerFunc_HorizontalFilterPal8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 270336, ts->swidth, ts->tmpbuff.Ptr());
		}
		else if (ts->srcPF == Media::PF_B8G8R8)
		{
			LanczosResizerFunc_HorizontalFilterB8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 262144, ts->swidth, ts->tmpbuff.Ptr());
		}
		else if (ts->srcPF == Media::PF_LE_R16G16B16)
		{
			LanczosResizerFunc_HorizontalFilterR16G16B16(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgb16Table.Ptr(), ts->swidth, ts->tmpbuff.Ptr());
		}
		else
		{
			LanczosResizerFunc_HorizontalFilterB8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 262144, ts->swidth, ts->tmpbuff.Ptr());
		}
		break;
	case FuncType::HFilterPA:
		if (ts->swidth != ts->tmpbuffSize)
		{
			if (ts->tmpbuff.SetTo(tmpBuff))
				MemFreeAArr(tmpBuff);
			ts->tmpbuffSize = ts->swidth;
			ts->tmpbuff = MemAllocAArr(UInt8, ts->swidth << 3);
		}
		if (ts->srcPF == Media::PF_PAL_8 || ts->srcPF == Media::PF_PAL_W8)
		{
			LanczosResizerFunc_HorizontalFilterPal8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 270336, ts->swidth, ts->tmpbuff.Ptr());
		}
		else if (ts->srcPF == Media::PF_B8G8R8)
		{
			LanczosResizerFunc_HorizontalFilterB8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 262144, ts->swidth, ts->tmpbuff.Ptr());
		}
		else if (ts->srcPF == Media::PF_LE_R16G16B16)
		{
			LanczosResizerFunc_HorizontalFilterR16G16B16(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgb16Table.Ptr(), ts->swidth, ts->tmpbuff.Ptr());
		}
		else
		{
			LanczosResizerFunc_HorizontalFilterB8G8R8A8PA(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 262144, ts->swidth, ts->tmpbuff.Ptr());
		}
		break;
	case FuncType::VFilter:
		if (ts->destPF == Media::PF_B8G8R8)
		{
			LanczosResizerFunc_VerticalFilterB8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		}
		else
		{
			LanczosResizerFunc_VerticalFilterB8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->tap, ts->index.Ptr(), ts->weight.Ptr(), ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		}
		break;
	case FuncType::Expand:
		if (ts->srcPF == Media::PF_PAL_8 || ts->srcPF == Media::PF_PAL_W8)
		{
			LanczosResizerFunc_ExpandPal8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 270336);
		}
		else if (ts->srcPF == Media::PF_B8G8R8)
		{
			LanczosResizerFunc_ExpandB8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 262144);
		}
		else if (ts->srcPF == Media::PF_LE_R16G16B16)
		{
			LanczosResizerFunc_ExpandR16G16B16(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgb16Table.Ptr());
		}
		else
		{
			LanczosResizerFunc_ExpandB8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 262144);
		}
		break;
	case FuncType::ExpandPA:
		if (ts->srcPF == Media::PF_PAL_8 || ts->srcPF == Media::PF_PAL_W8)
		{
			LanczosResizerFunc_ExpandPal8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 270336);
		}
		else if (ts->srcPF == Media::PF_B8G8R8)
		{
			LanczosResizerFunc_ExpandB8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 262144);
		}
		else if (ts->srcPF == Media::PF_LE_R16G16B16)
		{
			LanczosResizerFunc_ExpandR16G16B16(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgb16Table.Ptr());
		}
		else
		{
			LanczosResizerFunc_ExpandB8G8R8A8PA(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr() + 262144);
		}
		break;
	case FuncType::Collapse:
		if (ts->destPF == Media::PF_B8G8R8)
		{
			LanczosResizerFunc_CollapseB8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		}
		else
		{
			LanczosResizerFunc_CollapseB8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr());
		}
		break;
	case FuncType::ImgCopy:
		if (ts->destPF == Media::PF_B8G8R8)
		{
			if (ts->srcPF == Media::PF_PAL_8 || ts->srcPF == Media::PF_PAL_W8)
			{
				LanczosResizerFunc_ImgCopyPal8_B8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 270336);
			}
			else if (ts->srcPF == Media::PF_B8G8R8)
			{
				LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 262144);
			}
			else if (ts->srcPF == Media::PF_LE_R16G16B16)
			{
				LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgb16Table.Ptr());
			}
			else
			{
				LanczosResizerFunc_ImgCopyB8G8R8A8_B8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 262144);
			}
		}
		else
		{
			if (ts->srcPF == Media::PF_PAL_8 || ts->srcPF == Media::PF_PAL_W8)
			{
				LanczosResizerFunc_ImgCopyPal8_B8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 270336);
			}
			else if (ts->srcPF == Media::PF_B8G8R8)
			{
				LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 262144);
			}
			else if (ts->srcPF == Media::PF_LE_R16G16B16)
			{
				LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgb16Table.Ptr());
			}
			else
			{
				LanczosResizerFunc_ImgCopyB8G8R8A8_B8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 262144);
			}
		}
		break;
	case FuncType::ImgCopyPA:
		if (ts->destPF == Media::PF_B8G8R8)
		{
			if (ts->srcPF == Media::PF_PAL_8 || ts->srcPF == Media::PF_PAL_W8)
			{
				LanczosResizerFunc_ImgCopyPal8_B8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 270336);
			}
			else if (ts->srcPF == Media::PF_B8G8R8)
			{
				LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 262144);
			}
			else if (ts->srcPF == Media::PF_LE_R16G16B16)
			{
				LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgb16Table.Ptr());
			}
			else
			{
				LanczosResizerFunc_ImgCopyB8G8R8A8PA_B8G8R8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 262144);
			}
		}
		else
		{
			if (ts->srcPF == Media::PF_PAL_8 || ts->srcPF == Media::PF_PAL_W8)
			{
				LanczosResizerFunc_ImgCopyPal8_B8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 270336);
			}
			else if (ts->srcPF == Media::PF_B8G8R8)
			{
				LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 262144);
			}
			else if (ts->srcPF == Media::PF_LE_R16G16B16)
			{
				LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgb16Table.Ptr());
			}
			else
			{
				LanczosResizerFunc_ImgCopyB8G8R8A8PA_B8G8R8A8(ts->inPt.Ptr(), ts->outPt.Ptr(), ts->dwidth, ts->height, ts->sstep, ts->dstep, ts->me->rgbTable.Ptr(), ts->me->rgbTable.Ptr() + 262144);
			}
		}
		break;
	default:
	case FuncType::NoFunction:
		break;
	}
}

void Media::Resizer::LanczosResizerRGB_C8::DestoryHori()
{
	UnsafeArray<OSInt> hIndex;
	UnsafeArray<Int64> hWeight;
	if (this->hIndex.SetTo(hIndex))
	{
		MemFreeAArr(hIndex);
		this->hIndex = 0;
	}
	if (this->hWeight.SetTo(hWeight))
	{
		MemFreeAArr(hWeight);
		this->hWeight = 0;
	}
	this->hsSize = 0;
}

void Media::Resizer::LanczosResizerRGB_C8::DestoryVert()
{
	UnsafeArray<OSInt> vIndex;
	UnsafeArray<Int64> vWeight;
	if (this->vIndex.SetTo(vIndex))
	{
		MemFreeAArr(vIndex);
		this->vIndex = 0;
	}
	if (this->vWeight.SetTo(vWeight))
	{
		MemFreeAArr(vWeight);
		this->vWeight = 0;
	}
	this->vsSize = 0;
	this->vsStep = 0;
}

Media::Resizer::LanczosResizerRGB_C8::LanczosResizerRGB_C8(UOSInt hnTap, UOSInt vnTap, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType) : Media::ImageResizer(srcAlphaType), srcProfile(srcProfile), destProfile(destProfile)
{
	UOSInt i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 6)
	{
		this->nThread = 6;
	}

	this->hnTap = hnTap << 1;
	this->vnTap = vnTap << 1;
	this->rgbChanged = true;
	this->rgbTable = 0;
	this->rgb16Changed = true;
	this->rgb16Table = 0;
	this->srcPF = Media::PF_B8G8R8A8;
	this->destPF = Media::PF_B8G8R8A8;
	this->srcPal = 0;
	NN<Media::ColorManagerSess> nncolorSess;
	if (colorSess.SetTo(nncolorSess))
	{
		this->colorSess = nncolorSess;
		nncolorSess->AddHandler(*this);
	}
	else
	{
		this->colorSess = 0;
	}

	this->params = MemAllocArr(Media::Resizer::LanczosResizerRGB_C8::TaskParam, this->nThread);
	MemClear(this->params.Ptr(), sizeof(Media::Resizer::LanczosResizerRGB_C8::TaskParam) * this->nThread);
	i = nThread;
	while(i-- > 0)
	{
		this->params[i].me = *this;
	}
	NEW_CLASSNN(this->ptask, Sync::ParallelTask(this->nThread, false));

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

Media::Resizer::LanczosResizerRGB_C8::~LanczosResizerRGB_C8()
{
	UOSInt i;
	NN<Media::ColorManagerSess> nncolorSess;
	UnsafeArray<UInt8> rgbTable;
	UnsafeArray<UInt8> tmpBuff;
	UnsafeArray<UInt8> buffPtr;
	if (this->colorSess.SetTo(nncolorSess))
	{
		nncolorSess->RemoveHandler(*this);
	}
	this->ptask.Delete();
	i = this->nThread;
	while (i-- > 0)
	{
		if (this->params[i].tmpbuff.SetTo(tmpBuff))
		{
			MemFreeAArr(tmpBuff);
			this->params[i].tmpbuff = 0;
			this->params[i].tmpbuffSize = 0;
		}
	}
	MemFreeArr(this->params);

	DestoryHori();
	DestoryVert();
	if (this->buffPtr.SetTo(buffPtr))
	{
		MemFreeAArr(buffPtr);
		this->buffPtr = 0;
	}
	if (this->rgbTable.SetTo(rgbTable))
	{
		MemFreeArr(rgbTable);
	}
	if (this->rgb16Table.SetTo(rgbTable))
	{
		MemFreeArr(rgbTable);
	}
}

void Media::Resizer::LanczosResizerRGB_C8::Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
	Media::Resizer::LanczosResizerFunc::Parameter prm;
	Double w;
	Double h;
	OSInt siWidth;
	OSInt siHeight;
	UnsafeArray<UInt8> buffPtr;
	UnsafeArray<OSInt> hIndex;
	UnsafeArray<Int64> hWeight;
	UnsafeArray<OSInt> vIndex;
	UnsafeArray<Int64> vWeight;
	if (dwidth < this->hnTap || dheight < this->vnTap)
		return;

#if defined(VERBOSE)
	//printf("LanczosResizerRGB_C8: Resize %lf x %lf -> %d x %d\r\n", swidth, sheight, (UInt32)dwidth, (UInt32)dheight);
#endif
	w = xOfst + swidth;
	h = yOfst + sheight;
	siWidth = (OSInt)w;
	siHeight = (OSInt)h;
	w -= OSInt2Double(siWidth);
	h -= OSInt2Double(siHeight);
	if (w > 0)
		siWidth++;
	if (h > 0)
		siHeight++;

	if (siWidth < (OSInt)this->hnTap || siHeight < (OSInt)this->vnTap)
		return;

	if (this->rgbChanged)
	{
		this->rgbChanged = false;
		this->rgb16Changed = true;
		UpdateRGBTable();
	}

	if (siWidth != (OSInt)dwidth && siHeight != (OSInt)dheight)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (!this->hIndex.SetTo(hIndex) || !this->hWeight.SetTo(hWeight) || this->hsSize != swidth || this->hdSize != dwidth || this->hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > UOSInt2Double(dwidth))
			{
				Media::Resizer::LanczosResizerFunc::SetupDecimationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 8, xOfst);
			}
			else
			{
				Media::Resizer::LanczosResizerFunc::SetupInterpolationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 8, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			this->hIndex = hIndex = prm.index;
			this->hWeight = hWeight = prm.weight;
			hTap = prm.tap;
		}

		if (!this->vIndex.SetTo(vIndex) || !this->vWeight.SetTo(vWeight) || this->vsSize != sheight || this->vdSize != dheight || this->vsStep != (OSInt)hdSize * 8 || this->vsOfst != yOfst)
		{
			DestoryVert();
			vsStep = (OSInt)hdSize * 8;

			if (sheight > UOSInt2Double(dheight))
			{
				Media::Resizer::LanczosResizerFunc::SetupDecimationParameterV(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			else
			{
				Media::Resizer::LanczosResizerFunc::SetupInterpolationParameterV(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			this->vIndex = vIndex = prm.index;
			this->vWeight = vWeight = prm.weight;
			vTap = prm.tap;
		}
		
		if (!this->buffPtr.SetTo(buffPtr) || siHeight != buffH || (dwidth != buffW))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = 0;
			}
			buffW = dwidth;
			buffH = siHeight;
			this->buffPtr = buffPtr = MemAllocA(UInt8, buffW * (UOSInt)buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			this->MTHorizontalFilterPA(src, buffPtr, dwidth, (UOSInt)siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, (UOSInt)siWidth);
/*			UInt8 *tmpbuff = MemAllocA(UInt8, siWidth << 3);
			LanczosResizer8_C8_horizontal_filter_pa(src, buffPtr, dwidth, siHeight, hTap, hIndex, hWeight, sbpl, dwidth << 3, this->rgbTable, siWidth, tmpbuff);
			MemFreeA(tmpbuff);*/
		}
		else
		{
			this->MTHorizontalFilter(src, buffPtr, dwidth, (UOSInt)siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, (UOSInt)siWidth);
/*			UInt8 *tmpbuff = MemAllocA(UInt8, (UOSInt)siWidth << 3);
			LanczosResizer8_C8_horizontal_filter(src, buffPtr, dwidth, (UOSInt)siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, this->rgbTable, (UOSInt)siWidth, tmpbuff);
			MemFreeA(tmpbuff);*/
		}
		this->MTVerticalFilter(buffPtr, dest, dwidth, dheight, vTap, vIndex, vWeight, (OSInt)dwidth << 3, dbpl);
//		LanczosResizer8_C8_vertical_filter(buffPtr, dest, dwidth, dheight, vTap, vIndex, vWeight, dwidth << 3, dbpl, this->rgbTable);
	}
	else if (siWidth != (OSInt)dwidth)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (!this->hIndex.SetTo(hIndex) || !this->hWeight.SetTo(hWeight) || hsSize != swidth || hdSize != dwidth || hsOfst != xOfst)
		{
			DestoryHori();

			if (swidth > UOSInt2Double(dwidth))
			{
				Media::Resizer::LanczosResizerFunc::SetupDecimationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 8, xOfst);
			}
			else
			{
				Media::Resizer::LanczosResizerFunc::SetupInterpolationParameterH(this->hnTap, swidth, siWidth, dwidth, prm, 8, xOfst);
			}
			hsSize = swidth;
			hdSize = dwidth;
			hsOfst = xOfst;
			this->hIndex = hIndex = prm.index;
			this->hWeight = hWeight = prm.weight;
			hTap = prm.tap;
		}
		if (!this->buffPtr.SetTo(buffPtr) || siHeight != buffH || (dwidth != buffW))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = 0;
			}
			buffW = dwidth;
			buffH = siHeight;
			this->buffPtr = buffPtr = MemAllocAArr(UInt8, buffW * (UOSInt)buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			this->MTHorizontalFilterPA(src, buffPtr, dwidth, (UOSInt)siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, (UOSInt)siWidth);
		}
		else
		{
			this->MTHorizontalFilter(src, buffPtr, dwidth, (UOSInt)siHeight, hTap, hIndex, hWeight, sbpl, (OSInt)dwidth << 3, (UOSInt)siWidth);
		}
		this->MTCollapse(buffPtr, dest, dwidth, dheight, (OSInt)dwidth << 3, dbpl);
	}
	else if (siHeight != (OSInt)dheight)
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (!this->vIndex.SetTo(vIndex) || !this->vWeight.SetTo(vWeight) || vsSize != sheight || vdSize != dheight || vsStep != Double2Int32(swidth) * 8 || vsOfst != yOfst)
		{
			DestoryVert();
			vsStep = siWidth * 8;

			if (sheight > UOSInt2Double(dheight))
			{
				Media::Resizer::LanczosResizerFunc::SetupDecimationParameterV(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			else
			{
				Media::Resizer::LanczosResizerFunc::SetupInterpolationParameterV(this->vnTap, sheight, siHeight, dheight, prm, vsStep, yOfst);
			}
			vsSize = sheight;
			vdSize = dheight;
			vsOfst = yOfst;
			this->vIndex = vIndex = prm.index;
			this->vWeight = vWeight = prm.weight;
			vTap = prm.tap;
		}
		if (!this->buffPtr.SetTo(buffPtr) || siHeight != buffH || (siWidth != (OSInt)buffW))
		{
			if (this->buffPtr.SetTo(buffPtr))
			{
				MemFreeAArr(buffPtr);
				this->buffPtr = 0;
			}
			buffW = (UOSInt)siWidth;
			buffH = siHeight;
			this->buffPtr = buffPtr = MemAllocA(UInt8, buffW * (UOSInt)buffH << 3);
		}
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			this->MTExpandPA(src, buffPtr, (UOSInt)siWidth, (UOSInt)siHeight, sbpl, siWidth << 3);
		}
		else
		{
			this->MTExpand(src, buffPtr, (UOSInt)siWidth, (UOSInt)siHeight, sbpl, siWidth << 3);
		}
		this->MTVerticalFilter(buffPtr, dest, (UOSInt)siWidth, dheight, vTap, vIndex, vWeight, siWidth << 3, dbpl);
	}
	else
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->srcAlphaType == Media::AT_ALPHA)
		{
			this->MTCopyPA(src, dest, (UOSInt)siWidth, dheight, sbpl, dbpl);
		}
		else
		{
			this->MTCopy(src, dest, (UOSInt)siWidth, dheight, sbpl, dbpl);
		}
	}
}

Bool Media::Resizer::LanczosResizerRGB_C8::Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg)
{
	if (srcImg->info.fourcc != 0 && srcImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (destImg->info.fourcc != 0 && destImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (srcImg->info.pf != Media::PF_B8G8R8A8 && srcImg->info.pf != Media::PF_B8G8R8 && srcImg->info.pf != Media::PF_PAL_8 && srcImg->info.pf != Media::PF_PAL_W8)
		return false;
	if (destImg->info.pf != Media::PF_B8G8R8A8 && destImg->info.pf != Media::PF_B8G8R8)
		return false;
	//destImg->info.color.rgbGamma = srcImg->info.color.rgbGamma;
	this->SetDestProfile(destImg->info.color);
	this->SetSrcProfile(srcImg->info.color);
	this->SetSrcPixelFormat(srcImg->info.pf, srcImg->pal);
	this->SetDestPixelFormat(destImg->info.pf);
	if (srcImg->info.fourcc == destImg->info.fourcc)
	{
		Resize(srcImg->data.Ptr(), (OSInt)srcImg->GetDataBpl(), UOSInt2Double(srcImg->info.dispSize.x), UOSInt2Double(srcImg->info.dispSize.y), 0, 0, destImg->data.Ptr(), (OSInt)destImg->GetDataBpl(), destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
	else
	{
		OSInt dbpl = (OSInt)destImg->GetDataBpl();
		Resize(srcImg->data.Ptr(), (OSInt)srcImg->GetDataBpl(), UOSInt2Double(srcImg->info.dispSize.x), UOSInt2Double(srcImg->info.dispSize.y), 0, 0, destImg->data.Ptr() + (OSInt)(destImg->info.storeSize.y - 1) * dbpl, -dbpl, destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
}

void Media::Resizer::LanczosResizerRGB_C8::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void Media::Resizer::LanczosResizerRGB_C8::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	this->rgbChanged = true;
}

void Media::Resizer::LanczosResizerRGB_C8::SetSrcProfile(NN<const Media::ColorProfile> srcProfile)
{
	if (!this->srcProfile.Equals(srcProfile))
	{
		this->srcProfile.Set(srcProfile);
		this->rgbChanged = true;
	}
}

void Media::Resizer::LanczosResizerRGB_C8::SetDestProfile(NN<const Media::ColorProfile> destProfile)
{
	if (!this->destProfile.Equals(destProfile))
	{
		this->destProfile.Set(destProfile);
		this->rgbChanged = true;
	}
}

Media::AlphaType Media::Resizer::LanczosResizerRGB_C8::GetDestAlphaType()
{
	if (this->srcAlphaType == Media::AT_IGNORE_ALPHA)
	{
		return Media::AT_IGNORE_ALPHA;
	}
	else if (this->srcAlphaType == Media::AT_ALPHA_ALL_FF)
	{
		return Media::AT_ALPHA_ALL_FF;
	}
	else if (this->srcPF == Media::PF_PAL_8)
	{
		return Media::AT_ALPHA;
	}
	else
	{
		return Media::AT_PREMUL_ALPHA;
	}
}

void Media::Resizer::LanczosResizerRGB_C8::SetSrcPixelFormat(Media::PixelFormat srcPF, UnsafeArrayOpt<const UInt8> srcPal)
{
	this->srcPF = srcPF;
	this->srcPal = srcPal;
	if (!this->srcPal.IsNull() && !this->rgbChanged)
	{
		this->UpdatePalTable();
	}
}

void Media::Resizer::LanczosResizerRGB_C8::SetDestPixelFormat(Media::PixelFormat destPF)
{
	this->destPF = destPF;
}

Bool Media::Resizer::LanczosResizerRGB_C8::IsSrcUInt16() const
{
	return this->srcPF == Media::PF_LE_R16G16B16 ||
		this->srcPF == Media::PF_LE_R16G16B16A16 ||
		this->srcPF == Media::PF_LE_W16 ||
		this->srcPF == Media::PF_LE_W16A16 ||
		this->srcPF == Media::PF_LE_B16G16R16A16 ||
		this->srcPF == Media::PF_LE_B16G16R16;
}

Bool Media::Resizer::LanczosResizerRGB_C8::IsSupported(NN<const Media::FrameInfo> srcInfo)
{
	if (srcInfo->fourcc != 0)
		return false;
	if (srcInfo->pf != Media::PF_B8G8R8A8 &&
		srcInfo->pf != Media::PF_B8G8R8 &&
		srcInfo->pf != Media::PF_PAL_8 &&
		srcInfo->pf != Media::PF_PAL_W8 &&
		srcInfo->pf != Media::PF_LE_R16G16B16)
		return false;
	return true;
}

Optional<Media::StaticImage> Media::Resizer::LanczosResizerRGB_C8::ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *newImage;
	if (srcImage->GetImageType() != Media::RasterImage::ImageType::Static || !IsSupported(srcImage->info))
	{
		return 0;
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
	this->SetSrcPixelFormat(srcImage->info.pf, srcImage->pal);
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
	if (this->destPF == Media::PF_B8G8R8)
	{
		destInfo.pf = Media::PF_B8G8R8;
		destInfo.storeBPP = 24;
	}
	else
	{
		destInfo.pf = Media::PF_B8G8R8A8;
		destInfo.storeBPP = 32;
	}
	NEW_CLASS(newImage, Media::StaticImage(destInfo));
	Int32 tlx = (Int32)srcTL.x;
	Int32 tly = (Int32)srcTL.y;
	OSInt bpp = srcImage->info.storeBPP >> 3;
	Resize(NN<Media::StaticImage>::ConvertFrom(srcImage)->data.Ptr() + (tlx * bpp) + tly * (OSInt)srcImage->GetDataBpl(), (OSInt)srcImage->GetDataBpl(), srcBR.x - srcTL.x, srcBR.y - srcTL.y, srcTL.x - tlx, srcTL.y - tly, newImage->data.Ptr(), (OSInt)newImage->GetDataBpl(), newImage->info.dispSize.x, newImage->info.dispSize.y);
	return newImage;
}
