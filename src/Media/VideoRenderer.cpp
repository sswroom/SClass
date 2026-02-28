#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/PNGExporter.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Math/Math_C.h"
#include "Media/DeinterlaceLR.h"
#include "Media/ImageCopy.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil_C.h"
#include "Media/VideoRenderer.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/DeintResizerLR_C32.h"
#include "Media/Resizer/LanczosResizerLR_C16.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"

#define PREPROCTIME 0
#define PROCDELAYBUFF 32
#define DISPDELAYBUFF 32

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void Media::VideoRenderer::CalDisplayRect(UIntOS srcWidth, UIntOS srcHeight, DrawRect *rect)
{
	Double par;
	if (this->forcePAR == 0)
	{
		par = this->videoInfo.CalcPAR() / this->monPAR;
	}
	else
	{
		par = this->forcePAR / this->monPAR;
	}

	if (UIntOS2Double(this->outputSize.x) * UIntOS2Double(srcHeight) * par > UIntOS2Double(this->outputSize.y) * UIntOS2Double(srcWidth))
	{
		rect->size.y = this->outputSize.y;
		rect->size.x = (UIntOS)Double2IntOS(UIntOS2Double(this->outputSize.y) / par * UIntOS2Double(srcWidth) / UIntOS2Double(srcHeight));
	}
	else
	{
		rect->size.x = this->outputSize.x;
		rect->size.y = (UIntOS)Double2IntOS(UIntOS2Double(this->outputSize.x) * UIntOS2Double(srcHeight) * par / UIntOS2Double(srcWidth));
	}
	rect->tl.x = (IntOS)(this->outputSize.x - rect->size.x) >> 1;
	rect->tl.y = (IntOS)(this->outputSize.y - rect->size.y) >> 1;
}


