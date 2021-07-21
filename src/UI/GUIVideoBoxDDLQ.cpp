#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/Deinterlace8.h"
#include "Media/Resizer/LanczosResizerH8_8.h"
#include "Sync/MutexUsage.h"
#include "UI/GUIVideoBoxDDLQ.h"

void UI::GUIVideoBoxDDLQ::ProcessVideo(ThreadStat *tstat, VideoBuff *vbuff, VideoBuff *vbuff2)
{
	OSInt rect[4];
	UOSInt srcWidth = 0;
	UOSInt srcHeight = 0;
	UOSInt cropWidth;
	UOSInt cropHeight;
	UOSInt cropDY;
	UInt8 *srcBuff = tstat->lrBuff;
	UOSInt sizeNeeded;

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

	Manage::HiResClock clk;
	tstat->csconv->ConvertV2(&vbuff->srcBuff, tstat->lrBuff, this->videoInfo->dispWidth, this->videoInfo->dispHeight, this->videoInfo->storeWidth, this->videoInfo->storeHeight, (OSInt)this->videoInfo->dispWidth * 4, vbuff->frameType, vbuff->ycOfst);
	tstat->csTime = clk.GetTimeDiff();

	if (vbuff->frameType == Media::FT_NON_INTERLACE)
	{
		srcWidth = this->videoInfo->dispWidth;
		srcHeight = this->videoInfo->dispHeight;
		cropWidth = srcWidth - this->cropLeft - this->cropRight;
		cropHeight = srcHeight - this->cropTop - this->cropBottom;
		cropDY = this->cropTop;
		srcBuff = tstat->lrBuff;
	}
	else if (vbuff->frameType == Media::FT_FIELD_TF)
	{
		srcWidth = this->videoInfo->dispWidth;
		srcHeight = this->videoInfo->dispHeight << 1;
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
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, (OSInt)srcWidth << 2));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, (OSInt)srcWidth << 2);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_FIELD_BF)
	{
		srcWidth = this->videoInfo->dispWidth;
		srcHeight = this->videoInfo->dispHeight << 1;
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
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, (OSInt)srcWidth << 2));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, (OSInt)srcWidth << 2);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 1, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_MERGED_TF)
	{
		srcWidth = this->videoInfo->dispWidth;
		srcHeight = this->videoInfo->dispHeight;
		cropWidth = (OSInt)srcWidth - (this->cropLeft + this->cropRight);
		cropHeight = (OSInt)srcHeight - (this->cropTop + this->cropBottom);
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
		tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_MERGED_BF)
	{
		srcWidth = this->videoInfo->dispWidth;
		srcHeight = this->videoInfo->dispHeight;
		cropWidth = (OSInt)srcWidth - (this->cropLeft + this->cropRight);
		cropHeight = (OSInt)srcHeight - (this->cropTop + this->cropBottom);
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
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, (OSInt)srcWidth << 2));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, (OSInt)srcWidth << 2);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 1, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_INTERLACED_TFF)
	{
		srcWidth = this->videoInfo->dispWidth;
		srcHeight = this->videoInfo->dispHeight;
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
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, (OSInt)srcWidth << 3));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, (OSInt)srcWidth << 3);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_INTERLACED_BFF)
	{
		srcWidth = this->videoInfo->dispWidth;
		srcHeight = this->videoInfo->dispHeight;
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
			NEW_CLASS(tstat->deint, Media::Deinterlace8(srcHeight >> 1, (OSInt)srcWidth << 3));
		}
		else
		{
			tstat->deint->Reinit(srcHeight >> 1, (OSInt)srcWidth << 3);
		}
		srcBuff = tstat->diBuff;
		tstat->deint->Deinterlace(tstat->lrBuff + (srcWidth * 4), tstat->diBuff, 1, srcWidth, (OSInt)srcWidth << 2);
	}
	else if (vbuff->frameType == Media::FT_INTERLACED_NODEINT)
	{
		srcWidth = this->videoInfo->dispWidth;
		srcHeight = this->videoInfo->dispHeight;
		cropWidth = srcWidth - this->cropLeft - this->cropRight;
		cropHeight = srcHeight - this->cropTop - this->cropBottom;
		cropDY = this->cropTop;
		srcBuff = tstat->lrBuff;
	}
	else
	{
		srcWidth = this->videoInfo->dispWidth;
		srcHeight = this->videoInfo->dispHeight;
		cropWidth = srcWidth - this->cropLeft - this->cropRight;
		cropHeight = srcHeight - this->cropTop - this->cropBottom;
		cropDY = this->cropTop;
		srcBuff = tstat->lrBuff;
	}
	
