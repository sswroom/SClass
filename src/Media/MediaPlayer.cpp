#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Media/MediaPlayer.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

extern "C"
{
	void MediaPlayer_VideoCropImageY(UInt8 *yptr, UIntOS w, UIntOS h, UIntOS ySplit, UIntOS *crops);
}

void Media::MediaPlayer::PlayTime(Data::Duration time)
{
	NN<Media::VideoSource> vdecoder;
	NN<Media::AudioSource> adecoder;
	NN<Media::VideoSource> currVStm;
	NN<Media::AudioSource> currAStm;
	if (this->currVDecoder.SetTo(vdecoder))
	{
		vdecoder->SeekToTime(time);
	}
	else if (this->currVStm.SetTo(currVStm))
	{
		currVStm->SeekToTime(time);
	}

	if (this->currADecoder.SetTo(adecoder))
	{
		time = adecoder->SeekToTime(time);
	}
	else if (this->currAStm.SetTo(currAStm))
	{
		time = currAStm->SeekToTime(time);
	}
	else
	{
	}
	Bool found = false;
	NN<Media::VideoRenderer> vrenderer;
	NN<Media::AudioRenderer> arenderer;
	if (this->arenderer.SetTo(arenderer))
	{
		arenderer->AudioInit(this->clk);
		arenderer->SetEndNotify(OnAudioEnd, this);
	}
	if (this->vrenderer.SetTo(vrenderer))
	{
		vrenderer->VideoInit(this->clk);
		vrenderer->SetEndNotify(OnVideoEnd, this);
	}
	if (this->arenderer.SetTo(arenderer))
	{
		this->audioPlaying = true;
		arenderer->Start();
#if defined(VERBOSE)
		printf("MediaPlayer: audio started\r\n");
#endif
		found = true;
	}
	if (this->vrenderer.SetTo(vrenderer))
	{
		this->videoPlaying = true;
		vrenderer->VideoStart();
		found = true;
#if defined(VERBOSE)
		printf("MediaPlayer: video started\r\n");
#endif
	}
	if (found)
	{
		this->playing = true;
#if defined(VERBOSE)
		printf("MediaPlayer: playing\r\n");
#endif
	}
}