void Media::VideoRenderer::ProcessVideo(NN<ThreadStat> tstat, VideoBuff *vbuff, VideoBuff *vbuff2)
{
	NN<Media::CS::CSConverter> csconv;
	NN<Media::MonitorSurface> destSurface;
	UnsafeArray<UInt8> vsrcBuff;
	DrawRect rect;
	UIntOS srcWidth = 0;
	UIntOS srcHeight = 0;
	UIntOS cropWidth;
	UIntOS cropHeight;
	UIntOS cropDY;
	UInt8 *srcBuff = tstat->lrBuff;
	UIntOS sizeNeeded;
	UIntOS cropTotal = tstat->me->cropLeft + tstat->me->cropRight + tstat->me->cropTop + tstat->me->cropBottom;
	Double par;
	if (!vbuff->srcBuff.SetTo(vsrcBuff))
	{
		return;
	}
	if (tstat->me->forcePAR == 0)
	{
		par = tstat->me->videoInfo.CalcPAR() / tstat->me->monPAR;
	}
	else
	{
		par = tstat->me->forcePAR / tstat->me->monPAR;
	}
	
	if (tstat->me->captureFrame)
	{
		tstat->me->captureFrame = false;
		Media::FrameInfo *info = &tstat->me->videoInfo;
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
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
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
		NN<Media::VideoSource> video;
		if (this->video.SetTo(video) && Media::CS::CSConverter::NewConverter(fcc, info->storeBPP, info->pf, info->color, 0, 32, Media::PF_B8G8R8A8, color, yuvType, this->colorSess).SetTo(csconv))
		{
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			UIntOS i;
			NN<Media::StaticImage> simg;
			NEW_CLASSNN(simg, Media::StaticImage(info->dispSize, 0, 32, Media::PF_B8G8R8A8, 0, color, yuvType, Media::AT_ALPHA_ALL_FF, vbuff->ycOfst));
			csconv->ConvertV2(&vsrcBuff, simg->data, info->dispSize.x, info->dispSize.y, info->storeSize.x, info->storeSize.y, (IntOS)simg->GetDataBpl(), vbuff->frameType, vbuff->ycOfst);
			ImageUtil_ImageFillAlpha32(simg->data.Ptr(), info->dispSize.x, info->dispSize.y, simg->GetDataBpl(), 0xff);
			sptr = video->GetSourceName(sbuff).Or(sbuff);
			i = Text::StrLastIndexOfCharC(sbuff, (UIntOS)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
			sptr = &sbuff[i + 1];
			Data::DateTime dt;
			dt.SetCurrTime();
			sptr = Text::StrConcatC(sptr, UTF8STRC("Snapshot"));
			sptr = dt.ToString(sptr, "yyyyMMdd_HHmmssfff");
			sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
			Media::ImageList imgList(CSTRP(sbuff, sptr));
			imgList.AddImage(simg, 0);
			Exporter::PNGExporter exporter;
			IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			exporter.ExportFile(fs, CSTRP(sbuff, sptr), imgList, nullptr);
			csconv.Delete();
		}
	}

	if (!tstat->csconv.SetTo(csconv))
	{
		return;
	}

	if ((vbuff->frameType == Media::FT_NON_INTERLACE || vbuff->frameType == Media::FT_INTERLACED_NODEINT) && par == 1.0 && cropTotal == 0 && tstat->me->videoInfo.dispSize == tstat->me->outputSize && tstat->me->outputBpp == 32)
	{
		if (tstat->procType != 2)
		{
			tstat->procType = 2;
			tstat->cs10Bit = tstat->me->curr10Bit;
			tstat->me->CreateCSConv(tstat, &tstat->me->videoInfo);
		}
		else if (tstat->cs10Bit != tstat->me->curr10Bit)
		{
			tstat->cs10Bit = tstat->me->curr10Bit;
			tstat->me->CreateCSConv(tstat, &tstat->me->videoInfo);
		}
		vbuff->destSize = tstat->me->videoInfo.dispSize;
		vbuff->destBitDepth = 32;
		if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
		{
			vbuff->destSurface.Delete();
			vbuff->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
		}
		Manage::HiResClock clk;
		if (vbuff->destSurface.SetTo(destSurface))
		{
			IntOS destBpl;
			UnsafeArray<UInt8> destBuff;
			if (destSurface->LockSurface(destBpl).SetTo(destBuff))
			{
				csconv->ConvertV2(&vsrcBuff, destBuff, tstat->me->videoInfo.dispSize.x, tstat->me->videoInfo.dispSize.y, tstat->me->videoInfo.storeSize.x, tstat->me->videoInfo.storeSize.y, destBpl, vbuff->frameType, vbuff->ycOfst);
				destSurface->UnlockSurface();
			}
		}
		tstat->csTime = clk.GetTimeDiff();

		Sync::MutexUsage mutUsage(tstat->me->buffMut);
		vbuff->isOutputReady = true;
		vbuff->isProcessing = false;
		mutUsage.EndUse();
		tstat->me->dispEvt.Set();
	}
	else
	{
		if (tstat->procType == 2)
		{
			tstat->me->CreateThreadResizer(tstat);
			tstat->me->CreateCSConv(tstat, &tstat->me->videoInfo);
		}
		else if ((tstat->resizerBitDepth != tstat->me->outputBpp || tstat->resizer10Bit != tstat->me->curr10Bit) && tstat->me->outputBpp != 0)
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
		csconv->ConvertV2(&vsrcBuff, tstat->lrBuff, tstat->me->videoInfo.dispSize.x, tstat->me->videoInfo.dispSize.y, tstat->me->videoInfo.storeSize.x, tstat->me->videoInfo.storeSize.y, (IntOS)tstat->me->videoInfo.dispSize.x * 8, vbuff->frameType, vbuff->ycOfst);
		tstat->csTime = clk.GetTimeDiff();

		if (tstat->procType == 1)
		{
			if (vbuff->frameType == Media::FT_NON_INTERLACE)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
				cropWidth = srcWidth - tstat->me->cropLeft - tstat->me->cropRight;
				cropHeight = srcHeight - tstat->me->cropTop - tstat->me->cropBottom;
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, &rect);
				vbuff->destSize = rect.size;
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
				{
					vbuff->destSurface.Delete();
					vbuff->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
				}
				if (vbuff->destSurface.SetTo(destSurface))
				{
					IntOS destBpl;
					UnsafeArray<UInt8> destBuff;
					if (destSurface->LockSurface(destBpl).SetTo(destBuff))
					{
						tstat->dresizer->DeintResize(Media::DeinterlacingResizer::DT_FULL_FRAME, srcBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight), destBuff, (UIntOS)destBpl, vbuff->destSize.x, vbuff->destSize.y, false);
						destSurface->UnlockSurface();
					}
				}
			}
			else if (vbuff->frameType == Media::FT_FIELD_TF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y << 1;
				cropWidth = srcWidth - ((tstat->me->cropLeft + tstat->me->cropRight) << 1);
				cropHeight = srcHeight - ((tstat->me->cropTop + tstat->me->cropBottom) << 1);
				cropDY = tstat->me->cropTop << 1;
				sizeNeeded = srcWidth * srcHeight << 3;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, &rect);
				vbuff->destSize = rect.size;
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
				{
					vbuff->destSurface.Delete();
					vbuff->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
				}
				if (vbuff->destSurface.SetTo(destSurface))
				{
					IntOS destBpl;
					UnsafeArray<UInt8> destBuff;
					if (destSurface->LockSurface(destBpl).SetTo(destBuff))
					{
						tstat->dresizer->DeintResize(Media::DeinterlacingResizer::DT_TOP_FIELD, srcBuff + ((cropDY >> 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight >> 1), destBuff, (UIntOS)destBpl, vbuff->destSize.x, vbuff->destSize.y, false);
						destSurface->UnlockSurface();
					}
				}
			}
			else if (vbuff->frameType == Media::FT_FIELD_BF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y << 1;
				cropWidth = srcWidth - ((tstat->me->cropLeft + tstat->me->cropRight) << 1);
				cropHeight = srcHeight - ((tstat->me->cropTop + tstat->me->cropBottom) << 1);
				cropDY = tstat->me->cropTop << 1;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, &rect);
				vbuff->destSize = rect.size;
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
				{
					vbuff->destSurface.Delete();
					vbuff->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
				}
				if (vbuff->destSurface.SetTo(destSurface))
				{
					IntOS destBpl;
					UnsafeArray<UInt8> destBuff;
					if (destSurface->LockSurface(destBpl).SetTo(destBuff))
					{
						tstat->dresizer->DeintResize(Media::DeinterlacingResizer::DT_BOTTOM_FIELD, srcBuff + ((cropDY >> 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight >> 1), destBuff, (UIntOS)destBpl, vbuff->destSize.x, vbuff->destSize.y, false);
						destSurface->UnlockSurface();
					}
				}
			}
			else if (vbuff->frameType == Media::FT_MERGED_TF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, &rect);
				vbuff->destSize = rect.size;
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
				{
					vbuff->destSurface.Delete();
					vbuff->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
				}
				if (vbuff->destSurface.SetTo(destSurface))
				{
					IntOS destBpl;
					UnsafeArray<UInt8> destBuff;
					if (destSurface->LockSurface(destBpl).SetTo(destBuff))
					{
						tstat->dresizer->DeintResize(Media::DeinterlacingResizer::DT_TOP_FIELD, srcBuff + ((cropDY >> 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight >> 1), destBuff, (UIntOS)destBpl, vbuff->destSize.x, vbuff->destSize.y, false);
						destSurface->UnlockSurface();
					}
				}
			}
			else if (vbuff->frameType == Media::FT_MERGED_BF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, &rect);
				vbuff->destSize = rect.size;
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
				{
					vbuff->destSurface.Delete();
					vbuff->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
				}
				if (vbuff->destSurface.SetTo(destSurface))
				{
					IntOS destBpl;
					UnsafeArray<UInt8> destBuff;
					if (destSurface->LockSurface(destBpl).SetTo(destBuff))
					{
						tstat->dresizer->DeintResize(Media::DeinterlacingResizer::DT_BOTTOM_FIELD, srcBuff + ((cropDY >> 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight >> 1), destBuff, (UIntOS)destBpl, vbuff->destSize.x, vbuff->destSize.y, false);
						destSurface->UnlockSurface();
					}
				}
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_TFF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, &rect);
				vbuff->destSize = rect.size;
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
				{
					vbuff->destSurface.Delete();
					vbuff->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
				}
				if (vbuff->destSurface.SetTo(destSurface))
				{
					IntOS destBpl;
					UnsafeArray<UInt8> destBuff;
					if (destSurface->LockSurface(destBpl).SetTo(destBuff))
					{
						tstat->dresizer->DeintResize(Media::DeinterlacingResizer::DT_TOP_FIELD, srcBuff + ((cropDY >> 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 4, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight >> 1), destBuff, (UIntOS)destBpl, vbuff->destSize.x, vbuff->destSize.y, false);
						destSurface->UnlockSurface();
					}
				}
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_BFF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
				cropWidth = srcWidth - (tstat->me->cropLeft + tstat->me->cropRight);
				cropHeight = srcHeight - (tstat->me->cropTop + tstat->me->cropBottom);
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, &rect);
				vbuff->destSize = rect.size;
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
				{
					vbuff->destSurface.Delete();
					vbuff->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
				}
				if (vbuff->destSurface.SetTo(destSurface))
				{
					IntOS destBpl;
					UnsafeArray<UInt8> destBuff;
					if (destSurface->LockSurface(destBpl).SetTo(destBuff))
					{
						tstat->dresizer->DeintResize(Media::DeinterlacingResizer::DT_BOTTOM_FIELD, srcBuff + (((cropDY >> 1) + 1) * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 4, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight >> 1), destBuff, (UIntOS)destBpl, vbuff->destSize.x, vbuff->destSize.y, false);
						destSurface->UnlockSurface();
					}
				}
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_NODEINT)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
				cropWidth = srcWidth - tstat->me->cropLeft - tstat->me->cropRight;
				cropHeight = srcHeight - tstat->me->cropTop - tstat->me->cropBottom;
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;

				tstat->me->CalDisplayRect(cropWidth, cropHeight, &rect);
				vbuff->destSize = rect.size;
				vbuff->destBitDepth = tstat->resizerBitDepth;
				if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
				{
					vbuff->destSurface.Delete();
					vbuff->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
				}
				if (vbuff->destSurface.SetTo(destSurface))
				{
					IntOS destBpl;
					UnsafeArray<UInt8> destBuff;
					if (destSurface->LockSurface(destBpl).SetTo(destBuff))
					{
						tstat->dresizer->DeintResize(Media::DeinterlacingResizer::DT_FULL_FRAME, srcBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight), destBuff, (UIntOS)destBpl, vbuff->destSize.x, vbuff->destSize.y, false);
						destSurface->UnlockSurface();
					}
				}
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
				vbuff2->destSize = vbuff->destSize;
				vbuff2->destBitDepth = tstat->resizerBitDepth;
				if (!vbuff2->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff2->destSize))
				{
					vbuff2->destSurface.Delete();
					vbuff2->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff2->destSize, vbuff2->destBitDepth);
				}
				vbuff2->frameNum = vbuff->frameNum;
				vbuff2->frameTime = vbuff->frameTime + 16;

				Sync::MutexUsage mutUsage(tstat->me->buffMut);
				vbuff->isOutputReady = true;
				vbuff->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt.Set();

				if (vbuff2->destSurface.SetTo(destSurface))
				{
					IntOS destBpl;
					UnsafeArray<UInt8> destBuff;
					if (destSurface->LockSurface(destBpl).SetTo(destBuff))
					{
						if (vbuff2->frameType == Media::FT_INTERLACED_TFF)
						{
							tstat->dresizer->DeintResize(Media::DeinterlacingResizer::DT_BOTTOM_FIELD, tstat->lrBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight), destBuff, (UIntOS)destBpl, vbuff2->destSize.x, vbuff2->destSize.y, false);
						}
						else
						{
							tstat->dresizer->DeintResize(Media::DeinterlacingResizer::DT_TOP_FIELD, tstat->lrBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), srcWidth << 3, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight), destBuff, (UIntOS)destBpl, vbuff2->destSize.x, vbuff2->destSize.y, false);
						}
						destSurface->UnlockSurface();
					}
				}
				mutUsage.BeginUse();
				vbuff2->isOutputReady = true;
				vbuff2->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt.Set();
			}
			else
			{
				Sync::MutexUsage mutUsage(tstat->me->buffMut);
				vbuff->isOutputReady = true;
				vbuff->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt.Set();
			}

			tstat->hTime = ((Media::Resizer::DeintResizerLR_C32*)tstat->dresizer)->GetHAvgTime();
			tstat->vTime = ((Media::Resizer::DeintResizerLR_C32*)tstat->dresizer)->GetVAvgTime();
		}
		else
		{
			if (vbuff->frameType == Media::FT_NON_INTERLACE)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
				cropWidth = srcWidth - tstat->me->cropLeft - tstat->me->cropRight;
				cropHeight = srcHeight - tstat->me->cropTop - tstat->me->cropBottom;
				cropDY = tstat->me->cropTop;
				srcBuff = tstat->lrBuff;
			}
			else if (vbuff->frameType == Media::FT_FIELD_TF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y << 1;
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
				tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, (IntOS)srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_FIELD_BF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y << 1;
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
				tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 1, srcWidth, (IntOS)srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_MERGED_TF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
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
				tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, (IntOS)srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_MERGED_BF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
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
				tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 1, srcWidth, (IntOS)srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_TFF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
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
				tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, (IntOS)srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_BFF)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
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
				tstat->deint->Deinterlace(tstat->lrBuff + (srcWidth * 8), tstat->diBuff, 1, srcWidth, (IntOS)srcWidth << 3);
			}
			else if (vbuff->frameType == Media::FT_INTERLACED_NODEINT)
			{
				srcWidth = tstat->me->videoInfo.dispSize.x;
				srcHeight = tstat->me->videoInfo.dispSize.y;
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
			tstat->me->CalDisplayRect(cropWidth, cropHeight, &rect);
		//	tstat->me->VideoEndProc();
			vbuff->destSize = rect.size;
			vbuff->destBitDepth = tstat->resizerBitDepth;
			NN<Media::MonitorSurface> destSurface;
			if (!vbuff->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff->destSize))
			{
				vbuff->destSurface.Delete();
				vbuff->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff->destSize, vbuff->destBitDepth);
			}
			if (vbuff->destSurface.SetTo(destSurface))
			{
				IntOS destBpl;
				UnsafeArray<UInt8> destBuff;
				if (destSurface->LockSurface(destBpl).SetTo(destBuff))
				{
					tstat->resizer->Resize(srcBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), (IntOS)srcWidth << 3, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight), 0, 0, destBuff, destBpl, vbuff->destSize.x, vbuff->destSize.y);
					destSurface->UnlockSurface();
				}
			}

			if ((vbuff->frameType == Media::FT_INTERLACED_TFF || vbuff->frameType == Media::FT_INTERLACED_BFF) && vbuff2)
			{
				vbuff2->frameType = vbuff->frameType;
				vbuff2->destSize = vbuff->destSize;
				vbuff2->destBitDepth = tstat->resizerBitDepth;
				if (!vbuff2->destSurface.SetTo(destSurface) || !destSurface->IsDispSize(vbuff2->destSize))
				{
					vbuff2->destSurface.Delete();
					vbuff2->destSurface = tstat->me->surfaceMgr->CreateSurface(vbuff2->destSize, vbuff2->destBitDepth);
				}
				vbuff2->frameNum = vbuff->frameNum;
				vbuff2->frameTime = vbuff->frameTime + 16;

				Sync::MutexUsage mutUsage(tstat->me->buffMut);
				vbuff->isOutputReady = true;
				vbuff->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt.Set();

				if (vbuff2->frameType == Media::FT_INTERLACED_TFF)
				{
					tstat->deint->Deinterlace(tstat->lrBuff + (srcWidth * 8), tstat->diBuff, 1, srcWidth, (IntOS)srcWidth << 3);
				}
				else
				{
					tstat->deint->Deinterlace(tstat->lrBuff, tstat->diBuff, 0, srcWidth, (IntOS)srcWidth << 3);
				}
				if (vbuff2->destSurface.SetTo(destSurface))
				{
					IntOS destBpl;
					UnsafeArray<UInt8> destBuff;
					if (destSurface->LockSurface(destBpl).SetTo(destBuff))
					{
						tstat->resizer->Resize(tstat->diBuff + (cropDY * srcWidth << 3) + (tstat->me->cropLeft << 3), (IntOS)srcWidth << 3, UIntOS2Double(cropWidth), UIntOS2Double(cropHeight), 0, 0, destBuff, destBpl, vbuff2->destSize.x, vbuff2->destSize.y);
						destSurface->UnlockSurface();
					}
				}
				mutUsage.BeginUse();
				vbuff2->isOutputReady = true;
				vbuff2->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt.Set();
			}
			else
			{
				Sync::MutexUsage mutUsage(tstat->me->buffMut);
				vbuff->isOutputReady = true;
				vbuff->isProcessing = false;
				mutUsage.EndUse();
				tstat->me->dispEvt.Set();
			}
			tstat->hTime = 0;
			tstat->vTime = 0;
		}
	}
}

