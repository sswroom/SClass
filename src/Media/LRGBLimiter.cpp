#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/LRGBLimiter.h"
#include "Media/LRGBLimiterC.h"
#include "Sync/ThreadUtil.h"

void Media::LRGBLimiter::TaskFunc(AnyType userObj)
{
	NN<ThreadStatus> status = userObj.GetNN<ThreadStatus>();
	LRGBLimiter_LimitImageLRGB(status->imgPtr.Ptr(), status->w, status->h);
}

Media::LRGBLimiter::LRGBLimiter() : ptask((Sync::ThreadUtil::GetThreadCnt() >= 4)?4:Sync::ThreadUtil::GetThreadCnt(), false)
{
}

Media::LRGBLimiter::~LRGBLimiter()
{
}

void Media::LRGBLimiter::LimitImageLRGB(UnsafeArray<UInt8> imgPtr, UOSInt w, UOSInt h)
{
	ThreadStatus status[4];
	UOSInt j = this->ptask.GetThreadCnt();
	UOSInt i = j;
	UOSInt lastH = h;
	UOSInt thisH;
	while (i-- > 0)
	{
		thisH = MulDivUOS(h, i, j);
		status[i].w = w;
		status[i].h = lastH - thisH;
		status[i].imgPtr = imgPtr + w * 8 * thisH;
		this->ptask.AddTask(TaskFunc, &status[i]);
		lastH = thisH;
	}
	this->ptask.WaitForIdle();
}
