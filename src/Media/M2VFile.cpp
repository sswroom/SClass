#include "Stdafx.h"
#include "Media/M2VFile.h"
#include "Media/M2VStreamSource.h"
#include "Media/MPEGVideoParser.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#define BUFFSIZE 1048576

UInt32 __stdcall Media::M2VFile::PlayThread(AnyType userData)
{
	NN<Media::M2VFile> me = userData.GetNN<Media::M2VFile>();
	UOSInt buffSize;
	me->playStarted = true;
	me->playing = true;
	me->stm->ClearFrameBuff();
	me->stm->SetStreamTime(me->startTime);
	buffSize = 0;
	while (!me->playToStop)
	{
		buffSize = me->stmData->GetRealData(me->readOfst, BUFFSIZE - buffSize, me->readBuff.SubArray(buffSize));
		if (buffSize == 0)
		{
			me->stm->EndFrameStream();
			break;
		}
		me->stm->WriteFrameStream(me->readBuff.Arr().Ptr(), buffSize);
		me->readOfst += buffSize;
		buffSize = 0;
	}

	me->playing = false;
	return 0;
}

Media::M2VFile::M2VFile(NN<IO::StreamData> stmData) : Media::MediaFile(stmData->GetFullName()), readBuff(1048576)
{
	this->stmData = stmData->GetPartialData(0, this->fleng = stmData->GetDataSize());
	this->readOfst = 0;
	this->playing = false;
	this->playToStop = false;
	this->startTime = 0;

	UOSInt buffSize = this->stmData->GetRealData(0, 1024, this->readBuff);
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	Media::FrameInfo info;
	Media::MPEGVideoParser::GetFrameInfo(this->readBuff.Arr().Ptr(), 1024, info, frameRateNorm, frameRateDenorm, this->bitRate, false);
	NEW_CLASS(this->stm, Media::M2VStreamSource(*this));
	this->stm->DetectStreamInfo(this->readBuff.Arr().Ptr(), buffSize);
}

Media::M2VFile::~M2VFile()
{
	if (this->playing)
	{
		this->StopVideo();
	}
	DEL_CLASS(this->stm);
	this->stmData.Delete();
}

UOSInt Media::M2VFile::AddSource(Media::MediaSource *src, Int32 syncTime)
{
	return (UOSInt)-1;
}

Media::MediaSource *Media::M2VFile::GetStream(UOSInt index, Int32 *syncTime)
{
	if (index != 0)
		return 0;
	if (syncTime)
		*syncTime = 0;
	return this->stm;
}

void Media::M2VFile::KeepStream(UOSInt index, Bool toKeep)
{
}

UnsafeArrayOpt<UTF8Char> Media::M2VFile::GetMediaName(UnsafeArray<UTF8Char> buff)
{
	return this->GetSourceName(buff);
}

Data::Duration Media::M2VFile::GetStreamTime()
{
	return Data::Duration::FromRatioU64(this->fleng, this->bitRate >> 3);
}

Bool Media::M2VFile::StartAudio()
{
	return false;
}

Bool Media::M2VFile::StopAudio()
{
	return false;
}

Bool Media::M2VFile::StartVideo()
{
	if (this->playing)
		return false;
	this->playStarted = false;
	this->playToStop = false;
	Sync::ThreadUtil::Create(PlayThread, this);
	return true;
}

Bool Media::M2VFile::StopVideo()
{
	this->playToStop = true;
	while (this->playing)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->readOfst = 0;
	this->startTime = 0;
	return true;
}

Bool Media::M2VFile::IsRunning()
{
	return this->playing;
}

Data::Duration Media::M2VFile::SeekToTime(Data::Duration mediaTime)
{
	if (this->playing)
		return 0;
	this->readOfst = mediaTime.MultiplyU64(this->bitRate >> 3);
	this->startTime = mediaTime;
	return mediaTime;
}

Bool Media::M2VFile::IsRealTimeSrc()
{
	return false;
}

Bool Media::M2VFile::CanSeek()
{
	return true;
}

UOSInt Media::M2VFile::GetDataSeekCount()
{
	return this->stmData->GetSeekCount();
}
