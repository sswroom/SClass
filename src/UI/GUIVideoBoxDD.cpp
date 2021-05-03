#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/PNGExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamWriter.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/DeinterlaceLR.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/DeintResizerLR_C32.h"
#include "Media/Resizer/LanczosResizerLR_C16.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIVideoBoxDD.h"
#include "UI/MessageDialog.h"

//DeinterlaceResizer
//#define _DEBUG
#define PREPROCTIME 0
#define PROCDELAYBUFF 32
#define DISPDELAYBUFF 32

void UI::GUIVideoBoxDD::CalDisplayRect(OSInt srcWidth, OSInt srcHeight, OSInt *rect)
{
	Double par;
	if (this->forcePAR == 0)
	{
		par = this->videoInfo->par2 / this->monPAR;
	}
	else
	{
		par = this->forcePAR / this->monPAR;
	}

	if (this->surfaceW * srcHeight * par > this->surfaceH * srcWidth)
	{
		rect[3] = this->surfaceH;
		rect[2] = Math::Double2Int32(this->surfaceH / par * srcWidth / srcHeight);
	}
	else
	{
		rect[2] = this->surfaceW;
		rect[3] = Math::Double2Int32(this->surfaceW * srcHeight * par / srcWidth);
	}
	rect[0] = (this->surfaceW - rect[2]) >> 1;
	rect[1] = (this->surfaceH - rect[3]) >> 1;
}

void UI::GUIVideoBoxDD::UpdateFromBuff(VideoBuff *vbuff)
{
	OSInt rect[4];

	this->VideoBeginProc();
	Sync::MutexUsage mutUsage(this->surfaceMut);
	OSInt vwidth = this->videoInfo->dispWidth - cropLeft - cropRight;
	OSInt vheight = this->videoInfo->dispHeight - cropTop - cropBottom;
	if (this->videoInfo->ftype == Media::FT_FIELD_BF || this->videoInfo->ftype == Media::FT_FIELD_TF)
	{
		vheight = vheight << 1;
	}
	this->CalDisplayRect(vwidth, vheight, rect);
	if (rect[2] == vbuff->destW && rect[3] == vbuff->destH && this->surfaceBuff && vbuff->destBitDepth == this->bitDepth)
	{
		OSInt dlineSize;
		UInt8 *dptr = this->LockSurfaceDirect(&dlineSize);
		if (dptr)
		{
			dptr = dptr + (rect[1] * dlineSize + rect[0] * (this->bitDepth >> 3));
			if (this->bitDepth == 16)
			{
				this->outputCopier->Copy16(vbuff->destBuff, rect[2] * 2, dptr, dlineSize, rect[2], rect[3]);
			}
			else
			{
				this->outputCopier->Copy16(vbuff->destBuff, rect[2] * 4, dptr, dlineSize, rect[2], rect[3]);
			}
			this->LockSurfaceUnlock();
		}
	}
	mutUsage.EndUse();
	this->VideoEndProc();
	
}

