#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AVIUtl/AUIVideo.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include <windows.h>

UInt32 __stdcall Media::AVIUtl::AUIVideo::PlayThread(void *userObj)
{
	Media::AVIUtl::AUIVideo *me = (Media::AVIUtl::AUIVideo *)userObj;
	UInt8 *frameBuff;
	Int32 lastFrameNum = -2;
	Int32 thisFrameNum;
	OSInt buffSize;

	me->threadRunning = true;
	frameBuff = MemAlloc(UInt8, me->GetMaxFrameSize());
	while (!me->threadToStop)
	{
		if (me->playing)
		{
			me->frameNumMut->Lock();
			thisFrameNum = me->currFrameNum;
			me->currFrameNum++;
			me->frameNumMut->Unlock();
			if (thisFrameNum >= me->frameCnt)
			{
				me->playing = false;
			}
			else
			{
				buffSize = me->plugin->GetVideoFrame(me->input->hand, thisFrameNum, frameBuff);
				me->playCb(MulDiv(thisFrameNum, 1000 * me->frameRateDenorm, me->frameRateNorm), thisFrameNum, &frameBuff, (UInt32)buffSize, me->plugin->IsVideoKeyFrame(me->input->hand, thisFrameNum)?(Media::IVideoSource::FS_I):(Media::IVideoSource::FS_P), me->playCbData, Media::FT_INTERLACED_TFF, (thisFrameNum != lastFrameNum + 1)?Media::IVideoSource::FF_DISCONTTIME:Media::IVideoSource::FF_NONE, Media::YCOFST_C_CENTER_LEFT);
				lastFrameNum = thisFrameNum;
			}
		}
		else
		{
			me->threadEvt->Wait(10000);
		}
	}
	MemFree(frameBuff);
	me->threadRunning = false;
	return 0;
}

UOSInt Media::AVIUtl::AUIVideo::GetMaxFrameSize()
{
	if (this->frameInfo->byteSize != 0)
	{
		return this->frameInfo->byteSize;
	}
	else if (this->frameInfo->storeBPP != 0)
	{
		return this->frameInfo->storeBPP * this->frameInfo->storeWidth * this->frameInfo->storeHeight;
	}
	else
	{
		return this->frameInfo->storeWidth * this->frameInfo->storeHeight * 4;
	}
}

Media::AVIUtl::AUIVideo::AUIVideo(Media::AVIUtl::AUIPlugin *plugin, Media::AVIUtl::AUIPlugin::AUIInput *input, Media::FrameInfo *frameInfo, Int32 frameRateNorm, Int32 frameRateDenorm, Int32 frameCnt)
{
	this->plugin = plugin;
	this->input = input;
	this->frameInfo = frameInfo;
	this->frameRateNorm = frameRateNorm;
	this->frameRateDenorm = frameRateDenorm;
	this->frameCnt = frameCnt;
	this->currFrameNum = 0;

	this->playing = false;
	this->threadRunning = false;
	this->threadToStop = false;
	this->playCb = 0;
	this->playCbData = 0;
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"Media.AVIUtl.AUIVideo.threadEvt"));
	NEW_CLASS(this->frameNumMut, Sync::Mutex());

	Sync::Thread::Create(PlayThread, this);
	while (!this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

Media::AVIUtl::AUIVideo::~AUIVideo()
{
	this->Stop();
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}

	if (Sync::Interlocked::Decrement(&this->input->useCnt) == 0)
	{
		this->plugin->CloseInput(this->input->hand);
		MemFree(this->input);
	}
	DEL_CLASS(this->plugin);
	DEL_CLASS(this->frameInfo);
	DEL_CLASS(this->threadEvt);
	DEL_CLASS(this->frameNumMut);
}

UTF8Char *Media::AVIUtl::AUIVideo::GetSourceName(UTF8Char *buff)
{
	return 0;
}

const UTF8Char *Media::AVIUtl::AUIVideo::GetFilterName()
{
	return (const UTF8Char*)"AUIVideo";
}

Bool Media::AVIUtl::AUIVideo::GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	info->Set(this->frameInfo);
	*frameRateNorm = this->frameRateNorm;
	*frameRateDenorm = this->frameRateDenorm;
	*maxFrameSize = this->GetMaxFrameSize();
	return true;
}

Bool Media::AVIUtl::AUIVideo::Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData)
{
	if (this->playing)
		return false;
	this->playCb = cb;
	this->fcCb = fcCb;
	this->playCbData = userData;
	return true;
}

Bool Media::AVIUtl::AUIVideo::Start()
{
	if (this->playing)
		return false;
	this->playing = true;
	this->threadEvt->Set();
	return true;
}

void Media::AVIUtl::AUIVideo::Stop()
{
	this->playing = false;
	this->frameNumMut->Lock();
	this->currFrameNum = 0;
	this->frameNumMut->Unlock();
}

Bool Media::AVIUtl::AUIVideo::IsRunning()
{
	return this->playing;
}

Int32 Media::AVIUtl::AUIVideo::GetStreamTime()
{
	return MulDiv(this->frameCnt, this->frameRateDenorm * 1000, this->frameRateNorm);
}

Bool Media::AVIUtl::AUIVideo::CanSeek()
{
	return true;
}

Int32 Media::AVIUtl::AUIVideo::SeekToTime(Int32 time)
{
	this->frameNumMut->Lock();
	this->currFrameNum = MulDiv(time, this->frameRateNorm, this->frameRateDenorm * 1000);
	if (this->currFrameNum < 0)
		this->currFrameNum = 0;
	else if (this->currFrameNum > this->frameCnt)
		this->currFrameNum = this->frameCnt;
	Int32 t = MulDiv(this->currFrameNum, this->frameRateDenorm * 1000, this->frameRateNorm);
	this->frameNumMut->Unlock();
	return t;
}

Bool Media::AVIUtl::AUIVideo::IsRealTimeSrc()
{
	return false;
}

Bool Media::AVIUtl::AUIVideo::TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime)
{
	/////////////////////////////////////////
	return false;
}

OSInt Media::AVIUtl::AUIVideo::GetDataSeekCount()
{
	return 0;
}

OSInt Media::AVIUtl::AUIVideo::GetFrameCount()
{
	return this->frameCnt;
}

UInt32 Media::AVIUtl::AUIVideo::GetFrameTime(UOSInt frameIndex)
{
	return MulDiv((Int32)frameIndex, this->frameRateDenorm * 1000, this->frameRateNorm);
}

void Media::AVIUtl::AUIVideo::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
	Int32 i;
	UInt32 dataSize = (UInt32)this->GetMaxFrameSize();
	i = 0;
	while (i < this->frameCnt)
	{
		if (!cb(MulDiv(i, this->frameRateDenorm * 1000, this->frameRateNorm), i, dataSize, Media::IVideoSource::FS_I, Media::FT_NON_INTERLACE, userData, Media::YCOFST_C_CENTER_LEFT))
		{
			break;
		}
		i++;
	}
}

OSInt Media::AVIUtl::AUIVideo::ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype)
{
	return 0;
}