//	this->VideoBeginProc();
	this->CalDisplayRect(cropWidth, cropHeight, rect);
//	this->VideoEndProc();
	vbuff->destW = rect[2];
	vbuff->destH = rect[3];
	if (vbuff->destSize < vbuff->destW * vbuff->destH)
	{
		vbuff->destSize = vbuff->destW * vbuff->destH;
		if (vbuff->destBuff)
		{
			MemFreeA(vbuff->destBuff);
		}
		vbuff->destBuff = MemAllocA(UInt8, vbuff->destSize << 2);
	}
	tstat->resizer->Resize(srcBuff + (cropDY * srcWidth << 2) + (this->cropLeft << 2), srcWidth << 2, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight), 0, 0, vbuff->destBuff, vbuff->destW << 2, vbuff->destW, vbuff->destH);
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
		vbuff2->destW = vbuff->destW;
		vbuff2->destH = vbuff->destH;
		if (vbuff2->destSize < vbuff2->destW * vbuff2->destH)
		{
			vbuff2->destSize = vbuff2->destW * vbuff2->destH;
			if (vbuff2->destBuff)
			{
				MemFreeA(vbuff2->destBuff);
			}
			vbuff2->destBuff = MemAllocA(UInt8, vbuff2->destSize << 2);
		}
		tstat->resizer->Resize(tstat->diBuff + (cropDY * srcWidth << 2) + (this->cropLeft << 2), srcWidth << 2, Math::UOSInt2Double(cropWidth), Math::UOSInt2Double(cropHeight), 0, 0, vbuff2->destBuff, vbuff2->destW << 2, vbuff2->destW, vbuff2->destH);
		vbuff2->frameNum = vbuff->frameNum;
		vbuff2->frameTime = vbuff->frameTime + 17;
		Sync::MutexUsage mutUsage(this->buffMut);
		vbuff->isOutputReady = true;
		vbuff->isProcessing = false;
		vbuff2->isOutputReady = true;
		vbuff2->isProcessing = false;
		mutUsage.EndUse();
		this->dispEvt->Set();
	}
	else
	{
		Sync::MutexUsage mutUsage(this->buffMut);
		vbuff->isOutputReady = true;
		vbuff->isProcessing = false;
		mutUsage.EndUse();
		this->dispEvt->Set();
	}
}

Media::IImgResizer *UI::GUIVideoBoxDDLQ::CreateResizer(Media::ColorManagerSess *colorMgr, Int32 bitDepth)
{
	Media::IImgResizer *resizer;
	NEW_CLASS(resizer, Media::Resizer::LanczosResizerH8_8(4, 3, Media::AT_NO_ALPHA));
	return resizer;
}

void UI::GUIVideoBoxDDLQ::CreateCSConv(ThreadStat *tstat, Media::FrameInfo *info)
{
	SDEL_CLASS(tstat->csconv);
	Media::ColorProfile::YUVType yuvType = info->yuvType;
	if (yuvType == Media::ColorProfile::YUVT_UNKNOWN)
	{
		if (info->dispHeight > 576)
		{
			yuvType = Media::ColorProfile::YUVT_BT709;
		}
		else
		{
			yuvType = Media::ColorProfile::YUVT_BT601;
		}
	}
	Media::ColorProfile color(Media::ColorProfile::CPT_VDISPLAY);
	tstat->csconv = Media::CS::CSConverter::NewConverter(info->fourcc, info->storeBPP, info->pf, info->color, 0, 32, Media::PF_B8G8R8A8, &color, yuvType, this->colorSess);
	if (info->dispWidth * info->dispHeight > tstat->lrSize)
	{
		tstat->lrSize = info->dispWidth * info->dispHeight;
		if (tstat->lrBuff)
		{
			MemFreeA(tstat->lrBuff);
		}
		tstat->lrBuff = MemAllocA(UInt8, tstat->lrSize * 4);
	}
}

void UI::GUIVideoBoxDDLQ::CreateThreadResizer(ThreadStat *tstat)
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

UI::GUIVideoBoxDDLQ::GUIVideoBoxDDLQ(UI::GUICore *ui, UI::GUIClientControl *parent, Media::ColorManagerSess *colorSess, OSInt buffCnt, OSInt threadCnt) : UI::GUIVideoBoxDD(ui, parent, colorSess, buffCnt, threadCnt)
{
}

UI::GUIVideoBoxDDLQ::~GUIVideoBoxDDLQ()
{
	this->StopThreads();
}
