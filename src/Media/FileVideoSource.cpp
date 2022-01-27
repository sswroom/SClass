#include "Stdafx.h"
#include "Media/FileVideoSource.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

#define BUFFCNT 16

UInt32 __stdcall Media::FileVideoSource::PlayThread(void *userObj)
{
	Media::FileVideoSource *me = (Media::FileVideoSource*)userObj;
	UInt8 *frameBuff;
	UInt8 *frameBuff2;
	UInt32 lastFrameNum;
	UInt32 frameNum;
	UOSInt frameSize;
	UOSInt currPart;
	UOSInt nextPart;
	Bool needNotify = false;
	OSInt nextIndex;

	me->playing = true;
	me->mainEvt->Set();
	Sync::Thread::SetPriority(Sync::Thread::TP_HIGHEST);
	frameBuff = MemAllocA(UInt8, me->maxFrameSize);
	nextIndex = BUFFCNT;
	while (nextIndex-- > 0)
	{
		me->outputFrames[nextIndex].frameBuff = MemAllocA(UInt8, me->maxFrameSize);
	}
	lastFrameNum = me->currFrameNum - 2;
	while (!me->playToStop)
	{
		if (me->playEnd)
		{
			me->playEvt->Wait(1000);
		}
		else
		{
			frameNum = me->currFrameNum++;
			if (frameNum >= (UOSInt)me->frameParts->GetCount())
			{
				while (me->outputCount > 0 && me->outputRunning)
				{
					me->playEvt->Wait(1000);
				}
				needNotify = true;
				me->playEnd = true;
				me->currFrameNum = 0;
				break;
			}
			else
			{
				currPart = me->frameParts->GetItem(frameNum);
				if (frameNum + 1 >= (UOSInt)me->frameParts->GetCount())
				{
					nextPart = me->frameOfsts->GetCount();
				}
				else
				{
					nextPart = me->frameParts->GetItem(frameNum + 1);
				}

				frameSize = 0;
				while (currPart < nextPart)
				{
					frameSize += me->data->GetRealData(me->frameOfsts->GetItem(currPart), me->frameSizes->GetItem(currPart), &frameBuff[frameSize]);
					currPart++;
				}
				if (frameSize > 0)
				{
					UInt32 frameTime = me->frameTimes->GetItem(frameNum);
					if (frameTime == 0)
					{
						frameTime = MulDivU32(frameNum, me->frameRateDenorm * 1000, me->frameRateNorm);
					}

					Media::FrameType ftype;
					Bool isOdd;
					if ((me->frameRateNorm / me->frameRateDenorm) >= 100)
					{
						isOdd = (frameNum & 2) != 0;
					}
					else
					{
						isOdd = (frameNum & 1) != 0;
					}
					if (me->frameInfo->ftype == Media::FT_FIELD_TF)
					{
						if (isOdd)
						{
							ftype = Media::FT_FIELD_BF;
						}
						else
						{
							ftype = Media::FT_FIELD_TF;
						}
					}
					else if (me->frameInfo->ftype == Media::FT_FIELD_BF)
					{
						if (isOdd)
						{
							ftype = Media::FT_FIELD_TF;
						}
						else
						{
							ftype = Media::FT_FIELD_BF;
						}
					}
					else
					{
						ftype = me->frameInfo->ftype;
					}
					while (me->outputCount >= BUFFCNT)
					{
						me->playEvt->Wait(100);
						if (me->playToStop)
							break;
					}
					Sync::MutexUsage mutUsage(me->outputMut);
					nextIndex = (me->outputStart + me->outputCount) & (BUFFCNT - 1);
					mutUsage.EndUse();
					if (!me->playToStop)
					{
						me->outputFrames[nextIndex].frameTime = frameTime;
						me->outputFrames[nextIndex].frameNum = frameNum;
						frameBuff2 = me->outputFrames[nextIndex].frameBuff;
						me->outputFrames[nextIndex].frameBuff = frameBuff;
						me->outputFrames[nextIndex].frameSize = frameSize;
						me->outputFrames[nextIndex].frameStruct = me->frameIsKey->GetItem(frameNum)?(Media::IVideoSource::FS_I):(Media::IVideoSource::FS_P);
						me->outputFrames[nextIndex].fType = ftype;
						me->outputFrames[nextIndex].flags = (frameNum != lastFrameNum + 1)?Media::IVideoSource::FF_DISCONTTIME:Media::IVideoSource::FF_NONE;
						me->outputFrames[nextIndex].ycOfst = Media::YCOFST_C_CENTER_LEFT;
						mutUsage.BeginUse();
						me->outputCount++;
						mutUsage.EndUse();
						me->outputEvt->Set();
						frameBuff = frameBuff2;
					}
				}
				lastFrameNum = frameNum;
			}
		}
	}
	while (me->outputCount > 0 && me->outputRunning)
	{
		me->playEvt->Wait(100);
	}
	nextIndex = BUFFCNT;
	while (nextIndex-- > 0)
	{
		MemFreeA(me->outputFrames[nextIndex].frameBuff);
	}
	MemFreeA(frameBuff);
	me->playing = false;
	me->mainEvt->Set();
	if (needNotify)
	{
		me->fcCb(Media::IVideoSource::FC_ENDPLAY, me->playCbData);
	}
	return 1003;
}