void UI::GUIVideoBoxDD::ProcessVideo(ThreadStat *tstat, VideoBuff *vbuff, VideoBuff *vbuff2)
{
	OSInt rect[4];
	OSInt srcWidth = 0;
	OSInt srcHeight = 0;
	OSInt cropWidth;
	OSInt cropHeight;
	OSInt cropDY;
	UInt8 *srcBuff = tstat->lrBuff;
	UOSInt sizeNeeded;
	OSInt cropTotal = tstat->me->cropLeft + tstat->me->cropRight + tstat->me->cropTop + tstat->me->cropBottom;
	Double par;
	if (tstat->me->forcePAR == 0)
	{
		par = tstat->me->videoInfo->par2 / tstat->me->monPAR;
	}
	else
	{
		par = tstat->me->forcePAR / tstat->me->monPAR;
	}
	
	if (tstat->me->captureFrame)
	{
		tstat->me->captureFrame = false;
		Media::FrameInfo *info = tstat->me->videoInfo;
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
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		Int32 fcc = info->fourcc;
		if (fcc == FFMT_YUV444P10LE)
		{
			fcc = FFMT_YUV444P10LEP;
		}
		else if (fcc == FFMT_YUV420P10LE)
		{
			fcc = *(Int32*)"P016";
		}
		else if (fcc == FFMT_YUV420P12LE)
		{
			fcc = *(Int32*)"P016";
		}
		else if (fcc == FFMT_YUV420P8)
		{
			fcc = *(Int32*)"YV12";
		}
		Media::CS::CSConverter *csconv = Media::CS::CSConverter::NewConverter(fcc, info->storeBPP, info->pf, info->color, 0, 32, Media::PF_B8G8R8A8, &color, yuvType, this->colorSess);
		if (csconv)
		{
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			OSInt i;
			Media::ImageList *imgList;
			Media::StaticImage *simg;
			NEW_CLASS(simg, Media::StaticImage(info->dispWidth, info->dispHeight, 0, 32, Media::PF_B8G8R8A8, 0, &color, yuvType, Media::AT_NO_ALPHA, vbuff->ycOfst));
			csconv->ConvertV2(&vbuff->srcBuff, simg->data, info->dispWidth, info->dispHeight, info->storeWidth, info->storeHeight, simg->GetDataBpl(), vbuff->frameType, vbuff->ycOfst);
			ImageUtil_ImageFillAlpha32(simg->data, info->dispWidth, info->dispHeight, simg->GetDataBpl(), 0xff);
			this->video->GetSourceName(sbuff);
			i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
			sptr = &sbuff[i + 1];
			Data::DateTime dt;
			dt.SetCurrTime();
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Snapshot");
			sptr = dt.ToString(sptr, "yyyyMMdd_HHmmssfff");
			sptr = Text::StrConcat(sptr, (const UTF8Char*)".png");
			NEW_CLASS(imgList, Media::ImageList(sbuff));
			imgList->AddImage(simg, 0);
			Exporter::PNGExporter exporter;
			IO::FileStream *fs;
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			exporter.ExportFile(fs, sbuff, imgList, 0);
			DEL_CLASS(imgList);
			DEL_CLASS(csconv);
		}
	}

	if ((vbuff->frameType == Media::FT_NON_INTERLACE || vbuff->frameType == Media::FT_INTERLACED_NODEINT) && par == 1.0 && cropTotal == 0 && tstat->me->videoInfo->dispWidth == tstat->me->surfaceW && tstat->me->videoInfo->dispHeight <= tstat->me->surfaceH && tstat->me->bitDepth == 32)
	{
		if (tstat->procType != 2)
		{
			tstat->procType = 2;
			tstat->cs10Bit = tstat->me->curr10Bit;
			tstat->me->CreateCSConv(tstat, tstat->me->videoInfo);
		}
		else if (tstat->cs10Bit != tstat->me->curr10Bit)
		{
			tstat->cs10Bit = tstat->me->curr10Bit;
			tstat->me->CreateCSConv(tstat, tstat->me->videoInfo);
		}
		vbuff->destW = tstat->me->videoInfo->dispWidth;
		vbuff->destH = tstat->me->videoInfo->dispHeight;
		vbuff->destBitDepth = 32;
		if (vbuff->destSize < vbuff->destW * vbuff->destH)
		{
			vbuff->destSize = vbuff->destW * vbuff->destH;
			if (vbuff->destBuff)
			{
				MemFreeA64(vbuff->destBuff);
			}
			vbuff->destBuff = MemAllocA64(UInt8, vbuff->destSize << 2);
		}
		Manage::HiResClock clk;
		tstat->csconv->ConvertV2(&vbuff->srcBuff, vbuff->destBuff, tstat->me->videoInfo->dispWidth, tstat->me->videoInfo->dispHeight, tstat->me->videoInfo->storeWidth, tstat->me->videoInfo->storeHeight, tstat->me->videoInfo->dispWidth * 4, vbuff->frameType, vbuff->ycOfst);
		tstat->csTime = clk.GetTimeDiff();

		Sync::MutexUsage mutUsage(tstat->me->buffMut);
		vbuff->isOutputReady = true;
		vbuff->isProcessing = false;
		mutUsage.EndUse();
		tstat->me->dispEvt->Set();
	}
	else
	{
		if (tstat->procType == 2)
		{
			tstat->me->CreateThreadResizer(tstat);
			tstat->me->CreateCSConv(tstat, tstat->me->videoInfo);
		}
		else if ((tstat->resizerBitDepth != tstat->me->bitDepth || tstat->resizer10Bit != tstat->me->curr10Bit) && tstat->me->bitDepth != 0)
		{
			tstat->me->CreateThreadResizer(tstat);
		}
		else if (tstat->resizerSrcRefLuminance != tstat->me->currSrcRefLuminance)
		{
			tstat->resizerSrcRefLuminance = tstat->me->currSrcRefLuminance;
			if (tstat->resizer)
			{
				tstat->resizer->SetSrcRefLuminance(tstat->resizerSrcRefLuminance);
			}
			if (tstat->dresizer)
			{
				tstat->dresizer->SetDISrcRefLuminance(tstat->resizerSrcRefLuminance);
			}
		}

		Manage::HiResClock clk;
		tstat->csconv->ConvertV2(&vbuff->srcBuff, tstat->lrBuff, tstat->me->videoInfo->dispWidth, tstat->me->videoInfo->dispHeight, tstat->me->videoInfo->storeWidth, tstat->me->videoInfo->storeHeight, tstat->me->videoInfo->dispWidth * 8, vbuff->frameType, vbuff->ycOfst);
		tstat->csTime = clk.GetTimeDiff();

		if (tstat->procType == 1)
		{
			if (vbuff->frameType == Media::FT_NON_INTERLACE)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - tstat->me->cropLeft - tstat->me->cropRight;
				cropHeight = srcHeight - tstat->me->cropTop - tstat->me->cropBottom;
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, rect);
				vbuff->destW = rect[2];
				vbuff->destH = rect[3];
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (vbuff->destSize < vbuff->destW * vbuff->destH)
				{
					vbuff->destSize = vbuff->destW * vbuff->destH;
					if (vbuff->destBuff)
					{
						MemFreeA64(vbuff->destBuff);
					}
					vbuff->destBuff = MemAllocA64(UInt8, vbuff->destSize << 2);
				}
				tstat->dresizer->DeintResize(Media::IDeintResizer::DT_FULL_FRAME, srcBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight), vbuff->destBuff, vbuff->destW * vbuff->destBitDepth >> 3, vbuff->destW, vbuff->destH);
			}
			else if (vbuff->frameType == Media::FT_FIELD_TF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight << 1;
				cropWidth = srcWidth - ((tstat->me->cropLeft + tstat->me->cropRight) << 1);
				cropHeight = srcHeight - ((tstat->me->cropTop + tstat->me->cropBottom) << 1);
				cropDY = tstat->me->cropTop << 1;
				sizeNeeded = srcWidth * srcHeight << 3;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, rect);
				vbuff->destW = rect[2];
				vbuff->destH = rect[3];
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (vbuff->destSize < vbuff->destW * vbuff->destH)
				{
					vbuff->destSize = vbuff->destW * vbuff->destH;
					if (vbuff->destBuff)
					{
						MemFreeA64(vbuff->destBuff);
					}
					vbuff->destBuff = MemAllocA64(UInt8, vbuff->destSize << 2);
				}
				tstat->dresizer->DeintResize(Media::IDeintResizer::DT_TOP_FIELD, srcBuff + ((cropDY >> 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight >> 1), vbuff->destBuff, vbuff->destW * vbuff->destBitDepth >> 3, vbuff->destW, vbuff->destH);
			}
			else if (vbuff->frameType == Media::FT_FIELD_BF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight << 1;
				cropWidth = srcWidth - ((tstat->me->cropLeft + tstat->me->cropRight) << 1);
				cropHeight = srcHeight - ((tstat->me->cropTop + tstat->me->cropBottom) << 1);
				cropDY = tstat->me->cropTop << 1;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, rect);
				vbuff->destW = rect[2];
				vbuff->destH = rect[3];
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (vbuff->destSize < vbuff->destW * vbuff->destH)
				{
					vbuff->destSize = vbuff->destW * vbuff->destH;
					if (vbuff->destBuff)
					{
						MemFreeA64(vbuff->destBuff);
					}
					vbuff->destBuff = MemAllocA64(UInt8, vbuff->destSize << 2);
				}
				tstat->dresizer->DeintResize(Media::IDeintResizer::DT_BOTTOM_FIELD, srcBuff + ((cropDY >> 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight >> 1), vbuff->destBuff, vbuff->destW * vbuff->destBitDepth >> 3, vbuff->destW, vbuff->destH);
			}
			else if (vbuff->frameType == Media::FT_MERGED_TF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, rect);
				vbuff->destW = rect[2];
				vbuff->destH = rect[3];
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (vbuff->destSize < vbuff->destW * vbuff->destH)
				{
					vbuff->destSize = vbuff->destW * vbuff->destH;
					if (vbuff->destBuff)
					{
						MemFreeA64(vbuff->destBuff);
					}
					vbuff->destBuff = MemAllocA64(UInt8, vbuff->destSize << 2);
				}
				tstat->dresizer->DeintResize(Media::IDeintResizer::DT_TOP_FIELD, srcBuff + ((cropDY >> 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight >> 1), vbuff->destBuff, vbuff->destW * vbuff->destBitDepth >> 3, vbuff->destW, vbuff->destH);
			}
			else if (vbuff->frameType == Media::FT_MERGED_BF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, rect);
				vbuff->destW = rect[2];
				vbuff->destH = rect[3];
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (vbuff->destSize < vbuff->destW * vbuff->destH)
				{
					vbuff->destSize = vbuff->destW * vbuff->destH;
					if (vbuff->destBuff)
					{
						MemFreeA64(vbuff->destBuff);
					}
					vbuff->destBuff = MemAllocA64(UInt8, vbuff->destSize << 2);
				}
				tstat->dresizer->DeintResize(Media::IDeintResizer::DT_BOTTOM_FIELD, srcBuff + ((cropDY >> 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight >> 1), vbuff->destBuff, vbuff->destW * vbuff->destBitDepth >> 3, vbuff->destW, vbuff->destH);
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_TFF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, rect);
				vbuff->destW = rect[2];
				vbuff->destH = rect[3];
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (vbuff->destSize < vbuff->destW * vbuff->destH)
				{
					vbuff->destSize = vbuff->destW * vbuff->destH;
					if (vbuff->destBuff)
					{
						MemFreeA64(vbuff->destBuff);
					}
					vbuff->destBuff = MemAllocA64(UInt8, vbuff->destSize << 2);
				}
				tstat->dresizer->DeintResize(Media::IDeintResizer::DT_TOP_FIELD, srcBuff + ((cropDY >> 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 4, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight >> 1), vbuff->destBuff, vbuff->destW * vbuff->destBitDepth >> 3, vbuff->destW, vbuff->destH);
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_BFF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, rect);
				vbuff->destW = rect[2];
				vbuff->destH = rect[3];
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (vbuff->destSize < vbuff->destW * vbuff->destH)
				{
					vbuff->destSize = vbuff->destW * vbuff->destH;
					if (vbuff->destBuff)
					{
						MemFreeA64(vbuff->destBuff);
					}
					vbuff->destBuff = MemAllocA64(UInt8, vbuff->destSize << 2);
				}
				tstat->dresizer->DeintResize(Media::IDeintResizer::DT_BOTTOM_FIELD, srcBuff + (((cropDY >> 1) + 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 4, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight >> 1), vbuff->destBuff, vbuff->destW * vbuff->destBitDepth >> 3, vbuff->destW, vbuff->destH);
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_NODEINT)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - tstat->me->cropLeft - tstat->me->cropRight;
				cropHeight = srcHeight - tstat->me->cropTop - tstat->me->cropBottom;
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, rect);
				vbuff->destW = rect[2];
				vbuff->destH = rect[3];
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (vbuff->destSize < vbuff->destW * vbuff->destH)
				{
					vbuff->destSize = vbuff->destW * vbuff->destH;
					if (vbuff->destBuff)
					{
						MemFreeA64(vbuff->destBuff);
					}
					vbuff->destBuff = MemAllocA64(UInt8, vbuff->destSize << 2);
				}
				tstat->dresizer->DeintResize(Media::IDeintResizer::DT_FULL_FRAME, srcBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight), vbuff->destBuff, vbuff->destW * vbuff->destBitDepth >> 3, vbuff->destW, vbuff->destH);
			}
			else
			{
				cropWidth = srcWidth;
				cropHeight = srcHeight;
				cropDY = 0;
			}

			if ((vbuff->frameType == Media::FT_INTERLACED_TFF || vbuff->frameType == Media::FT_INTERLACED_BFF) && vbuff2)
			{
				vbuff2->frameType = vbuff->frameType;
				vbuff2->destW = vbuff->destW;
				vbuff2->destH = vbuff->destH;
				vbuff2->destBitDepth = tstat->resizerBitDepth;
				if (vbuff2->destSize < vbuff2->destW * vbuff2->destH)
				{
					vbuff2->destSize = vbuff2->destW * vbuff2->destH;
					if (vbuff2->destBuff)
					{
						MemFreeA64(vbuff2->destBuff);
					}
					vbuff2->destBuff = MemAllocA64(UInt8, vbuff2->destSize << 2);
				}
				vbuff2->frameNum = vbuff->frameNum;
				vbuff2->frameTime = vbuff->frameTime + 16;

				Sync::MutexUsage mutUsage(tstat->me->buffMut);
				vbuff->isOutputReady = true;
				vbuff->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt->Set();

				if (vbuff2->frameType == Media::FT_INTERLACED_TFF)
				{
					tstat->dresizer->DeintResize(Media::IDeintResizer::DT_BOTTOM_FIELD, tstat->lrBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight), vbuff2->destBuff, vbuff2->destW * vbuff2->destBitDepth >> 3, vbuff2->destW, vbuff2->destH);
				}
				else
				{
					tstat->dresizer->DeintResize(Media::IDeintResizer::DT_TOP_FIELD, tstat->lrBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight), vbuff2->destBuff, vbuff2->destW * vbuff2->destBitDepth >> 3, vbuff2->destW, vbuff2->destH);
				}
				mutUsage.BeginUse();
				vbuff2->isOutputReady = true;
				vbuff2->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt->Set();
			}
			else
			{
				Sync::MutexUsage mutUsage(tstat->me->buffMut);
				vbuff->isOutputReady = true;
				vbuff->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt->Set();
			}

			tstat->hTime = ((Media::Resizer::DeintResizerLR_C32*)tstat->dresizer)->GetHAvgTime();
			tstat->vTime = ((Media::Resizer::DeintResizerLR_C32*)tstat->dresizer)->GetVAvgTime();
		}
		else
		{
			if (vbuff->frameType == Media::FT_NON_INTERLACE)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - tstat->me->cropLeft - tstat->me->cropRight;
				cropHeight = srcHeight - tstat->me->cropTop - tstat->me->cropBottom;
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;
			}
			else if (vbuff->frameType == Media::FT_FIELD_TF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight << 1;
				cropWidth = srcWidth - ((tstat->me->cropLeft + tstat->me->cropRight) << 1);
				cropHeight = srcHeight - ((tstat->me->cropTop + tstat->me->cropBottom) << 1);
				cropDY = tstat->me->cropTop << 1;
				sizeNeeded = srcWidth * srcHeight << 3;
				if (tstat->diSize < sizeNeeded)
				{
					if (tstat->diBuff)
					{
						MemFreeA64(tstat->diBuff);
					}
					tstat->diSize = sizeNeeded;
					tstat->diBuff = MemAllocA64(UInt8, sizeNeeded);
				}
				if (tstat->deint == 0)
				{
					NEW_CLASS(tstat->deint, Media::DeinterlaceLR(srcHeight >> 1, srcWidth << 3));
				}
				else
				{
					tstat->deint->Reinit(srcHeight >> 1, srcWidth << 3);
				}
				srcBuff = tstat->diBuff;
				tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_FIELD_BF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight << 1;
				cropWidth = srcWidth - ((tstat->me->cropLeft + tstat->me->cropRight) << 1);
				cropHeight = srcHeight - ((tstat->me->cropTop + tstat->me->cropBottom) << 1);
				cropDY = tstat->me->cropTop << 1;
				sizeNeeded = srcWidth * srcHeight << 3;
				if (tstat->diSize < sizeNeeded)
				{
					if (tstat->diBuff)
					{
						MemFreeA64(tstat->diBuff);
					}
					tstat->diSize = sizeNeeded;
					tstat->diBuff = MemAllocA64(UInt8, sizeNeeded);
				}
				if (tstat->deint == 0)
				{
					NEW_CLASS(tstat->deint, Media::DeinterlaceLR(srcHeight >> 1, srcWidth << 3));
				}
				else
				{
					tstat->deint->Reinit(srcHeight >> 1, srcWidth << 3);
				}
				srcBuff = tstat->diBuff;
				tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 1, srcWidth, srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_MERGED_TF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				sizeNeeded = srcWidth * srcHeight << 3;
				if (tstat->diSize < sizeNeeded)
				{
					if (tstat->diBuff)
					{
						MemFreeA64(tstat->diBuff);
					}
					tstat->diSize = sizeNeeded;
					tstat->diBuff = MemAllocA64(UInt8, sizeNeeded);
				}
				if (tstat->deint == 0)
				{
					NEW_CLASS(tstat->deint, Media::DeinterlaceLR(srcHeight >> 1, srcWidth << 3));
				}
				else
				{
					tstat->deint->Reinit(srcHeight >> 1, srcWidth << 3);
				}
				srcBuff = tstat->diBuff;
				tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_MERGED_BF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				sizeNeeded = srcWidth * srcHeight << 3;
				if (tstat->diSize < sizeNeeded)
				{
					if (tstat->diBuff)
					{
						MemFreeA64(tstat->diBuff);
					}
					tstat->diSize = sizeNeeded;
					tstat->diBuff = MemAllocA64(UInt8, sizeNeeded);
				}
				if (tstat->deint == 0)
				{
					NEW_CLASS(tstat->deint, Media::DeinterlaceLR(srcHeight >> 1, srcWidth << 3));
				}
				else
				{
					tstat->deint->Reinit(srcHeight >> 1, srcWidth << 3);
				}
				srcBuff = tstat->diBuff;
				tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 1, srcWidth, srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_TFF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				sizeNeeded = srcWidth * srcHeight << 3;
				if (tstat->diSize < sizeNeeded)
				{
					if (tstat->diBuff)
					{
						MemFreeA64(tstat->diBuff);
					}
					tstat->diSize = sizeNeeded;
					tstat->diBuff = MemAllocA64(UInt8, sizeNeeded);
				}
				if (tstat->deint == 0)
				{
					NEW_CLASS(tstat->deint, Media::DeinterlaceLR(srcHeight >> 1, srcWidth << 4));
				}
				else
				{
					tstat->deint->Reinit(srcHeight >> 1, srcWidth << 4);
				}
				srcBuff = tstat->diBuff;
				tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_BFF)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				sizeNeeded = srcWidth * srcHeight << 3;
				if (tstat->diSize < sizeNeeded)
				{
					if (tstat->diBuff)
					{
						MemFreeA64(tstat->diBuff);
					}
					tstat->diSize = sizeNeeded;
					tstat->diBuff = MemAllocA64(UInt8, sizeNeeded);
				}
				if (tstat->deint == 0)
				{
					NEW_CLASS(tstat->deint, Media::DeinterlaceLR(srcHeight >> 1, srcWidth << 4));
				}
				else
				{
					tstat->deint->Reinit(srcHeight >> 1, srcWidth << 4);
				}
				srcBuff = tstat->diBuff;
				tstat->deint->Deinterlace(tstat->lrBuff + (srcWidth * 8), tstat->diBuff, 1, srcWidth, srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_NODEINT)
			{
				srcWidth = tstat->me->videoInfo->dispWidth;
				srcHeight = tstat->me->videoInfo->dispHeight;
				cropWidth = srcWidth - tstat->me->cropLeft - tstat->me->cropRight;
				cropHeight = srcHeight - tstat->me->cropTop - tstat->me->cropBottom;
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;
			}
			else
			{
				cropWidth = 0;
				cropHeight = 0;
				cropDY = 0;
			}
			
		//	tstat->me->VideoBeginProc();
			tstat->me->CalDisplayRect(cropWidth, cropHeight, rect);
		//	tstat->me->VideoEndProc();
			vbuff->destW = rect[2];
			vbuff->destH = rect[3];
			vbuff->destBitDepth = tstat->resizerBitDepth;
			if (vbuff->destSize < vbuff->destW * vbuff->destH)
			{
				vbuff->destSize = vbuff->destW * vbuff->destH;
				if (vbuff->destBuff)
				{
					MemFreeA64(vbuff->destBuff);
				}
				vbuff->destBuff = MemAllocA64(UInt8, vbuff->destSize << 2);
			}
			tstat->resizer->Resize(srcBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight), 0, 0, vbuff->destBuff, vbuff->destW * vbuff->destBitDepth >> 3, vbuff->destW, vbuff->destH);

			if ((vbuff->frameType == Media::FT_INTERLACED_TFF || vbuff->frameType == Media::FT_INTERLACED_BFF) && vbuff2)
			{
				vbuff2->frameType = vbuff->frameType;
				vbuff2->destW = vbuff->destW;
				vbuff2->destH = vbuff->destH;
				vbuff2->destBitDepth = tstat->resizerBitDepth;
				if (vbuff2->destSize < vbuff2->destW * vbuff2->destH)
				{
					vbuff2->destSize = vbuff2->destW * vbuff2->destH;
					if (vbuff2->destBuff)
					{
						MemFreeA64(vbuff2->destBuff);
					}
					vbuff2->destBuff = MemAllocA64(UInt8, vbuff2->destSize << 2);
				}
				vbuff2->frameNum = vbuff->frameNum;
				vbuff2->frameTime = vbuff->frameTime + 16;

				Sync::MutexUsage mutUsage(tstat->me->buffMut);
				vbuff->isOutputReady = true;
				vbuff->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt->Set();

				if (vbuff2->frameType == Media::FT_INTERLACED_TFF)
				{
					tstat->deint->Deinterlace(tstat->lrBuff + (srcWidth * 8), tstat->diBuff, 1, srcWidth, srcWidth << 3);
				}
				else
				{
					tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, srcWidth << 3);
				}
				tstat->resizer->Resize(tstat->diBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, Math::OSInt2Double(cropWidth), Math::OSInt2Double(cropHeight), 0, 0, vbuff2->destBuff, vbuff2->destW * vbuff2->destBitDepth >> 3, vbuff2->destW, vbuff2->destH);
				mutUsage.BeginUse();
				vbuff2->isOutputReady = true;
				vbuff2->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt->Set();
			}
			else
			{
				Sync::MutexUsage mutUsage(tstat->me->buffMut);
				vbuff->isOutputReady = true;
				vbuff->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt->Set();
			}
			tstat->hTime = 0;
			tstat->vTime = 0;
		}
	}
}

