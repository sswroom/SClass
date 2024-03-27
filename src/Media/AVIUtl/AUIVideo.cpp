#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AVIUtl/AUIVideo.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include <windows.h>

UInt32 __stdcall Media::AVIUtl::AUIVideo::PlayThread(AnyType userObj)
{
	NotNullPtr<Media::AVIUtl::AUIVideo> me = userObj.GetNN<Media::AVIUtl::AUIVideo>();
	UInt8 *frameBuff;
	UInt32 lastFrameNum = (UInt32)-2;
	UInt32 thisFrameNum;
	UOSInt buffSize;

	me->threadRunning = true;
	frameBuff = MemAlloc(UInt8, me->GetMaxFrameSize());
	while (!me->threadToStop)
	{
		if (me->playing)
		{
			Sync::MutexUsage mutUsage(me->frameNumMut);
			thisFrameNum = me->currFrameNum;
			me->currFrameNum++;
			mutUsage.EndUse();
			if (thisFrameNum >= me->frameCnt)
			{
				me->playing = false;
			}
			else
			{
				buffSize = me->plugin->GetVideoFrame(me->input->hand, thisFrameNum, frameBuff);
				me->playCb(MulDivU32(thisFrameNum, 1000 * me->frameRateDenorm, me->frameRateNorm), thisFrameNum, &frameBuff, (UInt32)buffSize, me->plugin->IsVideoKeyFrame(me->input->hand, thisFrameNum)?(Media::IVideoSource::FS_I):(Media::IVideoSource::FS_P), me->playCbData, Media::FT_INTERLACED_TFF, (thisFrameNum != lastFrameNum + 1)?Media::IVideoSource::FF_DISCONTTIME:Media::IVideoSource::FF_NONE, Media::YCOFST_C_CENTER_LEFT);
				lastFrameNum = thisFrameNum;
			}
		}
		else
		{
			me->threadEvt.Wait(10000);
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
		return this->frameInfo->storeBPP * this->frameInfo->storeSize.CalcArea();
	}
	else
	{
		return this->frameInfo->storeSize.CalcArea() * 4;
	}
}

Media::AVIUtl::AUIVideo::AUIVideo(Media::AVIUtl::AUIPlugin *plugin, Media::AVIUtl::AUIPlugin::AUIInput *input, NotNullPtr<const Media::FrameInfo> frameInfo, UInt32 frameRateNorm, UInt32 frameRateDenorm, UInt32 frameCnt)
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

	Sync::ThreadUtil::Create(PlayThread, this);
	while (!this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Media::AVIUtl::AUIVideo::~AUIVideo()
{
	this->Stop();
	this->threadToStop = true;
	this->threadEvt.Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}

	if (Sync::Interlocked::DecrementU32(this->input->useCnt) == 0)
	{
		this->plugin->CloseInput(this->input->hand);
		MemFree(this->input);
	}
	DEL_CLASS(this->plugin);
}

UTF8Char *Media::AVIUtl::AUIVideo::GetSourceName(UTF8Char *buff)
{
	return 0;
}

Text::CStringNN Media::AVIUtl::AUIVideo::GetFilterName()
{
	return CSTR("AUIVideo");
}

Bool Media::AVIUtl::AUIVideo::GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	info->Set(this->frameInfo);
	frameRateNorm.Set(this->frameRateNorm);
	frameRateDenorm.Set(this->frameRateDenorm);
	maxFrameSize.Set(this->GetMaxFrameSize());
	return true;
}

Bool Media::AVIUtl::AUIVideo::Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData)
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
	this->threadEvt.Set();
	return true;
}

void Media::AVIUtl::AUIVideo::Stop()
{
	this->playing = false;
	Sync::MutexUsage mutUsage(this->frameNumMut);
	this->currFrameNum = 0;
}

Bool Media::AVIUtl::AUIVideo::IsRunning()
{
	return this->playing;
}

Data::Duration Media::AVIUtl::AUIVideo::GetStreamTime()
{
	return Data::Duration::FromRatioU64(this->frameCnt * (UInt64)this->frameRateDenorm, this->frameRateNorm);
}

Bool Media::AVIUtl::AUIVideo::CanSeek()
{
	return true;
}

Data::Duration Media::AVIUtl::AUIVideo::SeekToTime(Data::Duration time)
{
	Sync::MutexUsage mutUsage(this->frameNumMut);
	this->currFrameNum = (UInt32)time.SecsMulDivU32(this->frameRateNorm, this->frameRateDenorm);
	if (this->currFrameNum > this->frameCnt)
		this->currFrameNum = this->frameCnt;
	return Data::Duration::FromRatioU64(this->currFrameNum * this->frameRateDenorm, this->frameRateNorm);
}

Bool Media::AVIUtl::AUIVideo::IsRealTimeSrc()
{
	return false;
}

Bool Media::AVIUtl::AUIVideo::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	/////////////////////////////////////////
	return false;
}

UOSInt Media::AVIUtl::AUIVideo::GetDataSeekCount()
{
	return 0;
}

Bool Media::AVIUtl::AUIVideo::HasFrameCount()
{
	return true;
}

UOSInt Media::AVIUtl::AUIVideo::GetFrameCount()
{
	return this->frameCnt;
}

Data::Duration Media::AVIUtl::AUIVideo::GetFrameTime(UOSInt frameIndex)
{
	return Data::Duration::FromRatioU64(frameIndex * (UInt64)this->frameRateDenorm, this->frameRateNorm);
}

void Media::AVIUtl::AUIVideo::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
	UInt32 i;
	UInt32 dataSize = (UInt32)this->GetMaxFrameSize();
	i = 0;
	while (i < this->frameCnt)
	{
		if (!cb(MulDivU32(i, this->frameRateDenorm * 1000, this->frameRateNorm), i, dataSize, Media::IVideoSource::FS_I, Media::FT_NON_INTERLACE, userData, Media::YCOFST_C_CENTER_LEFT))
		{
			break;
		}
		i++;
	}
}

UOSInt Media::AVIUtl::AUIVideo::ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype)
{
	return 0;
}