NN<Media::ImageResizer> Media::VideoRenderer::CreateResizer(NN<Media::ColorManagerSess> colorSess, UInt32 bitDepth, Double srcRefLuminance)
{
	NN<Media::ImageResizer> resizer;
	Media::ColorProfile destColor(Media::ColorProfile::CPT_VDISPLAY);
	if (bitDepth == 16)
	{
		NEW_CLASSNN(resizer, Media::Resizer::LanczosResizerLR_C16(4, 3, destColor, colorSess, Media::AT_IGNORE_ALPHA, srcRefLuminance));
	}
	else if (this->curr10Bit)
	{
		NEW_CLASSNN(resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, destColor, colorSess, Media::AT_IGNORE_ALPHA, srcRefLuminance, Media::PF_LE_A2B10G10R10));
	}
	else
	{
		NEW_CLASSNN(resizer, Media::Resizer::LanczosResizerLR_C32(4, 3, destColor, colorSess, Media::AT_IGNORE_ALPHA, srcRefLuminance, this->outputPf));
	}
	return resizer;
}

void Media::VideoRenderer::CreateCSConv(NN<ThreadStat> tstat, Media::FrameInfo *info)
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
	Media::ColorProfile color(Media::ColorProfile::CPT_VDISPLAY);
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

	if (tstat->procType == 2)
	{
		if (tstat->cs10Bit)
		{
			tstat->csconv = Media::CS::CSConverter::NewConverter(fcc, info->storeBPP, info->pf, info->color, 0, 32, Media::PF_LE_A2B10G10R10, color, yuvType, this->colorSess);
		}
		else
		{
			tstat->csconv = Media::CS::CSConverter::NewConverter(fcc, info->storeBPP, info->pf, info->color, 0, 32, Media::PF_B8G8R8A8, color, yuvType, this->colorSess);
		}
	}
	else
	{
		color.GetRTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
		color.GetGTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
		color.GetBTranParam()->Set(Media::CS::TRANT_LINEAR, 1.0);
		tstat->csconv = Media::CS::CSConverter::NewConverter(fcc, info->storeBPP, info->pf, info->color, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, color, yuvType, this->colorSess);
		if (info->dispSize.CalcArea() > tstat->lrSize)
		{
			tstat->lrSize = info->dispSize.CalcArea();
			if (tstat->lrBuff)
			{
				MemFreeA64(tstat->lrBuff);
			}
			tstat->lrBuff = MemAllocA64(UInt8, tstat->lrSize * 8);
		}
	}
}

