#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSI420_LRGB.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void CSI420_LRGB_do_yv12rgb(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, IntOS width, IntOS height, IntOS dbpl, IntOS isFirst, IntOS isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
}

UInt32 Media::CS::CSI420_LRGB::WorkerThread(AnyType obj)
{
	NN<CSI420_LRGB> converter = obj.GetNN<CSI420_LRGB>();
	UIntOS threadId = converter->currId;
	THREADSTAT *ts = &converter->stats[threadId];

	ts->status = 1;
	converter->evtMain.Set();
	while (true)
	{
		ts->evt->Wait();
		if (ts->status == 2)
		{
			break;
		}
		else if (ts->status == 3)
		{
			CSI420_LRGB_do_yv12rgb(ts->yPtr.Ptr(), ts->uPtr.Ptr(), ts->vPtr.Ptr(), ts->dest.Ptr(), ts->width, ts->height, ts->dbpl, ts->isFirst, ts->isLast, ts->csLineBuff.Ptr(), ts->csLineBuff2.Ptr(), converter->yuv2rgb.Ptr(), converter->rgbGammaCorr.Ptr());
			ts->status = 4;
			converter->evtMain.Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain.Set();
	return 0;
}

Media::CS::CSI420_LRGB::CSI420_LRGB(NN<const Media::ColorProfile> srcColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess) : Media::CS::CSYUV_LRGB(srcColor, yuvType, colorSess)
{
	UIntOS i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();

	stats = MemAllocArr(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASSNN(stats[i].evt, Sync::Event());
		stats[i].status = 0;
		stats[i].csLineSize = 0;
		stats[i].csLineBuff = MemAllocArr(UInt8, 0);
		stats[i].csLineBuff2 = MemAllocArr(UInt8, 0);

		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this);
		while (stats[i].status == 0)
		{
			this->evtMain.Wait();
		}
	}
}

Media::CS::CSI420_LRGB::~CSI420_LRGB()
{
	UIntOS i = nThread;
	Bool exited;
	while (i-- > 0)
	{
		if (stats[i].status != 0)
		{
			stats[i].status = 2;
			stats[i].evt->Set();
		}
	}
	while (true)
	{
		exited = true;
		i = nThread;
		while (i-- > 0)
		{
			if (stats[i].status != 0)
			{
				if (stats[i].status == 2)
				{
					stats[i].evt->Set();
					exited = false;
					break;
				}
				else if (stats[i].status > 0)
				{
					stats[i].status = 2;
					stats[i].evt->Set();
					exited = false;
					break;
				}
				else
				{
					stats[i].evt->Set();
					exited = false;
					break;
				}
			}
		}
		if (exited)
			break;

		this->evtMain.Wait(100);
	}
	i = nThread;
	while (i-- > 0)
	{
		MemFreeArr(stats[i].csLineBuff);
		MemFreeArr(stats[i].csLineBuff2);
		stats[i].evt.Delete();
	}
	MemFreeArr(stats);
}

///////////////////////////////////////////////////////
void Media::CS::CSI420_LRGB::ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UIntOS isLast = 1;
	UIntOS isFirst = 0;
	UIntOS i = this->nThread;
	UIntOS lastHeight = dispHeight;
	UIntOS currHeight;
	UIntOS cSize = dispWidth << 4;

	while (i-- > 0)
	{
		if (i == 0)
			isFirst = 1;
		currHeight = MulDivOS(i, dispHeight, nThread) & ~1;

		stats[i].yPtr = srcPtr[0] + srcStoreWidth * currHeight;
		stats[i].uPtr = srcPtr[0] + srcStoreWidth * srcStoreHeight + ((srcStoreWidth * currHeight) >> 2);
		stats[i].vPtr = stats[i].uPtr + ((srcStoreWidth * srcStoreHeight) >> 2);
		stats[i].dest = destPtr + destRGBBpl * currHeight;
		stats[i].isFirst = isFirst;
		stats[i].isLast = isLast;
		isLast = 0;
		stats[i].width = dispWidth;
		stats[i].height = lastHeight - currHeight;
		stats[i].dbpl = destRGBBpl;

		if (stats[i].csLineSize < dispWidth)
		{
			MemFreeArr(stats[i].csLineBuff);
			MemFreeArr(stats[i].csLineBuff2);
			stats[i].csLineSize = dispWidth;
			stats[i].csLineBuff = MemAllocArr(UInt8, cSize << 1);
			stats[i].csLineBuff2 = MemAllocArr(UInt8, cSize << 1);
		}
		stats[i].status = 3;
		stats[i].evt->Set();
		lastHeight = currHeight;
	}
	Bool exited;
	while (true)
	{
		exited = true;
		i = nThread;
		while (i-- > 0)
		{
			if (stats[i].status == 3)
			{
				stats[i].evt->Set();
				exited = false;
				break;
			}
		}
		if (exited)
			break;
		this->evtMain.Wait();
	}
}

UIntOS Media::CS::CSI420_LRGB::GetSrcFrameSize(UIntOS width, UIntOS height)
{
	return (width * height * 3) >> 1;
}
