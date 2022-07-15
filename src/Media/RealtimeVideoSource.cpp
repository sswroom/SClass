#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/RealtimeVideoSource.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

void __stdcall Media::RealtimeVideoSource::FrameCB(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	Media::RealtimeVideoSource *me = (Media::RealtimeVideoSource*)userData;
	Sync::MutexUsage mutUsage(&me->frameMut);
	me->frameTime = frameTime;
	me->frameNum = frameNum;
	MemCopyNO(me->frameBuff, imgData[0], dataSize);
	me->frameSize = dataSize;
	me->frameType = frameType;
}

Media::RealtimeVideoSource::RealtimeVideoSource(Media::IRealtimeVideoSource *capSrc)
{
	this->capSrc = capSrc;
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
	if (this->frameBuff)
	{
		MemFree(this->frameBuff);
	}
}

UTF8Char *Media::RealtimeVideoSource::GetName(UTF8Char *buff)
{
	return this->capSrc->GetSourceName(buff);
}

Bool Media::RealtimeVideoSource::GetVideoInfo(Media::FrameInfo *info, UInt32 *rateNorm, UInt32 *rateDenorm, UOSInt *maxFrameSize)
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
	this->capSrc->Init(FrameCB, 0, this);
	if (!this->capSrc->Start())
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
	Sync::MutexUsage mutUsage(&this->frameMut);
	frameSize = this->frameSize;
	this->lastFrameTime = this->frameTime;
	MemCopyNO(frameBuff, this->frameBuff, frameSize);
	*frameTime = this->frameTime;
	*ftype = this->frameType;
	return frameSize;
}