void Media::VideoRenderer::CreateThreadResizer(NN<ThreadStat> tstat)
{
	SDEL_CLASS(tstat->resizer);
	SDEL_CLASS(tstat->dresizer);
	Media::ColorProfile destColor(Media::ColorProfile::CPT_VDISPLAY);

	if (this->outputBpp == 16)
	{
		NEW_CLASS(tstat->resizer, Media::Resizer::LanczosResizerLR_C16(4, 3, destColor, colorSess, Media::AT_IGNORE_ALPHA, tstat->resizerSrcRefLuminance));
		tstat->procType = 0;
		tstat->resizerBitDepth = 16;
	}
	else if (this->curr10Bit)
	{
		NEW_CLASS(tstat->dresizer, Media::Resizer::DeintResizerLR_C32(0, 0, destColor, colorSess, Media::AT_IGNORE_ALPHA, tstat->resizerSrcRefLuminance, Media::PF_LE_A2B10G10R10));
		tstat->procType = 1;
		tstat->resizerBitDepth = 32;
	}
	else
	{
		NEW_CLASS(tstat->dresizer, Media::Resizer::DeintResizerLR_C32(0, 0, destColor, colorSess, Media::AT_IGNORE_ALPHA, tstat->resizerSrcRefLuminance, this->outputPf));
		tstat->procType = 1;
		tstat->resizerBitDepth = 32;
	}
	tstat->resizer10Bit = this->curr10Bit;
}

void __stdcall Media::VideoRenderer::OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	NN<Media::VideoRenderer> me = userData.GetNN<Media::VideoRenderer>();
	if (me->ignoreFrameTime)
	{
		frameTime = MulDivU32(frameNum, me->frameRateDenorm * 1000, me->frameRateNorm);
	}
#if defined(VERBOSE)
	printf("OnVideoFrame, frameTime = %d\r\n", frameTime);
#endif

	UnsafeArray<UInt8> srcBuff;
	UIntOS i;
	UIntOS j;
	Int32 picCnt = Sync::Interlocked::IncrementI32(me->picCnt);
	Bool found = false;
	UIntOS frameIndex;
	if ((picCnt % 30) == 0)
	{
		flags = (Media::VideoSource::FrameFlag)(flags | Media::VideoSource::FF_FORCEDISP);
	}
	if (flags & Media::VideoSource::FF_DISCONTTIME)
	{
		Bool found = true;

		while (me->playing)
		{
			found = false;
			frameIndex = 0;
			me->dispMut.LockRead();
			while (frameIndex < me->allBuffCnt && me->playing)
			{
				if (!me->buffs[frameIndex].isProcessing)
				{
					me->buffs[frameIndex].isEmpty = true;
					me->buffs[frameIndex].isOutputReady = false;
					me->buffEvt.Set();
				}
				else
				{
					found = true;
				}

				frameIndex++;
			}
			me->dispMut.UnlockRead();
			if (!found)
			{
				break;
			}
			me->dispEvt.Wait(100);
		}
		NN<Media::RefClock> dispClk;
		me->dispMut.LockRead();
		if (me->dispClk.SetTo(dispClk)) dispClk->Start(frameTime.AddMS(-me->timeDelay - me->avOfst));
		me->dispMut.UnlockRead();
	}
	if (frameType == Media::FT_DISCARD)
		return;
	if (me->updatingSize)
		return;

	UIntOS buffCnt = me->buffCnt;
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
		me->buffEvt.Wait(10);
		if(!me->playing)
			return;
	}
	if (!me->buffs[frameIndex].srcBuff.SetTo(srcBuff))
	{
		mutUsage.EndUse();
		return;
	}

	i = 0;
	j = me->imgFilters.GetCount();
	while (i < j)
	{
		me->imgFilters.GetItemNoCheck(i)->ProcessImage(imgData[0], me->videoInfo.fourcc, me->videoInfo.storeBPP, me->videoInfo.pf, me->videoInfo.dispSize.x, me->videoInfo.dispSize.y, frameType, ycOfst);
		i++;
	}

	if (me->videoInfo.fourcc == FFMT_YUV444P10LE)
	{
		if (me->videoInfo.storeSize.x & 31)
		{
			MemCopyNANC(srcBuff.Ptr(), imgData[0].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
			MemCopyNANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y * 2, imgData[1].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
			MemCopyNANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y * 4, imgData[2].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
		}
		else
		{
			MemCopyANC(srcBuff.Ptr(), imgData[0].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
			MemCopyANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y * 2, imgData[1].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
			MemCopyANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y * 4, imgData[2].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
		}
		//			ImageUtil_YUV_Y416ShiftW(me->buffs[frameIndex].srcBuff, imgData[0], imgData[1], imgData[2], me->videoInfo.storeSize.x * me->videoInfo.dispSize.y, 0);
	}
	else if (me->videoInfo.fourcc == FFMT_YUV420P10LE)
	{
		ImageUtil_CopyShiftW(imgData[0].Ptr(), srcBuff.Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2, 6);
		ImageUtil_UVInterleaveShiftW(srcBuff.Ptr() + (me->videoInfo.storeSize.x * me->videoInfo.storeSize.y << 1), imgData[1].Ptr(), imgData[2].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y >> 2, 6);
	}
	else if (me->videoInfo.fourcc == FFMT_YUV420P12LE)
	{
		ImageUtil_CopyShiftW(imgData[0].Ptr(), srcBuff.Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2, 4);
		ImageUtil_UVInterleaveShiftW(srcBuff.Ptr() + (me->videoInfo.storeSize.x * me->videoInfo.storeSize.y << 1), imgData[1].Ptr(), imgData[2].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y >> 2, 4);
	}
	else if (me->videoInfo.fourcc == FFMT_YUV420P8)
	{
		MemCopyNANC(srcBuff.Ptr(), imgData[0].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y);
		MemCopyNANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y, imgData[2].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y >> 2);
		MemCopyNANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y + (me->videoInfo.storeSize.x * me->videoInfo.storeSize.y >> 2), imgData[1].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y >> 2);
	}
	else
	{
		MemCopyANC(srcBuff.Ptr(), imgData[0].Ptr(), dataSize);
	}
	me->buffs[frameIndex].frameTime = frameTime;
	me->buffs[frameIndex].frameNum = frameNum;
	me->buffs[frameIndex].discontTime = (flags & Media::VideoSource::FF_DISCONTTIME) != 0;
	me->buffs[frameIndex].flags = flags;
	Media::VideoRenderer::DeinterlaceType deintType = me->deintType;
	if (!me->manualDeint && me->refRate != 0 && me->refRate <= 30)
	{
		deintType = Media::VideoRenderer::DT_30P_MODE;
	}
	
	if (deintType == Media::VideoRenderer::DT_FROM_VIDEO)
	{
		me->buffs[frameIndex].frameType = frameType;
	}
	else if (deintType == Media::VideoRenderer::DT_PROGRESSIVE)
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
	else if (deintType == Media::VideoRenderer::DT_INTERLACED_TFF)
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
	else if (deintType == Media::VideoRenderer::DT_INTERLACED_BFF)
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
	else if (deintType == Media::VideoRenderer::DT_30P_MODE)
	{
		if ((flags & Media::VideoSource::FF_DISCONTTIME) == 0 && me->lastFrameTime < frameTime && me->lastFrameTime + 30 > frameTime)
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
			me->buffEvt.Wait(10);
			if(!me->playing)
				return;
		}
		if (!me->buffs[frameIndex].srcBuff.SetTo(srcBuff))
		{
			mutUsage.EndUse();
			return;
		}

		if (me->videoInfo.fourcc == FFMT_YUV444P10LE)
		{
			if (me->videoInfo.storeSize.x & 31)
			{
				MemCopyNANC(srcBuff.Ptr(), imgData[0].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
				MemCopyNANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y * 2, imgData[1].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
				MemCopyNANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y * 4, imgData[2].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
			}
			else
			{
				MemCopyANC(srcBuff.Ptr(), imgData[0].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
				MemCopyANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y * 2, imgData[1].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
				MemCopyANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y * 4, imgData[2].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2);
			}
//			ImageUtil_YUV_Y416ShiftW(me->buffs[frameIndex].srcBuff, imgData[0], imgData[1], imgData[2], me->videoInfo.storeSize.x * me->videoInfo.dispSize.y, 0);
		}
		else if (me->videoInfo.fourcc == FFMT_YUV420P10LE)
		{
			ImageUtil_CopyShiftW(imgData[0].Ptr(), srcBuff.Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2, 6);
			ImageUtil_UVInterleaveShiftW(srcBuff.Ptr() + (me->videoInfo.storeSize.x * me->videoInfo.storeSize.y << 1), imgData[1].Ptr(), imgData[2].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y >> 2, 6);
		}
		else if (me->videoInfo.fourcc == FFMT_YUV420P12LE)
		{
			ImageUtil_CopyShiftW(imgData[0].Ptr(), srcBuff.Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y * 2, 4);
			ImageUtil_UVInterleaveShiftW(srcBuff.Ptr() + (me->videoInfo.storeSize.x * me->videoInfo.storeSize.y << 1), imgData[1].Ptr(), imgData[2].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y >> 2, 4);
		}
		else if (me->videoInfo.fourcc == FFMT_YUV420P8)
		{
			MemCopyNANC(srcBuff.Ptr(), imgData[0].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y);
			MemCopyNANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y, imgData[2].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y >> 2);
			MemCopyNANC(srcBuff.Ptr() + me->videoInfo.storeSize.x * me->videoInfo.storeSize.y + (me->videoInfo.storeSize.x * me->videoInfo.storeSize.y >> 2), imgData[1].Ptr(), me->videoInfo.storeSize.x * me->videoInfo.dispSize.y >> 2);
		}
		else
		{
			MemCopyANC(srcBuff.Ptr(), imgData[0].Ptr(), dataSize);
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

void __stdcall Media::VideoRenderer::OnVideoChange(Media::VideoSource::FrameChange fc, AnyType userData)
{
	NN<Media::VideoRenderer> me = userData.GetNN<Media::VideoRenderer>();
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UIntOS frameSize;
	if (fc == Media::VideoSource::FC_PAR)
	{
		NN<Media::VideoSource> video;
		me->VideoBeginProc();
		if (me->video.SetTo(video))
		{
			video->GetVideoInfo(me->videoInfo, frameRateNorm, frameRateDenorm, frameSize);
			me->frameRateNorm = frameRateNorm;
			me->frameRateDenorm = frameRateDenorm;
		}
		me->VideoEndProc();
	}
	else if (fc == Media::VideoSource::FC_ENDPLAY)
	{
		me->StopPlay();
		if (me->endHdlr)
		{
			me->endHdlr(me->endHdlrObj);
		}
	}
	else if (fc == Media::VideoSource::FC_CROP)
	{
		me->UpdateCrop();
	}
}

UInt32 __stdcall Media::VideoRenderer::ProcessThread(AnyType userObj)
{
	NN<ThreadStat> tstat = userObj.GetNN<ThreadStat>();
	Sync::ThreadUtil::SetName(CSTR("VideoRendererP"));
	UIntOS i;
	VideoBuff *buff = 0;
	VideoBuff *buff2;
	Bool found;
	Bool hasNew;
	UIntOS minIndex = 0;
	Data::Duration minTime = 0;
	UInt32 minFrame = 0;

	tstat->status = 1;
	while (!tstat->me->threadToStop)
	{
		found = false;
		hasNew = false;
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
					hasNew = true;
				}
				else if (tstat->me->buffs[i].frameNum == minFrame && tstat->me->buffs[i].frameTime < minTime)
				{
					minIndex = i;
					minTime = tstat->me->buffs[i].frameTime;
					minFrame = tstat->me->buffs[i].frameNum;
					hasNew = true;
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
					tstat->me->buffEvt.Wait(10);
				}
			}
			else
			{
				buff2 = 0;
			}

			Data::Duration currTime = 0;
			Bool toSkip = false;
			NN<Media::RefClock> dispClk;

			tstat->me->dispMut.LockRead();
			if (tstat->me->dispClk.SetTo(dispClk))
			{
				if (!dispClk->Running() || buff->discontTime)
				{
					dispClk->Start(buff->frameTime.AddMS(-tstat->me->timeDelay - tstat->me->avOfst));
					currTime = buff->frameTime;
				}
				else
				{
					Int32 procDelay = tstat->me->CalProcDelay();
					Int32 dispDelay = tstat->me->CalDispDelay();
					currTime = dispClk->GetCurrTime().AddMS(tstat->me->timeDelay + tstat->me->avOfst + procDelay + dispDelay + PREPROCTIME);
					if (buff->flags & Media::VideoSource::FF_FORCEDISP)
					{
					}
					else if (currTime > buff->frameTime && ((buff->flags & Media::VideoSource::FF_REALTIME) == 0 || tstat->me->hasAudio || hasNew))
					{
						toSkip = true;
					}
				}
			}
			else
			{
				toSkip = true;;
			}
			tstat->me->dispMut.UnlockRead();

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
				tstat->me->buffEvt.Set();
				Sync::Interlocked::IncrementU32(tstat->me->frameSkipBeforeProc);
			}
			else
			{
				Manage::HiResClock clk;
				Double t;
				clk.Start();
				tstat->me->VideoBeginProc();
				if (buff->srcBuff.NotNull())
				{
					tstat->me->ProcessVideo(tstat, buff, buff2);
				}
				tstat->me->VideoEndProc();
				t = clk.GetTimeDiff();
				tstat->me->procMut.LockWrite();
				tstat->me->procDelayBuff[tstat->me->procThisCount & (PROCDELAYBUFF - 1)] = t;
				tstat->me->procThisCount++;
				tstat->me->procCnt++;
				tstat->me->procMut.UnlockWrite();
			}
			tstat->me->dispEvt.Set();
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