UInt32 __stdcall Media::FileVideoSource::OutputThread(void *userObj)
{
	Media::FileVideoSource *me = (Media::FileVideoSource*)userObj;
	OSInt nextIndex;
	me->outputRunning = true;
	me->mainEvt->Set();
	Sync::Thread::SetPriority(Sync::Thread::TP_HIGHEST);
	while (!me->outputToStop)
	{
		if (me->outputCount > 0)
		{
			nextIndex = me->outputStart;
			me->playCb(me->outputFrames[nextIndex].frameTime, me->outputFrames[nextIndex].frameNum, &me->outputFrames[nextIndex].frameBuff, me->outputFrames[nextIndex].frameSize, me->outputFrames[nextIndex].frameStruct, me->playCbData, me->outputFrames[nextIndex].fType, me->outputFrames[nextIndex].flags, me->outputFrames[nextIndex].ycOfst);
			Sync::MutexUsage mutUsage(me->outputMut);
			me->outputStart = (me->outputStart + 1) & (BUFFCNT - 1);
			me->outputCount--;
			mutUsage.EndUse();
			me->playEvt->Set();
		}
		else
		{
			me->outputEvt->Wait(1000);
		}
	}
	me->outputRunning = false;
	Sync::MutexUsage mutUsage(me->outputMut);
	me->outputCount = 0;
	mutUsage.EndUse();
	me->playEvt->Set();
	me->mainEvt->Set();
	return 0;
}

Media::FileVideoSource::FileVideoSource(IO::IStreamData *data, Media::FrameInfo *frameInfo, UInt32 frameRateNorm, UInt32 frameRateDenorm, Bool timeBased)
{
	NEW_CLASS(this->frameInfo, Media::FrameInfo());
	this->frameInfo->Set(frameInfo);
	this->data = data->GetPartialData(0, data->GetDataSize());
	this->frameRateNorm = frameRateNorm;
	this->frameRateDenorm = frameRateDenorm;
	this->maxFrameSize = 0;
	this->currFrameNum = 0;
	this->timeBased = timeBased;

	NEW_CLASS(this->frameOfsts, Data::ArrayListUInt64());
	NEW_CLASS(this->frameSizes, Data::ArrayListUInt32());
	NEW_CLASS(this->frameIsKey, Data::ArrayList<Bool>());
	NEW_CLASS(this->frameParts, Data::ArrayListUInt32());
	NEW_CLASS(this->frameTimes, Data::ArrayListUInt32());

	this->playing = false;
	this->outputRunning = false;
	this->outputStart = 0;
	this->outputCount = 0;
	this->outputFrames = MemAlloc(OutputFrameInfo, BUFFCNT);
	NEW_CLASS(this->outputMut, Sync::Mutex());
	NEW_CLASS(this->mainEvt, Sync::Event(true, (const UTF8Char*)"Media.FileVideoSource.mainEvt"));
	NEW_CLASS(this->playEvt, Sync::Event(true, (const UTF8Char*)"Media.FileVideoSource.playEvt"));
	NEW_CLASS(this->outputEvt, Sync::Event(true, (const UTF8Char*)"Media.FileVideoSource.outputEvt"));
	NEW_CLASS(this->pbcMut, Sync::Mutex());
}

Media::FileVideoSource::~FileVideoSource()
{
	Stop();

	MemFree(this->outputFrames);
	DEL_CLASS(this->frameOfsts);
	DEL_CLASS(this->frameSizes);
	DEL_CLASS(this->frameIsKey);
	DEL_CLASS(this->frameParts);
	DEL_CLASS(this->frameTimes);
	DEL_CLASS(this->data);
	DEL_CLASS(this->outputEvt);
	DEL_CLASS(this->outputMut);
	DEL_CLASS(this->playEvt);
	DEL_CLASS(this->pbcMut);
	DEL_CLASS(this->mainEvt);
	DEL_CLASS(this->frameInfo);
}

