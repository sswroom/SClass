#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/FrequencyGraph.h"
#include "SSWR/AVIRead/AVIRAudioViewerForm.h"
#include "SSWR/AVIRead/AVIRMediaForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Writer.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

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
	Media::IMediaSource *medSource;
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
		while ((medSource = mFile->GetStream(k++, &syncTime)) != 0)
		{
			Media::MediaType mtype = medSource->GetMediaType();
			sptr = sbuff;
			if (mtype == Media::MEDIA_TYPE_VIDEO)
			{
				if (this->activeVideo == 0)
				{
					SetActiveVideo((Media::IVideoSource*)medSource);
				}
				if (this->activeVideo == medSource)
					*sptr++ = '*';
				Text::StrConcat(sptr, (const UTF8Char*)"(V)");
			}
			else if (mtype == Media::MEDIA_TYPE_AUDIO)
			{
				if (this->activeAudio == 0)
				{
					SetActiveAudio((Media::IAudioSource*)medSource, syncTime);
				}
				if (this->activeAudio == medSource)
					*sptr++ = '*';
				Text::StrConcat(sptr, (const UTF8Char*)"(A)");
			}
			medSource->GetSourceName(sptr);
			this->lbFiles->AddItem(sbuff, medSource);
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
					sptr = Text::StrConcat(sptr, (const UTF8Char*)"&");
				}
				sptr = Text::StrInt32(sptr, (Int32)j + 1);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" ");
				sptr = Text::StrConcat(sptr, this->currChapters->GetChapterName(j));
				this->mnuChapters->AddItem(sbuff, (UInt16)(MNU_PB_CHAPTERS + j), UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
				j++;
			}
		}
		else
		{
			this->mnuChapters->AddItem((const UTF8Char*)"No Chapters", MNU_PB_CHAPTERS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
			this->mnuChapters->SetItemEnabled(MNU_PB_CHAPTERS, false);
		}
	}
	else
	{
		this->mnuChapters->AddItem((const UTF8Char*)"No Chapter info", MNU_PB_CHAPTERS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		this->mnuChapters->SetItemEnabled(MNU_PB_CHAPTERS, false);
	}
	this->UpdateMenu();
}

void SSWR::AVIRead::AVIRMediaForm::SetActiveVideo(Media::IVideoSource *video)
{
	SDEL_CLASS(this->currDecoder);
	this->currDecoder = this->decoders->DecodeVideo(video);
	if (this->currDecoder)
	{
		this->activeVideo = video;
		this->vbdMain->SetVideo(this->currDecoder);
		this->clk->Stop();
	}
	else
	{
		this->activeVideo = video;
		this->vbdMain->SetVideo(this->activeVideo);
		this->clk->Stop();
	}
}

void SSWR::AVIRead::AVIRMediaForm::SetActiveAudio(Media::IAudioSource *audio, Int32 timeDelay)
{
	this->core->BindAudio(0);
	SDEL_CLASS(this->currADecoder);
	this->activeAudio = 0;

	this->vbdMain->SetTimeDelay(timeDelay);

	this->audRenderer = this->core->BindAudio(audio);
	if (this->audRenderer)
	{
		this->activeAudio = audio;
		return;
	}
	this->currADecoder = this->adecoders->DecodeAudio(audio);
	if (this->currADecoder)
	{
		this->audRenderer = this->core->BindAudio(this->currADecoder);
		if (this->audRenderer)
		{
			this->activeAudio = audio;
			return;
		}
		else
		{
			SDEL_CLASS(this->currADecoder);
		}
	}
}