Media::IImgResizer *UI::GUIVideoBoxDD::CreateResizer(Media::ColorManagerSess *colorSess, Int32 bitDepth, Double srcRefLuminance)
{
	Media::IImgResizer *resizer;
	Media::ColorProfile destColor(Media::ColorProfile::CPT_VDISPLAY);
	if (bitDepth == 16)
	{
		NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C16(4, 3, &destColor, colorSess, Media::AT_NO_ALPHA, srcRefLuminance));
	}
	else if (this->curr10Bit)
	{
		NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, &destColor, colorSess, Media::AT_NO_ALPHA, srcRefLuminance, Media::PF_LE_A2B10G10R10));
	}
	else
	{
		NEW_CLASS(resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, &destColor, colorSess, Media::AT_NO_ALPHA, srcRefLuminance, this->GetPixelFormat()));
	}
	return resizer;
}

void UI::GUIVideoBoxDD::CreateCSConv(ThreadStat *tstat, Media::FrameInfo *info)
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
	Int32 fcc = info->fourcc;
	if (fcc == FFMT_YUV444P10LE)
	{
		fcc = FFMT_YUV444P10LEP;
	}
	else if (fcc == FFMT_YUV420P10LE)
	{
		fcc = *(Int32*)"P016";
	}
	else if (fcc == FFMT_YUV420P12LE)
	{
		fcc = *(Int32*)"P016";
	}
	else if (fcc == FFMT_YUV420P8)
	{
		fcc = *(Int32*)"YV12";
	}

	if (tstat->procType == 2)
	{
		if (tstat->cs10Bit)
		{
			tstat->csconv = Media::CS::CSConverter::NewConverter(fcc, info->storeBPP, info->pf, info->color, 0, 32, Media::PF_LE_A2B10G10R10, &color, yuvType, this->colorSess);
		}
		else
		{
			tstat->csconv = Media::CS::CSConverter::NewConverter(fcc, info->storeBPP, info->pf, info->color, 0, 32, Media::PF_B8G8R8A8, &color, yuvType, this->colorSess);
		}
	}
	else
	{
		color.GetRTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
		color.GetGTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
		color.GetBTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
		tstat->csconv = Media::CS::CSConverter::NewConverter(fcc, info->storeBPP, info->pf, info->color, *(Int32*)"LRGB", 64, Media::PF_UNKNOWN, &color, yuvType, this->colorSess);
		if (info->dispWidth * info->dispHeight > tstat->lrSize)
		{
			tstat->lrSize = info->dispWidth * info->dispHeight;
			if (tstat->lrBuff)
			{
				MemFreeA64(tstat->lrBuff);
			}
			tstat->lrBuff = MemAllocA64(UInt8, tstat->lrSize * 8);
		}
	}
}

void UI::GUIVideoBoxDD::CreateThreadResizer(ThreadStat *tstat)
{
	SDEL_CLASS(tstat->resizer);
	SDEL_CLASS(tstat->dresizer);
	Media::ColorProfile destColor(Media::ColorProfile::CPT_VDISPLAY);

	if (this->bitDepth == 16)
	{
		NEW_CLASS(tstat->resizer, Media::Resizer::LanczosResizerLR_C16(4, 3, &destColor, colorSess, Media::AT_NO_ALPHA, tstat->resizerSrcRefLuminance));
		tstat->procType = 0;
		tstat->resizerBitDepth = 16;
	}
	else if (this->curr10Bit)
	{
		NEW_CLASS(tstat->dresizer, Media::Resizer::DeintResizerLR_C32(0, 0, &destColor, colorSess, Media::AT_NO_ALPHA, tstat->resizerSrcRefLuminance, Media::PF_LE_A2B10G10R10));
		tstat->procType = 1;
		tstat->resizerBitDepth = 32;
	}
	else
	{
		NEW_CLASS(tstat->dresizer, Media::Resizer::DeintResizerLR_C32(0, 0, &destColor, colorSess, Media::AT_NO_ALPHA, tstat->resizerSrcRefLuminance, this->GetPixelFormat()));
		tstat->procType = 1;
		tstat->resizerBitDepth = 32;
	}
	tstat->resizer10Bit = this->curr10Bit;
}

void __stdcall UI::GUIVideoBoxDD::OnVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	UI::GUIVideoBoxDD *me = (UI::GUIVideoBoxDD*)userData;
	if (me->ignoreFrameTime)
	{
		frameTime = MulDiv32(frameNum, me->frameRateDenorm * 1000, me->frameRateNorm);
	}
#ifdef _DEBUG
	if (me->debugLog)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		if (frameTime < me->debugFrameTime)
		{
			sptr = Text::StrInt32(Text::StrConcat(sbuff, (const UTF8Char*)"*"), frameNum);
		}
		else
		{
			sptr = Text::StrInt32(sbuff, frameNum);
		}
		if (me->debugFrameNum != frameNum && me->debugFrameNum != frameNum - 1)
		{
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"?");
		}
		me->debugFrameNum = frameNum;
		if (flags & Media::IVideoSource::FF_DISCONTTIME)
		{
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"D");
		}
		if (flags & Media::IVideoSource::FF_BFRAMEPROC)
		{
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"B");
		}
		if (flags & Media::IVideoSource::FF_RFF)
		{
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"R");
		}
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"\t");
		sptr = Text::StrInt32(sptr, me->dispClk->GetCurrTime());
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"\t");
		sptr = Text::StrInt32(sptr, frameTime);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"\t");
		sptr = Text::StrOSInt(sptr, dataSize);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"\t");
		switch (frameStruct)
		{
		case Media::IVideoSource::FS_I:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"I");
			break;
		case Media::IVideoSource::FS_P:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"P");
			break;
		case Media::IVideoSource::FS_B:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"B");
			break;
		case Media::IVideoSource::FS_N:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"N");
			break;
		}
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"\t");

		switch (frameType)
		{
		case Media::FT_NON_INTERLACE:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Progressive");
			break;
		case Media::FT_INTERLACED_TFF:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Interlaced TFF");
			break;
		case Media::FT_INTERLACED_BFF:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Interlaced BFF");
			break;
		case Media::FT_FIELD_TF:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Top Field");
			break;
		case Media::FT_FIELD_BF:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Bottom Field");
			break;
		case Media::FT_MERGED_TF:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Merged Top Field");
			break;
		case Media::FT_MERGED_BF:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Merged Bottom Field");
			break;
		case Media::FT_DISCARD:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Discard");
			break;
		case Media::FT_INTERLACED_NODEINT:
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Interlaced (No deinterlace)");
			break;
		}
		me->debugLog->WriteLine(sbuff);
		me->debugFrameTime = frameTime;
	}