void __stdcall Media::MediaPlayer::OnVideoEnd(AnyType userObj)
{
	NN<Media::MediaPlayer> me = userObj.GetNN<Media::MediaPlayer>();
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

void __stdcall Media::MediaPlayer::OnAudioEnd(AnyType userObj)
{
	NN<Media::MediaPlayer> me = userObj.GetNN<Media::MediaPlayer>();
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

void __stdcall Media::MediaPlayer::VideoCropImage(AnyType userObj, Data::Duration frameTime, UInt32 frameNum, NN<Media::StaticImage> img)
{
	NN<Media::MediaPlayer> me = userObj.GetNN<Media::MediaPlayer>();
	UIntOS w = img->info.dispSize.x;
	UIntOS h = img->info.dispSize.y;
	UnsafeArray<UInt8> yptr = img->data;
	UIntOS ySplit;
	UIntOS crops[4];
	NN<Media::VideoRenderer> vrenderer;
	if (!me->vrenderer.SetTo(vrenderer))
	{
		img.Delete();
		return;
	}
	if (img->info.fourcc == *(UInt32*)"YV12")
	{
		if (w & 3)
		{
			w = w + 4 - (w & 3);
		}
		ySplit = 1;
	}
	else if (img->info.fourcc == *(UInt32*)"YUY2")
	{
		ySplit = 2;
	}
	else
	{
		img.Delete();
		return;
	}
	MediaPlayer_VideoCropImageY(yptr.Ptr(), w, h, ySplit, crops);
	img.Delete();
	NN<Media::VideoSource> vdecoder;
	if (me->currVDecoder.SetTo(vdecoder))
		vdecoder->SetBorderCrop(crops[0], crops[1], crops[2], crops[3]);
	vrenderer->UpdateCrop();
}

void Media::MediaPlayer::ReleaseAudio()
{
	NN<Media::AudioDevice> audioDev;
	if (this->audioDev.SetTo(audioDev)) audioDev->BindAudio(nullptr);
	this->arenderer = nullptr;
	this->currADecoder.Delete();
}

Bool Media::MediaPlayer::SwitchAudioSource(NN<Media::AudioSource> asrc, Int32 syncTime)
{
	NN<Media::AudioDevice> audioDev;
	NN<Media::AudioSource> adecoder;
	Bool ret = false;
	if (!this->audioDev.SetTo(audioDev))
	{
		return false;
	}
	NN<Media::VideoRenderer> vrenderer;
	if (this->arenderer.NotNull())
	{
		if (this->vrenderer.SetTo(vrenderer)) vrenderer->SetTimeDelay(syncTime);
		this->currAStm = asrc.Ptr();
		ret = true;
	}
	else
	{
		this->currADecoder = this->adecoders.DecodeAudio(asrc);
		if (this->currADecoder.SetTo(adecoder))
		{
			if ((this->arenderer = audioDev->BindAudio(adecoder)).NotNull())
			{
				if (this->vrenderer.SetTo(vrenderer)) vrenderer->SetTimeDelay(syncTime);
				this->currAStm = asrc.Ptr();
				ret = true;
			}
			else
			{
				this->currADecoder.Delete();
			}
		}
	}
	return ret;
}

Media::MediaPlayer::MediaPlayer(NN<Media::VideoRenderer> vrenderer, Optional<Media::AudioDevice> audioDev)
{
	this->audioDev = audioDev;
	this->vrenderer = vrenderer;

	this->currFile = nullptr;
	this->currChapInfo = nullptr;
	this->arenderer = nullptr;
	this->currADecoder = nullptr;
	this->currVDecoder = nullptr;
	this->currVStm = nullptr;
	this->currAStm = nullptr;
	this->playing = false;
	this->videoPlaying = false;
	this->audioPlaying = false;
	this->endHdlr = 0;
	this->endObj = 0;
}

Media::MediaPlayer::~MediaPlayer()
{
	if (this->currFile.NotNull())
	{
		this->LoadMedia(nullptr);
	}
}

void Media::MediaPlayer::SetEndHandler(PBEndHandler hdlr, AnyType userObj)
{
	this->endHdlr = hdlr;
	this->endObj = userObj;
}

Bool Media::MediaPlayer::LoadMedia(Optional<Media::MediaFile> file)
{
	NN<Media::VideoSource> vdecoder;
	NN<Media::AudioRenderer> arenderer;
	Bool videoFound;
	this->currFile = file;
	if (this->arenderer.SetTo(arenderer)) arenderer->Stop();
	NN<Media::VideoRenderer> vrenderer;
	if (this->vrenderer.SetTo(vrenderer))
	{
		vrenderer->StopPlay();
		vrenderer->SetVideo(nullptr);
	}
	this->ReleaseAudio();
	this->currAStm = nullptr;
	this->currVStm = nullptr;
	this->currChapInfo = nullptr;
	this->currVDecoder.Delete();
	NN<Media::MediaFile> currFile;
	if (!this->currFile.SetTo(currFile) || !this->vrenderer.SetTo(vrenderer))
	{
		return true;
	}
	this->currChapInfo = currFile->GetChapterInfo();

	UIntOS i = 0;
	Int32 syncTime;
	videoFound = false;
	while (true)
	{
		Media::MediaType mt;
		NN<Media::MediaSource> msrc;
		if (!currFile->GetStream(i, syncTime).SetTo(msrc))
			break;
		mt = msrc->GetMediaType();
		if (mt == Media::MEDIA_TYPE_VIDEO && !videoFound)
		{
			NN<Media::VideoSource> vsrc = NN<Media::VideoSource>::ConvertFrom(msrc);
			this->currVDecoder = this->vdecoders.DecodeVideo(vsrc);
			if (this->currVDecoder.SetTo(vdecoder))
			{
				this->currVStm = vsrc.Ptr();
				vrenderer->SetVideo(vdecoder);
				this->clk.Stop();
			}
			else
			{
				this->currVStm = vsrc.Ptr();
				vrenderer->SetVideo(this->currVStm);
				this->clk.Stop();
			}
		}
		else if (mt == Media::MEDIA_TYPE_AUDIO && this->arenderer.IsNull())
		{
			this->SwitchAudioSource(NN<Media::AudioSource>::ConvertFrom(msrc), syncTime);
		}
		i++;
	}
	vrenderer->SetHasAudio(this->arenderer.NotNull());
	this->currTime = 0;

	return videoFound || this->arenderer.NotNull();
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
	NN<Media::AudioRenderer> arenderer;
	NN<Media::VideoRenderer> vrenderer;
	if (this->vrenderer.SetTo(vrenderer))
	{
		vrenderer->StopPlay();
	}
	if (this->arenderer.SetTo(arenderer))
	{
		arenderer->Stop();
	}
	this->videoPlaying = false;
	this->audioPlaying = false;
	this->currTime = 0;
	return true;
}

Bool Media::MediaPlayer::SeekTo(Data::Duration time)
{
	if (IsPlaying())
	{
		this->StopPlayback();
		this->PlayTime(time);
		this->pbLastChapter = (UIntOS)-1;
	}
	else
	{
		this->currTime = time;
	}
	return true;
}

Bool Media::MediaPlayer::SwitchAudio(UIntOS index)
{
	this->ReleaseAudio();
	NN<Media::MediaFile> currFile;
	if (!this->currFile.SetTo(currFile))
	{
		return true;
	}

	UIntOS i = 0;
	Int32 syncTime;
	while (true)
	{
		Media::MediaType mt;
		NN<Media::MediaSource> msrc;
		if (!currFile->GetStream(i, syncTime).SetTo(msrc))
			break;
		mt = msrc->GetMediaType();
		if (mt == Media::MEDIA_TYPE_AUDIO)
		{
			if (index == 0)
			{
				return this->SwitchAudioSource(NN<Media::AudioSource>::ConvertFrom(msrc), syncTime);
			}
			index--;
		}
		i++;
	}
	return false;
}

Bool Media::MediaPlayer::IsPlaying()
{
	NN<Media::AudioRenderer> arenderer;
	NN<Media::VideoSource> currVStm;
	if (this->arenderer.SetTo(arenderer) && arenderer->IsPlaying())
	{
		return true;
	}
	if (this->currVStm.SetTo(currVStm))
	{
		if (currVStm->IsRunning())
			return true;
	}
	return false;
}

Bool Media::MediaPlayer::PrevChapter()
{
	NN<Media::ChapterInfo> currChapInfo;
	UIntOS i;
	if (this->IsPlaying() && this->currChapInfo.SetTo(currChapInfo))
	{
		currTime = this->clk.GetCurrTime();
		i = currChapInfo->GetChapterIndex(currTime);
		if (i + 1 == this->pbLastChapter)
		{
			i++;
		}
		i--;
		if (i < 0)
		{
			i = 0;
		}
		this->SeekTo(currChapInfo->GetChapterTime(i));
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
	NN<Media::ChapterInfo> currChapInfo;
	UIntOS i;
	if (this->currChapInfo.SetTo(currChapInfo) && this->IsPlaying())
	{
		currTime = this->clk.GetCurrTime();
		i = currChapInfo->GetChapterIndex(currTime);
		if (i + 1 == this->pbLastChapter)
		{
			i++;
		}
		if (i < currChapInfo->GetChapterCnt() - 1)
		{
			this->SeekTo(currChapInfo->GetChapterTime(i + 1));
			this->pbLastChapter = i + 1;
		}
		return true;
	}
	return false;
}

Bool Media::MediaPlayer::GotoChapter(UIntOS chapter)
{
	NN<Media::ChapterInfo> currChapInfo;
	if (this->currChapInfo.SetTo(currChapInfo))
	{
		this->SeekTo(currChapInfo->GetChapterTime(chapter));
		this->pbLastChapter = chapter;
		return true;
	}
	else
	{
		return false;
	}
}

Data::Duration Media::MediaPlayer::GetCurrTime()
{
	return this->clk.GetCurrTime();
}

Bool Media::MediaPlayer::GetVideoSize(OutParam<UIntOS> w, OutParam<UIntOS> h)
{
	Media::FrameInfo info;
	UIntOS cropLeft;
	UIntOS cropTop;
	UIntOS cropRight;
	UIntOS cropBottom;
	UIntOS vw;
	UIntOS vh;
	UInt32 tmpV;
	NN<Media::VideoSource> vdecoder;
	NN<Media::VideoSource> currVStm;
	if (this->currVStm.SetTo(currVStm))
	{
		if (this->currVDecoder.SetTo(vdecoder))
		{
			vdecoder->GetVideoInfo(info, tmpV, tmpV, vw);
		}
		else
		{
			currVStm->GetVideoInfo(info, tmpV, tmpV, vw);
		}
		currVStm->GetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
		vw = info.dispSize.x - cropLeft - cropRight;
		vh = info.dispSize.y - cropTop - cropBottom;
		if (info.ftype == Media::FT_FIELD_BF || info.ftype == Media::FT_FIELD_TF)
		{
			vh = vh << 1;
		}
		if (info.par2 > 1)
		{
			vh = (UInt32)Double2Int32(UIntOS2Double(vh) * info.par2);
		}
		else
		{
			vw = (UInt32)Double2Int32(UIntOS2Double(vw) / info.par2);
		}
		w.Set(vw);
		h.Set(vh);
		return true;
	}
	else
	{
		return false;
	}
}

void Media::MediaPlayer::DetectCrop()
{
	NN<Media::VideoSource> vdecoder;
	if (this->currVDecoder.SetTo(vdecoder))
	{
		vdecoder->CaptureImage(VideoCropImage, this);
	}
}

Optional<Media::VideoRenderer> Media::MediaPlayer::GetVideoRenderer()
{
	return this->vrenderer;
}

void Media::MediaPlayer::Close()
{
	this->StopPlayback();
	this->vrenderer = nullptr;
}