#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUY2_LRGBC.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void CSYUY2_LRGBC_do_yuy2rgb(UInt8 *src, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
}

UInt32 Media::CS::CSYUY2_LRGBC::WorkerThread(AnyType obj)
{
	NN<CSYUY2_LRGBC> converter = obj.GetNN<CSYUY2_LRGBC>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt threadId = converter->currId;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("CSYUY2_LRGBC")), threadId);
	Sync::ThreadUtil::SetName(CSTRP(sbuff, sptr));
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
			CSYUY2_LRGBC_do_yuy2rgb(ts->yPtr.Ptr(), ts->dest.Ptr(), ts->width, ts->height, ts->dbpl, converter->yuv2rgb.Ptr(), converter->rgbGammaCorr.Ptr());
			ts->status = 4;
			converter->evtMain.Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain.Set();
	return 0;
}

Media::CS::CSYUY2_LRGBC::CSYUY2_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess) : Media::CS::CSYUV_LRGBC(srcProfile, destProfile, yuvType, colorSess)
{
	UOSInt i;
	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 8) this->nThread = 8;

	stats = MemAllocArr(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASSNN(stats[i].evt, Sync::Event());
		stats[i].status = 0;

		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this);
		while (stats[i].status == 0)
		{
			this->evtMain.Wait();
		}
	}
}

Media::CS::CSYUY2_LRGBC::~CSYUY2_LRGBC()
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

		this->evtMain.Wait(100);
	}
	i = nThread;
	while (i-- > 0)
	{
		stats[i].evt.Delete();
	}
	MemFreeArr(stats);
}

void Media::CS::CSYUY2_LRGBC::ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
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
		this->evtMain.Wait();
	}
}

UOSInt Media::CS::CSYUY2_LRGBC::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 1;
}
