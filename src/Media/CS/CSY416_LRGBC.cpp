#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/CSY416_LRGBC.h"
#include "Sync/Thread.h"

extern "C"
{
	void CSY416_LRGBC_convert(UInt8 *yPtr, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, UOSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
}

UInt32 Media::CS::CSY416_LRGBC::WorkerThread(void *obj)
{
	CSY416_LRGBC *converter = (CSY416_LRGBC*)obj;
	OSInt threadId = converter->currId;
	THREADSTAT *ts = &converter->stats[threadId];

	ts->status = 1;
	converter->evtMain->Set();
	while (true)
	{
		ts->evt->Wait();
		if (ts->status == 2)
		{
			break;
		}
		else if (ts->status == 3)
		{
			CSY416_LRGBC_convert(ts->yPtr, ts->dest, ts->width, ts->height, ts->dbpl, ts->yBpl, converter->yuv2rgb, converter->rgbGammaCorr);
			ts->status = 4;
			converter->evtMain->Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain->Set();
	return 0;
}

void Media::CS::CSY416_LRGBC::WaitForWorker(Int32 jobStatus)
{
	OSInt i;
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
		this->evtMain->Wait();
	}
}

Media::CS::CSY416_LRGBC::CSY416_LRGBC(const Media::ColorProfile *srcProfile, const Media::ColorProfile *destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSYUV16_LRGBC(srcProfile, destProfile, yuvType, colorSess)
{
	UOSInt i;
	this->nThread = Sync::Thread::GetThreadCnt();
	if (this->nThread > 2)
	{
		this->nThread = 2;
	}
	NEW_CLASS(evtMain, Sync::Event((const UTF8Char*)"Media.CS.CSY416_LRGBC.evtMain"));
	stats = MemAlloc(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event((const UTF8Char*)"Media.CS.CSY416_LRGBC.stats.evt"));
		stats[i].status = 0;

		currId = i;
		Sync::Thread::Create(WorkerThread, this, 65536);
		while (stats[i].status == 0)
		{
			evtMain->Wait();
		}
	}
}

Media::CS::CSY416_LRGBC::~CSY416_LRGBC()
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

		evtMain->Wait(100);
	}
	i = nThread;
	while (i-- > 0)
	{
		DEL_CLASS(stats[i].evt);
	}
	DEL_CLASS(evtMain);
	MemFree(stats);
}

void Media::CS::CSY416_LRGBC::ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;
	
	i = this->nThread;
	lastHeight = dispHeight;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, dispHeight, this->nThread);

		stats[i].yPtr = srcPtr[0] + (srcStoreWidth * currHeight << 3);
		stats[i].dest = ((UInt8*)destPtr) + destRGBBpl * currHeight;
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

UOSInt Media::CS::CSY416_LRGBC::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 3;
}
