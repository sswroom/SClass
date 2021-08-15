#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/MediaPlayer.h"

extern "C"
{
	void MediaPlayer_VideoCropImageY(UInt8 *yptr, UOSInt w, UOSInt h, UOSInt ySplit, UOSInt *crops);
}

void Media::MediaPlayer::PlayTime(UInt32 time)
{
	if (this->currVDecoder)
	{
		this->currVDecoder->SeekToTime(time);
	}
	else if (this->currVStm)
	{
		this->currVStm->SeekToTime(time);
	}

	if (this->currADecoder)
	{
		time = this->currADecoder->SeekToTime(time);
	}
	else if (this->currAStm)
	{
		time = this->currAStm->SeekToTime(time);
	}
	else
	{
	}
	Bool found = false;
	if (this->arenderer)
	{
		this->arenderer->AudioInit(this->clk);
		this->arenderer->SetEndNotify(OnAudioEnd, this);
	}
	if (this->vrenderer)
	{
		this->vrenderer->VideoInit(this->clk);
		this->vrenderer->SetEndNotify(OnVideoEnd, this);
	}
	if (this->arenderer)
	{
		this->audioPlaying = true;
		this->arenderer->Start();
		found = true;
	}
	if (this->vrenderer)
	{
		this->videoPlaying = true;
		this->vrenderer->VideoStart();
		found = true;
	}
	if (found)
	{
		this->playing = true;
	}
}

void __stdcall Media::MediaPlayer::OnVideoEnd(void *userObj)
{
	Media::MediaPlayer *me = (Media::MediaPlayer*)userObj;
	Bool audPlaying = me->audioPlaying;
	me->videoPlaying = false;
	if (!audPlaying)
	{
		if (me->endHdlr)
		{
			me->endHdlr(me->endObj);
		}
	}
}

void __stdcall Media::MediaPlayer::OnAudioEnd(void *userObj)
{
	Media::MediaPlayer *me = (Media::MediaPlayer*)userObj;
	Bool vidPlaying = me->videoPlaying;
	me->audioPlaying = false;
	if (!vidPlaying)
	{
		if (me->endHdlr)
		{
			me->endHdlr(me->endObj);
		}
	}
}

void __stdcall Media::MediaPlayer::VideoCropImage(void *userObj, UInt32 frameTime, UInt32 frameNum, Media::StaticImage *img)
{
	Media::MediaPlayer *me = (Media::MediaPlayer*)userObj;
	UOSInt w = img->info->dispWidth;
	UOSInt h = img->info->dispHeight;
	UInt8 *yptr = img->data;
	UOSInt ySplit;
	UOSInt crops[4];
	if (img->info->fourcc == *(UInt32*)"YV12")
	{
		if (w & 3)
		{
			w = w + 4 - (w & 3);
		}
		ySplit = 1;
	}
	else if (img->info->fourcc == *(UInt32*)"YUY2")
	{
		ySplit = 2;
	}
	else
	{
		DEL_CLASS(img);
		return;
	}
	MediaPlayer_VideoCropImageY(yptr, w, h, ySplit, crops);
	DEL_CLASS(img);
	me->currVDecoder->SetBorderCrop(crops[0], crops[1], crops[2], crops[3]);
	me->vrenderer->UpdateCrop();
}

void Media::MediaPlayer::ReleaseAudio()
{
	this->audioDev->BindAudio(0);
	this->arenderer = 0;
	SDEL_CLASS(this->currADecoder);
}

Bool Media::MediaPlayer::SwitchAudioSource(Media::IAudioSource *asrc, Int32 syncTime)
{
	Bool ret = false;
	this->arenderer = this->audioDev->BindAudio(asrc);
	if (this->arenderer)
	{
		this->vrenderer->SetTimeDelay(syncTime);
		this->currAStm = asrc;
		ret = true;
	}
	else
	{
		this->currADecoder = this->adecoders->DecodeAudio(asrc);
		if (this->currADecoder)
		{
			if ((this->arenderer = this->audioDev->BindAudio(this->currADecoder)) != 0)
			{
				this->vrenderer->SetTimeDelay(syncTime);
				this->currAStm = asrc;
				ret = true;
			}
			else
			{
				SDEL_CLASS(this->currADecoder);
			}
		}
	}
	return ret;
}

