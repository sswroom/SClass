#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSP010_RGB8.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void CSP010_RGB8_do_p010rgb8(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, UOSInt isFirst, UOSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
	void CSP010_RGB8_do_p010rgb2(UInt8 *yPtr, UInt8 *uvPtr, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, UOSInt isFirst, UOSInt isLast, UInt8 *csLineBuff, UInt8 *csLineBuff2, Int64 *yuv2rgb, UInt8 *rgbGammaCorr);
}

UInt32 Media::CS::CSP010_RGB8::WorkerThread(AnyType obj)
{
	NN<CSP010_RGB8> converter = obj.GetNN<CSP010_RGB8>();
	UOSInt threadId = converter->currId;
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
			if (ts->width & 7)
			{
				CSP010_RGB8_do_p010rgb2(ts->yPtr.Ptr(), ts->uvPtr.Ptr(), ts->dest.Ptr(), ts->width, ts->height, ts->dbpl, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, converter->yuv2rgb.Ptr(), converter->rgbGammaCorr.Ptr());
			}
			else
			{
				CSP010_RGB8_do_p010rgb8(ts->yPtr.Ptr(), ts->uvPtr.Ptr(), ts->dest.Ptr(), ts->width, ts->height, ts->dbpl, ts->isFirst, ts->isLast, ts->csLineBuff, ts->csLineBuff2, converter->yuv2rgb.Ptr(), converter->rgbGammaCorr.Ptr());
			}
			ts->status = 4;
			converter->evtMain.Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain.Set();
	return 0;
}

Media::CS::CSP010_RGB8::CSP010_RGB8(NN<const Media::ColorProfile> srcColor, NN<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess) : Media::CS::CSYUV10_RGB8(srcColor, destColor, yuvType, colorSess)
{
	UOSInt i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();

	stats = MemAllocArr(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASSNN(stats[i].evt, Sync::Event());
		stats[i].status = 0;
		stats[i].csLineSize = 0;
		stats[i].csLineBuff = 0;
		stats[i].csNALineBuff = 0;
		stats[i].csLineBuff2 = 0;
		stats[i].csNALineBuff2 = 0;

		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this, 65536);
		while (stats[i].status == 0)
		{
			this->evtMain.Wait();
		}
	}
}

Media::CS::CSP010_RGB8::~CSP010_RGB8()
{
	UOSInt i = nThread;
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
		if (stats[i].csNALineBuff)
		{
			MemFree(stats[i].csNALineBuff);
			stats[i].csNALineBuff = 0;
			stats[i].csLineBuff = 0;
		}
		if (stats[i].csNALineBuff2)
		{
			MemFree(stats[i].csNALineBuff2);
			stats[i].csNALineBuff2 = 0;
			stats[i].csLineBuff2 = 0;
		}
		stats[i].evt.Delete();
	}
	MemFreeArr(stats);

	this->Release();
}

///////////////////////////////////////////////////////
void Media::CS::CSP010_RGB8::ConvertV2(UnsafeArray<UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UOSInt isLast = 1;
	UOSInt isFirst = 0;
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;
	UOSInt cSize = dispWidth << 4;
	if (srcStoreWidth & 3)
	{
		srcStoreWidth = srcStoreWidth + 4 - (srcStoreWidth & 3);
	}
	if (srcStoreHeight & 1)
	{
		srcStoreHeight = srcStoreHeight + 1;
	}
	

	while (i-- > 0)
	{
		if (i == 0)
			isFirst = 1;
		currHeight = MulDivUOS(i, dispHeight, nThread) & (UOSInt)~1;

		stats[i].yPtr = srcPtr[0] + (srcStoreWidth * currHeight << 1);
		stats[i].uvPtr = srcPtr[0] + (srcStoreWidth * srcStoreHeight << 1) + (srcStoreWidth * currHeight);
		stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
		stats[i].isFirst = isFirst;
		stats[i].isLast = isLast;
		isLast = 0;
		stats[i].width = dispWidth;
		stats[i].height = lastHeight - currHeight;
		stats[i].dbpl = destRGBBpl;

		if (stats[i].csLineSize < dispWidth)
		{
			if (stats[i].csNALineBuff)
				MemFree(stats[i].csNALineBuff);
			if (stats[i].csNALineBuff2)
				MemFree(stats[i].csNALineBuff2);
			stats[i].csLineSize = dispWidth;
			stats[i].csNALineBuff = MemAlloc(UInt8, (cSize << 1) + 15);
			stats[i].csNALineBuff2 = MemAlloc(UInt8, (cSize << 1) + 15);
			if (((OSInt)stats[i].csNALineBuff) & 15)
			{
				stats[i].csLineBuff = stats[i].csNALineBuff + 16 - (((OSInt)stats[i].csNALineBuff) & 15);
			}
			else
			{
				stats[i].csLineBuff = stats[i].csNALineBuff;
			}
			if (((OSInt)stats[i].csNALineBuff2) & 15)
			{
				stats[i].csLineBuff2 = stats[i].csNALineBuff2 + 16 - (((OSInt)stats[i].csNALineBuff2) & 15);
			}
			else
			{
				stats[i].csLineBuff2 = stats[i].csNALineBuff2;
			}
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

UOSInt Media::CS::CSP010_RGB8::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return (width * height * 3);
}