UInt32 __stdcall Media::VideoRenderer::DisplayThread(AnyType userObj)
{
	NN<Media::VideoRenderer> me = userObj.GetNN<Media::VideoRenderer>();
	Sync::ThreadUtil::SetName(CSTR("VideoRendererD"));
	UIntOS i;
	Bool found;
	Bool found2;
	UInt32 minFrame = 0;
	UInt32 minFrame2 = 0;
	Data::Duration minTime = 0;
	Data::Duration minTime2 = 0;
	UIntOS lastW = 0;
	UIntOS lastH = 0;
	Bool toClear;
	UIntOS minIndex = 0;
	NN<Media::RefClock> dispClk;
	me->dispRunning = true;
	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_HIGHEST);
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
				me->dispMut.LockRead();
				if (me->dispClk.SetTo(dispClk))
				{
					if (me->buffs[minIndex].isOutputReady)
					{
						Data::Duration t;
//						Int32 dispDelay = me->CalDispDelay();
						Bool skipFrame = false;;
						t = dispClk->GetCurrTime().AddMS(me->timeDelay + me->avOfst);
						if (found2 && (me->buffs[minIndex].flags & Media::VideoSource::FF_FORCEDISP) == 0)
						{
							if (t > minTime2.AddMS(17) && ((me->buffs[minIndex].flags & Media::VideoSource::FF_REALTIME) == 0 || me->hasAudio))
							{
								skipFrame = true;
							}
						}

						if (skipFrame)
						{
							me->buffs[minIndex].isEmpty = true;
							me->buffEvt.Set();
							me->dispMut.UnlockRead();

							me->frameSkipAfterProc++;
						}
						else
						{
							Manage::HiResClock clk;
							Double dispT;
							Media::VideoSource::FrameFlag flags = me->buffs[minIndex].flags;
							Data::Duration frameTime = me->buffs[minIndex].frameTime;
							UInt32 frameNum = me->buffs[minIndex].frameNum;
							////////////////////////////////
							
							if ((flags & Media::VideoSource::FF_REALTIME) && !me->hasAudio)
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
									dispJitter = 1000 / (Int32)me->refRate;
								}
								while ((t = dispClk->GetCurrTime().AddMS(me->timeDelay + me->avOfst + dispJitter)) < frameTime)
								{
									if ((frameTime - t).GetTotalMS() > 3000 && me->videoDelay.GetTotalMS() > -3000)
										break;
									me->dispEvt.Wait(frameTime - t);
									if (me->dispToStop || !me->playing)
										break;
								}
							}
							Data::Duration startTime = dispClk->GetCurrTime();
							me->videoDelay = (startTime - frameTime);
							me->dispMut.UnlockRead();
							clk.Start();
							me->VideoBeginProc();
							Sync::MutexUsage mutUsage;
							me->LockUpdateSize(mutUsage);

							DrawRect rect;
							UIntOS vwidth = me->videoInfo.dispSize.x - me->cropLeft - me->cropRight;
							UIntOS vheight = me->videoInfo.dispSize.y - me->cropTop - me->cropBottom;
							if (me->videoInfo.ftype == Media::FT_FIELD_BF || me->videoInfo.ftype == Media::FT_FIELD_TF)
							{
								vheight = vheight << 1;
							}
							me->CalDisplayRect(vwidth, vheight, &rect);
							toClear = false;
							if (rect.size == me->buffs[minIndex].destSize)
							{
								if (rect.size.x != lastW || rect.size.y != lastH || me->toClear)
								{
									toClear = true;
									lastW = rect.size.x;
									lastH = rect.size.y;
									me->toClear = false;
								}
								NN<Media::MonitorSurface> surface;
								if (me->buffs[minIndex].destSurface.SetTo(surface))
									me->DrawFromSurface(surface, rect.tl, rect.size, toClear);
							}
							me->buffs[minIndex].isEmpty = true;
							me->buffEvt.Set();

							me->dispFrameTime = frameTime;
							me->dispFrameNum = frameNum;
							mutUsage.EndUse();
							me->VideoEndProc();
							dispT = clk.GetTimeDiff();
							me->dispDelayMut.LockWrite();
							me->dispDelayBuff[me->dispCnt & (DISPDELAYBUFF - 1)] = dispT;
							me->dispJitterBuff[me->dispCnt & (DISPDELAYBUFF - 1)] = startTime.AddSecDbl(dispT) - frameTime.AddMS(me->timeDelay + me->avOfst);
							me->dispCnt++;
							me->dispDelayMut.UnlockWrite();
							
							me->frameDispCnt++;
						}
					}
					else
					{
						me->dispMut.UnlockRead();
						me->dispEvt.Wait(10);
					}
				}
				else
				{
					i = me->allBuffCnt;
					while (i-- > 0)
					{
						me->buffs[i].isEmpty = true;
					}
					me->dispMut.UnlockRead();
					me->buffEvt.Set();
				}
			}
			else
			{
				me->dispEvt.Wait(1000);
			}
		}
		else
		{
			me->dispEvt.Wait(1000);
		}
	}
	me->dispRunning = false;
	return 0;
}

