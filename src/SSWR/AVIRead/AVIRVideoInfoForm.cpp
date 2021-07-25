#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/IVideoSource.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "SSWR/AVIRead/AVIRVideoInfoForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	DecodeStatus *status = (DecodeStatus *)userData;

	if (frameType != Media::FT_DISCARD)
	{
		status->sampleCnt++;
		status->lastSampleTime = frameTime;
	}
}

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnVideoChange(Media::IVideoSource::FrameChange frChg, void *userData)
{
	DecodeStatus *status = (DecodeStatus *)userData;
	if (frChg == Media::IVideoSource::FC_ENDPLAY)
	{
		status->isEnd = true;
		status->evt->Set();
	}
}

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnAudioEnd(void *userData)
{
	DecodeStatus *status = (DecodeStatus *)userData;
	status->isEnd = true;
	status->evt->Set();
}

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnFileHandler(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRVideoInfoForm *me = (SSWR::AVIRead::AVIRVideoInfoForm*)userObj;
	UOSInt i = 0;
	Bool succ;
	while (i < nFiles)
	{
		succ = me->OpenFile(files[i]);
		if (succ)
			break;
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnStreamChg(void *userObj)
{
	SSWR::AVIRead::AVIRVideoInfoForm *me = (SSWR::AVIRead::AVIRVideoInfoForm*)userObj;
	UOSInt i = me->lbStream->GetSelectedIndex();
	SSWR::AVIRead::AVIRVideoInfoForm::DecodeStatus *decStatus;
	if (me->currFile == 0)
	{
		me->txtStream->SetText((const UTF8Char*)"");
		return;
	}
	Int32 syncTime;
	Media::IMediaSource *mediaSrc = me->currFile->GetStream((UOSInt)i, &syncTime);
	if (mediaSrc == 0)
	{
		me->txtStream->SetText((const UTF8Char*)"");
		return;
	}
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Sync Time = ");
	sb.AppendI32(syncTime);
	sb.Append((const UTF8Char*)"ms\r\n");
	sb.Append((const UTF8Char*)"Can Seek = ");
	sb.Append((const UTF8Char*)(mediaSrc->CanSeek()?"true":"false"));
	sb.Append((const UTF8Char*)"\r\n");
	if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
	{
		Media::IVideoSource *videoSrc = (Media::IVideoSource*)mediaSrc;
		Media::FrameInfo frameInfo;
		UInt32 rateNorm;
		UInt32 rateDenorm;
		UOSInt maxFrameSize;
		sb.Append((const UTF8Char*)"Media Type = Video\r\n");
		sb.Append((const UTF8Char*)"Frame Count = ");
		sb.AppendUOSInt(videoSrc->GetFrameCount());
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Stream Time = ");
		me->AppendTime(&sb, (UInt32)videoSrc->GetStreamTime());
		sb.Append((const UTF8Char*)"\r\n");
		if (videoSrc->GetVideoInfo(&frameInfo, &rateNorm, &rateDenorm, &maxFrameSize))
		{
			sb.Append((const UTF8Char*)"Frame Rate = ");
			sb.AppendU32(rateNorm);
			sb.Append((const UTF8Char*)"/");
			sb.AppendU32(rateDenorm);
			sb.Append((const UTF8Char*)" (");
			Text::SBAppendF64(&sb, rateNorm / (Double)rateDenorm);
			sb.Append((const UTF8Char*)")\r\n");
			sb.Append((const UTF8Char*)"Max Frame Size = ");
			sb.AppendUOSInt(maxFrameSize);
			sb.Append((const UTF8Char*)"\r\n");
			frameInfo.ToString(&sb);
		}
		decStatus = me->decStatus->GetItem(i);
		if (decStatus)
		{
			sb.Append((const UTF8Char*)"\r\nDecoded Frame Count = ");
			sb.AppendU64(decStatus->sampleCnt);
			sb.Append((const UTF8Char*)"\r\nDecoded Frame Time = ");
			me->AppendTime(&sb, decStatus->lastSampleTime);
		}
	}
	else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
	{
		Media::AudioFormat fmt;
		Media::IAudioSource *audioSrc = (Media::IAudioSource*)mediaSrc;
		sb.Append((const UTF8Char*)"Media Type = Audio\r\n");
		sb.Append((const UTF8Char*)"Sample Count = ");
		sb.AppendI64(audioSrc->GetSampleCount());
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Stream Time = ");
		me->AppendTime(&sb, (UInt32)audioSrc->GetStreamTime());
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Min Block Size = ");
		sb.AppendUOSInt(audioSrc->GetMinBlockSize());
		sb.Append((const UTF8Char*)"\r\n");
		audioSrc->GetFormat(&fmt);
		fmt.ToString(&sb);
		decStatus = me->decStatus->GetItem(i);
		if (decStatus)
		{
			sb.Append((const UTF8Char*)"\r\nDecoded sample Count = ");
			sb.AppendU64(decStatus->sampleCnt);
			sb.Append((const UTF8Char*)"\r\nDecoded Stream Time = ");
			me->AppendTime(&sb, (UInt32)(decStatus->sampleCnt * 1000 / fmt.frequency));
		}
	}

	me->txtStream->SetText(sb.ToString());
}

void __stdcall SSWR::AVIRead::AVIRVideoInfoForm::OnDecodeClicked(void *userObj)
{
	SSWR::AVIRead::AVIRVideoInfoForm *me = (SSWR::AVIRead::AVIRVideoInfoForm*)userObj;
	if (me->decStatus->GetCount() > 0 || me->currFile == 0)
	{
		return;
	}
	SSWR::AVIRead::AVIRVideoInfoForm::DecodeStatus *status;
	Media::IMediaSource *msrc;
	
	Sync::Event *evt;
	Media::Decoder::VideoDecoderFinder *vdecoders;
	Media::Decoder::AudioDecoderFinder *adecoders;
	NEW_CLASS(evt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRVideoInfoForm.OnDecodeClicked.evt"));
	NEW_CLASS(vdecoders, Media::Decoder::VideoDecoderFinder());
	NEW_CLASS(adecoders, Media::Decoder::AudioDecoderFinder());
	Bool isEnd;
	UOSInt i = 0;
	Int32 syncTime;
	while (true)
	{
		msrc = me->currFile->GetStream(i, &syncTime);
		if (msrc == 0)
		{
			break;
		}
		status = MemAlloc(SSWR::AVIRead::AVIRVideoInfoForm::DecodeStatus, 1);
		status->sampleCnt = 0;
		status->lastSampleTime = 0;
		status->isEnd = false;
		status->adecoder = 0;
		status->vdecoder = 0;
		status->renderer = 0;
		status->evt = evt;
		if (msrc->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
		{
			status->vdecoder = vdecoders->DecodeVideo((Media::IVideoSource*)msrc);
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
			status->adecoder = adecoders->DecodeAudio((Media::IAudioSource*)msrc);
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

		me->decStatus->Add(status);
		i++;
	}
	i = me->decStatus->GetCount();
	while (i-- > 0)
	{
		status = me->decStatus->GetItem(i);
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
		i = me->decStatus->GetCount();
		while (i-- > 0)
		{
			status = me->decStatus->GetItem(i);
			if (!status->isEnd)
			{
				isEnd = false;
			}
		}
		if (isEnd)
			break;
		evt->Wait(1000);
	}
	i = me->decStatus->GetCount();
	while (i-- > 0)
	{
		status = me->decStatus->GetItem(i);
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

	me->lblDecode->SetText((const UTF8Char*)"End Decoding");
}

Bool SSWR::AVIRead::AVIRVideoInfoForm::OpenFile(const UTF8Char *fileName)
{
	IO::StmData::FileData *fd;
	Media::MediaFile *mediaFile;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	mediaFile = (Media::MediaFile*)this->core->GetParserList()->ParseFileType(fd, IO::ParsedObject::PT_VIDEO_PARSER);
	DEL_CLASS(fd);
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
	Media::IMediaSource *mediaSrc;
	i = 0;
	while (true)
	{
		mediaSrc = mediaFile->GetStream(i, &syncTime);
		if (mediaSrc == 0)
			break;
		
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Stream ");
		sb.AppendUOSInt(i);
		if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
		{
			sb.Append((const UTF8Char*)" (Video)");
		}
		else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
		{
			sb.Append((const UTF8Char*)" (Audio)");
		}
		else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_SUBTITLE)
		{
			sb.Append((const UTF8Char*)" (Subtitle)");
		}
		this->lbStream->AddItem(sb.ToString(), (void*)i);
		i++;
	}
	return true;
}

void SSWR::AVIRead::AVIRVideoInfoForm::AppendTime(Text::StringBuilderUTF *sb, UInt32 t)
{
	sb->AppendU32(t / 3600000);
	t = t % 3600000;
	sb->AppendChar(':', 1);
	if (t < 600000)
	{
		sb->AppendChar('0', 1);
	}
	sb->AppendU32(t / 60000);
	t = t % 60000;
	sb->AppendChar(':', 1);
	if (t < 10000)
	{
		sb->AppendChar('0', 1);
	}
	sb->AppendU32(t / 1000);
	t = t % 1000;
	sb->AppendChar('.', 1);
	if (t < 10)
	{
		sb->AppendChar('0', 2);
	}
	else if (t < 100)
	{
		sb->AppendChar('0', 1);
	}
	sb->AppendU32(t);
}

void SSWR::AVIRead::AVIRVideoInfoForm::ClearDecode()
{
	SSWR::AVIRead::AVIRVideoInfoForm::DecodeStatus *status;
	UOSInt i = this->decStatus->GetCount();
	while (i-- > 0)
	{
		status = this->decStatus->GetItem(i);
		MemFree(status);
	}
	this->decStatus->Clear();
	this->lblDecode->SetText((const UTF8Char*)"");
}

SSWR::AVIRead::AVIRVideoInfoForm::AVIRVideoInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Video Info");
	
	this->core = core;
	this->currFile = 0;
	NEW_CLASS(this->decStatus, Data::ArrayList<SSWR::AVIRead::AVIRVideoInfoForm::DecodeStatus*>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlFile, UI::GUIPanel(ui, this));
	this->pnlFile->SetRect(0, 0, 100, 55, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlFile, (const UTF8Char*)"File Name"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlFile, (const UTF8Char*)""));
	this->txtFile->SetRect(104, 4, 800, 23, false);
	this->txtFile->SetReadOnly(true);
	NEW_CLASS(this->btnDecode, UI::GUIButton(ui, this->pnlFile, (const UTF8Char*)"Test Decode"));
	this->btnDecode->SetRect(104, 28, 150, 23, false);
	this->btnDecode->HandleButtonClick(OnDecodeClicked, this);
	NEW_CLASS(this->lblDecode, UI::GUILabel(ui, this->pnlFile, (const UTF8Char*)""));
	this->lblDecode->SetRect(254, 28, 100, 23, false);
	NEW_CLASS(this->lbStream, UI::GUIListBox(ui, this, false));
	this->lbStream->SetRect(0, 0, 150, 23, false);
	this->lbStream->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbStream->HandleSelectionChange(OnStreamChg, this);
	NEW_CLASS(this->hspStream, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->txtStream, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtStream->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtStream->SetReadOnly(true);

	this->HandleDropFiles(OnFileHandler, this);
}

SSWR::AVIRead::AVIRVideoInfoForm::~AVIRVideoInfoForm()
{
	this->ClearDecode();
	SDEL_CLASS(this->currFile);
	DEL_CLASS(this->decStatus);
}

void SSWR::AVIRead::AVIRVideoInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
