#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/FrequencyGraph.h"
#include "SSWR/AVIRead/AVIRAudioViewerForm.h"
#include "SSWR/AVIRead/AVIRMediaForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIFileDialog.h"

extern "C"
{
	void MediaPlayer_VideoCropImageY(UInt8 *yptr, UOSInt w, UOSInt h, UOSInt ySplit, UOSInt *crops);
}

typedef enum
{
	MNU_PB_START = 101,
	MNU_PB_STOP,
	MNU_PB_FWD,
	MNU_PB_BWD,
	MNU_PB_CHAP_PREV,
	MNU_PB_CHAP_NEXT,
	MNU_VIDEO_ORISIZE,
	MNU_VIDEO_FULLSCN,
	MNU_VIDEO_CROP,
	MNU_VIDEO_DEINT_AUTO,
	MNU_VIDEO_DEINT_PROG,
	MNU_VIDEO_DEINT_TFF,
	MNU_VIDEO_DEINT_BFF,
	MNU_VIDEO_DEINT_30P,
	MNU_POPV_REMOVE,
	MNU_POPV_SAVE_TIMECODE,
	MNU_POPA_REMOVE,
	MNU_POPA_FREQ_GRAPH,

	MNU_PB_CHAPTERS = 1000
} MenuItems;

void SSWR::AVIRead::AVIRMediaForm::UpdateStreamList()
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Media::MediaFile *mFile;
	NotNullPtr<Media::IMediaSource> medSource;
	Int32 syncTime;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	this->lbFiles->ClearItems();
	i = 0;
	j = this->files->GetCount();
	while (i < j)
	{
		mFile = this->files->GetItem(i);
		k = 0;
		while (medSource.Set(mFile->GetStream(k++, &syncTime)))
		{
			Media::MediaType mtype = medSource->GetMediaType();
			sptr = sbuff;
			if (mtype == Media::MEDIA_TYPE_VIDEO)
			{
				if (this->activeVideo == 0)
				{
					SetActiveVideo(NotNullPtr<Media::IVideoSource>::ConvertFrom(medSource));
				}
				if (this->activeVideo == medSource.Ptr())
					*sptr++ = '*';
				Text::StrConcatC(sptr, UTF8STRC("(V)"));
			}
			else if (mtype == Media::MEDIA_TYPE_AUDIO)
			{
				if (this->activeAudio == 0)
				{
					SetActiveAudio(NotNullPtr<Media::IAudioSource>::ConvertFrom(medSource), syncTime);
				}
				if (this->activeAudio == medSource.Ptr())
					*sptr++ = '*';
				Text::StrConcatC(sptr, UTF8STRC("(A)"));
			}
			sptr = medSource->GetSourceName(sptr);
			this->lbFiles->AddItem(CSTRP(sbuff, sptr), medSource.Ptr());
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRMediaForm::UpdateChapters()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	this->mnuChapters->ClearItems();
	if (this->currChapters)
	{
		i = this->currChapters->GetChapterCnt();
		if (i > 0)
		{
			j = 0;
			while (j < i)
			{
				sptr = sbuff;
				if (j < 9)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC("&"));
				}
				sptr = Text::StrInt32(sptr, (Int32)j + 1);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" "));
				sptr = Text::String::OrEmpty(this->currChapters->GetChapterName(j))->ConcatTo(sptr);
				this->mnuChapters->AddItem(CSTRP(sbuff, sptr), (UInt16)(MNU_PB_CHAPTERS + j), UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
				j++;
			}
		}
		else
		{
			this->mnuChapters->AddItem(CSTR("No Chapters"), MNU_PB_CHAPTERS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
			this->mnuChapters->SetItemEnabled(MNU_PB_CHAPTERS, false);
		}
	}
	else
	{
		this->mnuChapters->AddItem(CSTR("No Chapter info"), MNU_PB_CHAPTERS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		this->mnuChapters->SetItemEnabled(MNU_PB_CHAPTERS, false);
	}
	this->UpdateMenu();
}

void SSWR::AVIRead::AVIRMediaForm::SetActiveVideo(NotNullPtr<Media::IVideoSource> video)
{
	SDEL_CLASS(this->currDecoder);
	this->currDecoder = this->decoders->DecodeVideo(video);
	if (this->currDecoder)
	{
		this->activeVideo = video.Ptr();
		this->vbdMain->SetVideo(this->currDecoder);
		this->clk->Stop();
	}
	else
	{
		this->activeVideo = video.Ptr();
		this->vbdMain->SetVideo(this->activeVideo);
		this->clk->Stop();
	}
}