Media::MediaPlayer::MediaPlayer(Media::VideoRenderer *vrenderer, Media::AudioDevice *audioDev)
{
	this->audioDev = audioDev;
	this->vrenderer = vrenderer;
	NEW_CLASS(this->vdecoders, Media::Decoder::VideoDecoderFinder());
	NEW_CLASS(this->adecoders, Media::Decoder::AudioDecoderFinder());
	NEW_CLASS(this->clk, Media::RefClock());

	this->currFile = 0;
	this->currChapInfo = 0;
	this->arenderer = 0;
	this->currADecoder = 0;
	this->currVDecoder = 0;
	this->playing = false;
	this->videoPlaying = false;
	this->audioPlaying = false;
	this->endHdlr = 0;
	this->endObj = 0;
}

Media::MediaPlayer::~MediaPlayer()
{
	this->LoadMedia(0);
	DEL_CLASS(this->clk);
	DEL_CLASS(this->vdecoders);
	DEL_CLASS(this->adecoders);
}

void Media::MediaPlayer::SetEndHandler(PBEndHandler hdlr, void *userObj)
{
	this->endHdlr = hdlr;
	this->endObj = userObj;
}

Bool Media::MediaPlayer::LoadMedia(Media::MediaFile *file)
{
	Bool videoFound;
	this->currFile = file;
	if (this->arenderer) this->arenderer->Stop();
	if (this->vrenderer)
	{
		this->vrenderer->StopPlay();
		this->vrenderer->SetVideo(0);
	}
	this->ReleaseAudio();
	this->currAStm = 0;
	this->currVStm = 0;
	this->currChapInfo = 0;
	SDEL_CLASS(this->currVDecoder);
	if (this->currFile == 0)
	{
		return true;
	}
	this->currChapInfo = this->currFile->GetChapterInfo();

	UOSInt i = 0;
	Int32 syncTime;
	videoFound = false;
	while (true)
	{
		Media::MediaType mt;
		Media::IMediaSource *msrc = this->currFile->GetStream(i, &syncTime);
		if (msrc == 0)
			break;
		mt = msrc->GetMediaType();
		if (mt == Media::MEDIA_TYPE_VIDEO && !videoFound)
		{
			Media::IVideoSource *vsrc = (Media::IVideoSource*)msrc;
			this->currVDecoder = this->vdecoders->DecodeVideo(vsrc);
			if (this->currVDecoder)
			{
				this->currVStm = vsrc;
				this->vrenderer->SetVideo(this->currVDecoder);
				this->clk->Stop();
			}
			else
			{
				this->currVStm = vsrc;
				this->vrenderer->SetVideo(this->currVStm);
				this->clk->Stop();
			}
		}
		else if (mt == Media::MEDIA_TYPE_AUDIO && this->arenderer == 0)
		{
			this->SwitchAudioSource((Media::IAudioSource*)msrc, syncTime);
		}
		i++;
	}
	this->vrenderer->SetHasAudio(this->arenderer != 0);
	this->currTime = 0;

	return videoFound || this->arenderer;
}

Bool Media::MediaPlayer::StartPlayback()
{
	if (this->IsPlaying())
		return true;
	PlayTime(this->currTime);
	this->pbLastChapter = 0;
	return this->playing;
}

Bool Media::MediaPlayer::StopPlayback()
{
	this->playing = false;
	if (this->vrenderer)
	{
		this->vrenderer->StopPlay();
	}
	if (this->arenderer)
	{
		this->arenderer->Stop();
	}
	this->videoPlaying = false;
	this->audioPlaying = false;
	this->currTime = 0;
	return true;
}

Bool Media::MediaPlayer::SeekTo(UInt32 time)
{
	if (IsPlaying())
	{
		this->StopPlayback();
		this->PlayTime(time);
		this->pbLastChapter = (UOSInt)-1;
	}
	else
	{
		this->currTime = time;
	}
	return true;
}

