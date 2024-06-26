#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/Deinterlace8.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/MutexUsage.h"
#include "UI/GUIVideoBoxDDLQ.h"

void UI::GUIVideoBoxDDLQ::ProcessVideo(NN<ThreadStat> tstat, VideoBuff *vbuff, VideoBuff *vbuff2)
{
	NN<Media::CS::CSConverter> csconv;
	NN<Media::MonitorSurface> destSurface;
	UnsafeArray<UInt8> vsrcBuff;
	DrawRect rect;
	UOSInt srcWidth = 0;
	UOSInt srcHeight = 0;
	UOSInt cropWidth;
	UOSInt cropHeight;
	UOSInt cropDY;
	UInt8 *srcBuff = tstat->lrBuff;
	UOSInt sizeNeeded;
	if (!vbuff->srcBuff.SetTo(vsrcBuff))
		return;

#if 0
	tstat->me->buffMut->Lock();
	vbuff->isOutputReady = true;
	vbuff->isProcessing = false;
	tstat->me->buffMut->Unlock();
	tstat->me->dispEvt->Set();
	return;
#endif
	if ((tstat->resizerBitDepth != ((UI::GUIVideoBoxDDLQ*)tstat->me)->bitDepth || tstat->resizer10Bit != ((UI::GUIVideoBoxDDLQ*)tstat->me)->curr10Bit) && ((UI::GUIVideoBoxDDLQ*)tstat->me)->bitDepth != 0)
	{
		((UI::GUIVideoBoxDDLQ*)tstat->me)->CreateThreadResizer(tstat);
	}
	if (!tstat->csconv.SetTo(csconv))
		return;

	Manage::HiResClock clk;
	csconv->ConvertV2(&vsrcBuff, tstat->lrBuff, this->videoInfo.dispSize.x, this->videoInfo.dispSize.y, this->videoInfo.storeSize.x, this->videoInfo.storeSize.y, (OSInt)this->videoInfo.dispSize.x * 4, vbuff->frameType, vbuff->ycOfst);
	tstat->csTime = clk.GetTimeDiff();

	if (vbuff->frameType == Media::FT_NON_INTERLACE)
	{
		srcWidth = this->videoInfo.dispSize.x;
		srcHeight = this->videoInfo.dispSize.y;
		cropWidth = srcWidth - this->cropLeft - this->cropRight;
		cropHeight = srcHeight - this->cropTop - this->cropBottom;
		cropDY = this->cropTop;
		srcBuff = tstat->lrBuff;
	}
	else if (vbuff->frameType == Media::FT_FIELD_TF)
	{
		srcWidth = this->videoInfo.dispSize.x;
		srcHeight = this->videoInfo.dispSize.y << 1;
		cropWidth = srcWidth - (this->cropLeft + this->cropRight);
		cropHeight = srcHeight - (this->cropTop + this->cropBottom);
		cropDY = this->cropTop << 1;
		sizeNeeded = srcWidth * srcHeight << 2;
		if (tstat->diSize < sizeNeeded)
		{
			if (tstat->diBuff)
			{
				MemFreeA(tstat->diBuff);
			}
			tstat->diSize = sizeNeeded;
			tstat->diBuff = MemAllocA(UInt8, sizeNeeded);
		}
		if (tstat->deint == 0)
		{
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, srcWidth << 2));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, srcWidth << 2);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_FIELD_BF)
	{
		srcWidth = this->videoInfo.dispSize.x;
		srcHeight = this->videoInfo.dispSize.y << 1;
		cropWidth = srcWidth - (this->cropLeft + this->cropRight);
		cropHeight = srcHeight - (this->cropTop + this->cropBottom);
		cropDY = this->cropTop << 1;
		sizeNeeded = srcWidth * srcHeight << 2;
		if (tstat->diSize < sizeNeeded)
		{
			if (tstat->diBuff)
			{
				MemFreeA(tstat->diBuff);
			}
			tstat->diSize = sizeNeeded;
			tstat->diBuff = MemAllocA(UInt8, sizeNeeded);
		}
		if (tstat->deint == 0)
		{
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, srcWidth << 2));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, srcWidth << 2);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 1, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_MERGED_TF)
	{
		srcWidth = this->videoInfo.dispSize.x;
		srcHeight = this->videoInfo.dispSize.y;
		cropWidth = srcWidth - (this->cropLeft + this->cropRight);
		cropHeight = srcHeight - (this->cropTop + this->cropBottom);
		cropDY = this->cropTop;
		sizeNeeded = srcWidth * srcHeight << 2;
		if (tstat->diSize < sizeNeeded)
		{
			if (tstat->diBuff)
			{
				MemFreeA(tstat->diBuff);
			}
			tstat->diSize = sizeNeeded;
			tstat->diBuff = MemAllocA(UInt8, sizeNeeded);
		}
		if (tstat->deint == 0)
		{
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, srcWidth << 2));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, srcWidth << 2);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_MERGED_BF)
	{
		srcWidth = this->videoInfo.dispSize.x;
		srcHeight = this->videoInfo.dispSize.y;
		cropWidth = srcWidth - (this->cropLeft + this->cropRight);
		cropHeight = srcHeight - (this->cropTop + this->cropBottom);
		cropDY = this->cropTop;
		sizeNeeded = srcWidth * srcHeight << 2;
		if (tstat->diSize < sizeNeeded)
		{
			if (tstat->diBuff)
			{
				MemFreeA(tstat->diBuff);
			}
			tstat->diSize = sizeNeeded;
			tstat->diBuff = MemAllocA(UInt8, sizeNeeded);
		}
		if (tstat->deint == 0)
		{
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, srcWidth << 2));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, srcWidth << 2);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 1, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_INTERLACED_TFF)
	{
		srcWidth = this->videoInfo.dispSize.x;
		srcHeight = this->videoInfo.dispSize.y;
		cropWidth = srcWidth - (this->cropLeft + this->cropRight);
		cropHeight = srcHeight - (this->cropTop + this->cropBottom);
		cropDY = this->cropTop;
		sizeNeeded = srcWidth * srcHeight << 2;
		if (tstat->diSize < sizeNeeded)
		{
			if (tstat->diBuff)
			{
				MemFreeA(tstat->diBuff);
			}
			tstat->diSize = sizeNeeded;
			tstat->diBuff = MemAllocA(UInt8, sizeNeeded);
		}
		if (tstat->deint == 0)
		{
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, srcWidth << 3));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, srcWidth << 3);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_INTERLACED_BFF)
	{
		srcWidth = this->videoInfo.dispSize.x;
		srcHeight = this->videoInfo.dispSize.y;
		cropWidth = srcWidth - (this->cropLeft + this->cropRight);
		cropHeight = srcHeight - (this->cropTop + this->cropBottom);
		cropDY = this->cropTop;
		sizeNeeded = srcWidth * srcHeight << 2;
		if (tstat->diSize < sizeNeeded)
		{
			if (tstat->diBuff)
			{
				MemFreeA(tstat->diBuff);
			}
			tstat->diSize = sizeNeeded;
			tstat->diBuff = MemAllocA(UInt8, sizeNeeded);
		}
		if (tstat->deint == 0)
		{
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, srcWidth << 3));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, srcWidth << 3);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff + (srcWidth * 4), tstat->diBuff, 1, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_INTERLACED_NODEINT)
	{
		srcWidth = this->videoInfo.dispSize.x;
		srcHeight = this->videoInfo.dispSize.y;
		cropWidth = srcWidth - this->cropLeft - this->cropRight;
		cropHeight = srcHeight - this->cropTop - this->cropBottom;
		cropDY = this->cropTop;
		srcBuff = tstat->lrBuff;
	}
	else
	{
		srcWidth = this->videoInfo.dispSize.x;
		srcHeight = this->videoInfo.dispSize.y;
		cropWidth = srcWidth - this->cropLeft - this->cropRight;
		cropHeight = srcHeight - this->cropTop - this->cropBottom;
		cropDY = this->cropTop;
		srcBuff = tstat->lrBuff;
	}
	