#endif

	OSInt i;
	OSInt j;
	Int32 picCnt = Sync::Interlocked::Increment(&me->picCnt);
	Bool found = false;
	UOSInt frameIndex;
	if ((picCnt % 30) == 0)
	{
		flags = (Media::IVideoSource::FrameFlag)(flags | Media::IVideoSource::FF_FORCEDISP);
	}
	if (flags & Media::IVideoSource::FF_DISCONTTIME)
	{
		Bool found = true;

		while (me->playing)
		{
			found = false;
			frameIndex = 0;
			me->dispMut->LockRead();
			while (frameIndex < me->allBuffCnt && me->playing)
			{
				if (!me->buffs[frameIndex].isProcessing)
				{
					me->buffs[frameIndex].isEmpty = true;
					me->buffs[frameIndex].isOutputReady = false;
					me->buffEvt->Set();
				}
				else
				{
					found = true;
				}

				frameIndex++;
			}
			me->dispMut->UnlockRead();
			if (!found)
			{
				break;
			}
			me->dispEvt->Wait(100);
		}
		me->dispMut->LockRead();
		me->dispClk->Start(frameTime - me->timeDelay - me->avOfst);
		me->dispMut->UnlockRead();
	}
	if (frameType == Media::FT_DISCARD)
		return;

	UOSInt buffCnt = me->buffCnt;
	if (frameType == Media::FT_FIELD_TF || frameType == Media::FT_FIELD_BF || frameType == Media::FT_MERGED_TF || frameType == Media::FT_MERGED_BF)
	{
		buffCnt = me->allBuffCnt;
	}
	Sync::MutexUsage mutUsage(me->buffMut);
	while (!found)
	{
		mutUsage.BeginUse();
		frameIndex = 0;
		while (frameIndex < buffCnt)
		{
			if (me->buffs[frameIndex].isEmpty)
			{
				found = true;
				break;
			}
			frameIndex++;
		}
		if (found)
			break;
		mutUsage.EndUse();
		me->buffEvt->Wait(10);
		if(!me->playing)
			return;
	}
	if (me->buffs[frameIndex].srcBuff == 0)
	{
		mutUsage.EndUse();
		return;
	}

	i = 0;
	j = me->imgFilters->GetCount();
	while (i < j)
	{
		me->imgFilters->GetItem(i)->ProcessImage(imgData[0], me->videoInfo->fourcc, me->videoInfo->storeBPP, me->videoInfo->pf, me->videoInfo->dispWidth, me->videoInfo->dispHeight, frameType, ycOfst);
		i++;
	}

	if (me->videoInfo->fourcc == FFMT_YUV444P10LE)
	{
		if (me->videoInfo->storeWidth & 31)
		{
			MemCopyNANC(me->buffs[frameIndex].srcBuff, imgData[0], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
			MemCopyNANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight * 2, imgData[1], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
			MemCopyNANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight * 4, imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
		}
		else
		{
			MemCopyANC(me->buffs[frameIndex].srcBuff, imgData[0], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
			MemCopyANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight * 2, imgData[1], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
			MemCopyANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight * 4, imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
		}
		//			ImageUtil_YUV_Y416ShiftW(me->buffs[frameIndex].srcBuff, imgData[0], imgData[1], imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight, 0);
	}
	else if (me->videoInfo->fourcc == FFMT_YUV420P10LE)
	{
		ImageUtil_CopyShiftW(imgData[0], me->buffs[frameIndex].srcBuff, me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2, 6);
		ImageUtil_UVInterleaveShiftW(me->buffs[frameIndex].srcBuff + (me->videoInfo->storeWidth * me->videoInfo->storeHeight << 1), imgData[1], imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight >> 2, 6);
	}
	else if (me->videoInfo->fourcc == FFMT_YUV420P12LE)
	{
		ImageUtil_CopyShiftW(imgData[0], me->buffs[frameIndex].srcBuff, me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2, 4);
		ImageUtil_UVInterleaveShiftW(me->buffs[frameIndex].srcBuff + (me->videoInfo->storeWidth * me->videoInfo->storeHeight << 1), imgData[1], imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight >> 2, 4);
	}
	else if (me->videoInfo->fourcc == FFMT_YUV420P8)
	{
		MemCopyNANC(me->buffs[frameIndex].srcBuff, imgData[0], me->videoInfo->storeWidth * me->videoInfo->dispHeight);
		MemCopyNANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight, imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight >> 2);
		MemCopyNANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight + (me->videoInfo->storeWidth * me->videoInfo->storeHeight >> 2), imgData[1], me->videoInfo->storeWidth * me->videoInfo->dispHeight >> 2);
	}
	else
	{
		MemCopyANC(me->buffs[frameIndex].srcBuff, imgData[0], dataSize);
	}
	me->buffs[frameIndex].frameTime = frameTime;
	me->buffs[frameIndex].frameNum = frameNum;
	me->buffs[frameIndex].discontTime = (flags & Media::IVideoSource::FF_DISCONTTIME) != 0;
	me->buffs[frameIndex].flags = flags;
	UI::GUIVideoBoxDD::DeinterlaceType deintType = me->deintType;
	if (!me->manualDeint && me->refRate != 0 && me->refRate <= 30)
	{
		deintType = UI::GUIVideoBoxDD::DT_30P_MODE;
	}
	
	if (deintType == UI::GUIVideoBoxDD::DT_FROM_VIDEO)
	{
		me->buffs[frameIndex].frameType = frameType;
	}
	else if (deintType == UI::GUIVideoBoxDD::DT_PROGRESSIVE)
	{
		if (frameType == Media::FT_FIELD_TF || frameType == Media::FT_FIELD_BF)
		{
			me->buffs[frameIndex].frameType = frameType;
		}
		else if (frameType == Media::FT_INTERLACED_BFF || frameType == Media::FT_INTERLACED_TFF || frameType == Media::FT_MERGED_TF || frameType == Media::FT_MERGED_BF)
		{
			me->buffs[frameIndex].frameType = Media::FT_INTERLACED_NODEINT;
		}
		else
		{
			me->buffs[frameIndex].frameType = Media::FT_NON_INTERLACE;
		}
	}
	else if (deintType == UI::GUIVideoBoxDD::DT_INTERLACED_TFF)
	{
		if (frameType == Media::FT_FIELD_TF || frameType == Media::FT_FIELD_BF || frameType == Media::FT_MERGED_TF || frameType == Media::FT_MERGED_BF)
		{
			me->buffs[frameIndex].frameType = frameType;
		}
		else
		{
			me->buffs[frameIndex].frameType = Media::FT_INTERLACED_TFF;
		}
	}
	else if (deintType == UI::GUIVideoBoxDD::DT_INTERLACED_BFF)
	{
		if (frameType == Media::FT_FIELD_TF || frameType == Media::FT_FIELD_BF || frameType == Media::FT_MERGED_TF || frameType == Media::FT_MERGED_BF)
		{
			me->buffs[frameIndex].frameType = frameType;
		}
		else
		{
			me->buffs[frameIndex].frameType = Media::FT_INTERLACED_BFF;
		}
	}
	else if (deintType == UI::GUIVideoBoxDD::DT_30P_MODE)
	{
		if ((flags & Media::IVideoSource::FF_DISCONTTIME) == 0 && me->lastFrameTime < frameTime && me->lastFrameTime + 30 > frameTime)
		{
			mutUsage.EndUse();
			return;
		}

		if (frameType == Media::FT_FIELD_TF || frameType == Media::FT_MERGED_TF)
		{
			me->buffs[frameIndex].frameType = frameType;
		}
		else if (frameType == Media::FT_FIELD_BF || frameType == Media::FT_MERGED_BF)
		{
			mutUsage.EndUse();
			return;
		}
		else if (frameType == Media::FT_INTERLACED_TFF)
		{
			me->buffs[frameIndex].frameType = Media::FT_MERGED_TF;
		}
		else if (frameType == Media::FT_INTERLACED_BFF)
		{
			me->buffs[frameIndex].frameType = Media::FT_MERGED_BF;
		}
		else
		{
			me->buffs[frameIndex].frameType = frameType;
		}
	}
	me->lastFrameTime = frameTime;
	
	/////////////////////////////////////////
	if (me->buffs[frameIndex].frameType == Media::FT_INTERLACED_TFF)
	{
		found = true;
		me->buffs[frameIndex].frameType = Media::FT_MERGED_TF;
		frameType = Media::FT_MERGED_BF;
	}
	else if (me->buffs[frameIndex].frameType == Media::FT_INTERLACED_BFF)
	{
		found = true;
		me->buffs[frameIndex].frameType = Media::FT_MERGED_BF;
		frameType = Media::FT_MERGED_TF;
	}
	else
	{
		found = false;
	}

	me->buffs[frameIndex].ycOfst = ycOfst;
	me->buffs[frameIndex].isProcessing = false;
	me->buffs[frameIndex].isOutputReady = false;
	me->buffs[frameIndex].isEmpty = false;
	mutUsage.EndUse();

	i = me->threadCnt;
	while (i-- > 0)
	{
		if (me->tstats[i].status == 1)
		{
			me->tstats[i].evt->Set();
			break;
		}
	}

	/////////////////////////////////////////
	if (found)
	{
		found = false;
		while (!found)
		{
			mutUsage.BeginUse();
			frameIndex = 0;
			while (frameIndex < me->allBuffCnt)
			{
				if (me->buffs[frameIndex].isEmpty)
				{
					found = true;
					break;
				}
				frameIndex++;
			}
			if (found)
				break;
			mutUsage.EndUse();
			me->buffEvt->Wait(10);
			if(!me->playing)
				return;
		}
		if (me->buffs[frameIndex].srcBuff == 0)
		{
			mutUsage.EndUse();
			return;
		}

		if (me->videoInfo->fourcc == FFMT_YUV444P10LE)
		{
			if (me->videoInfo->storeWidth & 31)
			{
				MemCopyNANC(me->buffs[frameIndex].srcBuff, imgData[0], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
				MemCopyNANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight * 2, imgData[1], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
				MemCopyNANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight * 4, imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
			}
			else
			{
				MemCopyANC(me->buffs[frameIndex].srcBuff, imgData[0], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
				MemCopyANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight * 2, imgData[1], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
				MemCopyANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight * 4, imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2);
			}
//			ImageUtil_YUV_Y416ShiftW(me->buffs[frameIndex].srcBuff, imgData[0], imgData[1], imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight, 0);
		}
		else if (me->videoInfo->fourcc == FFMT_YUV420P10LE)
		{
			ImageUtil_CopyShiftW(imgData[0], me->buffs[frameIndex].srcBuff, me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2, 6);
			ImageUtil_UVInterleaveShiftW(me->buffs[frameIndex].srcBuff + (me->videoInfo->storeWidth * me->videoInfo->storeHeight << 1), imgData[1], imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight >> 2, 6);
		}
		else if (me->videoInfo->fourcc == FFMT_YUV420P12LE)
		{
			ImageUtil_CopyShiftW(imgData[0], me->buffs[frameIndex].srcBuff, me->videoInfo->storeWidth * me->videoInfo->dispHeight * 2, 4);
			ImageUtil_UVInterleaveShiftW(me->buffs[frameIndex].srcBuff + (me->videoInfo->storeWidth * me->videoInfo->storeHeight << 1), imgData[1], imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight >> 2, 4);
		}
		else if (me->videoInfo->fourcc == FFMT_YUV420P8)
		{
			MemCopyNANC(me->buffs[frameIndex].srcBuff, imgData[0], me->videoInfo->storeWidth * me->videoInfo->dispHeight);
			MemCopyNANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight, imgData[2], me->videoInfo->storeWidth * me->videoInfo->dispHeight >> 2);
			MemCopyNANC(me->buffs[frameIndex].srcBuff + me->videoInfo->storeWidth * me->videoInfo->storeHeight + (me->videoInfo->storeWidth * me->videoInfo->storeHeight >> 2), imgData[1], me->videoInfo->storeWidth * me->videoInfo->dispHeight >> 2);
		}
		else
		{
			MemCopyANC(me->buffs[frameIndex].srcBuff, imgData[0], dataSize);
		}
		me->buffs[frameIndex].frameTime = frameTime + 16;
		me->buffs[frameIndex].frameNum = frameNum;
		me->buffs[frameIndex].discontTime = false;
		me->buffs[frameIndex].flags = flags;
		me->buffs[frameIndex].frameType = frameType;
		me->buffs[frameIndex].ycOfst = ycOfst;
		me->buffs[frameIndex].isProcessing = false;
		me->buffs[frameIndex].isOutputReady = false;
		me->buffs[frameIndex].isEmpty = false;
		mutUsage.EndUse();

		i = me->threadCnt;
		while (i-- > 0)
		{
			if (me->tstats[i].status == 1)
			{
				me->tstats[i].evt->Set();
				break;
			}
		}
	}
}

