#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSUYVY_LRGBC.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void CSUYVY_LRGBC_do_yuy2rgb(UInt8 *src, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
}

UInt32 Media::CS::CSUYVY_LRGBC::WorkerThread(AnyType obj)
{
	NN<CSUYVY_LRGBC> converter = obj.GetNN<CSUYVY_LRGBC>();
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	UOSInt threadId = converter->currId;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("CSUYVY_LRGBC")), threadId);
	Sync::ThreadUtil::SetName(CSTRP(sbuff, sptr));
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
			CSUYVY_LRGBC_do_yuy2rgb(ts->yPtr, ts->dest, ts->width, ts->height, ts->dbpl, converter->yuv2rgb, converter->rgbGammaCorr);
			ts->status = 4;
			converter->evtMain->Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain->Set();
	return 0;
}

Media::CS::CSUYVY_LRGBC::CSUYVY_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSYUV_LRGBC(srcProfile, destProfile, yuvType, colorSess)
{
	UOSInt i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 8)
	{
		this->nThread = 8;
	}

	NEW_CLASS(evtMain, Sync::Event());
	stats = MemAlloc(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event());
		stats[i].status = 0;

		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this);
		while (stats[i].status == 0)
		{
			evtMain->Wait();
		}
	}
}

Media::CS::CSUYVY_LRGBC::~CSUYVY_LRGBC()
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

void Media::CS::CSUYVY_LRGBC::ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;

	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, dispHeight, nThread) & (UOSInt)~1;

		stats[i].yPtr = srcPtr[0] + currHeight * (srcStoreWidth << 1);
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

UOSInt Media::CS::CSUYVY_LRGBC::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 1;
}