void Media::FileVideoSource::AddNewFrame(UInt64 frameOfst, UInt32 frameSize, Bool isKey, UInt32 frameTime)
{
	this->frameIsKey->Add(isKey);
	this->frameParts->Add((UInt32)this->frameOfsts->GetCount());
	this->frameOfsts->Add(frameOfst);
	this->frameSizes->Add(frameSize);
	this->frameTimes->Add(frameTime);
	this->currFrameSize = frameSize;

	if (this->maxFrameSize < frameSize)
	{
		this->maxFrameSize = frameSize;
	}
}

void Media::FileVideoSource::AddFramePart(UInt64 frameOfst, UInt32 frameSize)
{
	this->frameOfsts->Add(frameOfst);
	this->frameSizes->Add(frameSize);
	this->currFrameSize += frameSize;
	if (this->maxFrameSize < currFrameSize)
	{
		this->maxFrameSize = currFrameSize;
	}
}

void Media::FileVideoSource::SetFrameRate(UInt32 frameRateNorm, UInt32 frameRateDenorm)
{
	this->frameRateNorm = frameRateNorm;
	this->frameRateDenorm = frameRateDenorm;
}

UTF8Char *Media::FileVideoSource::GetSourceName(UTF8Char *buff)
{
	return this->data->GetFullName()->ConcatTo(buff);
}

Text::CString Media::FileVideoSource::GetFilterName()
{
	return CSTR("FileVideoSource");
}

Bool Media::FileVideoSource::GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	info->Set(this->frameInfo);
	*frameRateNorm = this->frameRateNorm;
	*frameRateDenorm = this->frameRateDenorm;
	*maxFrameSize = this->maxFrameSize;
	return true;
}

Bool Media::FileVideoSource::Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData)
{
	if (this->playing)
	{
		if (this->playEnd)
		{
			Stop();
		}
		else
		{
			return false;
		}
	}
	this->playCb = cb;
	this->fcCb = fcCb;
	this->playCbData = userData;
	return true;
}

Bool Media::FileVideoSource::Start()
{
	if (this->playing)
	{
		if (this->playEnd)
		{
			Stop();
		}
		else
		{
			return false;
		}
	}
	Sync::MutexUsage mutUsage(this->pbcMut);
	this->playEnd = false;
	this->playToStop = false;
	this->outputToStop = false;
	this->outputStart = 0;
	this->outputCount = 0;
	Sync::Thread::Create(OutputThread, this);
	while (!this->outputRunning)
	{
		this->mainEvt->Wait(100);
	}
	Sync::Thread::Create(PlayThread, this);
	while (!this->playing && !this->playEnd)
	{
		this->mainEvt->Wait(100);
	}
	mutUsage.EndUse();
	return true;
}

void Media::FileVideoSource::Stop()
{
	Sync::MutexUsage mutUsage(this->pbcMut);
	if (this->outputRunning)
	{
		this->outputToStop = true;
		this->outputEvt->Set();
		while (this->outputRunning)
		{
			this->mainEvt->Wait(100);
		}
	}
	if (this->playing)
	{
		this->playToStop = true;
		this->playEvt->Set();
		while (this->playing)
		{
			this->mainEvt->Wait(100);
		}
	}
	mutUsage.EndUse();
}

Bool Media::FileVideoSource::IsRunning()
{
	return this->playing && !this->playEnd;
}

Int32 Media::FileVideoSource::GetStreamTime()
{
	return (Int32)MulDivU32((UInt32)this->frameParts->GetCount(), this->frameRateDenorm * 1000, this->frameRateNorm);
}

Bool Media::FileVideoSource::CanSeek()
{
	return true;
}

UInt32 Media::FileVideoSource::SeekToTime(UInt32 time)
{
	if (this->timeBased)
	{
		UOSInt lastKey = 0;
		UInt32 lastKeyTime = 0;
		UInt32 thisTime;
		UOSInt i = 0;
		UOSInt j = this->frameParts->GetCount();
		while (i < j)
		{
			if (this->frameIsKey->GetItem(i))
			{
				thisTime = this->frameTimes->GetItem(i);
				if (thisTime > time)
					break;
				lastKey = i;
				lastKeyTime = thisTime;
			}
			i++;
		}
		this->currFrameNum = (UInt32)lastKey;
		return lastKeyTime;
	}
	else
	{
		UInt32 newNum = MulDivU32(time, this->frameRateNorm, this->frameRateDenorm * 1000);
		if (newNum > this->frameParts->GetCount())
		{
			newNum = (UInt32)this->frameParts->GetCount();
		}
		if (newNum != this->currFrameNum)
		{
			while (newNum >= 0 && !this->frameIsKey->GetItem(newNum))
				newNum--;
			this->currFrameNum = newNum;
		}
		return MulDivU32(this->currFrameNum, this->frameRateDenorm * 1000, this->frameRateNorm);;
	}
}