void __stdcall UI::GUIVideoBoxDD::OnVideoChange(Media::IVideoSource::FrameChange fc, void *userData)
{
	UI::GUIVideoBoxDD *me = (UI::GUIVideoBoxDD*)userData;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UOSInt frameSize;
	if (fc == Media::IVideoSource::FC_PAR)
	{
		me->VideoBeginProc();
		me->video->GetVideoInfo(me->videoInfo, &frameRateNorm, &frameRateDenorm, &frameSize);
		me->frameRateNorm = frameRateNorm;
		me->frameRateDenorm = frameRateDenorm;
		me->VideoEndProc();
	}
	else if (fc == Media::IVideoSource::FC_ENDPLAY)
	{
		me->StopPlay();
		if (me->endHdlr)
		{
			me->endHdlr(me->endHdlrObj);
		}
	}
	else if (fc == Media::IVideoSource::FC_CROP)
	{
		me->UpdateCrop();
	}
}

UInt32 __stdcall UI::GUIVideoBoxDD::ProcessThread(void *userObj)
{
	ThreadStat *tstat = (ThreadStat*)userObj;
	OSInt i;
	VideoBuff *buff = 0;
	VideoBuff *buff2;
	Bool found;
	OSInt minIndex = 0;
	UInt32 minTime = 0;
	UInt32 minFrame = 0;

	tstat->status = 1;
	while (!tstat->me->threadToStop)
	{
		found = false;
		Sync::MutexUsage mutUsage(tstat->me->buffMut);
		i = tstat->me->allBuffCnt;
		while (i-- > 0)
		{
			if (!tstat->me->buffs[i].isEmpty && !tstat->me->buffs[i].isProcessing && !tstat->me->buffs[i].isOutputReady)
			{
				if (!found)
				{
					found = true;
					minIndex = i;
					minTime = tstat->me->buffs[i].frameTime;
					minFrame = tstat->me->buffs[i].frameNum;
				}
				else if (tstat->me->buffs[i].frameNum < minFrame)
				{
					minIndex = i;
					minTime = tstat->me->buffs[i].frameTime;
					minFrame = tstat->me->buffs[i].frameNum;
				}
				else if (tstat->me->buffs[i].frameNum == minFrame && tstat->me->buffs[i].frameTime < minTime)
				{
					minIndex = i;
					minTime = tstat->me->buffs[i].frameTime;
					minFrame = tstat->me->buffs[i].frameNum;
				}
			}
		}
		if (found)
		{
			buff = &tstat->me->buffs[minIndex];
			buff->isProcessing = true;
		}
		mutUsage.EndUse();

		if (found)
		{
			tstat->status = 2;
			if (buff->frameType == Media::FT_INTERLACED_TFF || buff->frameType == Media::FT_INTERLACED_BFF)
			{
				buff2 = 0;
				while (buff2 == 0)
				{
					mutUsage.BeginUse();
					i = tstat->me->allBuffCnt;
					while (i-- > 0)
					{
						if (tstat->me->buffs[i].isEmpty)
						{
							buff2 = &tstat->me->buffs[i];
							buff2->isOutputReady = false;
							buff2->isProcessing = true;
							buff2->isEmpty = false;
							break;
						}
					}
					mutUsage.EndUse();
					if (buff2)
						break;
					tstat->me->buffEvt->Wait(10);
				}
			}
			else
			{
				buff2 = 0;
			}

			Int32 currTime = -1;
			Bool toSkip = false;

			tstat->me->dispMut->LockRead();
			if (tstat->me->dispClk)
			{
				if (!tstat->me->dispClk->Running() || buff->discontTime)
				{
					tstat->me->dispClk->Start(buff->frameTime - tstat->me->timeDelay - tstat->me->avOfst);
					currTime = buff->frameTime;
				}
				else
				{
					Int32 procDelay = tstat->me->CalProcDelay();
					Int32 dispDelay = tstat->me->CalDispDelay();
					currTime = tstat->me->dispClk->GetCurrTime() + tstat->me->timeDelay + tstat->me->avOfst + procDelay + dispDelay + PREPROCTIME;
					if (buff->flags & Media::IVideoSource::FF_FORCEDISP)
					{
					}
					else if (currTime > (Int32)buff->frameTime && ((buff->flags & Media::IVideoSource::FF_REALTIME) == 0 || tstat->me->hasAudio))
					{
						toSkip = true;
					}
				}
			}
			else
			{
				toSkip = true;;
			}
			tstat->me->dispMut->UnlockRead();

			if (toSkip)
			{
				mutUsage.BeginUse();
				buff->isOutputReady = false;
				buff->isProcessing = false;
				buff->isEmpty = true;
				if (buff2)
				{
					buff2->isOutputReady = false;
					buff2->isProcessing = false;
					buff2->isEmpty = true;
				}
				mutUsage.EndUse();
				tstat->me->buffEvt->Set();
				Sync::Interlocked::Increment(&tstat->me->debugFrameSkip1);
			}
			else
			{
				Manage::HiResClock clk;
				Double t;
				clk.Start();
				tstat->me->VideoBeginProc();
				if (buff->srcBuff)
				{
					tstat->me->ProcessVideo(tstat, buff, buff2);
				}
				tstat->me->VideoEndProc();
				t = clk.GetTimeDiff();
				tstat->me->procMut->LockWrite();
				tstat->me->procDelayBuff[tstat->me->procThisCount & (PROCDELAYBUFF - 1)] = t;
				tstat->me->procThisCount++;
				tstat->me->procCnt++;
				tstat->me->procMut->UnlockWrite();
			}
			tstat->me->dispEvt->Set();
		}
		else
		{
			tstat->status = 1;
			tstat->evt->Wait(1000);
		}
	}
	tstat->status = 0;
	return 0;
}

