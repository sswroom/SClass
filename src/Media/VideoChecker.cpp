#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VideoChecker.h"

void __stdcall Media::VideoChecker::OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	NN<DecodeStatus> status = userData.GetNN<DecodeStatus>();
	if (frameType != Media::FT_DISCARD)
	{
		status->sampleCnt++;
		status->lastSampleTime = frameTime;
	}
}

void __stdcall Media::VideoChecker::OnVideoChange(Media::VideoSource::FrameChange frChg, AnyType userData)
{
	NN<DecodeStatus> status = userData.GetNN<DecodeStatus>();
	if (frChg == Media::VideoSource::FC_ENDPLAY)
	{
		status->isEnd = true;
		status->evt->Set();
	}
}

void __stdcall Media::VideoChecker::OnAudioEnd(AnyType userData)
{
	NN<DecodeStatus> status = userData.GetNN<DecodeStatus>();
	status->isEnd = true;
	status->evt->Set();
}

Media::VideoChecker::VideoChecker(Bool allowTimeSkip)
{
	this->allowTimeSkip = allowTimeSkip;
}

Media::VideoChecker::~VideoChecker()
{
}

void Media::VideoChecker::SetAllowTimeSkip(Bool allowTimeSkip)
{
	this->allowTimeSkip = allowTimeSkip;
}

Bool Media::VideoChecker::IsValid(NN<Media::MediaFile> mediaFile)
{
	DecodeStatus *status;
	NN<Media::MediaSource> msrc;
	NN<Media::AudioSource> adecoder;
	NN<Media::VideoSource> vdecoder;
	Data::ArrayList<DecodeStatus*> statusList;
	Bool isEnd;
	UOSInt i = 0;
	Int32 syncTime;
	Bool valid = true;
	Data::Duration videoTime = 0;
	while (true)
	{
		if (!mediaFile->GetStream(i, syncTime).SetTo(msrc))
		{
			break;
		}
		status = MemAlloc(DecodeStatus, 1);
		status->sampleCnt = 0;
		status->lastSampleTime = 0;
		status->isEnd = false;
		status->adecoder = 0;
		status->vdecoder = 0;
		status->renderer = 0;
		status->evt = evt;
		if (msrc->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
		{
			status->vdecoder = vdecoders.DecodeVideo(NN<Media::VideoSource>::ConvertFrom(msrc));
			if (status->vdecoder.SetTo(vdecoder))
			{
				vdecoder->Init(OnVideoFrame, OnVideoChange, status);
			}
			else
			{
				status->isEnd = true;
			}
		}
		else if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
		{
			status->adecoder = adecoders.DecodeAudio(NN<Media::AudioSource>::ConvertFrom(msrc));
			if (status->adecoder.SetTo(adecoder))
			{
				NEW_CLASS(status->renderer, Media::NullRenderer());
				status->renderer->BindAudio(adecoder);
				status->renderer->SetEndNotify(OnAudioEnd, status);
				status->renderer->AudioInit(0);
			}
			else
			{
				status->isEnd = true;
			}
		}
		else
		{
			valid = false;
		}

		statusList.Add(status);
		i++;
	}
	if (valid)
	{
		i = statusList.GetCount();
		while (i-- > 0)
		{
			status = statusList.GetItem(i);
			if (status->vdecoder.SetTo(vdecoder))
			{
				vdecoder->Start();
			}
			if (status->adecoder.NotNull())
			{
				status->renderer->Start();
			}
		}
		isEnd = false;
		while (!isEnd)
		{
			isEnd = true;
			i = statusList.GetCount();
			while (i-- > 0)
			{
				status = statusList.GetItem(i);
				if (!status->isEnd)
				{
					isEnd = false;
				}
			}
			if (isEnd)
				break;
			evt.Wait(1000);
		}

		i = statusList.GetCount();
		while (i-- > 0)
		{
			status = statusList.GetItem(i);
			if (mediaFile->GetStream(i, syncTime).SetTo(msrc))
			{
				if (msrc->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
				{
					videoTime = status->lastSampleTime;
				}
			}
		}

		i = statusList.GetCount();
		while (i-- > 0)
		{
			status = statusList.GetItem(i);
			if (status->renderer)
			{
				status->sampleCnt = status->renderer->GetSampleCnt();
			}
			SDEL_CLASS(status->renderer);
			status->vdecoder.Delete();
			status->adecoder.Delete();

			if (mediaFile->GetStream(i, syncTime).SetTo(msrc))
			{
				if (msrc->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
				{
					Media::VideoSource *video = (Media::VideoSource *)msrc.Ptr();
					OSInt frameDiff = (OSInt)(video->GetFrameCount() - status->sampleCnt);
					if (frameDiff < -10 || frameDiff > 10)
					{
						valid = false;
					}
				}
				else if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
				{
					Media::AudioSource *audio = (Media::AudioSource *)msrc.Ptr();
					Media::AudioFormat fmt;
					audio->GetFormat(fmt);
					Data::Duration tdiff = audio->GetStreamTime() - Data::Duration::FromRatioU64(status->sampleCnt, fmt.frequency);
					Data::Duration tdiff2 = videoTime - Data::Duration::FromRatioU64(status->sampleCnt, fmt.frequency);
					if (this->allowTimeSkip)
					{
						if ((tdiff.GetSeconds() <= -1 || tdiff.GetSeconds() >= 1) && (tdiff2.GetSeconds() <= -1 || tdiff2.GetSeconds() >= 1))
						{
							valid = false;
						}
					}
					else
					{
						if ((tdiff.GetSeconds() <= -1 || tdiff.GetSeconds() >= 1) || (tdiff2.GetSeconds() <= -1 || tdiff2.GetSeconds() >= 1))
						{
							valid = false;
						}
					}
				}
				else
				{
					valid = false;
				}
			}
			else
			{
				valid = false;
			}
		}
		
	}
	i = statusList.GetCount();
	while (i-- > 0)
	{
		status = statusList.GetItem(i);
		MemFree(status);
	}
	return valid;
}