Bool __stdcall SSWR::AVIRead::AVIRMediaForm::OnFileRClicked(void *userObj, OSInt scnX, OSInt scnY, UI::GUIControl::MouseButton btn)
{
	SSWR::AVIRead::AVIRMediaForm *me = (SSWR::AVIRead::AVIRMediaForm *)userObj;
	me->popMedia = (Media::IMediaSource*)me->lbFiles->GetSelectedItem();
	if (me->popMedia == 0)
	{
	}
	else
	{
		Media::MediaType mediaType = me->popMedia->GetMediaType();
		if (mediaType == Media::MEDIA_TYPE_VIDEO)
		{
			me->mnuVideo->ShowMenu(me->lbFiles, scnX, scnY);
		}
		else if (mediaType == Media::MEDIA_TYPE_AUDIO)
		{
			me->mnuAudio->ShowMenu(me->lbFiles, scnX, scnY);
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRMediaForm::OnFileDblClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMediaForm *me = (SSWR::AVIRead::AVIRMediaForm*)userObj;
	Media::IMediaSource *mediaSrc = (Media::IMediaSource*)me->lbFiles->GetSelectedItem();
	if (mediaSrc == 0)
	{
	}
	else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
	{
		SSWR::AVIRead::AVIRAudioViewerForm *frm;
		Media::IAudioSource *audSrc = (Media::IAudioSource*)mediaSrc;
		NEW_CLASS(frm, SSWR::AVIRead::AVIRAudioViewerForm(0, me->ui, me->core, audSrc));
		frm->ShowDialog(me);
		DEL_CLASS(frm);
	}
}

void __stdcall SSWR::AVIRead::AVIRMediaForm::VideoCropImage(void *userObj, UInt32 frameTime, UInt32 frameNum, Media::StaticImage *img)
{
	SSWR::AVIRead::AVIRMediaForm *me = (SSWR::AVIRead::AVIRMediaForm*)userObj;
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
	me->currDecoder->SetBorderCrop(crops[0], crops[1], crops[2], crops[3]);
	me->vbdMain->UpdateCrop();
}

Bool __stdcall SSWR::AVIRead::AVIRMediaForm::OnFrameTime(UInt32 frameTime, UOSInt frameNum, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, void *userData, Media::YCOffset ycOfst)
{
	UTF8Char sbuff[64];
	IO::Writer *writer = (IO::Writer *)userData;
	Text::StrUInt32(sbuff, frameTime);
	writer->WriteLine(sbuff);
	return true;
}

void SSWR::AVIRead::AVIRMediaForm::PBStart(UInt32 startTime)
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

SSWR::AVIRead::AVIRMediaForm::AVIRMediaForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::MediaFile *mediaFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"Media Form - "), mediaFile->GetSourceNameObj());
	this->SetText(sbuff);

	this->core = core;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->activeVideo = 0;
	this->activeAudio = 0;
	this->audRenderer = 0;
	NEW_CLASS(this->files, Data::ArrayList<Media::MediaFile*>());
	this->files->Add(mediaFile);
	this->currChapters = mediaFile->GetChapterInfo();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lbFiles, UI::GUIListBox(ui, this, false));
	this->lbFiles->SetRect(0, 0, 160, 100, false);
	this->lbFiles->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbFiles->HandleRightClicked(OnFileRClicked, this);
	this->lbFiles->HandleDoubleClicked(OnFileDblClicked, this);
	NEW_CLASS(this->hsplit, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 56, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vbdMain, UI::GUIVideoBoxDD(ui, this, this->colorSess, 5, Sync::Thread::GetThreadCnt()));
	this->vbdMain->SetDockType(UI::GUIControl::DOCK_FILL);

	UI::GUIMenu *mnu;
	UI::GUIMenu *mnu2;
	NEW_CLASS(this->mnu, UI::GUIMainMenu());
	mnu = this->mnu->AddSubMenu((const UTF8Char*)"&Playback");
	mnu->AddItem((const UTF8Char*)"&Start", MNU_PB_START, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_SPACE);
	mnu->AddItem((const UTF8Char*)"S&top", MNU_PB_STOP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_S);
	mnu->AddItem((const UTF8Char*)"&Forward 10 Seconds", MNU_PB_FWD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_ADD);
	mnu->AddItem((const UTF8Char*)"&Backward 10 Seconds", MNU_PB_BWD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_SUBTRACT);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"&Previous Chapter", MNU_PB_CHAP_PREV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_LEFT);
	mnu->AddItem((const UTF8Char*)"&Next Chapter", MNU_PB_CHAP_NEXT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_RIGHT);
	this->mnuChapters = mnu->AddSubMenu((const UTF8Char*)"&Chapters");

	mnu = this->mnu->AddSubMenu((const UTF8Char*)"&Video");
	mnu->AddItem((const UTF8Char*)"&Original Size", MNU_VIDEO_ORISIZE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_1);
	mnu->AddItem((const UTF8Char*)"Switch &Fullscreen", MNU_VIDEO_FULLSCN, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_ENTER);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"&Crop Detect", MNU_VIDEO_CROP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_R);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"&Deinterlace");
	mnu2->AddItem((const UTF8Char*)"&From Video", MNU_VIDEO_DEINT_AUTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Force &Progressive", MNU_VIDEO_DEINT_PROG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Force Interlaced (&TFF)", MNU_VIDEO_DEINT_TFF, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Force Interlaced (&BFF)", MNU_VIDEO_DEINT_BFF, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"&30P Mode", MNU_VIDEO_DEINT_30P, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	
	mnu = this->mnu->AddSubMenu((const UTF8Char*)"&Audio");

	this->SetMenu(this->mnu);

	NEW_CLASS(this->mnuVideo, UI::GUIPopupMenu());
	this->mnuVideo->AddItem((const UTF8Char*)"&Remove", MNU_POPV_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuVideo->AddItem((const UTF8Char*)"&Save Timecode file", MNU_POPV_SAVE_TIMECODE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	NEW_CLASS(this->mnuAudio, UI::GUIPopupMenu());
	this->mnuAudio->AddItem((const UTF8Char*)"&Remove", MNU_POPA_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuAudio->AddItem((const UTF8Char*)"Frequency Graph", MNU_POPA_FREQ_GRAPH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);


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

	DEL_CLASS(this->mnuAudio);
	DEL_CLASS(this->mnuVideo);
	if (this->audRenderer)
	{
		this->core->BindAudio(0);
	}
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}


void SSWR::AVIRead::AVIRMediaForm::EventMenuClicked(UInt16 cmdId)
{
	UInt32 currTime;
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
			this->PBStart(currTime + 10000);
			this->pbLastChapter = (UOSInt)-1;
		}
		break;
	case MNU_PB_BWD:
		if (this->PBIsPlaying())
		{
			currTime = this->clk->GetCurrTime() - 10000;
			this->PBStop();
			if (currTime < 0)
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
			UOSInt w1;
			UOSInt h1;
			UOSInt w2;
			UOSInt h2;
			UOSInt cropLeft;
			UOSInt cropTop;
			UOSInt cropRight;
			UOSInt cropBottom;
			UOSInt vw;
			UOSInt vh;
			UInt32 tmpV;

			if (this->currDecoder)
			{
				this->currDecoder->GetVideoInfo(&info, &tmpV, &tmpV, &w1);
			}
			else
			{
				this->activeVideo->GetVideoInfo(&info, &tmpV, &tmpV, &w1);
			}
			this->activeVideo->GetBorderCrop(&cropLeft, &cropTop, &cropRight, &cropBottom);
			if (this->vbdMain->IsFullScreen())
			{
				this->vbdMain->SwitchFullScreen(false, false);
			}
			this->vbdMain->GetSizeP(&w1, &h1);
			this->GetSizeP(&w2, &h2);

			vw = info.dispWidth - cropLeft - cropRight;
			vh = info.dispHeight - cropTop - cropBottom;
			if (info.ftype == Media::FT_FIELD_BF || info.ftype == Media::FT_FIELD_TF)
			{
				vh = vh << 1;
			}
			if (info.par2 > 1)
			{
				vh = (UOSInt)Math::Double2Int32(Math::UOSInt2Double(vh) * info.par2);
			}
			else
			{
				vw = (UOSInt)Math::Double2Int32(Math::UOSInt2Double(vw) / info.par2);
			}

			this->SetFormState(UI::GUIForm::FS_NORMAL);
			this->SetSizeP(w2 - w1 + vw, h2 - h1 + vh);
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
			UI::FileDialog *dlg;
			NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"SaveTimecode", true));
			dlg->AddFilter((const UTF8Char*)"*.tc2", (const UTF8Char*)"Timecode V2");
			if (dlg->ShowDialog(this->hwnd))
			{
				Media::IVideoSource *video;
				UTF8Char sbuff[40];
				UOSInt j;
				IO::FileStream *fs;
				Text::UTF8Writer *writer;
				video = (Media::IVideoSource*)this->popMedia;
				NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
				NEW_CLASS(writer, Text::UTF8Writer(fs));
				writer->WriteLine((const UTF8Char*)"# timecode format v2");
				j = video->GetFrameCount();
				if (j >= 0)
				{
					i = 0;
					while (i < j)
					{
						Text::StrUInt32(sbuff, video->GetFrameTime(i));
						writer->WriteLine(sbuff);
						i++;
					}
				}
				else
				{
					video->EnumFrameInfos(OnFrameTime, writer);
				}
				DEL_CLASS(writer);
				DEL_CLASS(fs);
			}
			DEL_CLASS(dlg);
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
			Media::DrawImage *img = Media::FrequencyGraph::CreateGraph(this->core->GetDrawEngine(), audio, 2048, 2048, Math::FFTCalc::WT_BLACKMANN_HARRIS, 12.0);
			t = clk.GetTimeDiff();
			if (img)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char *)"t=");
				Text::SBAppendF64(&sb, t);
				UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char *)"Test", this);

				UTF8Char sbuff[512];
				Media::ImageList *imgList;
				Media::StaticImage *simg = img->ToStaticImage();
				this->core->GetDrawEngine()->DeleteImage(img);
				audio->GetSourceName(sbuff);
				NEW_CLASS(imgList, Media::ImageList(sbuff));
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