UInt32 __stdcall UI::GUIVideoBoxDD::DisplayThread(void *userObj)
{
	UI::GUIVideoBoxDD *me = (UI::GUIVideoBoxDD*)userObj;
	OSInt i;
	Bool found;
	Bool found2;
	UInt32 minFrame = 0;
	UInt32 minFrame2 = 0;
	UInt32 minTime = 0;
	UInt32 minTime2 = 0;
	OSInt lastW = 0;
	OSInt lastH = 0;
	Bool toClear;
	OSInt minIndex = 0;
	me->dispRunning = true;
	Sync::Thread::SetPriority(Sync::Thread::TP_HIGHEST);
	while (!me->dispToStop)
	{
		if (me->playing)
		{
			found = false;
			found2 = false;
			i = me->allBuffCnt;
			while (i-- > 0)
			{
				if (!me->buffs[i].isEmpty)
				{
					if (!found)
					{
						found = true;
						minTime = me->buffs[i].frameTime;
						minFrame = me->buffs[i].frameNum;
						minIndex = i;
					}
					else if (minFrame > me->buffs[i].frameNum)
					{
						found2 = true;
						minFrame2 = minFrame;
						minTime2 = minTime;
						minTime = me->buffs[i].frameTime;
						minFrame = me->buffs[i].frameNum;
						minIndex = i;
					}
					else if (minFrame == me->buffs[i].frameNum && minTime > me->buffs[i].frameTime)
					{
						found2 = true;
						minFrame2 = minFrame;
						minTime2 = minTime;
						minTime = me->buffs[i].frameTime;
						minFrame = me->buffs[i].frameNum;
						minIndex = i;
					}
					else if (!found2 || minFrame2 > me->buffs[i].frameNum)
					{
						found2 = true;
						minTime2 = me->buffs[i].frameTime;
						minFrame2 = me->buffs[i].frameNum;
					}
					else if (minFrame2 == me->buffs[i].frameNum && minTime2 > me->buffs[i].frameTime)
					{
						minTime2 = me->buffs[i].frameTime;
						minFrame2 = me->buffs[i].frameNum;
					}
				}
			}
			if (found)
			{
				me->dispMut->LockRead();
				if (me->dispClk)
				{
					if (me->buffs[minIndex].isOutputReady)
					{
						Int32 t;
//						Int32 dispDelay = me->CalDispDelay();
						Bool skipFrame = false;;
						t = (me->dispClk->GetCurrTime() + me->timeDelay + me->avOfst);
						if (found2 && (me->buffs[minIndex].flags & Media::IVideoSource::FF_FORCEDISP) == 0)
						{
							if (t > (Int32)(minTime2 + 17) && ((me->buffs[minIndex].flags & Media::IVideoSource::FF_REALTIME) == 0 || me->hasAudio))
							{
								skipFrame = true;
							}
						}

#ifdef _DEBUG
						if (me->debugLog2)
						{
							Text::StringBuilderUTF8 sb;
							if (found2)
							{
								sb.Append((const UTF8Char*)"t2=");
								sb.AppendI32(t);
								sb.Append((const UTF8Char*)"\tft=");
								sb.AppendU32(minTime2);
								sb.Append((const UTF8Char*)"\tfnum=");
								sb.AppendU32(me->buffs[minIndex].frameNum);
								if (skipFrame)
								{
									sb.Append((const UTF8Char*)"\tSkipped");
								}
							}
							else
							{
								sb.Append((const UTF8Char*)"t1=");
								sb.AppendI32((me->dispClk->GetCurrTime() + me->timeDelay + me->avOfst));
								sb.Append((const UTF8Char*)"\tft=");
								sb.AppendU32(minTime);
								sb.Append((const UTF8Char*)"\tfnum=");
								sb.AppendU32(me->buffs[minIndex].frameNum);
							}
							me->debugLog2->WriteLine(sb.ToString());
						}
#endif
						if (skipFrame)
						{
							me->buffs[minIndex].isEmpty = true;
							me->buffEvt->Set();
							me->dispMut->UnlockRead();

							me->debugFrameSkip2++;
						}
						else
						{
/*							Manage::HiResClock clk;
							Double dispT;
							Media::IVideoSource::FrameFlag flags = me->buffs[minIndex].flags;
							UInt32 frameTime = me->buffs[minIndex].frameTime;
							Int32 frameNum = me->buffs[minIndex].frameNum;
							////////////////////////////////
							me->UpdateFromBuff(&me->buffs[minIndex]);
							me->buffs[minIndex].isEmpty = true;
							me->buffEvt->Set();
							
							if ((flags & Media::IVideoSource::FF_REALTIME) && !me->hasAudio)
							{
							}
							else
							{
								while ((t = (me->dispClk->GetCurrTime() + me->timeDelay + me->avOfst + dispDelay)) < frameTime)
								{
									if (frameTime - t > 3000 && (flags & Media::IVideoSource::FF_DISCONTTIME) == 0)
										break;
									me->dispEvt->Wait(frameTime - t);
									if (me->dispToStop || !me->playing)
										break;
								}
							}
							me->videoDelay = me->dispClk->GetCurrTime() - frameTime;
							me->dispMut->UnlockRead();
							clk.Start();
							me->surfaceMut->Lock();
							me->DrawToScreen();
							me->dispFrameTime = frameTime;
							me->dispFrameNum = frameNum;
							me->surfaceMut->Unlock();
							dispT = clk.GetTimeDiff();
							me->dispDelayMut->LockWrite();
							me->dispDelayBuff[me->dispCnt & (DISPDELAYBUFF - 1)] = dispT;
							me->dispCnt++;
							me->dispDelayMut->UnlockWrite();

							me->debugFrameDisp++;*/

							Manage::HiResClock clk;
							Double dispT;
							Media::IVideoSource::FrameFlag flags = me->buffs[minIndex].flags;
							UInt32 frameTime = me->buffs[minIndex].frameTime;
							Int32 frameNum = me->buffs[minIndex].frameNum;
							////////////////////////////////
							
							if ((flags & Media::IVideoSource::FF_REALTIME) && !me->hasAudio)
							{
							}
							else
							{
								Int32 dispJitter = me->CalDispJitter();
								if (dispJitter < -30 || dispJitter > 30)
								{
									dispJitter = 0;
								}
								if (me->refRate == 0)
								{
									dispJitter = 16;
								}
								else
								{
									dispJitter = 1000 / me->refRate;
								}
								while ((t = (me->dispClk->GetCurrTime() + me->timeDelay + me->avOfst + dispJitter)) < (Int32)frameTime)
								{
									if (frameTime - t > 3000 && me->videoDelay > -3000)
										break;
									me->dispEvt->Wait(frameTime - t);
									if (me->dispToStop || !me->playing)
										break;
								}
							}
							Int32 startTime = me->dispClk->GetCurrTime();
							me->videoDelay = startTime - frameTime;
							me->dispMut->UnlockRead();
							clk.Start();
							me->VideoBeginProc();
							Sync::MutexUsage mutUsage(me->surfaceMut);

							OSInt rect[4];
							OSInt vwidth = me->videoInfo->dispWidth - me->cropLeft - me->cropRight;
							OSInt vheight = me->videoInfo->dispHeight - me->cropTop - me->cropBottom;
							if (me->videoInfo->ftype == Media::FT_FIELD_BF || me->videoInfo->ftype == Media::FT_FIELD_TF)
							{
								vheight = vheight << 1;
							}
							me->CalDisplayRect(vwidth, vheight, rect);
							toClear = false;
							if (rect[2] == me->buffs[minIndex].destW && rect[3] == me->buffs[minIndex].destH)
							{
								if (rect[2] != lastW || rect[3] != lastH || me->toClear)
								{
									toClear = true;
									lastW = rect[2];
									lastH = rect[3];
									me->toClear = false;
								}
								if (me->buffs[minIndex].destBitDepth == 16)
								{
									me->DrawFromBuff(me->buffs[minIndex].destBuff, rect[2] << 1, rect[0], rect[1], rect[2], rect[3], toClear);
								}
								else
								{
									UInt8 *dispBuff = me->buffs[minIndex].destBuff;
									me->DrawFromBuff(dispBuff, rect[2] << 2, rect[0], rect[1], rect[2], rect[3], toClear);
								}
							}
							me->buffs[minIndex].isEmpty = true;
							me->buffEvt->Set();

							me->dispFrameTime = frameTime;
							me->dispFrameNum = frameNum;
							mutUsage.EndUse();
							me->VideoEndProc();
							dispT = clk.GetTimeDiff();
							me->dispDelayMut->LockWrite();
							me->dispDelayBuff[me->dispCnt & (DISPDELAYBUFF - 1)] = dispT;
							me->dispJitterBuff[me->dispCnt & (DISPDELAYBUFF - 1)] = (startTime + dispT * 1000.0) - (Int32)frameTime + me->timeDelay + me->avOfst;
							me->dispCnt++;
							me->dispDelayMut->UnlockWrite();
							
							me->debugFrameDisp++;
						}
					}
					else
					{
						me->dispMut->UnlockRead();
						me->dispEvt->Wait(10);
					}
				}
				else
				{
					i = me->allBuffCnt;
					while (i-- > 0)
					{
						me->buffs[i].isEmpty = true;
					}
					me->dispMut->UnlockRead();
					me->buffEvt->Set();
				}
			}
			else
			{
				me->dispEvt->Wait(1000);
			}
		}
		else
		{
			me->dispEvt->Wait(1000);
		}
	}
	me->dispRunning = false;
	return 0;
}

