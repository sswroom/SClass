#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSP216_LRGBC.h"
#include "Sync/Thread.h"

extern "C"
{
	void CSP216_LRGBC_do_yuy2rgb(UInt8 *srcY, UInt8 *srcUV, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
}

UInt32 Media::CS::CSP216_LRGBC::WorkerThread(void *obj)
{
	CSP216_LRGBC *converter = (CSP216_LRGBC*)obj;
	UOSInt threadId = converter->currId;
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
			CSP216_LRGBC_do_yuy2rgb(ts->yPtr, ts->uvPtr, ts->dest, ts->width, ts->height, ts->dbpl, converter->yuv2rgb, converter->rgbGammaCorr);
			ts->status = 4;
			converter->evtMain->Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain->Set();
	return 0;
}

Media::CS::CSP216_LRGBC::CSP216_LRGBC(const Media::ColorProfile *srcProfile, const Media::ColorProfile *destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSYUV16_LRGBC(srcProfile, destProfile, yuvType, colorSess)
{
	UOSInt i;
	this->nThread = Sync::Thread::GetThreadCnt();

	NEW_CLASS(evtMain, Sync::Event());
	stats = MemAlloc(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event());
		stats[i].status = 0;

		currId = i;
		Sync::Thread::Create(WorkerThread, this);
		while (stats[i].status == 0)
		{
			evtMain->Wait();
		}
	}
}

Media::CS::CSP216_LRGBC::~CSP216_LRGBC()
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
//					break;
				}
				else if (stats[i].status > 0)
				{
					stats[i].status = 2;
					stats[i].evt->Set();
					exited = false;
//					break;
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

void Media::CS::CSP216_LRGBC::ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;

	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, dispHeight, nThread) & (UOSInt)~1;

		stats[i].yPtr = srcPtr[0] + currHeight * (srcStoreWidth << 1);
		stats[i].uvPtr = srcPtr[0] + currHeight * srcStoreWidth + (srcStoreHeight * srcStoreWidth << 1);
		stats[i].dest = destPtr + destRGBBpl * (OSInt)currHeight;
		stats[i].width = dispWidth;
		stats[i].height = lastHeight - currHeight;
		stats[i].dbpl = destRGBBpl;

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
		evtMain->Wait();
	}
}

UOSInt Media::CS::CSP216_LRGBC::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 2;
}