Bool Media::MediaPlayer::SwitchAudio(UOSInt index)
{
	this->ReleaseAudio();
	if (this->currFile == 0)
	{
		return true;
	}

	UOSInt i = 0;
	Int32 syncTime;
	while (true)
	{
		Media::MediaType mt;
		Media::IMediaSource *msrc = this->currFile->GetStream(i, &syncTime);
		if (msrc == 0)
			break;
		mt = msrc->GetMediaType();
		if (mt == Media::MEDIA_TYPE_AUDIO)
		{
			if (index == 0)
			{
				return this->SwitchAudioSource((Media::IAudioSource*)msrc, syncTime);
			}
			index--;
		}
		i++;
	}
	return false;
}

Bool Media::MediaPlayer::IsPlaying()
{
	if (this->arenderer && this->arenderer->IsPlaying())
	{
		return true;
	}
	if (this->currVStm)
	{
		if (this->currVStm->IsRunning())
			return true;
	}
	return false;
}

Bool Media::MediaPlayer::PrevChapter()
{
	UOSInt i;
	if (this->IsPlaying() && this->currChapInfo)
	{
		currTime = this->clk->GetCurrTime();
		i = this->currChapInfo->GetChapterIndex(currTime);
		if (i + 1 == this->pbLastChapter)
		{
			i++;
		}
		i--;
		if (i < 0)
		{
			i = 0;
		}
		this->SeekTo(this->currChapInfo->GetChapterTime(i));
		this->pbLastChapter = i;
		return true;
	}
	else
	{
		return false;
	}
}

Bool Media::MediaPlayer::NextChapter()
{
	UOSInt i;
	if (this->currChapInfo && this->IsPlaying())
	{
		currTime = this->clk->GetCurrTime();
		i = this->currChapInfo->GetChapterIndex(currTime);
		if (i + 1 == this->pbLastChapter)
		{
			i++;
		}
		if (i < this->currChapInfo->GetChapterCnt() - 1)
		{
			this->SeekTo(this->currChapInfo->GetChapterTime(i + 1));
			this->pbLastChapter = i + 1;
		}
		return true;
	}
	return false;
}

Bool Media::MediaPlayer::GotoChapter(UOSInt chapter)
{
	if (this->currChapInfo)
	{
		this->SeekTo(this->currChapInfo->GetChapterTime(chapter));
		this->pbLastChapter = chapter;
		return true;
	}
	else
	{
		return false;
	}
}

UInt32 Media::MediaPlayer::GetCurrTime()
{
	return this->clk->GetCurrTime();
}

Bool Media::MediaPlayer::GetVideoSize(UOSInt *w, UOSInt *h)
{
	Media::FrameInfo info;
	UOSInt cropLeft;
	UOSInt cropTop;
	UOSInt cropRight;
	UOSInt cropBottom;
	UOSInt vw;
	UOSInt vh;
	UInt32 tmpV;
	if (this->currVStm)
	{
		if (this->currVDecoder)
		{
			this->currVDecoder->GetVideoInfo(&info, &tmpV, &tmpV, &vw);
		}
		else
		{
			this->currVStm->GetVideoInfo(&info, &tmpV, &tmpV, &vw);
		}
		this->currVStm->GetBorderCrop(&cropLeft, &cropTop, &cropRight, &cropBottom);
		vw = info.dispWidth - cropLeft - cropRight;
		vh = info.dispHeight - cropTop - cropBottom;
		if (info.ftype == Media::FT_FIELD_BF || info.ftype == Media::FT_FIELD_TF)
		{
			vh = vh << 1;
		}
		if (info.par2 > 1)
		{
			vh = (UInt32)Math::Double2Int32(Math::UOSInt2Double(vh) * info.par2);
		}
		else
		{
			vw = (UInt32)Math::Double2Int32(Math::UOSInt2Double(vw) / info.par2);
		}
		*w = vw;
		*h = vh;
		return true;
	}
	else
	{
		return false;
	}
}

void Media::MediaPlayer::DetectCrop()
{
	if (this->currVDecoder)
	{
		this->currVDecoder->CaptureImage(VideoCropImage, this);
	}
}
