#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Thread.h"
#include "Media/RealtimeVideoSource.h"
#include <windows.h>

void __stdcall Media::RealtimeVideoSource::FrameCB(UInt32 frameTime, UInt32 frameNum, UInt8 *imgData, UInt32 dataSize, Bool deltaFrame, void *userData, Media::FrameType frameType, Bool discontTime)
{
	Media::RealtimeVideoSource *me = (Media::RealtimeVideoSource*)userData;
	me->frameMut->Lock();
	me->frameTime = frameTime;
	me->frameNum = frameNum;
	MemCopy(me->frameBuff, imgData, dataSize);
	me->frameSize = dataSize;
	me->frameType = frameType;
	me->frameMut->Unlock();
}

Media::RealtimeVideoSource::RealtimeVideoSource(Media::IRealtimeVideoSource *capSrc)
{
	this->capSrc = capSrc;
	NEW_CLASS(this->frameMut, Sync::Mutex());
	capSrc->GetVideoInfo(&this->frameInfo, &this->frameRateNorm, &this->frameRateDenorm, &this->maxFrameSize);
	this->frameBuff = MemAlloc(UInt8, this->maxFrameSize);
	this->frameSize = 0;
	this->started = false;
	this->frameTime = -1;
	this->frameType = Media::FT_NON_INTERLACE;
	this->lastFrameTime = -1;
}

Media::RealtimeVideoSource::~RealtimeVideoSource()
{
	this->Stop();
	if (this->capSrc)
	{
		DEL_CLASS(this->capSrc);
	}
	if (this->frameMut)
	{
		DEL_CLASS(this->frameMut);
	}
	if (this->frameBuff)
	{
		MemFree(this->frameBuff);
	}
}

WChar *Media::RealtimeVideoSource::GetName(WChar *buff)
{
	return this->capSrc->GetName(buff);
}

Bool Media::RealtimeVideoSource::GetVideoInfo(Media::FrameInfo *info, Int32 *rateNorm, Int32 *rateDenorm, Int32 *maxFrameSize)
{
	return this->capSrc->GetVideoInfo(info, rateNorm, rateDenorm, maxFrameSize);
}

void Media::RealtimeVideoSource::Start()
{
	if (started)
		return;
	this->lastFrameTime = -1;
	this->frameType = Media::FT_NON_INTERLACE;
	this->frameTime = -1;
	this->started = true;
	if (!this->capSrc->Start(FrameCB, this))
	{
		this->started = false;
	}
}

void Media::RealtimeVideoSource::Stop()
{
	if (!started)
		return;
	this->started = false;
	this->capSrc->Stop();
}

Int32 Media::RealtimeVideoSource::GetStreamTime()
{
	return -1;
}

Bool Media::RealtimeVideoSource::CanSeek()
{
	return false;
}

Int32 Media::RealtimeVideoSource::SeekToTime(Int32 time)
{
	return -1;
}

Bool Media::RealtimeVideoSource::IsRealTimeSrc()
{
	return true;
}

OSInt Media::RealtimeVideoSource::ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype)
{
	if (!started)
		return 0;
	while (this->frameTime == this->lastFrameTime && started)
	{
		Sync::Thread::Sleep(10);
	}
	if (!started)
	{
		return 0;
	}
	OSInt frameSize;
	this->frameMut->Lock();
	frameSize = this->frameSize;
	this->lastFrameTime = this->frameTime;
	MemCopy(frameBuff, this->frameBuff, frameSize);
	*frameTime = this->frameTime;
	*ftype = this->frameType;
	this->frameMut->Unlock();
	return frameSize;
}