void Media::VideoRenderer::StopThreads()
{
	Bool found;
	UIntOS i;
	this->threadToStop = true;
	if (this->playing)
	{
		SetVideo(nullptr);
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->tstats[i].evt->Set();
	}
	this->dispToStop = true;
	this->dispEvt.Set();
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
		Sync::SimpleThread::Sleep(10);
	}
	while (this->dispRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

void Media::VideoRenderer::VideoBeginLoad()
{
	this->videoPause = true;
	while (this->videoProcCnt > 0)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

void Media::VideoRenderer::VideoEndLoad()
{
	this->videoPause = false;
}

void Media::VideoRenderer::VideoBeginProc()
{
	while (this->videoPause)
	{
		Sync::SimpleThread::Sleep(10);
	}
	Sync::Interlocked::IncrementI32(this->videoProcCnt);
}

void Media::VideoRenderer::VideoEndProc()
{
	Sync::Interlocked::DecrementI32(this->videoProcCnt);
}

Int32 Media::VideoRenderer::CalProcDelay()
{
	Int32 procDelay;
	Double totalTime;
	IntOS i;
	this->procMut.LockRead();
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
		procDelay = Double2Int32(1000 * totalTime / this->procThisCount);
	}
	else
	{
		totalTime = 0;
		i = PROCDELAYBUFF;
		while (i-- > 0)
		{
			totalTime += this->procDelayBuff[i];
		}
		procDelay = Double2Int32(1000 * totalTime / PROCDELAYBUFF);
	}
	this->procMut.UnlockRead();
	return procDelay;
}

Int32 Media::VideoRenderer::CalDispDelay()
{
	Int32 dispDelay;
	Double totalTime;
	IntOS i;
//	return 0;
	this->dispDelayMut.LockRead();
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
		dispDelay = Double2Int32(1000 * totalTime / this->dispCnt);
	}
	else
	{
		totalTime = 0;
		i = DISPDELAYBUFF;
		while (i-- > 0)
		{
			totalTime += this->dispDelayBuff[i];
		}
		dispDelay = Double2Int32(1000 * totalTime / DISPDELAYBUFF);
	}
	this->dispDelayMut.UnlockRead();
	return dispDelay;
}

Int32 Media::VideoRenderer::CalDispJitter()
{
	Int32 dispJitter;
	Data::Duration totalJitter;
	IntOS i;
	this->dispDelayMut.LockRead();
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
			totalJitter = totalJitter + this->dispJitterBuff[i];
		}
		dispJitter = (Int32)(totalJitter.GetTotalMS() / this->dispCnt);
	}
	else
	{
		totalJitter = 0;
		i = DISPDELAYBUFF;
		while (i-- > 0)
		{
			totalJitter = totalJitter + this->dispJitterBuff[i];
		}
		dispJitter = (Int32)(totalJitter.GetTotalMS() / DISPDELAYBUFF);
	}
	this->dispDelayMut.UnlockRead();
	return dispJitter;
}

void Media::VideoRenderer::UpdateRefreshRate(UInt32 refRate)
{
	this->refRate = refRate;
}

void Media::VideoRenderer::UpdateOutputSize(Math::Size2D<UIntOS> outputSize)
{
	this->outputSize = outputSize;
}

void Media::VideoRenderer::UpdateDispInfo(Math::Size2D<UIntOS> outputSize, UInt32 bpp, Media::PixelFormat pf)
{
	this->outputSize = outputSize;
	this->outputBpp = bpp;
	this->outputPf = pf;
}