//	this->VideoBeginProc();
	this->CalDisplayRect(cropWidth, cropHeight, &rect);
//	this->VideoEndProc();
	vbuff->destSize = rect.size;
	vbuff->destBitDepth = 32;
	if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
	{
		vbuff->destSurface.Delete();
		vbuff->destSurface = tstat->me->GetSurfaceMgr()->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
	}
	if (vbuff->destSurface.SetTo(destSurface))
	{
		OSInt destBpl;
		UInt8* destBuff = destSurface->LockSurface(destBpl);
		tstat->resizer->Resize(srcBuff + (cropDY * srcWidth << 2) + (this->cropLeft << 2), (OSInt)srcWidth << 2, UOSInt2Double(cropWidth), UOSInt2Double(cropHeight), 0, 0,destBuff, destBpl, vbuff->destSize.x, vbuff->destSize.y);
		destSurface->UnlockSurface();
	}
	tstat->hTime = ((Media::Resizer::LanczosResizerH8_8*)tstat->resizer)->GetHAvgTime();
	tstat->vTime = ((Media::Resizer::LanczosResizerH8_8*)tstat->resizer)->GetVAvgTime();

	if ((vbuff->frameType == Media::FT_INTERLACED_TFF || vbuff->frameType == Media::FT_INTERLACED_BFF) && vbuff2)
	{
		if (vbuff->frameType == Media::FT_INTERLACED_TFF)
		{
			tstat->deint->Deinterlace(tstat->lrBuff + (srcWidth * 4), tstat->diBuff, 1, srcWidth, (OSInt)srcWidth << 2);
		}
		else
		{
			tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, (OSInt)srcWidth << 2);
		}
		vbuff2->destSize = vbuff->destSize;
		vbuff2->destBitDepth = 32;
		if (!vbuff2->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff2->destSize))
		{
			vbuff2->destSurface.Delete();
			vbuff2->destSurface = tstat->me->GetSurfaceMgr()->CreateSurface(vbuff2->destSize, vbuff2->destBitDepth);
		}
		if (vbuff2->destSurface.SetTo(destSurface))
		{
			OSInt destBpl;
			UInt8* destBuff = destSurface->LockSurface(destBpl);
			tstat->resizer->Resize(tstat->diBuff + (cropDY * srcWidth << 2) + (this->cropLeft << 2), (OSInt)srcWidth << 2, UOSInt2Double(cropWidth), UOSInt2Double(cropHeight), 0, 0, destBuff, destBpl, vbuff2->destSize.x, vbuff2->destSize.y);
			destSurface->UnlockSurface();
		}
		vbuff2->frameNum = vbuff->frameNum;
		vbuff2->frameTime = vbuff->frameTime + 17;
		Sync::MutexUsage mutUsage(this->buffMut);
		vbuff->isOutputReady = true;
		vbuff->isProcessing = false;
		vbuff2->isOutputReady = true;
		vbuff2->isProcessing = false;
		mutUsage.EndUse();
		this->dispEvt.Set();
	}
	else
	{
		Sync::MutexUsage mutUsage(this->buffMut);
		vbuff->isOutputReady = true;
		vbuff->isProcessing = false;
		mutUsage.EndUse();
		this->dispEvt.Set();
	}
}