void UI::GUIVideoBoxDD::StopThreads()
{
	Bool found;
	OSInt i;
	this->threadToStop = true;
	if (this->playing)
	{
		SetVideo(0);
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->tstats[i].evt->Set();
	}
	this->dispToStop = true;
	this->dispEvt->Set();
	while (true)
	{
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->tstats[i].status != 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			break;
		}
		Sync::Thread::Sleep(10);
	}
	while (this->dispRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

void UI::GUIVideoBoxDD::VideoBeginLoad()
{
	this->videoPause = true;
	while (this->videoProcCnt > 0)
	{
		Sync::Thread::Sleep(10);
	}
}

void UI::GUIVideoBoxDD::VideoEndLoad()
{
	this->videoPause = false;
}

void UI::GUIVideoBoxDD::VideoBeginProc()
{
	while (this->videoPause)
	{
		Sync::Thread::Sleep(10);
	}
	Sync::Interlocked::Increment(&this->videoProcCnt);
}

void UI::GUIVideoBoxDD::VideoEndProc()
{
	Sync::Interlocked::Decrement(&this->videoProcCnt);
}

Int32 UI::GUIVideoBoxDD::CalProcDelay()
{
	Int32 procDelay;
	Double totalTime;
	OSInt i;
	this->procMut->LockRead();
	if (this->procThisCount == 0)
	{
		procDelay = 0;
	}
	else if (this->procThisCount < PROCDELAYBUFF)
	{
		totalTime = 0;
		i = this->procThisCount;
		while (i-- > 0)
		{
			totalTime += this->procDelayBuff[i];
		}
		procDelay = Math::Double2Int32(1000 * totalTime / this->procThisCount);
	}
	else
	{
		totalTime = 0;
		i = PROCDELAYBUFF;
		while (i-- > 0)
		{
			totalTime += this->procDelayBuff[i];
		}
		procDelay = Math::Double2Int32(1000 * totalTime / PROCDELAYBUFF);
	}
	this->procMut->UnlockRead();
	return procDelay;
}

Int32 UI::GUIVideoBoxDD::CalDispDelay()
{
	Int32 dispDelay;
	Double totalTime;
	OSInt i;
//	return 0;
	this->dispDelayMut->LockRead();
	if (this->dispCnt == 0)
	{
		dispDelay = 0;
	}
	else if (this->dispCnt < DISPDELAYBUFF)
	{
		totalTime = 0;
		i = this->dispCnt;
		while (i-- > 0)
		{
			totalTime += this->dispDelayBuff[i];
		}
		dispDelay = Math::Double2Int32(1000 * totalTime / this->dispCnt);
	}
	else
	{
		totalTime = 0;
		i = DISPDELAYBUFF;
		while (i-- > 0)
		{
			totalTime += this->dispDelayBuff[i];
		}
		dispDelay = Math::Double2Int32(1000 * totalTime / DISPDELAYBUFF);
	}
	this->dispDelayMut->UnlockRead();
	return dispDelay;
}

Int32 UI::GUIVideoBoxDD::CalDispJitter()
{
	Int32 dispJitter;
	Double totalJitter;
	OSInt i;
	this->dispDelayMut->LockRead();
	if (this->dispCnt == 0)
	{
		dispJitter = 0;
	}
	else if (this->dispCnt < DISPDELAYBUFF)
	{
		totalJitter = 0;
		i = this->dispCnt;
		while (i-- > 0)
		{
			totalJitter += this->dispJitterBuff[i];
		}
		dispJitter = Math::Double2Int32(totalJitter / (Double)this->dispCnt);
	}
	else
	{
		totalJitter = 0;
		i = DISPDELAYBUFF;
		while (i-- > 0)
		{
			totalJitter += this->dispJitterBuff[i];
		}
		dispJitter = Math::Double2Int32(totalJitter / (Double)DISPDELAYBUFF);
	}
	this->dispDelayMut->UnlockRead();
	return dispJitter;
}

UI::GUIVideoBoxDD::GUIVideoBoxDD(UI::GUICore *ui, UI::GUIClientControl *parent, Media::ColorManagerSess *colorSess, UOSInt buffCnt, UOSInt threadCnt) : UI::GUIDDrawControl(ui, parent, false)
{
	UOSInt i;
	this->colorSess = colorSess;
	if (this->colorSess)
	{
		this->colorSess->AddHandler(this);
	}
	this->refRate = this->GetRefreshRate();
	this->manualDeint = false;
	this->video = 0;
	this->hasAudio = false;
	this->debugLog = 0;
	this->debugFS = 0;
	this->debugLog2 = 0;
	this->debugFS2 = 0;
	this->debugFrameTime = 0;
	this->debugFrameNum = 0;
	this->debugFrameDisp = 0;
	this->debugFrameSkip1 = 0;
	this->debugFrameSkip2 = 0;
	this->picCnt = 0;
	this->dispCnt = 0;
	this->endHdlr = 0;
	this->videoDelay = 0;
	this->dispFrameTime = 0;
	this->dispFrameNum = 0;
	this->srcYUVType = Media::ColorProfile::YUVT_UNKNOWN;
	NEW_CLASS(this->srcColor, Media::ColorProfile(Media::ColorProfile::CPT_VUNKNOWN));
	NEW_CLASS(this->videoInfo, Media::FrameInfo());
	NEW_CLASS(this->outputCopier, Media::ImageCopy());
	this->frameRateDenorm = 1;
	this->frameRateNorm = 30;
	this->ignoreFrameTime = false;
	this->forcePAR = 0;
	this->monPAR = 1.0;
	this->videoInfo->par2 = 1.0;
	this->deintType = UI::GUIVideoBoxDD::DT_FROM_VIDEO;
	this->curr10Bit = false;
	this->currSrcRefLuminance = 0;
	this->toClear = false;
	NEW_CLASS(this->ivtc, Media::VideoFilter::IVTCFilter(0));
	this->ivtc->SetEnabled(false);
	NEW_CLASS(this->uvOfst, Media::VideoFilter::UVOffsetFilter(0));
	NEW_CLASS(this->autoCrop, Media::VideoFilter::AutoCropFilter(0));
	this->autoCrop->SetEnabled(false);
	NEW_CLASS(this->imgFilters, Data::ArrayList<Media::IImgFilter*>());
	this->maHdlr = 0;
	this->maHdlrObj = 0;
	this->maDown = 0;
	this->maDownX = 0;
	this->maDownY = 0;
	this->maDownTime = 0;
	this->cropLeft = 0;
	this->cropTop = 0;
	this->cropRight = 0;
	this->cropBottom = 0;

#ifdef _DEBUG
	NEW_CLASS(this->debugFS, IO::FileStream(L"videoProc.log", IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(this->debugLog, IO::StreamWriter(this->debugFS, 65001));
	NEW_CLASS(this->debugFS2, IO::FileStream(L"videoDisp.log", IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(this->debugLog2, IO::StreamWriter(this->debugFS2, 65001));
#endif
	this->playing = false;
	this->captureFrame = false;
	this->threadCnt = threadCnt;
	this->buffCnt = buffCnt - threadCnt;
	if (this->buffCnt <= 0)
	{
		this->buffCnt = 1;
	}
	this->allBuffCnt = buffCnt;
	this->threadToStop = false;
	this->timeDelay = 0;
	NEW_CLASS(this->buffMut, Sync::Mutex());
	NEW_CLASS(this->buffEvt, Sync::Event(true, (const UTF8Char*)"UI.MSWindowVideoBoxDD.buffEvt"));
	NEW_CLASS(this->dispEvt, Sync::Event(true, (const UTF8Char*)"UI.MSWindowVideoBoxDD.dispEvt"));
	NEW_CLASS(this->dispMut, Sync::RWMutex());
	this->videoPause = false;
	this->videoProcCnt = 0;
	this->avOfst = 0;

	NEW_CLASS(this->procMut, Sync::RWMutex());
	this->procDelayBuff = MemAlloc(Double, PROCDELAYBUFF);
	this->procThisCount = 0;
	this->procCnt = 0;

	NEW_CLASS(this->dispDelayMut, Sync::RWMutex());
	this->dispDelayBuff = MemAlloc(Double, DISPDELAYBUFF);
	this->dispJitterBuff = MemAlloc(Double, DISPDELAYBUFF);

	this->buffs = MemAlloc(VideoBuff, this->allBuffCnt);
	i = allBuffCnt;
	while (i-- > 0)
	{
		this->buffs[i].isEmpty = true;
		this->buffs[i].isProcessing = false;
		this->buffs[i].isOutputReady = false;
		this->buffs[i].srcBuff = 0;
		this->buffs[i].frameTime = 0;
		this->buffs[i].discontTime = false;
		this->buffs[i].destBuff = 0;
		this->buffs[i].destSize = 0;
		this->buffs[i].destW = 0;
		this->buffs[i].destH = 0;
	}
	this->tstats = MemAlloc(ThreadStat, this->threadCnt);
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->tstats[i].status = 0;
		this->tstats[i].resizer = 0;
		this->tstats[i].dresizer = 0;
		this->tstats[i].procType = 0;
		this->tstats[i].csconv = 0;
		this->tstats[i].lrBuff = 0;
		this->tstats[i].lrSize = 0;
		this->tstats[i].diBuff = 0;
		this->tstats[i].diSize = 0;
		this->tstats[i].deint = 0;
		this->tstats[i].hTime = 0;
		this->tstats[i].vTime = 0;
		this->tstats[i].csTime = 0;
		this->tstats[i].resizerBitDepth = 0;
		this->tstats[i].resizer10Bit = false;
		this->tstats[i].me = this;
		NEW_CLASS(this->tstats[i].evt, Sync::Event(true, (const UTF8Char*)"UI.MSWindowVideoBoxDD.tstats.evt"));
		Sync::Thread::Create(ProcessThread, &this->tstats[i]);
	}

	this->dispToStop = false;
	this->dispRunning = false;
	this->dispForceUpdate = true;
	Sync::Thread::Create(DisplayThread, this);
	while (!this->dispRunning)
	{
		Sync::Thread::Sleep(10);
	}

	Bool found = false;
	while (true)
	{
		found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->tstats[i].status == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			break;
		}
		Sync::Thread::Sleep(10);
	}
}

UI::GUIVideoBoxDD::~GUIVideoBoxDD()
{
	UOSInt i;
	this->StopThreads();
	DEL_CLASS(this->dispEvt);
	DEL_CLASS(this->dispMut);

	if (this->colorSess)
	{
		this->colorSess->RemoveHandler(this);
	}

	i = this->threadCnt;
	while (i-- > 0)
	{
		SDEL_CLASS(this->tstats[i].resizer);
		SDEL_CLASS(this->tstats[i].dresizer);
		SDEL_CLASS(this->tstats[i].csconv);
		DEL_CLASS(this->tstats[i].evt);
		if (this->tstats[i].lrBuff)
		{
			MemFreeA(this->tstats[i].lrBuff);
			this->tstats[i].lrBuff = 0;
		}
		if (this->tstats[i].diBuff)
		{
			MemFreeA(this->tstats[i].diBuff);
			this->tstats[i].diBuff = 0;
		}
		if (this->tstats[i].deint)
		{
			DEL_CLASS(this->tstats[i].deint);
		}
	}
	MemFree(this->tstats);

	i = this->allBuffCnt;
	while (i-- > 0)
	{
		if (buffs[i].srcBuff)
		{
			MemFreeA64(buffs[i].srcBuff);
		}
		if (buffs[i].destBuff)
		{
			MemFreeA(buffs[i].destBuff);
		}
	}
	Media::IImgFilter *imgFilter;
	i = this->imgFilters->GetCount();
	while (i-- > 0)
	{
		imgFilter = this->imgFilters->GetItem(i);
		DEL_CLASS(imgFilter);
	}
	DEL_CLASS(this->imgFilters);

	DEL_CLASS(this->ivtc);
	DEL_CLASS(this->uvOfst);
	DEL_CLASS(this->autoCrop);
	DEL_CLASS(this->procMut);
	MemFree(this->procDelayBuff);
	DEL_CLASS(this->dispDelayMut);
	MemFree(this->dispDelayBuff);
	MemFree(this->dispJitterBuff);
	MemFree(this->buffs);
	DEL_CLASS(this->buffEvt);
	DEL_CLASS(this->buffMut);
	DEL_CLASS(this->videoInfo);
	DEL_CLASS(this->srcColor);
	DEL_CLASS(this->outputCopier);
#ifdef _DEBUG
	SDEL_CLASS(this->debugLog);
	SDEL_CLASS(this->debugFS);
	SDEL_CLASS(this->debugLog2);
	SDEL_CLASS(this->debugFS2);
#endif
}

const UTF8Char *UI::GUIVideoBoxDD::GetObjectClass()
{
	return (const UTF8Char*)"VideoBoxDD";
}

OSInt UI::GUIVideoBoxDD::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

void UI::GUIVideoBoxDD::OnSizeChanged(Bool updateScn)
{
	Bool curr10Bit = false;
	if (this->currScnMode == SM_VFS || this->currScnMode == SM_FS)
	{
		curr10Bit = this->colorSess->Get10BitColor();
	}
	this->curr10Bit = curr10Bit;
	this->toClear = true;

	UOSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
	}
}

void UI::GUIVideoBoxDD::SetVideo(Media::IVideoSource *video)
{
	UOSInt i;
	///////////////////////////////////////
	this->playing = false;
	this->dispMut->LockWrite();
	this->dispClk = 0;
	this->dispMut->UnlockWrite();

	this->VideoBeginLoad();
	this->avOfst = 0;
	if (this->video)
	{
		this->uvOfst->Stop();
	}

	this->video = video;
	if (this->video)
	{
		this->ivtc->SetSourceVideo(this->video);
		this->autoCrop->SetSourceVideo(this->ivtc);
		this->uvOfst->SetSourceVideo(this->autoCrop);
	}
	else
	{
		this->ivtc->SetSourceVideo(0);
		this->autoCrop->SetSourceVideo(0);
		this->uvOfst->SetSourceVideo(0);
	}
//	this->forseFT = false;
	i = this->allBuffCnt;
	while (i-- > 0)
	{
		if (this->buffs[i].srcBuff)
		{
			MemFreeA(this->buffs[i].srcBuff);
		}
		this->buffs[i].srcBuff = 0;
	}
	this->VideoEndLoad();

	this->procThisCount = 0;
	if (this->video)
	{
		Media::FrameInfo info;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		UOSInt frameSize;
 		if (!this->video->GetVideoInfo(&info, &frameRateNorm, &frameRateDenorm, &frameSize))
		{
			this->video = 0;
			this->ivtc->SetSourceVideo(0);
			this->autoCrop->SetSourceVideo(0);
			this->uvOfst->SetSourceVideo(0);
			return;
		}

		this->VideoBeginLoad();
		this->videoInfo->Set(&info);
		this->frameRateNorm = frameRateNorm;
		this->frameRateDenorm = frameRateDenorm;
		if (info.fourcc == FFMT_YUV444P10LE)
		{
			frameSize = info.storeWidth * info.storeHeight * 8;
		}
		i = this->allBuffCnt;
		while (i-- > 0)
		{
			this->buffs[i].srcBuff = MemAllocA64(UInt8, frameSize);
		}

		this->currSrcRefLuminance = Media::CS::TransferFunc::GetRefLuminance(info.color->rtransfer);
		this->srcColor->Set(info.color);
		this->srcYUVType = info.yuvType;
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->CreateThreadResizer(&this->tstats[i]);
			this->CreateCSConv(&this->tstats[i], &info);
		}
		this->VideoEndLoad();
		this->UpdateCrop();
	}
}

void UI::GUIVideoBoxDD::SetHasAudio(Bool hasAudio)
{
	this->hasAudio = hasAudio;
}

void UI::GUIVideoBoxDD::SetTimeDelay(Int32 timeDelay)
{
	this->timeDelay = timeDelay;
}

void UI::GUIVideoBoxDD::VideoInit(Media::RefClock *clk)
{
	if (this->tstats[0].csconv && !this->playing)
	{
		if (this->video)
		{
			this->dispClk = clk;
			//this->video->Init(OnVideoFrame, OnVideoChange, this);
			this->uvOfst->Init(OnVideoFrame, OnVideoChange, this);
		}
		this->ClearBuff();
	}
}

void UI::GUIVideoBoxDD::VideoStart()
{
	if (this->tstats[0].csconv && !this->playing)
	{
		this->ClearBuff();
		if (this->video)
		{
			if (this->uvOfst->Start())
			{
				this->captureFrame = false;
				this->playing = true;
			}
		}
	}
}

void UI::GUIVideoBoxDD::StopPlay()
{
	if (this->video)
	{
		this->playing = false;
		this->uvOfst->Stop();
		this->dispEvt->Set();

		this->ClearBuff();
		this->dispMut->LockWrite();
		this->dispClk = 0;
		this->dispMut->UnlockWrite();
	}
}

void UI::GUIVideoBoxDD::UpdateCrop()
{
	OSInt cropLeft;
	OSInt cropTop;
	OSInt cropRight;
	OSInt cropBottom;
	if (this->video == 0)
		return;
	this->video->GetBorderCrop(&cropLeft, &cropTop, &cropRight, &cropBottom);
	this->cropLeft = cropLeft;
	this->cropTop = cropTop;
	this->cropRight = cropRight;
	this->cropBottom = cropBottom;
}

/*void UI::GUIVideoBoxDD::SetFrameType(Bool forse, Media::FrameType frameType)
{
	this->forseFTVal = frameType;
	this->forseFT = forse;
}*/

void UI::GUIVideoBoxDD::SetDeintType(UI::GUIVideoBoxDD::DeinterlaceType deintType)
{
	this->deintType = deintType;
	this->manualDeint = true;
}

void UI::GUIVideoBoxDD::ClearBuff()
{
	OSInt i;
	i = this->allBuffCnt;
	while (i-- > 0)
	{
		if (!this->buffs[i].isEmpty)
		{
			this->buffs[i].isEmpty = true;
		}
	}
}

void UI::GUIVideoBoxDD::SetEndNotify(EndNotifier endHdlr, void *userObj)
{
	this->endHdlr = endHdlr;
	this->endHdlrObj = userObj;
}

void UI::GUIVideoBoxDD::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
	if (this->video && this->tstats[0].csconv)
	{
		if (!playing)
		{
			this->dispForceUpdate = true;
			this->dispEvt->Set();
		}
	}
}

