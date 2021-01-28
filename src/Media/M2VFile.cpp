#include "Stdafx.h"
#include "Media/M2VFile.h"
#include "Media/M2VStreamSource.h"
#include "Media/MPEGVideoParser.h"
#include "Sync/Thread.h"
#define BUFFSIZE 1048576

UInt32 __stdcall Media::M2VFile::PlayThread(void *userData)
{
	Media::M2VFile *me = (Media::M2VFile*)userData;
	OSInt buffSize;
	me->playStarted = true;
	me->playing = true;
	me->stm->ClearFrameBuff();
	me->stm->SetStreamTime(me->startTime);
	buffSize = 0;
	while (!me->playToStop)
	{
		buffSize = me->stmData->GetRealData(me->readOfst, BUFFSIZE - buffSize, &me->readBuff[buffSize]);
		if (buffSize == 0)
		{
			me->stm->EndFrameStream();
			break;
		}
		me->stm->WriteFrameStream(me->readBuff, buffSize);
		me->readOfst += buffSize;
		buffSize = 0;
	}

	me->playing = false;
	return 0;
}

Media::M2VFile::M2VFile(IO::IStreamData *stmData) : Media::MediaFile(stmData->GetFullName())
{
	this->stmData = stmData->GetPartialData(0, this->fleng = stmData->GetDataSize());
	this->readOfst = 0;
	this->readBuff = MemAlloc(UInt8, 1048576);
	this->playing = false;
	this->playToStop = false;
	this->startTime = 0;

	OSInt buffSize = this->stmData->GetRealData(0, 1024, this->readBuff);
	Int32 frameRateNorm;
	Int32 frameRateDenorm;
	FrameInfo info;
	Media::MPEGVideoParser::GetFrameInfo(this->readBuff, 1024, &info, &frameRateNorm, &frameRateDenorm, &this->bitRate, false);
	NEW_CLASS(this->stm, Media::M2VStreamSource(this));
	this->stm->DetectStreamInfo(this->readBuff, buffSize);
}

Media::M2VFile::~M2VFile()
{
	if (this->playing)
	{
		this->StopVideo();
	}
	DEL_CLASS(this->stm);
	DEL_CLASS(this->stmData);
	MemFree(this->readBuff);
}

UOSInt Media::M2VFile::AddSource(Media::IMediaSource *src, Int32 syncTime)
{
	return -1;
}

Media::IMediaSource *Media::M2VFile::GetStream(UOSInt index, Int32 *syncTime)
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

UTF8Char *Media::M2VFile::GetMediaName(UTF8Char *buff)
{
	return this->GetSourceName(buff);
}

Int32 Media::M2VFile::GetStreamTime()
{
	return (Int32)(this->fleng * 8000 / this->bitRate);
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
	Sync::Thread::Create(PlayThread, this);
	return true;
}

Bool Media::M2VFile::StopVideo()
{
	this->playToStop = true;
	while (this->playing)
	{
		Sync::Thread::Sleep(10);
	}
	this->readOfst = 0;
	this->startTime = 0;
	return true;
}

Bool Media::M2VFile::IsRunning()
{
	return this->playing;
}

Int32 Media::M2VFile::SeekToTime(Int32 mediaTime)
{
	if (this->playing)
		return 0;
	this->readOfst = mediaTime * this->bitRate / 8000;
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

OSInt Media::M2VFile::GetDataSeekCount()
{
	return this->stmData->GetSeekCount();
}