void SSWR::AVIRead::AVIRMediaForm::SetActiveAudio(NotNullPtr<Media::IAudioSource> audio, Int32 timeDelay)
{
	this->core->BindAudio(0);
	SDEL_CLASS(this->currADecoder);
	this->activeAudio = 0;

	this->vbdMain->SetTimeDelay(timeDelay);

	this->audRenderer = this->core->BindAudio(audio.Ptr());
	if (this->audRenderer)
	{
		this->activeAudio = audio.Ptr();
		return;
	}
	this->currADecoder = this->adecoders->DecodeAudio(audio);
	if (this->currADecoder)
	{
		this->audRenderer = this->core->BindAudio(this->currADecoder);
		if (this->audRenderer)
		{
			this->activeAudio = audio.Ptr();
			return;
		}
		else
		{
			SDEL_CLASS(this->currADecoder);
		}
	}
}

Bool __stdcall SSWR::AVIRead::AVIRMediaForm::OnFileRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn)
{
	NotNullPtr<SSWR::AVIRead::AVIRMediaForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMediaForm>();
	me->popMedia = (Media::IMediaSource*)me->lbFiles->GetSelectedItem().p;
	if (me->popMedia == 0)
	{
	}
	else
	{
		Media::MediaType mediaType = me->popMedia->GetMediaType();
		if (mediaType == Media::MEDIA_TYPE_VIDEO)
		{
			me->mnuVideo->ShowMenu(me->lbFiles, scnPos);
		}
		else if (mediaType == Media::MEDIA_TYPE_AUDIO)
		{
			me->mnuAudio->ShowMenu(me->lbFiles, scnPos);
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRMediaForm::OnFileDblClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRMediaForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMediaForm>();
	Media::IMediaSource *mediaSrc = (Media::IMediaSource*)me->lbFiles->GetSelectedItem().p;
	if (mediaSrc == 0)
	{
	}
	else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
	{
		Media::IAudioSource *audSrc = (Media::IAudioSource*)mediaSrc;
		SSWR::AVIRead::AVIRAudioViewerForm frm(0, me->ui, me->core, audSrc);
		frm.ShowDialog(me);
	}
}

void __stdcall SSWR::AVIRead::AVIRMediaForm::VideoCropImage(AnyType userObj, Data::Duration frameTime, UInt32 frameNum, NotNullPtr<Media::StaticImage> img)
{
	NotNullPtr<SSWR::AVIRead::AVIRMediaForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMediaForm>();
	UOSInt w = img->info.dispSize.x;
	UOSInt h = img->info.dispSize.y;
	UInt8 *yptr = img->data;
	UOSInt ySplit;
	UOSInt crops[4];
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

	MediaPlayer_VideoCropImageY(yptr, w, h, ySplit, crops);
	img.Delete();
	me->currDecoder->SetBorderCrop(crops[0], crops[1], crops[2], crops[3]);
	me->vbdMain->UpdateCrop();
}

Bool __stdcall SSWR::AVIRead::AVIRMediaForm::OnFrameTime(Data::Duration frameTime, UOSInt frameNum, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, AnyType userData, Media::YCOffset ycOfst)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	NotNullPtr<IO::Writer> writer = userData.GetNN<IO::Writer>();
	sptr = Text::StrInt64(sbuff, frameTime.GetTotalMS());
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

void SSWR::AVIRead::AVIRMediaForm::PBStart(Data::Duration startTime)
{
	if (this->currDecoder)
	{
		this->currDecoder->SeekToTime(startTime);
	}
	else if (this->activeVideo)
	{
		this->activeVideo->SeekToTime(startTime);
	}

	if (this->currADecoder)
	{
		startTime = this->currADecoder->SeekToTime(startTime);
	}
	else if (this->activeAudio)
	{
		startTime = this->activeAudio->SeekToTime(startTime);
	}
	else
	{
	}
	if (this->audRenderer) this->audRenderer->AudioInit(this->clk);
	this->vbdMain->VideoInit(this->clk);
	if (this->audRenderer) this->audRenderer->Start();
	this->vbdMain->VideoStart();
	this->playing = true;
}

void SSWR::AVIRead::AVIRMediaForm::PBStop()
{
	this->vbdMain->StopPlay();
	if (this->audRenderer) this->audRenderer->Stop();
}

Bool SSWR::AVIRead::AVIRMediaForm::PBIsPlaying()
{
	if (!this->playing)
	{
		return false;
	}
	if (this->audRenderer && this->audRenderer->IsPlaying())
		return true;
	if (this->activeVideo && this->activeVideo->IsRunning())
		return true;
	this->playing = false;
	return false;
}

SSWR::AVIRead::AVIRMediaForm::AVIRMediaForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Media::MediaFile *mediaFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = mediaFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Media Form - ")));
	this->SetText(CSTRP(sbuff, sptr));

	this->core = core;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->activeVideo = 0;
	this->activeAudio = 0;
	this->audRenderer = 0;
	NEW_CLASS(this->files, Data::ArrayList<Media::MediaFile*>());
	this->files->Add(mediaFile);
	this->currChapters = mediaFile->GetChapterInfo();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lbFiles = ui->NewListBox(*this, false);
	this->lbFiles->SetRect(0, 0, 160, 100, false);
	this->lbFiles->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbFiles->HandleRightClicked(OnFileRClicked, this);
	this->lbFiles->HandleDoubleClicked(OnFileDblClicked, this);
	this->hsplit = ui->NewHSplitter(*this, 3, false);
	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 56, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vbdMain, UI::GUIVideoBoxDD(ui, *this, this->colorSess, 5, Sync::ThreadUtil::GetThreadCnt()));
	this->vbdMain->SetDockType(UI::GUIControl::DOCK_FILL);

	NotNullPtr<UI::GUIMenu> mnu;
	NotNullPtr<UI::GUIMenu> mnu2;
	NEW_CLASSNN(this->mnu, UI::GUIMainMenu());
	mnu = this->mnu->AddSubMenu(CSTR("&Playback"));
	mnu->AddItem(CSTR("&Start"), MNU_PB_START, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_SPACE);
	mnu->AddItem(CSTR("S&top"), MNU_PB_STOP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_S);
	mnu->AddItem(CSTR("&Forward 10 Seconds"), MNU_PB_FWD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_ADD);
	mnu->AddItem(CSTR("&Backward 10 Seconds"), MNU_PB_BWD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_SUBTRACT);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("&Previous Chapter"), MNU_PB_CHAP_PREV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_LEFT);
	mnu->AddItem(CSTR("&Next Chapter"), MNU_PB_CHAP_NEXT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_RIGHT);
	this->mnuChapters = mnu->AddSubMenu(CSTR("&Chapters"));

	mnu = this->mnu->AddSubMenu(CSTR("&Video"));
	mnu->AddItem(CSTR("&Original Size"), MNU_VIDEO_ORISIZE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_1);
	mnu->AddItem(CSTR("Switch &Fullscreen"), MNU_VIDEO_FULLSCN, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_ENTER);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("&Crop Detect"), MNU_VIDEO_CROP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_R);
	mnu2 = mnu->AddSubMenu(CSTR("&Deinterlace"));
	mnu2->AddItem(CSTR("&From Video"), MNU_VIDEO_DEINT_AUTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Force &Progressive"), MNU_VIDEO_DEINT_PROG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Force Interlaced (&TFF)"), MNU_VIDEO_DEINT_TFF, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Force Interlaced (&BFF)"), MNU_VIDEO_DEINT_BFF, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("&30P Mode"), MNU_VIDEO_DEINT_30P, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	
	mnu = this->mnu->AddSubMenu(CSTR("&Audio"));

	this->SetMenu(this->mnu);

	NEW_CLASSNN(this->mnuVideo, UI::GUIPopupMenu());
	this->mnuVideo->AddItem(CSTR("&Remove"), MNU_POPV_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuVideo->AddItem(CSTR("&Save Timecode file"), MNU_POPV_SAVE_TIMECODE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	NEW_CLASSNN(this->mnuAudio, UI::GUIPopupMenu());
	this->mnuAudio->AddItem(CSTR("&Remove"), MNU_POPA_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuAudio->AddItem(CSTR("Frequency Graph"), MNU_POPA_FREQ_GRAPH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);


	this->currDecoder = 0;
	this->currADecoder = 0;
	NEW_CLASS(this->adecoders, Media::Decoder::AudioDecoderFinder());
	NEW_CLASS(this->decoders, Media::Decoder::VideoDecoderFinder());
	NEW_CLASS(this->clk, Media::RefClock());

	this->UpdateStreamList();
	this->UpdateChapters();
}

SSWR::AVIRead::AVIRMediaForm::~AVIRMediaForm()
{
	this->PBStop();
	this->vbdMain->SetVideo(0);

	SDEL_CLASS(this->currDecoder);
	SDEL_CLASS(this->currADecoder);
	DEL_CLASS(this->decoders);
	DEL_CLASS(this->adecoders);
	UOSInt i = this->files->GetCount();
	while (i-- > 0)
	{
		Media::MediaFile *file = this->files->GetItem(i);
		DEL_CLASS(file);
	}
	DEL_CLASS(this->files);
	DEL_CLASS(this->clk);

	this->mnuAudio.Delete();
	this->mnuVideo.Delete();
	if (this->audRenderer)
	{
		this->core->BindAudio(0);
	}
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}


void SSWR::AVIRead::AVIRMediaForm::EventMenuClicked(UInt16 cmdId)
{
	Data::Duration currTime;
	UOSInt i;
	if (cmdId >= MNU_PB_CHAPTERS)
	{
		i = (UOSInt)cmdId - MNU_PB_CHAPTERS;
		if (this->PBIsPlaying())
		{
			PBStop();
		}
		if (this->currChapters)
		{
			this->PBStart(this->currChapters->GetChapterTime(i));
			this->pbLastChapter = i;
		}
		return;
	}

	switch (cmdId)
	{
	case MNU_PB_START:
		if (!this->PBIsPlaying())
		{
			this->PBStart(0);
			this->pbLastChapter = 0;
		}
		break;
	case MNU_PB_STOP:
		this->PBStop();
		break;
	case MNU_PB_FWD:
		if (this->PBIsPlaying())
		{
			currTime = this->clk->GetCurrTime();
			this->PBStop();
			this->PBStart(currTime.AddMS(10000));
			this->pbLastChapter = (UOSInt)-1;
		}
		break;
	case MNU_PB_BWD:
		if (this->PBIsPlaying())
		{
			currTime = this->clk->GetCurrTime().AddMS(-10000);
			this->PBStop();
			if (currTime.IsNegative())
				currTime = 0;
			this->PBStart(currTime);
			this->pbLastChapter = (UOSInt)-1;
		}
		break;
	case MNU_PB_CHAP_PREV:
		if (this->PBIsPlaying() && this->currChapters)
		{
			currTime = this->clk->GetCurrTime();
			i = this->currChapters->GetChapterIndex(currTime);
			if (i + 1 == this->pbLastChapter)
			{
				i++;
			}
			i--;
			if ((OSInt)i < 0)
			{
				i = 0;
			}
			this->PBStop();
			this->PBStart(this->currChapters->GetChapterTime(i));
			this->pbLastChapter = i;
		}
		break;
	case MNU_PB_CHAP_NEXT:
		if (this->PBIsPlaying() && this->currChapters)
		{
			currTime = this->clk->GetCurrTime();
			i = this->currChapters->GetChapterIndex(currTime);
			if (i + 1 == this->pbLastChapter)
			{
				i++;
			}
			if (i < this->currChapters->GetChapterCnt() - 1)
			{
				this->PBStop();
				this->PBStart(this->currChapters->GetChapterTime(i + 1));
				this->pbLastChapter = i + 1;
			}
		}
		break;
	case MNU_VIDEO_ORISIZE:
		if (this->activeVideo)
		{
			Media::FrameInfo info;
			Math::Size2D<UOSInt> sz1;
			Math::Size2D<UOSInt> sz2;
			UOSInt cropLeft;
			UOSInt cropTop;
			UOSInt cropRight;
			UOSInt cropBottom;
			Math::Size2D<UOSInt> vSize;
			UInt32 tmpV;

			if (this->currDecoder)
			{
				this->currDecoder->GetVideoInfo(info, tmpV, tmpV, sz1.x);
			}
			else
			{
				this->activeVideo->GetVideoInfo(info, tmpV, tmpV, sz1.x);
			}
			this->activeVideo->GetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
			if (this->vbdMain->IsFullScreen())
			{
				this->vbdMain->SwitchFullScreen(false, false);
			}
			sz1 = this->vbdMain->GetSizeP();
			sz2 = this->GetSizeP();

			vSize.x = info.dispSize.x - cropLeft - cropRight;
			vSize.y = info.dispSize.y - cropTop - cropBottom;
			if (info.ftype == Media::FT_FIELD_BF || info.ftype == Media::FT_FIELD_TF)
			{
				vSize.y = vSize.y << 1;
			}
			if (info.par2 > 1)
			{
				vSize.y = (UOSInt)Double2Int32(UOSInt2Double(vSize.y) * info.par2);
			}
			else
			{
				vSize.x = (UOSInt)Double2Int32(UOSInt2Double(vSize.x) / info.par2);
			}

			this->SetFormState(UI::GUIForm::FS_NORMAL);
			this->SetSizeP(sz2 - sz1 + vSize);
		}
		break;
	case MNU_VIDEO_FULLSCN:
		this->vbdMain->SwitchFullScreen(!this->vbdMain->IsFullScreen(), true);
		break;
	case MNU_VIDEO_CROP:
		if (this->currDecoder)
		{
			this->currDecoder->CaptureImage(VideoCropImage, this);
		}
		break;
	case MNU_VIDEO_DEINT_AUTO:
//		this->vbdMain->SetFrameType(false, Media::FT_NON_INTERLACE);
		this->vbdMain->SetDeintType(UI::GUIVideoBoxDD::DT_FROM_VIDEO);
		break;
	case MNU_VIDEO_DEINT_PROG:
//		this->vbdMain->SetFrameType(true, Media::FT_NON_INTERLACE);
		this->vbdMain->SetDeintType(UI::GUIVideoBoxDD::DT_PROGRESSIVE);
		break;
	case MNU_VIDEO_DEINT_TFF:
//		this->vbdMain->SetFrameType(true, Media::FT_INTERLACED_TFF);
		this->vbdMain->SetDeintType(UI::GUIVideoBoxDD::DT_INTERLACED_TFF);
		break;
	case MNU_VIDEO_DEINT_BFF:
//		this->vbdMain->SetFrameType(true, Media::FT_INTERLACED_BFF);
		this->vbdMain->SetDeintType(UI::GUIVideoBoxDD::DT_INTERLACED_BFF);
		break;
	case MNU_VIDEO_DEINT_30P:
		this->vbdMain->SetDeintType(UI::GUIVideoBoxDD::DT_30P_MODE);
		break;
	case MNU_POPV_REMOVE:
		break;
	case MNU_POPV_SAVE_TIMECODE:
		{
			NotNullPtr<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"SaveTimecode", true);
			dlg->AddFilter(CSTR("*.tc2"), CSTR("Timecode V2"));
			if (dlg->ShowDialog(this->hwnd))
			{
				Media::IVideoSource *video;
				UTF8Char sbuff[40];
				UTF8Char *sptr;
				UOSInt j;
				video = (Media::IVideoSource*)this->popMedia;
				IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				Text::UTF8Writer writer(fs);
				writer.WriteLineC(UTF8STRC("# timecode format v2"));
				j = video->GetFrameCount();
				if (j >= 0)
				{
					i = 0;
					while (i < j)
					{
						sptr = Text::StrInt64(sbuff, video->GetFrameTime(i).GetTotalMS());
						writer.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
						i++;
					}
				}
				else
				{
					video->EnumFrameInfos(OnFrameTime, &writer);
				}
			}
			dlg.Delete();
		}
		break;
	case MNU_POPA_REMOVE:
		break;
	case MNU_POPA_FREQ_GRAPH:
		{
			Media::IAudioSource *audio = (Media::IAudioSource*)this->popMedia;
			Manage::HiResClock clk;
			Double t;
			clk.Start();
			NotNullPtr<Media::DrawImage> img;
			if (img.Set(Media::FrequencyGraph::CreateGraph(this->core->GetDrawEngine(), audio, 2048, 2048, Math::FFTCalc::WT_BLACKMANN_HARRIS, 12.0)))
			{
				t = clk.GetTimeDiff();
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("t="));
				sb.AppendDouble(t);
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("Test"), this);

				UTF8Char sbuff[512];
				UTF8Char *sptr;
				NotNullPtr<Media::ImageList> imgList;
				Media::StaticImage *simg = img->ToStaticImage();
				this->core->GetDrawEngine()->DeleteImage(img);
				sptr = audio->GetSourceName(sbuff);
				NEW_CLASSNN(imgList, Media::ImageList(CSTRP(sbuff, sptr)));
				imgList->AddImage(simg, 0);
				this->core->OpenObject(imgList);
			}
		}
		break;

	}
}
	
void SSWR::AVIRead::AVIRMediaForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
