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
	NN<Media::MediaFile> mFile;
	NN<Media::MediaSource> medSource;
	Int32 syncTime;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;

	this->lbFiles->ClearItems();
	i = 0;
	j = this->files.GetCount();
	while (i < j)
	{
		mFile = this->files.GetItemNoCheck(i);
		k = 0;
		while (mFile->GetStream(k++, syncTime).SetTo(medSource))
		{
			Media::MediaType mtype = medSource->GetMediaType();
			sptr = sbuff;
			if (mtype == Media::MEDIA_TYPE_VIDEO)
			{
				if (this->activeVideo.IsNull())
				{
					SetActiveVideo(NN<Media::VideoSource>::ConvertFrom(medSource));
				}
				if (this->activeVideo.OrNull() == medSource.Ptr())
					*sptr++ = '*';
				Text::StrConcatC(sptr, UTF8STRC("(V)"));
			}
			else if (mtype == Media::MEDIA_TYPE_AUDIO)
			{
				if (this->activeAudio.IsNull())
				{
					SetActiveAudio(NN<Media::AudioSource>::ConvertFrom(medSource), syncTime);
				}
				if (this->activeAudio.OrNull() == medSource.Ptr())
					*sptr++ = '*';
				Text::StrConcatC(sptr, UTF8STRC("(A)"));
			}
			sptr = medSource->GetSourceName(sptr).Or(sptr);
			this->lbFiles->AddItem(CSTRP(sbuff, sptr), medSource);
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRMediaForm::UpdateChapters()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	NN<Media::ChapterInfo> currChapters;
	this->mnuChapters->ClearItems();
	if (this->currChapters.SetTo(currChapters))
	{
		i = currChapters->GetChapterCnt();
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
				sptr = Text::String::OrEmpty(currChapters->GetChapterName(j))->ConcatTo(sptr);
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

void SSWR::AVIRead::AVIRMediaForm::SetActiveVideo(NN<Media::VideoSource> video)
{
	NN<Media::VideoSource> currDecoder;
	this->currDecoder.Delete();
	this->currDecoder = this->decoders->DecodeVideo(video);
	if (this->currDecoder.SetTo(currDecoder))
	{
		this->activeVideo = video;
		this->vbdMain->SetVideo(currDecoder);
		this->clk->Stop();
	}
	else
	{
		this->activeVideo = video.Ptr();
		this->vbdMain->SetVideo(this->activeVideo.OrNull());
		this->clk->Stop();
	}
}

void SSWR::AVIRead::AVIRMediaForm::SetActiveAudio(NN<Media::AudioSource> audio, Int32 timeDelay)
{
	this->core->BindAudio(0);
	this->currADecoder.Delete();
	this->activeAudio = 0;

	this->vbdMain->SetTimeDelay(timeDelay);

	this->audRenderer = this->core->BindAudio(audio);
	if (this->audRenderer.NotNull())
	{
		this->activeAudio = audio;
		return;
	}
	this->currADecoder = this->adecoders->DecodeAudio(audio);
	if (this->currADecoder.NotNull())
	{
		this->audRenderer = this->core->BindAudio(this->currADecoder);
		if (this->audRenderer.NotNull())
		{
			this->activeAudio = audio;
			return;
		}
		else
		{
			this->currADecoder.Delete();
		}
	}
}

Bool __stdcall SSWR::AVIRead::AVIRMediaForm::OnFileRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRMediaForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMediaForm>();
	me->popMedia = (Media::MediaSource*)me->lbFiles->GetSelectedItem().p;
	NN<Media::MediaSource> popMedia;
	if (!me->popMedia.SetTo(popMedia))
	{
	}
	else
	{
		Media::MediaType mediaType = popMedia->GetMediaType();
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
	NN<SSWR::AVIRead::AVIRMediaForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMediaForm>();
	NN<Media::MediaSource> mediaSrc;
	if (!me->lbFiles->GetSelectedItem().GetOpt<Media::MediaSource>().SetTo(mediaSrc))
	{
	}
	else if (mediaSrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
	{
		NN<Media::AudioSource> audSrc = NN<Media::AudioSource>::ConvertFrom(mediaSrc);
		SSWR::AVIRead::AVIRAudioViewerForm frm(0, me->ui, me->core, audSrc);
		frm.ShowDialog(me);
	}
}

void __stdcall SSWR::AVIRead::AVIRMediaForm::VideoCropImage(AnyType userObj, Data::Duration frameTime, UInt32 frameNum, NN<Media::StaticImage> img)
{
	NN<SSWR::AVIRead::AVIRMediaForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMediaForm>();
	NN<Media::VideoSource> currDecoder;
	if (!me->currDecoder.SetTo(currDecoder))
	{
		img.Delete();
		return;
	}
	UOSInt w = img->info.dispSize.x;
	UOSInt h = img->info.dispSize.y;
	UnsafeArray<UInt8> yptr = img->data;
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

	MediaPlayer_VideoCropImageY(yptr.Ptr(), w, h, ySplit, crops);
	img.Delete();
	currDecoder->SetBorderCrop(crops[0], crops[1], crops[2], crops[3]);
	me->vbdMain->UpdateCrop();
}

Bool __stdcall SSWR::AVIRead::AVIRMediaForm::OnFrameTime(Data::Duration frameTime, UOSInt frameNum, UOSInt dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, AnyType userData, Media::YCOffset ycOfst)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::Writer> writer = userData.GetNN<IO::Writer>();
	sptr = Text::StrInt64(sbuff, frameTime.GetTotalMS());
	writer->WriteLine(CSTRP(sbuff, sptr));
	return true;
}

void SSWR::AVIRead::AVIRMediaForm::PBStart(Data::Duration startTime)
{
	NN<Media::AudioRenderer> audRenderer;
	NN<Media::VideoSource> activeVideo;
	NN<Media::AudioSource> activeAudio;
	NN<Media::VideoSource> currDecoder;
	NN<Media::AudioSource> currADecoder;
	if (this->currDecoder.SetTo(currDecoder))
	{
		currDecoder->SeekToTime(startTime);
	}
	else if (this->activeVideo.SetTo(activeVideo))
	{
		activeVideo->SeekToTime(startTime);
	}

	if (this->currADecoder.SetTo(currADecoder))
	{
		startTime = currADecoder->SeekToTime(startTime);
	}
	else if (this->activeAudio.SetTo(activeAudio))
	{
		startTime = activeAudio->SeekToTime(startTime);
	}
	else
	{
	}
	if (this->audRenderer.SetTo(audRenderer)) audRenderer->AudioInit(this->clk);
	this->vbdMain->VideoInit(this->clk);
	if (this->audRenderer.SetTo(audRenderer)) audRenderer->Start();
	this->vbdMain->VideoStart();
	this->playing = true;
}

void SSWR::AVIRead::AVIRMediaForm::PBStop()
{
	NN<Media::AudioRenderer> audRenderer;
	this->vbdMain->StopPlay();
	if (this->audRenderer.SetTo(audRenderer)) audRenderer->Stop();
}

Bool SSWR::AVIRead::AVIRMediaForm::PBIsPlaying()
{
	NN<Media::AudioRenderer> audRenderer;
	NN<Media::VideoSource> activeVideo;
	if (!this->playing)
	{
		return false;
	}
	if (this->audRenderer.SetTo(audRenderer) && audRenderer->IsPlaying())
		return true;
	if (this->activeVideo.SetTo(activeVideo) && activeVideo->IsRunning())
		return true;
	this->playing = false;
	return false;
}

SSWR::AVIRead::AVIRMediaForm::AVIRMediaForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::MediaFile> mediaFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = mediaFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Media Form - ")));
	this->SetText(CSTRP(sbuff, sptr));

	this->core = core;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->activeVideo = 0;
	this->activeAudio = 0;
	this->audRenderer = 0;
	this->files.Add(mediaFile);
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
	NEW_CLASSNN(this->vbdMain, UI::GUIVideoBoxDD(ui, *this, this->colorSess, 5, Sync::ThreadUtil::GetThreadCnt()));
	this->vbdMain->SetDockType(UI::GUIControl::DOCK_FILL);

	NN<UI::GUIMenu> mnu;
	NN<UI::GUIMenu> mnu2;
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
	NEW_CLASSNN(this->adecoders, Media::Decoder::AudioDecoderFinder());
	NEW_CLASSNN(this->decoders, Media::Decoder::VideoDecoderFinder());
	NEW_CLASSNN(this->clk, Media::RefClock());

	this->UpdateStreamList();
	this->UpdateChapters();
}

SSWR::AVIRead::AVIRMediaForm::~AVIRMediaForm()
{
	this->PBStop();
	this->vbdMain->SetVideo(0);

	this->currDecoder.Delete();
	this->currADecoder.Delete();
	this->decoders.Delete();
	this->adecoders.Delete();
	this->files.DeleteAll();
	this->clk.Delete();

	this->mnuAudio.Delete();
	this->mnuVideo.Delete();
	if (this->audRenderer.NotNull())
	{
		this->core->BindAudio(0);
	}
	this->ClearChildren();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}


void SSWR::AVIRead::AVIRMediaForm::EventMenuClicked(UInt16 cmdId)
{
	NN<Media::VideoSource> activeVideo;
	NN<Media::ChapterInfo> currChapters;
	NN<Media::VideoSource> currDecoder;
	NN<Media::VideoSource> video;
	Data::Duration currTime;
	UOSInt i;
	if (cmdId >= MNU_PB_CHAPTERS)
	{
		i = (UOSInt)cmdId - MNU_PB_CHAPTERS;
		if (this->PBIsPlaying())
		{
			PBStop();
		}
		if (this->currChapters.SetTo(currChapters))
		{
			this->PBStart(currChapters->GetChapterTime(i));
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
		if (this->PBIsPlaying() && this->currChapters.SetTo(currChapters))
		{
			currTime = this->clk->GetCurrTime();
			i = currChapters->GetChapterIndex(currTime);
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
			this->PBStart(currChapters->GetChapterTime(i));
			this->pbLastChapter = i;
		}
		break;
	case MNU_PB_CHAP_NEXT:
		if (this->PBIsPlaying() && this->currChapters.SetTo(currChapters))
		{
			currTime = this->clk->GetCurrTime();
			i = currChapters->GetChapterIndex(currTime);
			if (i + 1 == this->pbLastChapter)
			{
				i++;
			}
			if (i < currChapters->GetChapterCnt() - 1)
			{
				this->PBStop();
				this->PBStart(currChapters->GetChapterTime(i + 1));
				this->pbLastChapter = i + 1;
			}
		}
		break;
	case MNU_VIDEO_ORISIZE:
		if (this->activeVideo.SetTo(activeVideo))
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

			if (this->currDecoder.SetTo(currDecoder))
			{
				currDecoder->GetVideoInfo(info, tmpV, tmpV, sz1.x);
			}
			else
			{
				activeVideo->GetVideoInfo(info, tmpV, tmpV, sz1.x);
			}
			activeVideo->GetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
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
		if (this->currDecoder.SetTo(currDecoder))
		{
			currDecoder->CaptureImage(VideoCropImage, this);
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
		if (Optional<Media::VideoSource>::ConvertFrom(this->popMedia).SetTo(video))
		{
			NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"SaveTimecode", true);
			dlg->AddFilter(CSTR("*.tc2"), CSTR("Timecode V2"));
			if (dlg->ShowDialog(this->hwnd))
			{
				UTF8Char sbuff[40];
				UnsafeArray<UTF8Char> sptr;
				UOSInt j;
				IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				Text::UTF8Writer writer(fs);
				writer.WriteLine(CSTR("# timecode format v2"));
				j = video->GetFrameCount();
				if (j >= 0)
				{
					i = 0;
					while (i < j)
					{
						sptr = Text::StrInt64(sbuff, video->GetFrameTime(i).GetTotalMS());
						writer.WriteLine(CSTRP(sbuff, sptr));
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
			NN<Media::AudioSource> audio;
			Manage::HiResClock clk;
			Double t;
			clk.Start();
			NN<Media::DrawImage> img;
			if (Optional<Media::AudioSource>::ConvertFrom(this->popMedia).SetTo(audio) && Media::FrequencyGraph::CreateGraph(this->core->GetDrawEngine(), audio, 2048, 2048, Math::FFTCalc::WT_BLACKMANN_HARRIS, 12.0).SetTo(img))
			{
				t = clk.GetTimeDiff();
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("t="));
				sb.AppendDouble(t);
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("Test"), this);

				UTF8Char sbuff[512];
				UnsafeArray<UTF8Char> sptr;
				NN<Media::ImageList> imgList;
				NN<Media::StaticImage> simg;
				if (img->ToStaticImage().SetTo(simg))
				{
					this->core->GetDrawEngine()->DeleteImage(img);
					sbuff[0] = 0;
					sptr = audio->GetSourceName(sbuff).Or(sbuff);
					NEW_CLASSNN(imgList, Media::ImageList(CSTRP(sbuff, sptr)));
					imgList->AddImage(simg, 0);
					this->core->OpenObject(imgList);
				}
				else
				{
					this->core->GetDrawEngine()->DeleteImage(img);
				}
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