void UI::GUIVideoBoxDD::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	if (this->video && this->tstats[0].csconv)
	{
		if (!playing)
		{
			this->dispForceUpdate = true;
			this->dispEvt->Set();
		}
	}
}

void UI::GUIVideoBoxDD::OnMonitorChanged()
{
	this->refRate = this->GetRefreshRate();
	this->manualDeint = false;
}

void UI::GUIVideoBoxDD::OnSurfaceCreated()
{
	if (!playing)
	{
		this->dispForceUpdate = true;
		this->dispEvt->Set();
	}
}

void UI::GUIVideoBoxDD::OnMouseWheel(OSInt x, OSInt y, Int32 amount)
{
}

void UI::GUIVideoBoxDD::OnMouseMove(OSInt x, OSInt y)
{
}

void UI::GUIVideoBoxDD::OnMouseDown(OSInt x, OSInt y, MouseButton button)
{
	if (button == UI::GUIControl::MBTN_LEFT && !this->maDown)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		this->maDownTime = dt.ToTicks();
		this->maDown = true;
		this->maDownX = x;
		this->maDownY = y;
	}
}

void UI::GUIVideoBoxDD::OnMouseUp(OSInt x, OSInt y, MouseButton button)
{
	if (this->maDown && button == UI::GUIControl::MBTN_LEFT)
	{
		this->maDown = false;
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.ToTicks() - this->maDownTime < 1000 && this->maHdlr != 0)
		{
			OSInt xDiff = x - this->maDownX;
			OSInt yDiff = y - this->maDownY;
			OSInt xType;
			OSInt yType;
			Double dpi = this->GetHDPI();
			if (xDiff >= dpi)
			{
				xType = 1;
			}
			else if (xDiff <= -dpi)
			{
				xType = -1;
			}
			else
			{
				xType = 0;
			}
			if (yDiff >= dpi)
			{
				yType = 1;
			}
			else if (yDiff <= -dpi)
			{
				yType = -1;
			}
			else
			{
				yType = 0;
			}
			if (xType == 1 && yType == 0)
			{
				this->maHdlr(this->maHdlrObj, UI::GUIVideoBoxDD::MA_START, x, y);
			}
			else if (xType == -1 && yType == 0)
			{
				this->maHdlr(this->maHdlrObj, UI::GUIVideoBoxDD::MA_STOP, x, y);
			}
			else if (xType == 0 && yType == 1)
			{
				this->maHdlr(this->maHdlrObj, UI::GUIVideoBoxDD::MA_PAUSE, x, y);
			}
		}
	}
}

void UI::GUIVideoBoxDD::GetDebugValues(DebugValue *dbg)
{
	this->dispMut->LockRead();
	if (this->dispClk && this->dispClk->Running())
	{
		dbg->currTime = this->dispClk->GetCurrTime();
	}
	else
	{
		dbg->currTime = 0;
	}
	this->dispMut->UnlockRead();
	dbg->dispDelay = this->CalDispDelay();
	dbg->procDelay = this->CalProcDelay();
	dbg->dispJitter = this->CalDispJitter();
	dbg->videoDelay = this->videoDelay;
	dbg->avOfst = this->avOfst;
	dbg->dispFrameTime = this->dispFrameTime;
	dbg->frameDispCnt = this->debugFrameDisp;
	dbg->frameSkip1 = this->debugFrameSkip1;
	dbg->frameSkip2 = this->debugFrameSkip2;
	dbg->srcDelay = this->timeDelay;
	dbg->dispFrameNum = this->dispFrameNum;
	dbg->srcYUVType = this->srcYUVType;
	dbg->color->Set(this->videoInfo->color);
	dbg->dispBitDepth = this->bitDepth;
	dbg->srcWidth = this->videoInfo->dispWidth;
	dbg->srcHeight = this->videoInfo->dispHeight;
	dbg->dispWidth = this->surfaceW;
	dbg->dispHeight = this->surfaceH;
	if (this->video)
	{
		dbg->decoderName = this->video->GetFilterName();
	}
	else
	{
		dbg->decoderName = 0;
	}
	if (this->video)
	{
		dbg->seekCnt = this->video->GetDataSeekCount();
	}
	else
	{
		dbg->seekCnt = 0;
	}
	if (this->forcePAR)
	{
		dbg->par = this->forcePAR / this->monPAR;
	}
	else
	{
		dbg->par = this->videoInfo->par2 / this->monPAR;
	}
	UOSInt i;
	dbg->format = this->videoInfo->fourcc;
	dbg->buffProc = 0;
	dbg->buffReady = 0;
	Double hTime = 0;
	Double vTime = 0;
	Double csTime = 0;
	i = this->threadCnt;
	while (i-- > 0)
	{
		hTime += this->tstats[i].hTime;
		vTime += this->tstats[i].vTime;
		csTime += this->tstats[i].csTime;
	}
	dbg->hTime = hTime / this->threadCnt;
	dbg->vTime = vTime / this->threadCnt;
	dbg->csTime = csTime / this->threadCnt;

	Sync::MutexUsage mutUsage(this->buffMut);
	i = 0;
	while (i < this->allBuffCnt)
	{
		if (this->buffs[i].isEmpty)
		{
		}
		else if (this->buffs[i].isOutputReady)
		{
			dbg->buffReady++;
		}
		else
		{
			dbg->buffProc++;
		}
		i++;
	}
	mutUsage.EndUse();
}

void UI::GUIVideoBoxDD::SetSrcRGBType(Media::CS::TransferType rgbType)
{
	UOSInt i;
	this->VideoBeginLoad();
	this->videoInfo->color->GetRTranParam()->Set(rgbType, 2.2);
	this->videoInfo->color->GetGTranParam()->Set(rgbType, 2.2);
	this->videoInfo->color->GetBTranParam()->Set(rgbType, 2.2);
	this->srcColor->GetRTranParam()->Set(rgbType, 2.2);
	this->srcColor->GetGTranParam()->Set(rgbType, 2.2);
	this->srcColor->GetBTranParam()->Set(rgbType, 2.2);
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(&this->tstats[i], this->videoInfo);
	}
	this->VideoEndLoad();
}

void UI::GUIVideoBoxDD::SetSrcPrimaries(Media::ColorProfile::ColorType colorType)
{
	UOSInt i;
	this->VideoBeginLoad();
	if (colorType == Media::ColorProfile::CT_VUNKNOWN)
	{
		this->videoInfo->color->GetPrimaries()->Set(this->srcColor->GetPrimaries());
	}
	else
	{
		this->videoInfo->color->GetPrimaries()->SetColorType(colorType);
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(&this->tstats[i], this->videoInfo);
	}
	this->VideoEndLoad();
}

void UI::GUIVideoBoxDD::SetSrcWP(Media::ColorProfile::WhitePointType wpType)
{
	UOSInt i;
	this->VideoBeginLoad();
	this->videoInfo->color->GetPrimaries()->SetWhiteType(wpType);
	this->videoInfo->color->GetPrimaries()->colorType = Media::ColorProfile::CT_CUSTOM;
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(&this->tstats[i], this->videoInfo);
	}
	this->VideoEndLoad();
}

void UI::GUIVideoBoxDD::SetSrcWPTemp(Double colorTemp)
{
	UOSInt i;
	this->VideoBeginLoad();
	this->videoInfo->color->GetPrimaries()->SetWhiteTemp(colorTemp);
	this->videoInfo->color->GetPrimaries()->colorType = Media::ColorProfile::CT_CUSTOM;
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(&this->tstats[i], this->videoInfo);
	}
	this->VideoEndLoad();
}

void UI::GUIVideoBoxDD::SetSrcYUVType(Media::ColorProfile::YUVType yuvType)
{
	UOSInt i;
	this->VideoBeginLoad();
	this->videoInfo->yuvType = yuvType;
	this->srcYUVType = yuvType;
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(&this->tstats[i], this->videoInfo);
	}
	this->VideoEndLoad();
}

void UI::GUIVideoBoxDD::SetSrcPAR(Double forcePAR)
{
	this->forcePAR = forcePAR;
}

void UI::GUIVideoBoxDD::SetMonPAR(Double forcePAR)
{
	this->monPAR = forcePAR;
}

void UI::GUIVideoBoxDD::SetIVTCEnable(Bool enableIVTC)
{
	this->ivtc->SetEnabled(enableIVTC);
}

void UI::GUIVideoBoxDD::SetUVOfst(Int32 uOfst, Int32 vOfst)
{
	this->uvOfst->SetOffset(uOfst, vOfst);
}

void UI::GUIVideoBoxDD::SetAutoCropEnable(Bool enableCrop)
{
	this->autoCrop->SetEnabled(enableCrop);
}

Int32 UI::GUIVideoBoxDD::GetAVOfst()
{
	return this->avOfst;
}

void UI::GUIVideoBoxDD::SetAVOfst(Int32 avOfst)
{
	this->avOfst = avOfst;
}

void UI::GUIVideoBoxDD::SetIgnoreFrameTime(Bool ignoreFrameTime)
{
	this->ignoreFrameTime = ignoreFrameTime;
}

void UI::GUIVideoBoxDD::AddImgFilter(Media::IImgFilter *imgFilter)
{
	this->imgFilters->Add(imgFilter);
}

void UI::GUIVideoBoxDD::HandleMouseActon(MouseActionHandler hdlr, void *userObj)
{
	this->maHdlr = hdlr;
	this->maHdlrObj = userObj;
}

void UI::GUIVideoBoxDD::Snapshot()
{
	if (this->playing)
	{
		this->captureFrame = true;
	}
}