Media::VideoRenderer::VideoRenderer(NN<Media::ColorManagerSess> colorSess, NN<Media::MonitorSurfaceMgr> surfaceMgr, UIntOS buffCnt, UIntOS threadCnt) : srcColor(Media::ColorProfile::CPT_VUNKNOWN), ivtc(0), uvOfst(0), autoCrop(0)
{
	UIntOS i;
	this->colorSess = colorSess;
	this->surfaceMgr = surfaceMgr;
	this->refRate = 0;
	this->outputSize = Math::Size2D<UIntOS>(0, 0);
	this->outputBpp = 0;
	this->outputPf = Media::PF_UNKNOWN;
	this->manualDeint = false;
	this->video = nullptr;
	this->hasAudio = false;
	this->frameDispCnt = 0;
	this->frameSkipBeforeProc = 0;
	this->frameSkipAfterProc = 0;
	this->picCnt = 0;
	this->dispCnt = 0;
	this->endHdlr = 0;
	this->videoDelay = 0;
	this->dispFrameTime = 0;
	this->dispFrameNum = 0;
	this->updatingSize = false;
	this->srcYUVType = Media::ColorProfile::YUVT_UNKNOWN;
	this->videoInfo.Clear();
	this->frameRateDenorm = 1;
	this->frameRateNorm = 30;
	this->ignoreFrameTime = false;
	this->forcePAR = 0;
	this->monPAR = 1.0;
	this->deintType = DT_FROM_VIDEO;
	this->curr10Bit = false;
	this->currSrcRefLuminance = 0;
	this->toClear = false;
	this->ivtc.SetEnabled(false);
	this->autoCrop.SetEnabled(false);
	this->cropLeft = 0;
	this->cropTop = 0;
	this->cropRight = 0;
	this->cropBottom = 0;
	this->dispClk = nullptr;

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
	this->videoPause = false;
	this->videoProcCnt = 0;
	this->avOfst = 0;

	this->procDelayBuff = MemAlloc(Double, PROCDELAYBUFF);
	this->procThisCount = 0;
	this->procCnt = 0;

	this->dispDelayBuff = MemAlloc(Double, DISPDELAYBUFF);
	this->dispJitterBuff = MemAlloc(Data::Duration, DISPDELAYBUFF);

	this->buffs = MemAlloc(VideoBuff, this->allBuffCnt);
	i = allBuffCnt;
	while (i-- > 0)
	{
		this->buffs[i].isEmpty = true;
		this->buffs[i].isProcessing = false;
		this->buffs[i].isOutputReady = false;
		this->buffs[i].srcBuff = nullptr;
		this->buffs[i].frameTime = 0;
		this->buffs[i].discontTime = false;
		this->buffs[i].destSurface = nullptr;
		this->buffs[i].destSize = Math::Size2D<UIntOS>(0, 0);
	}
	this->tstats = MemAlloc(ThreadStat, this->threadCnt);
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->tstats[i].status = 0;
		this->tstats[i].resizer = 0;
		this->tstats[i].dresizer = 0;
		this->tstats[i].procType = 0;
		this->tstats[i].csconv = nullptr;
		this->tstats[i].lrBuff = 0;
		this->tstats[i].lrSize = 0;
		this->tstats[i].diBuff = 0;
		this->tstats[i].diSize = 0;
		this->tstats[i].deint = 0;
		this->tstats[i].hTime = 0;
		this->tstats[i].vTime = 0;
		this->tstats[i].csTime = 0;
		this->tstats[i].resizerBitDepth = 0;
		this->tstats[i].resizerSrcRefLuminance = 0;
		this->tstats[i].resizer10Bit = false;
		this->tstats[i].me = this;
		NEW_CLASS(this->tstats[i].evt, Sync::Event(true));
		Sync::ThreadUtil::Create(ProcessThread, &this->tstats[i]);
	}

	this->dispToStop = false;
	this->dispRunning = false;
	this->dispForceUpdate = true;
	Sync::ThreadUtil::Create(DisplayThread, this);
	while (!this->dispRunning)
	{
		Sync::SimpleThread::Sleep(10);
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
		Sync::SimpleThread::Sleep(10);
	}
}

Media::VideoRenderer::~VideoRenderer()
{
	UIntOS i;
	this->updatingSize = true;
	this->StopThreads();

	i = this->threadCnt;
	while (i-- > 0)
	{
		SDEL_CLASS(this->tstats[i].resizer);
		SDEL_CLASS(this->tstats[i].dresizer);
		this->tstats[i].csconv.Delete();
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

	UnsafeArray<UInt8> srcBuff;
	i = this->allBuffCnt;
	while (i-- > 0)
	{
		if (buffs[i].srcBuff.SetTo(srcBuff))
		{
			MemFreeAArr(srcBuff);
		}
		buffs[i].destSurface.Delete();
	}
	this->imgFilters.DeleteAll();

	MemFree(this->procDelayBuff);
	MemFree(this->dispDelayBuff);
	MemFree(this->dispJitterBuff);
	MemFree(this->buffs);
}


void Media::VideoRenderer::SetVideo(Optional<Media::VideoSource> video)
{
	NN<Media::VideoSource> nnvideo;
	UIntOS i;
	UnsafeArray<UInt8> srcBuff;
	///////////////////////////////////////
	this->playing = false;
	this->dispMut.LockWrite();
	this->dispClk = nullptr;
	this->dispMut.UnlockWrite();

	this->VideoBeginLoad();
	this->avOfst = 0;
	if (this->video.NotNull())
	{
		this->uvOfst.Stop();
	}

	this->video = video;
	if (this->video.SetTo(nnvideo))
	{
		this->ivtc.SetSourceVideo(nnvideo);
		this->autoCrop.SetSourceVideo(&this->ivtc);
		this->uvOfst.SetSourceVideo(&this->autoCrop);
	}
	else
	{
		this->ivtc.SetSourceVideo(nullptr);
		this->autoCrop.SetSourceVideo(nullptr);
		this->uvOfst.SetSourceVideo(nullptr);
	}
//	this->forseFT = false;
	i = this->allBuffCnt;
	while (i-- > 0)
	{
		if (this->buffs[i].srcBuff.SetTo(srcBuff))
		{
			MemFreeAArr(srcBuff);
		}
		this->buffs[i].srcBuff = nullptr;
	}
	this->VideoEndLoad();

	this->procThisCount = 0;
	if (this->video.SetTo(nnvideo))
	{
		Media::FrameInfo info;
		UInt32 frameRateNorm;
		UInt32 frameRateDenorm;
		UIntOS frameSize;
 		if (!nnvideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, frameSize))
		{
			this->video = nullptr;
			this->ivtc.SetSourceVideo(nullptr);
			this->autoCrop.SetSourceVideo(nullptr);
			this->uvOfst.SetSourceVideo(nullptr);
			return;
		}

		this->VideoBeginLoad();
		this->videoInfo.Set(info);
		this->frameRateNorm = frameRateNorm;
		this->frameRateDenorm = frameRateDenorm;
		if (info.fourcc == FFMT_YUV444P10LE)
		{
			frameSize = info.storeSize.CalcArea() * 8;
		}
		i = this->allBuffCnt;
		while (i-- > 0)
		{
			this->buffs[i].srcBuff = MemAllocA64(UInt8, frameSize);
		}

		this->currSrcRefLuminance = Media::CS::TransferFunc::GetRefLuminance(info.color.rtransfer);
		this->srcColor.Set(info.color);
		this->srcYUVType = info.yuvType;
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->CreateThreadResizer(this->tstats[i]);
			this->CreateCSConv(this->tstats[i], &info);
		}
		this->VideoEndLoad();
		this->UpdateCrop();
	}
}

void Media::VideoRenderer::SetHasAudio(Bool hasAudio)
{
	this->hasAudio = hasAudio;
}

void Media::VideoRenderer::SetTimeDelay(Int32 timeDelay)
{
	this->timeDelay = timeDelay;
}

void Media::VideoRenderer::VideoInit(NN<Media::RefClock> clk)
{
	if (this->tstats[0].csconv.NotNull() && !this->playing)
	{
		if (this->video.NotNull())
		{
			this->dispClk = clk;
			//this->video->Init(OnVideoFrame, OnVideoChange, this);
			this->uvOfst.Init(OnVideoFrame, OnVideoChange, this);
		}
		this->ClearBuff();
	}
}

void Media::VideoRenderer::VideoStart()
{
	if (this->tstats[0].csconv.NotNull() && !this->playing)
	{
		this->ClearBuff();
		if (this->video.NotNull())
		{
			if (this->uvOfst.Start())
			{
				this->captureFrame = false;
				this->playing = true;
			}
		}
	}
}

void Media::VideoRenderer::StopPlay()
{
	if (this->video.NotNull())
	{
		this->playing = false;
		this->uvOfst.Stop();
		this->dispEvt.Set();

		this->ClearBuff();
		this->dispMut.LockWrite();
		this->dispClk = nullptr;
		this->dispMut.UnlockWrite();
	}
}

void Media::VideoRenderer::UpdateCrop()
{
	NN<Media::VideoSource> video;
	UIntOS cropLeft;
	UIntOS cropTop;
	UIntOS cropRight;
	UIntOS cropBottom;
	if (!this->video.SetTo(video))
		return;
	video->GetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
	this->cropLeft = cropLeft;
	this->cropTop = cropTop;
	this->cropRight = cropRight;
	this->cropBottom = cropBottom;
}

/*void Media::VideoRenderer::SetFrameType(Bool forse, Media::FrameType frameType)
{
	this->forseFTVal = frameType;
	this->forseFT = forse;
}*/

void Media::VideoRenderer::SetDeintType(Media::VideoRenderer::DeinterlaceType deintType)
{
	this->deintType = deintType;
	this->manualDeint = true;
}

