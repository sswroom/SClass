#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/CS/CSY416_LRGBC.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void CSY416_LRGBC_convert(UInt8 *yPtr, UInt8 *dest, UIntOS width, UIntOS height, IntOS dbpl, UIntOS yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
}

UInt32 Media::CS::CSY416_LRGBC::WorkerThread(AnyType obj)
{
	NN<CSY416_LRGBC> converter = obj.GetNN<CSY416_LRGBC>();
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
			CSY416_LRGBC_convert(ts->yPtr.Ptr(), ts->dest.Ptr(), ts->width, ts->height, ts->dbpl, ts->yBpl, converter->yuv2rgb.Ptr(), converter->rgbGammaCorr.Ptr());
			ts->status = 4;
			converter->evtMain.Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain.Set();
	return 0;
}

void Media::CS::CSY416_LRGBC::WaitForWorker(Int32 jobStatus)
{
	UIntOS i;
	Bool exited;
	while (true)
	{
		exited = true;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->stats[i].status == jobStatus)
			{
				this->stats[i].evt->Set();
				exited = false;
				break;
			}
		}
		if (exited)
			break;
		this->evtMain.Wait();
	}
}

Media::CS::CSY416_LRGBC::CSY416_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess) : Media::CS::CSYUV16_LRGBC(srcProfile, destProfile, yuvType, colorSess)
{
	UIntOS i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 2)
	{
		this->nThread = 2;
	}
	stats = MemAllocArr(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASSNN(stats[i].evt, Sync::Event());
		stats[i].status = 0;

		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this, 65536);
		while (stats[i].status == 0)
		{
			this->evtMain.Wait();
		}
	}
}

Media::CS::CSY416_LRGBC::~CSY416_LRGBC()
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
		stats[i].evt.Delete();
	}
	MemFreeArr(stats);
}

void Media::CS::CSY416_LRGBC::ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UIntOS i = this->nThread;
	UIntOS lastHeight = dispHeight;
	UIntOS currHeight;
	
	i = this->nThread;
	lastHeight = dispHeight;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, dispHeight, this->nThread);

		stats[i].yPtr = srcPtr[0] + (srcStoreWidth * currHeight << 3);
		stats[i].dest = destPtr + destRGBBpl * (IntOS)currHeight;
		stats[i].width = dispWidth;
		stats[i].height = lastHeight - currHeight;
		stats[i].dbpl = destRGBBpl;
		stats[i].yBpl = srcStoreWidth << 3;
		stats[i].status = 3;
		stats[i].evt->Set();
		lastHeight = currHeight;
	}
	WaitForWorker(3);
}

UIntOS Media::CS::CSY416_LRGBC::GetSrcFrameSize(UIntOS width, UIntOS height)
{
	return width * height << 3;
}
