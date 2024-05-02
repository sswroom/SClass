#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/IVideoSource.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "SSWR/AVIRead/AVIRVideoInfoForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	NN<DecodeStatus> status = userData.GetNN<DecodeStatus>();

	if (frameType != Media::FT_DISCARD)
	{
		status->sampleCnt++;
		status->lastSampleTime = frameTime;
	}
}

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnVideoChange(Media::IVideoSource::FrameChange frChg, AnyType userData)
{
	NN<DecodeStatus> status = userData.GetNN<DecodeStatus>();
	if (frChg == Media::IVideoSource::FC_ENDPLAY)
	{
		status->isEnd = true;
		status->evt->Set();
	}
}

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnAudioEnd(AnyType userData)
{
	NN<DecodeStatus> status = userData.GetNN<DecodeStatus>();
	status->isEnd = true;
	status->evt->Set();
}

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnFileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRVideoInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRVideoInfoForm>();
	UOSInt i = 0;
	Bool succ;
	UOSInt nFiles = files.GetCount();
	while (i < nFiles)
	{
		succ = me->OpenFile(files[i]->ToCString());
		if (succ)
			break;
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnStreamChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRVideoInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRVideoInfoForm>();
	UOSInt i = me->lbStream->GetSelectedIndex();
	NN<SSWR::AVIRead::AVIRVideoInfoForm::DecodeStatus> decStatus;
	if (me->currFile == 0)
	{
		me->txtStream->SetText(CSTR(""));
		return;
	}
	Int32 syncTime;
	NN<Media::IMediaSource> mediaSrc;
	if (!me->currFile->GetStream((UOSInt)i, syncTime).SetTo(mediaSrc))
	{
		me->txtStream->SetText(CSTR(""));
		return;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Sync Time = "));
	sb.AppendI32(syncTime);
	sb.AppendC(UTF8STRC("ms\r\n"));
	sb.AppendC(UTF8STRC("Can Seek = "));
	if (mediaSrc->CanSeek())
	{
		sb.AppendC(UTF8STRC("true"));
	}
	else
	{
		sb.AppendC(UTF8STRC("false"));
	}
	sb.AppendC(UTF8STRC("\r\n"));
	if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
	{
		NN<Media::IVideoSource> videoSrc = NN<Media::IVideoSource>::ConvertFrom(mediaSrc);
		Media::FrameInfo frameInfo;
		UInt32 rateNorm;
		UInt32 rateDenorm;
		UOSInt maxFrameSize;
		sb.AppendC(UTF8STRC("Media Type = Video\r\n"));
		sb.AppendC(UTF8STRC("Frame Count = "));
		sb.AppendUOSInt(videoSrc->GetFrameCount());
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Stream Time = "));
		sb.AppendDur(videoSrc->GetStreamTime());
		sb.AppendC(UTF8STRC("\r\n"));
		if (videoSrc->GetVideoInfo(frameInfo, rateNorm, rateDenorm, maxFrameSize))
		{
			sb.AppendC(UTF8STRC("Frame Rate = "));
			sb.AppendU32(rateNorm);
			sb.AppendC(UTF8STRC("/"));
			sb.AppendU32(rateDenorm);
			sb.AppendC(UTF8STRC(" ("));
			sb.AppendDouble(rateNorm / (Double)rateDenorm);
			sb.AppendC(UTF8STRC(")\r\n"));
			sb.AppendC(UTF8STRC("Max Frame Size = "));
			sb.AppendUOSInt(maxFrameSize);
			sb.AppendC(UTF8STRC("\r\n"));
			frameInfo.ToString(sb);
		}
		if (me->decStatus.GetItem(i).SetTo(decStatus))
		{
			sb.AppendC(UTF8STRC("\r\nDecoded Frame Count = "));
			sb.AppendU64(decStatus->sampleCnt);
			sb.AppendC(UTF8STRC("\r\nDecoded Frame Time = "));
			sb.AppendDur(decStatus->lastSampleTime);
		}
	}
	else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
	{
		Media::AudioFormat fmt;
		NN<Media::IAudioSource> audioSrc = NN<Media::IAudioSource>::ConvertFrom(mediaSrc);
		sb.AppendC(UTF8STRC("Media Type = Audio\r\n"));
		sb.AppendC(UTF8STRC("Sample Count = "));
		sb.AppendI64(audioSrc->GetSampleCount());
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Stream Time = "));
		sb.AppendDur(audioSrc->GetStreamTime());
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Min Block Size = "));
		sb.AppendUOSInt(audioSrc->GetMinBlockSize());
		sb.AppendC(UTF8STRC("\r\n"));
		audioSrc->GetFormat(fmt);
		fmt.ToString(sb);
		if (me->decStatus.GetItem(i).SetTo(decStatus))
		{
			sb.AppendC(UTF8STRC("\r\nDecoded sample Count = "));
			sb.AppendU64(decStatus->sampleCnt);
			sb.AppendC(UTF8STRC("\r\nDecoded Stream Time = "));
			sb.AppendDur(Data::Duration::FromRatioU64(decStatus->sampleCnt, fmt.frequency));
		}
	}

	me->txtStream->SetText(sb.ToCString());
}

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnDecodeClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRVideoInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRVideoInfoForm>();
	if (me->decStatus.GetCount() > 0 || me->currFile == 0)
	{
		return;
	}
	NN<SSWR::AVIRead::AVIRVideoInfoForm::DecodeStatus> status;
	NN<Media::IMediaSource> msrc;
	
	Sync::Event *evt;
	Media::Decoder::VideoDecoderFinder *vdecoders;
	Media::Decoder::AudioDecoderFinder *adecoders;
	NEW_CLASS(evt, Sync::Event(true));
	NEW_CLASS(vdecoders, Media::Decoder::VideoDecoderFinder());
	NEW_CLASS(adecoders, Media::Decoder::AudioDecoderFinder());
	Bool isEnd;
	UOSInt i = 0;
	Int32 syncTime;
	while (true)
	{
		if (!me->currFile->GetStream(i, syncTime).SetTo(msrc))
		{
			break;
		}
		status = MemAllocNN(SSWR::AVIRead::AVIRVideoInfoForm::DecodeStatus);
		status->sampleCnt = 0;
		status->lastSampleTime = 0;
		status->isEnd = false;
		status->adecoder = 0;
		status->vdecoder = 0;
		status->renderer = 0;
		status->evt = evt;
		if (msrc->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
		{
			status->vdecoder = vdecoders->DecodeVideo(NN<Media::IVideoSource>::ConvertFrom(msrc));
			if (status->vdecoder)
			{
				status->vdecoder->Init(OnVideoFrame, OnVideoChange, status);
			}
			else
			{
				status->isEnd = true;
			}
		}
		else if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
		{
			status->adecoder = adecoders->DecodeAudio(NN<Media::IAudioSource>::ConvertFrom(msrc));
			if (status->adecoder)
			{
				NEW_CLASS(status->renderer, Media::NullRenderer());
				status->renderer->BindAudio(status->adecoder);
				status->renderer->SetEndNotify(OnAudioEnd, status);
				status->renderer->AudioInit(0);
			}
			else
			{
				status->isEnd = true;
			}
		}

		me->decStatus.Add(status);
		i++;
	}
	i = me->decStatus.GetCount();
	while (i-- > 0)
	{
		status = me->decStatus.GetItemNoCheck(i);
		if (status->vdecoder)
		{
			status->vdecoder->Start();
		}
		if (status->adecoder)
		{
			status->renderer->Start();
		}
	}
	isEnd = false;
	while (!isEnd)
	{
		isEnd = true;
		i = me->decStatus.GetCount();
		while (i-- > 0)
		{
			status = me->decStatus.GetItemNoCheck(i);
			if (!status->isEnd)
			{
				isEnd = false;
			}
		}
		if (isEnd)
			break;
		evt->Wait(1000);
	}
	i = me->decStatus.GetCount();
	while (i-- > 0)
	{
		status = me->decStatus.GetItemNoCheck(i);
		if (status->renderer)
		{
			status->sampleCnt = status->renderer->GetSampleCnt();
		}
		SDEL_CLASS(status->renderer);
		SDEL_CLASS(status->vdecoder);
		SDEL_CLASS(status->adecoder);
	}
	DEL_CLASS(adecoders);
	DEL_CLASS(vdecoders);
	DEL_CLASS(evt);

	me->lblDecode->SetText(CSTR("End Decoding"));
}

Bool SSWR::AVIRead::AVIRVideoInfoForm::OpenFile(Text::CStringNN fileName)
{
	Media::MediaFile *mediaFile;
	{
		IO::StmData::FileData fd(fileName, false);
		mediaFile = (Media::MediaFile*)this->core->GetParserList()->ParseFileType(fd, IO::ParserType::MediaFile);
	}
	if (mediaFile == 0)
	{
		return false;
	}
	SDEL_CLASS(this->currFile);
	this->currFile = mediaFile;
	this->txtFile->SetText(fileName);

	this->lbStream->ClearItems();
	this->ClearDecode();
	UOSInt i;
	Int32 syncTime;
	Text::StringBuilderUTF8 sb;
	NN<Media::IMediaSource> mediaSrc;
	i = 0;
	while (true)
	{
		if (!mediaFile->GetStream(i, syncTime).SetTo(mediaSrc))
			break;
		
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Stream "));
		sb.AppendUOSInt(i);
		if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
		{
			sb.AppendC(UTF8STRC(" (Video)"));
		}
		else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
		{
			sb.AppendC(UTF8STRC(" (Audio)"));
		}
		else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_SUBTITLE)
		{
			sb.AppendC(UTF8STRC(" (Subtitle)"));
		}
		this->lbStream->AddItem(sb.ToCString(), (void*)i);
		i++;
	}
	return true;
}