void Media::VideoRenderer::ClearBuff()
{
	UIntOS i;
	i = this->allBuffCnt;
	while (i-- > 0)
	{
		if (!this->buffs[i].isEmpty)
		{
			this->buffs[i].isEmpty = true;
		}
	}
}

void Media::VideoRenderer::SetEndNotify(EndNotifier endHdlr, AnyType userObj)
{
	this->endHdlr = endHdlr;
	this->endHdlrObj = userObj;
}


void Media::VideoRenderer::SetSrcRGBType(Media::CS::TransferType rgbType)
{
	UIntOS i;
	this->VideoBeginLoad();
	this->videoInfo.color.GetRTranParam()->Set(rgbType, 2.2);
	this->videoInfo.color.GetGTranParam()->Set(rgbType, 2.2);
	this->videoInfo.color.GetBTranParam()->Set(rgbType, 2.2);
	this->srcColor.GetRTranParam()->Set(rgbType, 2.2);
	this->srcColor.GetGTranParam()->Set(rgbType, 2.2);
	this->srcColor.GetBTranParam()->Set(rgbType, 2.2);
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(this->tstats[i], &this->videoInfo);
	}
	this->VideoEndLoad();
}

void Media::VideoRenderer::SetSrcRGBTransfer(NN<const Media::CS::TransferParam> transf)
{
	UIntOS i;
	this->VideoBeginLoad();
	this->videoInfo.color.GetRTranParam()->Set(transf);
	this->videoInfo.color.GetGTranParam()->Set(transf);
	this->videoInfo.color.GetBTranParam()->Set(transf);
	this->srcColor.GetRTranParam()->Set(transf);
	this->srcColor.GetGTranParam()->Set(transf);
	this->srcColor.GetBTranParam()->Set(transf);
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(this->tstats[i], &this->videoInfo);
	}
	this->VideoEndLoad();
}

void Media::VideoRenderer::SetSrcPrimaries(Media::ColorProfile::ColorType colorType)
{
	UIntOS i;
	this->VideoBeginLoad();
	if (colorType == Media::ColorProfile::CT_VUNKNOWN)
	{
		this->videoInfo.color.GetPrimaries()->Set(this->srcColor.GetPrimaries());
	}
	else
	{
		this->videoInfo.color.GetPrimaries()->SetColorType(colorType);
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(this->tstats[i], &this->videoInfo);
	}
	this->VideoEndLoad();
}

void Media::VideoRenderer::SetSrcPrimaries(NN<const Media::ColorProfile::ColorPrimaries> primaries)
{
	UIntOS i;
	this->VideoBeginLoad();
	this->videoInfo.color.GetPrimaries()->Set(primaries);
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(this->tstats[i], &this->videoInfo);
	}
	this->VideoEndLoad();
}

void Media::VideoRenderer::SetSrcWP(Media::ColorProfile::WhitePointType wpType)
{
	UIntOS i;
	this->VideoBeginLoad();
	this->videoInfo.color.GetPrimaries()->SetWhiteType(wpType);
	this->videoInfo.color.GetPrimaries()->colorType = Media::ColorProfile::CT_CUSTOM;
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(this->tstats[i], &this->videoInfo);
	}
	this->VideoEndLoad();
}

void Media::VideoRenderer::SetSrcWPTemp(Double colorTemp)
{
	UIntOS i;
	this->VideoBeginLoad();
	this->videoInfo.color.GetPrimaries()->SetWhiteTemp(colorTemp);
	this->videoInfo.color.GetPrimaries()->colorType = Media::ColorProfile::CT_CUSTOM;
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(this->tstats[i], &this->videoInfo);
	}
	this->VideoEndLoad();
}

void Media::VideoRenderer::SetSrcYUVType(Media::ColorProfile::YUVType yuvType)
{
	UIntOS i;
	this->VideoBeginLoad();
	this->videoInfo.yuvType = yuvType;
	this->srcYUVType = yuvType;
	i = this->threadCnt;
	while (i-- > 0)
	{
		CreateCSConv(this->tstats[i], &this->videoInfo);
	}
	this->VideoEndLoad();
}

void Media::VideoRenderer::SetSrcPAR(Double forcePAR)
{
	this->forcePAR = forcePAR;
}

void Media::VideoRenderer::SetMonPAR(Double forcePAR)
{
	this->monPAR = forcePAR;
}

void Media::VideoRenderer::SetIVTCEnable(Bool enableIVTC)
{
	this->ivtc.SetEnabled(enableIVTC);
}

void Media::VideoRenderer::SetUVOfst(Int32 uOfst, Int32 vOfst)
{
	this->uvOfst.SetOffset(uOfst, vOfst);
}

void Media::VideoRenderer::SetAutoCropEnable(Bool enableCrop)
{
	this->autoCrop.SetEnabled(enableCrop);
}

Int32 Media::VideoRenderer::GetAVOfst()
{
	return this->avOfst;
}

void Media::VideoRenderer::SetAVOfst(Int32 avOfst)
{
	this->avOfst = avOfst;
}

void Media::VideoRenderer::SetIgnoreFrameTime(Bool ignoreFrameTime)
{
	this->ignoreFrameTime = ignoreFrameTime;
}

void Media::VideoRenderer::AddImgFilter(NN<Media::ImageFilter> imgFilter)
{
	this->imgFilters.Add(imgFilter);
}

void Media::VideoRenderer::Snapshot()
{
	if (this->playing)
	{
		this->captureFrame = true;
	}
}

void Media::VideoRenderer::GetStatus(NN<RendererStatus2> status)
{
	NN<Media::RefClock> dispClk;
	NN<Media::VideoSource> video;
	this->dispMut.LockRead();
	if (this->dispClk.SetTo(dispClk) && dispClk->Running())
	{
		status->currTime = dispClk->GetCurrTime();
	}
	else
	{
		status->currTime = 0;
	}
	this->dispMut.UnlockRead();
	status->dispDelay = this->CalDispDelay();
	status->procDelay = this->CalProcDelay();
	status->dispJitter = this->CalDispJitter();
	status->videoDelay = this->videoDelay;
	status->avOfst = this->avOfst;
	status->dispFrameTime = this->dispFrameTime;
	status->frameDispCnt = this->frameDispCnt;
	status->frameSkipBefore = this->frameSkipBeforeProc;
	status->frameSkipAfter = this->frameSkipAfterProc;
	status->srcDelay = this->timeDelay;
	status->dispFrameNum = this->dispFrameNum;
	status->srcYUVType = this->srcYUVType;
	status->color.Set(this->videoInfo.color);
	status->dispBitDepth = this->outputBpp;
	status->rotateType = this->GetRotateType();
	status->srcSize = this->videoInfo.dispSize;
	status->dispSize = this->outputSize;
	if (this->video.SetTo(video))
	{
		status->decoderName = video->GetFilterName();
	}
	else
	{
		status->decoderName = nullptr;
	}
	if (this->video.SetTo(video))
	{
		status->seekCnt = video->GetDataSeekCount();
	}
	else
	{
		status->seekCnt = 0;
	}
	if (this->forcePAR)
	{
		status->par = this->forcePAR / this->monPAR;
	}
	else
	{
		status->par = this->videoInfo.CalcPAR() / this->monPAR;
	}
	UIntOS i;
	status->format = this->videoInfo.fourcc;
	status->buffProc = 0;
	status->buffReady = 0;
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
	status->hTime = hTime / UIntOS2Double(this->threadCnt);
	status->vTime = vTime / UIntOS2Double(this->threadCnt);
	status->csTime = csTime / UIntOS2Double(this->threadCnt);

	Sync::MutexUsage mutUsage(this->buffMut);
	i = 0;
	while (i < this->allBuffCnt)
	{
		if (this->buffs[i].isEmpty)
		{
		}
		else if (this->buffs[i].isOutputReady)
		{
			status->buffReady++;
		}
		else
		{
			status->buffProc++;
		}
		i++;
	}
	mutUsage.EndUse();
}

NN<Media::MonitorSurfaceMgr> Media::VideoRenderer::GetSurfaceMgr()
{
	return this->surfaceMgr;
}