NN<Media::IImgResizer> UI::GUIVideoBoxDDLQ::CreateResizer(Media::ColorManagerSess *colorMgr, UInt32 bitDepth, Double srcRefLuminance)
{
	NN<Media::IImgResizer> resizer;
	NEW_CLASSNN(resizer, Media::Resizer::LanczosResizerH8_8(4, 3, Media::AT_NO_ALPHA));
	return resizer;
}

void UI::GUIVideoBoxDDLQ::CreateCSConv(NN<ThreadStat> tstat, Media::FrameInfo *info)
{
	tstat->csconv.Delete();
	Media::ColorProfile::YUVType yuvType = info->yuvType;
	if (yuvType == Media::ColorProfile::YUVT_UNKNOWN)
	{
		if (info->dispSize.y > 576)
		{
			yuvType = Media::ColorProfile::YUVT_BT709;
		}
		else
		{
			yuvType = Media::ColorProfile::YUVT_BT601;
		}
	}
	UInt32 fcc = info->fourcc;
	if (fcc == FFMT_YUV444P10LE)
	{
		fcc = FFMT_YUV444P10LEP;
	}
	else if (fcc == FFMT_YUV420P10LE)
	{
		fcc = *(UInt32*)"P016";
	}
	else if (fcc == FFMT_YUV420P12LE)
	{
		fcc = *(UInt32*)"P016";
	}
	else if (fcc == FFMT_YUV420P8)
	{
		fcc = *(UInt32*)"YV12";
	}
	Media::ColorProfile color(Media::ColorProfile::CPT_VDISPLAY);
	tstat->csconv = Media::CS::CSConverter::NewConverter(fcc, info->storeBPP, info->pf, info->color, 0, 32, Media::PF_B8G8R8A8, color, yuvType, this->colorSess);
	if (info->dispSize.CalcArea() > tstat->lrSize)
	{
		tstat->lrSize = info->dispSize.CalcArea();
		if (tstat->lrBuff)
		{
			MemFreeA(tstat->lrBuff);
		}
		tstat->lrBuff = MemAllocA(UInt8, tstat->lrSize * 4);
	}
}

void UI::GUIVideoBoxDDLQ::CreateThreadResizer(NN<ThreadStat> tstat)
{
	SDEL_CLASS(tstat->resizer);
	SDEL_CLASS(tstat->dresizer);
//	Media::ColorProfile destColor(Media::ColorProfile::CPT_VDISPLAY);

/*	if (bitDepth == 16)
	{
		NEW_CLASS(tstat->resizer, Media::Resizer::LanczosResizerLR_C16(4, 3, &destColor, colorSess, Media::AT_NO_ALPHA));
		tstat->procType = 0;
	}
	else if (this->curr10Bit)
	{
		NEW_CLASS(tstat->dresizer, Media::Resizer::DeintResizerLR_C32_10(4, 3, &destColor, colorSess, Media::AT_NO_ALPHA));
		tstat->procType = 1;
	}
	else
	{
		NEW_CLASS(tstat->dresizer, Media::Resizer::DeintResizerLR_C32_8(4, 3, &destColor, colorSess, Media::AT_NO_ALPHA));
		tstat->procType = 1;
	}*/
	NEW_CLASS(tstat->resizer, Media::Resizer::LanczosResizerH8_8(4, 3, Media::AT_NO_ALPHA));
	tstat->procType = 0;

	tstat->resizerBitDepth = this->bitDepth;
	tstat->resizer10Bit = this->curr10Bit;
}

UI::GUIVideoBoxDDLQ::GUIVideoBoxDDLQ(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::ColorManagerSess> colorSess, UOSInt buffCnt, UOSInt threadCnt) : UI::GUIVideoBoxDD(ui, parent, colorSess, buffCnt, threadCnt)
{
}

UI::GUIVideoBoxDDLQ::~GUIVideoBoxDDLQ()
{
	this->StopThreads();
}