void SSWR::AVIRead::AVIRVideoInfoForm::ClearDecode()
{
	this->decStatus.MemFreeAll();
	this->lblDecode->SetText(CSTR(""));
}

SSWR::AVIRead::AVIRVideoInfoForm::AVIRVideoInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Video Info"));
	
	this->core = core;
	this->currFile = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlFile = ui->NewPanel(*this);
	this->pnlFile->SetRect(0, 0, 100, 55, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFile = ui->NewLabel(this->pnlFile, CSTR("File Name"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	this->txtFile = ui->NewTextBox(this->pnlFile, CSTR(""));
	this->txtFile->SetRect(104, 4, 800, 23, false);
	this->txtFile->SetReadOnly(true);
	this->btnDecode = ui->NewButton(this->pnlFile, CSTR("Test Decode"));
	this->btnDecode->SetRect(104, 28, 150, 23, false);
	this->btnDecode->HandleButtonClick(OnDecodeClicked, this);
	this->lblDecode = ui->NewLabel(this->pnlFile, CSTR(""));
	this->lblDecode->SetRect(254, 28, 100, 23, false);
	this->lbStream = ui->NewListBox(*this, false);
	this->lbStream->SetRect(0, 0, 150, 23, false);
	this->lbStream->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbStream->HandleSelectionChange(OnStreamChg, this);
	this->hspStream = ui->NewHSplitter(*this, 3, false);
	this->txtStream = ui->NewTextBox(*this, CSTR(""), true);
	this->txtStream->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtStream->SetReadOnly(true);

	this->HandleDropFiles(OnFileHandler, this);
}

SSWR::AVIRead::AVIRVideoInfoForm::~AVIRVideoInfoForm()
{
	this->ClearDecode();
	SDEL_CLASS(this->currFile);
}

void SSWR::AVIRead::AVIRVideoInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