Bool Media::FileVideoSource::IsRealTimeSrc()
{
	return false;
}

Bool Media::FileVideoSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	///////////////////////////////////////////
	return false;
}

UOSInt Media::FileVideoSource::GetDataSeekCount()
{
	return this->data->GetSeekCount();
}

Bool Media::FileVideoSource::HasFrameCount()
{
	return true;
}

UOSInt Media::FileVideoSource::GetFrameCount()
{
	return this->frameParts->GetCount();
}

UInt32 Media::FileVideoSource::GetFrameTime(UOSInt frameIndex)
{
	UInt32 frameTime = this->frameTimes->GetItem(frameIndex);
	if (frameTime == 0)
	{
		frameTime = MulDivU32((UInt32)frameIndex, this->frameRateDenorm * 1000, this->frameRateNorm);
	}
	return frameTime;
}

void Media::FileVideoSource::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
	UOSInt frameSize;
	UOSInt currPart;
	UOSInt nextPart;
	UOSInt i = 0;
	UOSInt j = this->frameParts->GetCount();
	UInt32 lastFrameTime = 0;
	while (i < j)
	{
		currPart = this->frameParts->GetItem(i);
		if (i + 1 >= j)
		{
			nextPart = this->frameOfsts->GetCount();
		}
		else
		{
			nextPart = this->frameParts->GetItem(i + 1);
		}

		frameSize = 0;
		while (currPart < nextPart)
		{
			frameSize += this->frameSizes->GetItem(currPart);
			currPart++;
		}
		if (frameSize > 0)
		{
			UInt32 frameTime = this->frameTimes->GetItem(i);
			if (frameTime == 0)
			{
				frameTime = MulDivU32((UInt32)i, this->frameRateDenorm * 1000, this->frameRateNorm);
			}
			lastFrameTime = frameTime;

			Media::FrameType ftype;
			Bool isOdd;
			if ((this->frameRateNorm / this->frameRateDenorm) >= 100)
			{
				isOdd = (i & 2) != 0;
			}
			else
			{
				isOdd = (i & 1) != 0;
			}
			if (this->frameInfo->ftype == Media::FT_FIELD_TF)
			{
				if (isOdd)
				{
					ftype = Media::FT_FIELD_BF;
				}
				else
				{
					ftype = Media::FT_FIELD_TF;
				}
			}
			else if (this->frameInfo->ftype == Media::FT_FIELD_BF)
			{
				if (isOdd)
				{
					ftype = Media::FT_FIELD_TF;
				}
				else
				{
					ftype = Media::FT_FIELD_BF;
				}
			}
			else
			{
				ftype = this->frameInfo->ftype;
			}
			if (!cb(frameTime, i, (UInt32)frameSize, this->frameIsKey->GetItem(i)?(Media::IVideoSource::FS_I):(Media::IVideoSource::FS_P), ftype, userData, Media::YCOFST_C_TOP_LEFT))
				break;
		}
		else
		{
			if (!cb(lastFrameTime, i, 0, Media::IVideoSource::FS_N, Media::FT_NON_INTERLACE, userData, Media::YCOFST_C_TOP_LEFT))
			{
				break;
			}
		}
		i++;
	}
}

UOSInt Media::FileVideoSource::GetFrameSize(UOSInt frameIndex)
{
	if (frameIndex >= frameSizes->GetCount())
		return 0;

	return this->frameSizes->GetItem(frameIndex);
}

UOSInt Media::FileVideoSource::ReadFrame(UOSInt frameIndex, UInt8 *frameBuff)
{
	if(frameIndex >= frameSizes->GetCount())
		return 0;

	UOSInt frameSize = this->frameSizes->GetItem(frameIndex);
	return this->data->GetRealData(this->frameOfsts->GetItem(frameIndex), frameSize, frameBuff);
}

UOSInt Media::FileVideoSource::ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype)
{
	return 0;
}
