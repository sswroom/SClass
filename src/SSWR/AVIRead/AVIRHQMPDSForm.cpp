#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/CS/TransferFunc.h"
#include "Net/SSLEngineFactory.h"
#include "Net/URL.h"
#include "SSWR/AVIRead/AVIRCaptureDevForm.h"
#include "SSWR/AVIRead/AVIRColorSettingForm.h"
#include "SSWR/AVIRead/AVIRHQMPDSForm.h"
#include "SSWR/AVIRead/AVIRHQMPPlaylistForm.h"
#include "SSWR/AVIRead/AVIROpenFileForm.h"
#include "SSWR/AVIRead/AVIRSetAudioForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIVideoBoxDDLQ.h"
#include "UI/MessageDialog.h"

//#include <windows.h>
#define VFSMODE true

typedef enum
{
	MNU_FILE_CAPTURE_DEVICE = 101,
	MNU_FILE_OPEN,
	MNU_FILE_PLAYLIST,
	MNU_FILE_MON_COLOR,
	MNU_FILE_AUDIO_DEV,
	MNU_FILE_INFO,
	MNU_FILE_CLOSE,
	MNU_PB_START,
	MNU_PB_STOP,
	MNU_PB_FWD,
	MNU_PB_BWD,
	MNU_PB_FWD2,
	MNU_PB_BWD2,
	MNU_PB_CHAP_PREV,
	MNU_PB_CHAP_NEXT,
	MNU_PB_STORE_TIME,
	MNU_PB_RESUME_TIME,
	MNU_VIDEO_ORISIZE,
	MNU_VIDEO_FULLSCN,
	MNU_VIDEO_CROP,
	MNU_VIDEO_DEINT_AUTO,
	MNU_VIDEO_DEINT_PROG,
	MNU_VIDEO_DEINT_TFF,
	MNU_VIDEO_DEINT_BFF,
	MNU_VIDEO_DEINT_30P,
	MNU_VIDEO_TRANT_sRGB,
	MNU_VIDEO_TRANT_BT709,
	MNU_VIDEO_TRANT_GAMMA,
	MNU_VIDEO_TRANT_SMPTE240,
	MNU_VIDEO_TRANT_LINEAR,
	MNU_VIDEO_TRANT_BT1361,
	MNU_VIDEO_TRANT_LOG100,
	MNU_VIDEO_TRANT_LOGSQRT10,
	MNU_VIDEO_TRANT_NTSC,
	MNU_VIDEO_TRANT_SLOG,
	MNU_VIDEO_TRANT_SLOG1,
	MNU_VIDEO_TRANT_SLOG2,
	MNU_VIDEO_TRANT_SLOG3,
	MNU_VIDEO_TRANT_VLOG,
	MNU_VIDEO_PRIMARIES_SOURCE,
	MNU_VIDEO_PRIMARIES_SRGB,
	MNU_VIDEO_PRIMARIES_BT470M,
	MNU_VIDEO_PRIMARIES_BT470BG,
	MNU_VIDEO_PRIMARIES_SMPTE170M,
	MNU_VIDEO_PRIMARIES_SMPTE240M,
	MNU_VIDEO_PRIMARIES_GENERIC_FILM,
	MNU_VIDEO_PRIMARIES_BT2020,
	MNU_VIDEO_PRIMARIES_ADOBE,
	MNU_VIDEO_PRIMARIES_APPLE,
	MNU_VIDEO_PRIMARIES_CIERGB,
	MNU_VIDEO_PRIMARIES_COLORMATCH,
	MNU_VIDEO_PRIMARIES_WIDE,
	MNU_VIDEO_PRIMARIES_SGAMUT,
	MNU_VIDEO_PRIMARIES_SGAMUTCINE,
	MNU_VIDEO_PRIMARIES_DCI_P3,
	MNU_VIDEO_PRIMARIES_ACESGAMUT,
	MNU_VIDEO_PRIMARIES_ALEXAWIDE,
	MNU_VIDEO_PRIMARIES_VGAMUT,
	MNU_VIDEO_YUVT_BT601,
	MNU_VIDEO_YUVT_BT709,
	MNU_VIDEO_YUVT_FCC,
	MNU_VIDEO_YUVT_BT470BG,
	MNU_VIDEO_YUVT_SMPTE170M,
	MNU_VIDEO_YUVT_SMPTE240M,
	MNU_VIDEO_SPAR_AUTO,
	MNU_VIDEO_SPAR_SQR,
	MNU_VIDEO_SPAR_1333,
	MNU_VIDEO_SPAR_DVD4_3,
	MNU_VIDEO_SPAR_DVD16_9,
	MNU_VIDEO_MPAR_SQR,
	MNU_VIDEO_MPAR_1_2,
	MNU_VIDEO_IVTC_ENABLE,
	MNU_VIDEO_IVTC_DISABLE,
	MNU_VIDEO_UVOFST_LEFT,
	MNU_VIDEO_UVOFST_RIGHT,
	MNU_VIDEO_UVOFST_RESET,
	MNU_VIDEO_CROP_ENABLE,
	MNU_VIDEO_CROP_DISABLE,
	MNU_VIDEO_WP_D50,
	MNU_VIDEO_WP_D65,
	MNU_VIDEO_WP_2000K,
	MNU_VIDEO_WP_2500K,
	MNU_VIDEO_WP_3000K,
	MNU_VIDEO_WP_3500K,
	MNU_VIDEO_WP_4000K,
	MNU_VIDEO_WP_4500K,
	MNU_VIDEO_WP_5000K,
	MNU_VIDEO_WP_5500K,
	MNU_VIDEO_WP_6000K,
	MNU_VIDEO_WP_6500K,
	MNU_VIDEO_WP_7000K,
	MNU_VIDEO_WP_7500K,
	MNU_VIDEO_WP_8000K,
	MNU_VIDEO_WP_8500K,
	MNU_VIDEO_WP_9000K,
	MNU_VIDEO_WP_9500K,
	MNU_VIDEO_WP_10000K,
	MNU_VIDEO_WP_11000K,
	MNU_VIDEO_WP_12000K,
	MNU_VIDEO_WP_13000K,
	MNU_VIDEO_WP_14000K,
	MNU_VIDEO_WP_15000K,
	MNU_VIDEO_WP_16000K,
	MNU_VIDEO_WP_17000K,
	MNU_VIDEO_WP_18000K,
	MNU_VIDEO_WP_19000K,

	MNU_PB_CHAPTERS = 1000
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRHQMPDSForm::OnFileDrop(void *userObj, Text::String **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRHQMPDSForm *me = (SSWR::AVIRead::AVIRHQMPDSForm*)userObj;
	UOSInt i;

	me->player->StopPlayback();
	i = 0;
	while (i < nFiles)
	{
		if (me->OpenFile(files[i]->ToCString(), IO::ParserType::MediaFile))
			return;
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPDSForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPDSForm *me = (SSWR::AVIRead::AVIRHQMPDSForm*)userObj;
	if (me->dbgFrm)
	{
		Text::StringBuilderUTF8 sb;
		Media::VideoRenderer::RendererStatus dbg;
		me->vbox->GetStatus(&dbg);
		sb.AppendC(UTF8STRC("Curr Time: "));
		sb.AppendI32(dbg.currTime);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Disp Frame Time: "));
		sb.AppendI32(dbg.dispFrameTime);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Disp Frame Num: "));
		sb.AppendI32(dbg.dispFrameNum);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Proc Delay: "));
		sb.AppendI32(dbg.procDelay);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Disp Delay: "));
		sb.AppendI32(dbg.dispDelay);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Disp Jitter: "));
		sb.AppendI32(dbg.dispJitter);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Video Delay: "));
		sb.AppendI32(dbg.videoDelay);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Source Delay: "));
		sb.AppendI32(dbg.srcDelay);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Frame Displayed: "));
		sb.AppendI32(dbg.frameDispCnt);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Frame Skip before process: "));
		sb.AppendI32(dbg.frameSkipBefore);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Frame Skip after process: "));
		sb.AppendI32(dbg.frameSkipAfter);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("ProcTimes: "));
		Text::SBAppendF64(&sb, dbg.hTime);
		sb.AppendC(UTF8STRC(", "));
		Text::SBAppendF64(&sb, dbg.vTime);
		sb.AppendC(UTF8STRC(", "));
		Text::SBAppendF64(&sb, dbg.csTime);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Buff: "));
		sb.AppendI32(dbg.buffProc);
		sb.AppendC(UTF8STRC(","));
		sb.AppendI32(dbg.buffReady);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Src Size: "));
		sb.AppendOSInt(dbg.srcSize.x);
		sb.AppendC(UTF8STRC(" x "));
		sb.AppendOSInt(dbg.srcSize.y);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("PAR: "));
		Text::SBAppendF64(&sb, dbg.par);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Format: "));
		sb.Append(Media::CS::CSConverter::GetFormatName(dbg.format));
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Output Bitdepth: "));
		sb.AppendI32(dbg.dispBitDepth);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Src YUV Type: "));
		sb.Append(Media::ColorProfile::YUVTypeGetName(dbg.srcYUVType));
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Src R Transfer: "));
		sb.Append(Media::CS::TransferTypeGetName(dbg.color.GetRTranParam()->GetTranType()));
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Src G Transfer: "));
		sb.Append(Media::CS::TransferTypeGetName(dbg.color.GetGTranParam()->GetTranType()));
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Src B Transfer: "));
		sb.Append(Media::CS::TransferTypeGetName(dbg.color.GetBTranParam()->GetTranType()));
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Src Gamma: "));
		Text::SBAppendF64(&sb, dbg.color.GetRTranParam()->GetGamma());
		sb.AppendC(UTF8STRC("\r\n"));
		Media::ColorProfile::ColorPrimaries *primaries = dbg.color.GetPrimaries(); 
		sb.AppendC(UTF8STRC("Src RGB Primary: "));
		sb.Append(Media::ColorProfile::ColorTypeGetName(primaries->colorType));
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("-Red:   "));
		Text::SBAppendF64(&sb, primaries->rx);
		sb.AppendC(UTF8STRC(", "));
		Text::SBAppendF64(&sb, primaries->ry);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("-Green: "));
		Text::SBAppendF64(&sb, primaries->gx);
		sb.AppendC(UTF8STRC(", "));
		Text::SBAppendF64(&sb, primaries->gy);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("-Blue:  "));
		Text::SBAppendF64(&sb, primaries->bx);
		sb.AppendC(UTF8STRC(", "));
		Text::SBAppendF64(&sb, primaries->by);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("-White: "));
		Text::SBAppendF64(&sb, primaries->wx);
		sb.AppendC(UTF8STRC(", "));
		Text::SBAppendF64(&sb, primaries->wy);
		sb.AppendC(UTF8STRC("\r\n"));
		me->txtDebug->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPDSForm::OnDebugClosed(void *userObj, UI::GUIForm *frm)
{
	SSWR::AVIRead::AVIRHQMPDSForm *me = (SSWR::AVIRead::AVIRHQMPDSForm*)userObj;
	me->dbgFrm = 0;
}

Bool SSWR::AVIRead::AVIRHQMPDSForm::OpenFile(Text::CString fileName, IO::ParserType targetType)
{
	Parser::ParserList *parsers = this->core->GetParserList();
	IO::ParsedObject *pobj;
	IO::StmData::FileData *fd;

	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	pobj = parsers->ParseFileType(fd, targetType);
	DEL_CLASS(fd);
	if (pobj)
	{
		return OpenVideo((Media::MediaFile*)pobj);
	}
	return false;
}

Bool SSWR::AVIRead::AVIRHQMPDSForm::OpenVideo(Media::MediaFile *mf)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	OSInt i;
	OSInt j;

	this->player->LoadMedia(0);
	SDEL_CLASS(this->currFile);
	SDEL_CLASS(this->playlist);
	this->currFile = mf;
	sptr = this->currFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("HQMP3DS - ")));
	this->SetText(CSTRP(sbuff, sptr));
	this->player->LoadMedia(mf);
	this->currPBC = this->player;

	this->uOfst = 0;
	this->vOfst = 0;
	this->storeTime = -1;
	this->vbox->SetUVOfst(this->uOfst, this->vOfst);

	this->currChapInfo = this->currFile->GetChapterInfo();
	this->mnuChapters->ClearItems();
	if (this->currChapInfo)
	{
		i = this->currChapInfo->GetChapterCnt();
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
				sptr = this->currChapInfo->GetChapterName(j)->ConcatTo(sptr);
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

	return true;
}

void SSWR::AVIRead::AVIRHQMPDSForm::CloseFile()
{
	this->player->StopPlayback();
	this->player->LoadMedia(0);
	this->storeTime = -1;
	SDEL_CLASS(this->currFile);
	SDEL_CLASS(this->playlist);
	this->currPBC = this->player;
	if (this->qMode == SSWR::AVIRead::AVIRHQMPDSForm::QM_HQ)
	{
		this->SetText(CSTR("HQMP3HQ"));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPDSForm::QM_UQ)
	{
		this->SetText(CSTR("HQMP3UQ"));
	}
	else
	{
		this->SetText(CSTR("HQMP3"));
	}

	this->mnuChapters->ClearItems();
	this->mnuChapters->AddItem(CSTR("No Chapters"), MNU_PB_CHAPTERS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuChapters->SetItemEnabled(MNU_PB_CHAPTERS, false);
	this->UpdateMenu();
}

SSWR::AVIRead::AVIRHQMPDSForm::AVIRHQMPDSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, QualityMode qMode) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->qMode = qMode;
	if (this->qMode == SSWR::AVIRead::AVIRHQMPDSForm::QM_HQ)
	{
		this->SetText(CSTR("HQMP3HQ"));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPDSForm::QM_UQ)
	{
		this->SetText(CSTR("HQMP3UQ"));
	}
	else
	{
		this->SetText(CSTR("HQMP3"));
	}
	this->playlist = 0;
	this->storeTime = -1;

	UI::GUIMenu *mnu;
	UI::GUIMenu *mnu2;
	UI::GUIMenu *mnu3;
	NEW_CLASS(this->mnu, UI::GUIMainMenu());
	mnu = this->mnu->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("&Open..."), MNU_FILE_OPEN, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_O);
	mnu->AddItem(CSTR("Open C&apture Device"), MNU_FILE_CAPTURE_DEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Create Playlist"), MNU_FILE_PLAYLIST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("Set Monitor Color"), MNU_FILE_MON_COLOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Set Audio Device"), MNU_FILE_AUDIO_DEV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("Show &Info"), MNU_FILE_INFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Close"), MNU_FILE_CLOSE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_L);//VK_F4);

	mnu = this->mnu->AddSubMenu(CSTR("&Playback"));
	mnu->AddItem(CSTR("&Start"), MNU_PB_START, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_SPACE);
	mnu->AddItem(CSTR("S&top"), MNU_PB_STOP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_S);
	mnu->AddItem(CSTR("&Forward 10 Seconds"), MNU_PB_FWD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_ADD);
	mnu->AddItem(CSTR("&Backward 10 Seconds"), MNU_PB_BWD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_SUBTRACT);
	mnu->AddItem(CSTR("&Forward 1 Minute"), MNU_PB_FWD2, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_ADD);
	mnu->AddItem(CSTR("&Backward 1 Minute"), MNU_PB_BWD2, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_SUBTRACT);
	mnu->AddItem(CSTR("Store Curr Time"), MNU_PB_STORE_TIME, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_Q);
	mnu->AddItem(CSTR("Resume Stored Time"), MNU_PB_RESUME_TIME, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_W);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("&Previous Chapter"), MNU_PB_CHAP_PREV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_LEFT);
	mnu->AddItem(CSTR("&Next Chapter"), MNU_PB_CHAP_NEXT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_RIGHT);
	this->mnuChapters = mnu->AddSubMenu(CSTR("&Chapters"));

	mnu = this->mnu->AddSubMenu(CSTR("&Video"));
	mnu->AddItem(CSTR("&Original Size"), MNU_VIDEO_ORISIZE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_1);
	mnu->AddItem(CSTR("Switch &Fullscreen"), MNU_VIDEO_FULLSCN, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_ENTER);
	mnu->AddSeperator();
//	mnu->AddItem(CSTR("&Crop Detect"), MNU_VIDEO_CROP, 0, UI::GUIControl::GK_R);
	mnu2 = mnu->AddSubMenu(CSTR("&Deinterlace"));
	mnu2->AddItem(CSTR("&From Video"), MNU_VIDEO_DEINT_AUTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Force &Progressive"), MNU_VIDEO_DEINT_PROG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Force Interlaced (&TFF)"), MNU_VIDEO_DEINT_TFF, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Force Interlaced (&BFF)"), MNU_VIDEO_DEINT_BFF, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("30P Mode"), MNU_VIDEO_DEINT_30P, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Transfer Func"));
	mnu2->AddItem(CSTR("sRGB"), MNU_VIDEO_TRANT_sRGB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("BT.709"), MNU_VIDEO_TRANT_BT709, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Constant Gamma"), MNU_VIDEO_TRANT_GAMMA, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SMPTE 240M"), MNU_VIDEO_TRANT_SMPTE240, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Linear RGB"), MNU_VIDEO_TRANT_LINEAR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("BT.1361"), MNU_VIDEO_TRANT_BT1361, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Log100"), MNU_VIDEO_TRANT_LOG100, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("LogSqrt10"), MNU_VIDEO_TRANT_LOGSQRT10, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("NTSC"), MNU_VIDEO_TRANT_NTSC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Sony S-Log"), MNU_VIDEO_TRANT_SLOG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Sony S-Log1"), MNU_VIDEO_TRANT_SLOG1, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Sony S-Log2"), MNU_VIDEO_TRANT_SLOG2, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Sony S-Log3"), MNU_VIDEO_TRANT_SLOG3, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Panasonic V-Log"), MNU_VIDEO_TRANT_VLOG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("YUV Type"));
	mnu2->AddItem(CSTR("BT.601"), MNU_VIDEO_YUVT_BT601, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("BT.709"), MNU_VIDEO_YUVT_BT709, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("FCC"), MNU_VIDEO_YUVT_FCC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("BT.470BG"), MNU_VIDEO_YUVT_BT470BG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SMPTE170M"), MNU_VIDEO_YUVT_SMPTE170M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SMPTE240M"), MNU_VIDEO_YUVT_SMPTE240M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Color Primaries"));
	mnu2->AddItem(CSTR("From Video"), MNU_VIDEO_PRIMARIES_SOURCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("sRGB"), MNU_VIDEO_PRIMARIES_SRGB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Bt.470M"), MNU_VIDEO_PRIMARIES_BT470M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Bt.470BG"), MNU_VIDEO_PRIMARIES_BT470BG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SMPTE 170M"), MNU_VIDEO_PRIMARIES_SMPTE170M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SMPTE 240M"), MNU_VIDEO_PRIMARIES_SMPTE240M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Generic Film"), MNU_VIDEO_PRIMARIES_GENERIC_FILM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Bt.2020"), MNU_VIDEO_PRIMARIES_BT2020, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Adobe RGB"), MNU_VIDEO_PRIMARIES_ADOBE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Apple RGB"), MNU_VIDEO_PRIMARIES_APPLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("CIE RGB"), MNU_VIDEO_PRIMARIES_CIERGB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("ColorMatch RGB"), MNU_VIDEO_PRIMARIES_COLORMATCH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Wide"), MNU_VIDEO_PRIMARIES_WIDE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Sony S-Gamut3 (S-Gamut)"), MNU_VIDEO_PRIMARIES_SGAMUT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Sony S-Gamut3.Cine"), MNU_VIDEO_PRIMARIES_SGAMUTCINE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("DCI-P3"), MNU_VIDEO_PRIMARIES_DCI_P3, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("ACES-Gamut"), MNU_VIDEO_PRIMARIES_ACESGAMUT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("ALEXA Wide"), MNU_VIDEO_PRIMARIES_ALEXAWIDE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Panasonic V-Gamut"), MNU_VIDEO_PRIMARIES_VGAMUT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu(CSTR("Custom White Point"));
	mnu3->AddItem(CSTR("D50"), MNU_VIDEO_WP_D50, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("D65"), MNU_VIDEO_WP_D65, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("2000K"), MNU_VIDEO_WP_2000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("2500K"), MNU_VIDEO_WP_2500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("3000K"), MNU_VIDEO_WP_3000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("3500K"), MNU_VIDEO_WP_3500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("4000K"), MNU_VIDEO_WP_4000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("4500K"), MNU_VIDEO_WP_4500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("5000K"), MNU_VIDEO_WP_5000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("5500K"), MNU_VIDEO_WP_5500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("6000K"), MNU_VIDEO_WP_6000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("6500K"), MNU_VIDEO_WP_6500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("7000K"), MNU_VIDEO_WP_7000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("7500K"), MNU_VIDEO_WP_7500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("8000K"), MNU_VIDEO_WP_8000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("8500K"), MNU_VIDEO_WP_8500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("9000K"), MNU_VIDEO_WP_9000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("9500K"), MNU_VIDEO_WP_9500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("10000K"), MNU_VIDEO_WP_10000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("11000K"), MNU_VIDEO_WP_11000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("12000K"), MNU_VIDEO_WP_12000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("13000K"), MNU_VIDEO_WP_13000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("14000K"), MNU_VIDEO_WP_14000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("15000K"), MNU_VIDEO_WP_15000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("16000K"), MNU_VIDEO_WP_16000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("17000K"), MNU_VIDEO_WP_17000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("18000K"), MNU_VIDEO_WP_18000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("19000K"), MNU_VIDEO_WP_19000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Source PAR"));
	mnu2->AddItem(CSTR("From Video"), MNU_VIDEO_SPAR_AUTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Square Pixel"), MNU_VIDEO_SPAR_SQR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("1.333"), MNU_VIDEO_SPAR_1333, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("0.889 (4:3 on DVD)"), MNU_VIDEO_SPAR_DVD4_3, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("1.185 (16:9 on DVD)"), MNU_VIDEO_SPAR_DVD16_9, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Monitor PAR"));
	mnu2->AddItem(CSTR("Square Pixel"), MNU_VIDEO_MPAR_SQR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("1:2"), MNU_VIDEO_MPAR_1_2, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Filter"));
	mnu3 = mnu2->AddSubMenu(CSTR("IVTC"));
	mnu3->AddItem(CSTR("Enable"), MNU_VIDEO_IVTC_ENABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Disable"), MNU_VIDEO_IVTC_DISABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu(CSTR("UV Offset"));
	mnu3->AddItem(CSTR("Move Left"), MNU_VIDEO_UVOFST_LEFT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_OEM_4);
	mnu3->AddItem(CSTR("Move Right"), MNU_VIDEO_UVOFST_RIGHT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_OEM_6);
	mnu3->AddItem(CSTR("Reset"), MNU_VIDEO_UVOFST_RESET, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu(CSTR("Auto Crop"));
	mnu3->AddItem(CSTR("Enable"), MNU_VIDEO_CROP_ENABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_R);
	mnu3->AddItem(CSTR("Disable"), MNU_VIDEO_CROP_DISABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	
	mnu = this->mnu->AddSubMenu(CSTR("&Audio"));

	this->SetMenu(this->mnu);

	if (this->qMode == SSWR::AVIRead::AVIRHQMPDSForm::QM_HQ)
	{
		NEW_CLASS(this->vbox, UI::GUIVideoBoxDD(ui, this, this->colorSess, 6, 2));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPDSForm::QM_UQ)
	{
		NEW_CLASS(this->vbox, UI::GUIVideoBoxDD(ui, this, this->colorSess, 6, 2));
	}
	else
	{
		NEW_CLASS(this->vbox, UI::GUIVideoBoxDDLQ(ui, this, this->colorSess, 6, 2));
	}

	this->vbox->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vbox->SetUserFSMode(VFSMODE?(UI::GUIDDrawControl::SM_VFS):(UI::GUIDDrawControl::SM_FS));
	this->HandleDropFiles(OnFileDrop, this);
	this->uOfst = 0;
	this->vOfst = 0;

	NEW_CLASS(this->player, Media::MediaPlayer(this->vbox, this->core->GetAudioDevice()));
	this->currFile = 0;
	CloseFile();

	this->dbgFrm = 0;
	this->AddTimer(30, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHQMPDSForm::~AVIRHQMPDSForm()
{
	this->CloseFile();
	DEL_CLASS(this->player);
	if (this->dbgFrm)
		this->dbgFrm->Close();
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRHQMPDSForm::EventMenuClicked(UInt16 cmdId)
{
	Int32 currTime;
	OSInt i;
	if (cmdId >= MNU_PB_CHAPTERS)
	{
		i = cmdId - MNU_PB_CHAPTERS;
		if (this->currChapInfo)
		{
			this->player->GotoChapter(i);
		}
		return;
	}

	switch (cmdId)
	{
	case MNU_FILE_OPEN:
		{
			SSWR::AVIRead::AVIROpenFileForm dlg(0, this->ui, this->core, IO::ParserType::MediaFile);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Text::String *fname = dlg.GetFileName();
				UOSInt i = fname->IndexOf(':');
				if (i == 1 || i == INVALID_INDEX)
				{
					this->OpenFile(dlg.GetFileName()->ToCString(), dlg.GetParserType());
				}
				else
				{
					IO::ParsedObject *pobj = Net::URL::OpenObject(fname->ToCString(), CSTR("HQMP/1.0"), this->core->GetSocketFactory(), this->ssl, 15000);
					if (pobj == 0)
					{
						UI::MessageDialog::ShowDialog(CSTR("Error in loading file"), CSTR("HQMP"), this);
					}
					else
					{
						if (pobj->GetParserType() == IO::ParserType::MediaFile)
						{
							this->OpenVideo((Media::MediaFile*)pobj);
						}
						else
						{
							DEL_CLASS(pobj);
						}
					}
				}
			}
		}
		break;
	case MNU_FILE_CAPTURE_DEVICE:
		{
			SSWR::AVIRead::AVIRCaptureDevForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				UTF8Char sbuff[256];
				UTF8Char* sptr;
				Media::MediaFile *mf;
				sptr = dlg.capture->GetSourceName(sbuff);
				NEW_CLASS(mf, Media::MediaFile(CSTRP(sbuff, sptr)));
				mf->AddSource(dlg.capture, 0);
				this->OpenVideo(mf);
			}
		}
		break;
	case MNU_FILE_PLAYLIST:
		{
			SSWR::AVIRead::AVIRHQMPPlaylistForm dlg(0, this->ui, this->core, this->playlist);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->currPBC->StopPlayback();
				SDEL_CLASS(this->playlist);
				this->playlist = dlg.GetPlaylist();
				this->playlist->SetPlayer(this->player);
				this->currPBC = this->playlist;
			}
		}
		break;
	case MNU_FILE_MON_COLOR:
		{
			SSWR::AVIRead::AVIRColorSettingForm dlg(0, this->ui, this->core, this->GetHMonitor());
			dlg.ShowDialog(this);
		}
		break;
	case MNU_FILE_AUDIO_DEV:
		{
			SSWR::AVIRead::AVIRSetAudioForm dlg(0, this->ui, this->core);
			dlg.ShowDialog(this);
			this->player->SwitchAudio(0);
		}
		break;
	case MNU_FILE_INFO:
		if (this->dbgFrm == 0)
		{
			NEW_CLASS(this->dbgFrm, UI::GUIForm(0, 320, 360, ui));
			NEW_CLASS(this->txtDebug, UI::GUITextBox(ui, this->dbgFrm, CSTR(""), true));
			this->txtDebug->SetReadOnly(true);
			this->txtDebug->SetDockType(UI::GUIControl::DOCK_FILL);
			this->dbgFrm->SetFont(0, 0, 8.25, false);
			this->dbgFrm->SetText(CSTR("Info"));
			this->dbgFrm->Show();
			this->dbgFrm->HandleFormClosed(OnDebugClosed, this);
		}
		break;
	case MNU_FILE_CLOSE:
		this->CloseFile();
		break;
	case MNU_PB_START:
		if (!this->player->IsPlaying())
		{
			this->currPBC->StartPlayback();
		}
		break;
	case MNU_PB_STOP:
		this->currPBC->StopPlayback();
		break;
	case MNU_PB_FWD:
		if (this->player->IsPlaying())
		{
			currTime = this->player->GetCurrTime();
			this->player->SeekTo(currTime + 10000);
		}
		break;
	case MNU_PB_BWD:
		if (this->player->IsPlaying())
		{
			currTime = this->player->GetCurrTime() - 10000;
			if (currTime < 0)
				currTime = 0;
			this->player->SeekTo(currTime);
		}
		break;
	case MNU_PB_FWD2:
		if (this->player->IsPlaying())
		{
			currTime = this->player->GetCurrTime();
			this->player->SeekTo(currTime + 60000);
		}
		break;
	case MNU_PB_BWD2:
		if (this->player->IsPlaying())
		{
			currTime = this->player->GetCurrTime() - 60000;
			if (currTime < 0)
				currTime = 0;
			this->player->SeekTo(currTime);
		}
		break;
	case MNU_PB_STORE_TIME:
		if (this->player->IsPlaying())
		{
			this->storeTime = this->player->GetCurrTime();
		}
		break;
	case MNU_PB_RESUME_TIME:
		if (this->storeTime != -1)
		{
			if (!this->player->IsPlaying())
			{
				this->currPBC->StartPlayback();
			}
			this->player->SeekTo(storeTime);
		}
		break;
	case MNU_PB_CHAP_PREV:
		this->currPBC->PrevChapter();
		break;
	case MNU_PB_CHAP_NEXT:
		this->currPBC->NextChapter();
		break;
	case MNU_VIDEO_ORISIZE:
		{
			Math::Size2D<UOSInt> vSize;
			if (this->player->GetVideoSize(&vSize.x, &vSize.y))
			{
				Math::Size2D<UOSInt> size1;
				Math::Size2D<UOSInt> size2;

				if (this->vbox->IsFullScreen())
				{
					this->vbox->SwitchFullScreen(false, false);
				}
				size1 = this->vbox->GetSizeP();
				size2 = this->GetSizeP();

				this->SetFormState(UI::GUIForm::FS_NORMAL);
				if (size1 == vSize)
				{
					this->vbox->OnSizeChanged(false);
				}
				else
				{
					this->SetSizeP(size2 - size1 + vSize);
				}
			}
		}
		break;
	case MNU_VIDEO_FULLSCN:
		this->vbox->SwitchFullScreen(!this->vbox->IsFullScreen(), VFSMODE);
		break;
	case MNU_VIDEO_CROP:
//		this->player->DetectCrop();
		break;
	case MNU_VIDEO_DEINT_AUTO:
		this->vbox->SetDeintType(UI::GUIVideoBoxDD::DT_FROM_VIDEO);
		break;
	case MNU_VIDEO_DEINT_PROG:
		this->vbox->SetDeintType(UI::GUIVideoBoxDD::DT_PROGRESSIVE);
		break;
	case MNU_VIDEO_DEINT_TFF:
		this->vbox->SetDeintType(UI::GUIVideoBoxDD::DT_INTERLACED_TFF);
		break;
	case MNU_VIDEO_DEINT_BFF:
		this->vbox->SetDeintType(UI::GUIVideoBoxDD::DT_INTERLACED_BFF);
		break;
	case MNU_VIDEO_DEINT_30P:
		this->vbox->SetDeintType(UI::GUIVideoBoxDD::DT_30P_MODE);
		break;
	case MNU_VIDEO_TRANT_sRGB:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_sRGB);
		break;
	case MNU_VIDEO_TRANT_BT709:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_BT709);
		break;
	case MNU_VIDEO_TRANT_GAMMA:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_GAMMA);
		break;
	case MNU_VIDEO_TRANT_SMPTE240:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_SMPTE240);
		break;
	case MNU_VIDEO_TRANT_LINEAR:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_LINEAR);
		break;
	case MNU_VIDEO_TRANT_BT1361:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_BT1361);
		break;
	case MNU_VIDEO_TRANT_LOG100:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_LOG100);
		break;
	case MNU_VIDEO_TRANT_LOGSQRT10:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_LOGSQRT10);
		break;
	case MNU_VIDEO_TRANT_NTSC:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_NTSC);
		break;
	case MNU_VIDEO_TRANT_SLOG:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_SLOG);
		break;
	case MNU_VIDEO_TRANT_SLOG1:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_SLOG1);
		break;
	case MNU_VIDEO_TRANT_SLOG2:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_SLOG2);
		break;
	case MNU_VIDEO_TRANT_SLOG3:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_SLOG3);
		break;
	case MNU_VIDEO_TRANT_VLOG:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_VLOG);
		break;
	case MNU_VIDEO_PRIMARIES_SOURCE:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_VUNKNOWN);
		break;
	case MNU_VIDEO_PRIMARIES_SRGB:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_SRGB);
		break;
	case MNU_VIDEO_PRIMARIES_BT470M:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_BT470M);
		break;
	case MNU_VIDEO_PRIMARIES_BT470BG:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_BT470BG);
		break;
	case MNU_VIDEO_PRIMARIES_SMPTE170M:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_SMPTE170M);
		break;
	case MNU_VIDEO_PRIMARIES_SMPTE240M:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_SMPTE240M);
		break;
	case MNU_VIDEO_PRIMARIES_GENERIC_FILM:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_GENERIC_FILM);
		break;
	case MNU_VIDEO_PRIMARIES_BT2020:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_BT2020);
		break;
	case MNU_VIDEO_PRIMARIES_ADOBE:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_ADOBE);
		break;
	case MNU_VIDEO_PRIMARIES_APPLE:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_APPLE);
		break;
	case MNU_VIDEO_PRIMARIES_CIERGB:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_CIERGB);
		break;
	case MNU_VIDEO_PRIMARIES_COLORMATCH:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_COLORMATCH);
		break;
	case MNU_VIDEO_PRIMARIES_WIDE:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_WIDE);
		break;
	case MNU_VIDEO_PRIMARIES_SGAMUT:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_SGAMUT);
		break;
	case MNU_VIDEO_PRIMARIES_SGAMUTCINE:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_SGAMUTCINE);
		break;
	case MNU_VIDEO_PRIMARIES_DCI_P3:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_DCI_P3);
		break;
	case MNU_VIDEO_PRIMARIES_ACESGAMUT:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_ACESGAMUT);
		break;
	case MNU_VIDEO_PRIMARIES_ALEXAWIDE:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_ALEXAWIDE);
		break;
	case MNU_VIDEO_PRIMARIES_VGAMUT:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_VGAMUT);
		break;
	case MNU_VIDEO_YUVT_BT601:
		this->vbox->SetSrcYUVType(Media::ColorProfile::YUVT_BT601);
		break;
	case MNU_VIDEO_YUVT_BT709:
		this->vbox->SetSrcYUVType(Media::ColorProfile::YUVT_BT709);
		break;
	case MNU_VIDEO_YUVT_FCC:
		this->vbox->SetSrcYUVType(Media::ColorProfile::YUVT_FCC);
		break;
	case MNU_VIDEO_YUVT_BT470BG:
		this->vbox->SetSrcYUVType(Media::ColorProfile::YUVT_BT470BG);
		break;
	case MNU_VIDEO_YUVT_SMPTE170M:
		this->vbox->SetSrcYUVType(Media::ColorProfile::YUVT_SMPTE170M);
		break;
	case MNU_VIDEO_YUVT_SMPTE240M:
		this->vbox->SetSrcYUVType(Media::ColorProfile::YUVT_SMPTE240M);
		break;
	case MNU_VIDEO_SPAR_AUTO:
		this->vbox->SetSrcPAR(0);
		break;
	case MNU_VIDEO_SPAR_SQR:
		this->vbox->SetSrcPAR(1.0);
		break;
	case MNU_VIDEO_SPAR_1333:
		this->vbox->SetSrcPAR(3.0/4.0);
		break;
	case MNU_VIDEO_SPAR_DVD4_3:
		this->vbox->SetSrcPAR(1.125);
		break;
	case MNU_VIDEO_SPAR_DVD16_9:
		this->vbox->SetSrcPAR(0.84375);
		break;
	case MNU_VIDEO_MPAR_SQR:
		this->vbox->SetMonPAR(1.0);
		break;
	case MNU_VIDEO_MPAR_1_2:
		this->vbox->SetMonPAR(0.5);
		break;
	case MNU_VIDEO_IVTC_ENABLE:
		this->vbox->SetIVTCEnable(true);
		break;
	case MNU_VIDEO_IVTC_DISABLE:
		this->vbox->SetIVTCEnable(false);
		break;
	case MNU_VIDEO_UVOFST_LEFT:
		this->uOfst--;
		this->vOfst--;
		this->vbox->SetUVOfst(this->uOfst, this->vOfst);
		break;
	case MNU_VIDEO_UVOFST_RIGHT:
		this->uOfst++;
		this->vOfst++;
		this->vbox->SetUVOfst(this->uOfst, this->vOfst);
		break;
	case MNU_VIDEO_UVOFST_RESET:
		this->uOfst = 0;
		this->vOfst = 0;
		this->vbox->SetUVOfst(this->uOfst, this->vOfst);
		break;
	case MNU_VIDEO_CROP_ENABLE:
		this->vbox->SetAutoCropEnable(true);
		break;
	case MNU_VIDEO_CROP_DISABLE:
		this->vbox->SetAutoCropEnable(false);
		break;
	case MNU_VIDEO_WP_D50:
		this->vbox->SetSrcWP(Media::ColorProfile::WPT_D50);
		break;
	case MNU_VIDEO_WP_D65:
		this->vbox->SetSrcWP(Media::ColorProfile::WPT_D65);
		break;
	case MNU_VIDEO_WP_2000K:
		this->vbox->SetSrcWPTemp(2000);
		break;
	case MNU_VIDEO_WP_2500K:
		this->vbox->SetSrcWPTemp(2500);
		break;
	case MNU_VIDEO_WP_3000K:
		this->vbox->SetSrcWPTemp(3000);
		break;
	case MNU_VIDEO_WP_3500K:
		this->vbox->SetSrcWPTemp(3500);
		break;
	case MNU_VIDEO_WP_4000K:
		this->vbox->SetSrcWPTemp(4000);
		break;
	case MNU_VIDEO_WP_4500K:
		this->vbox->SetSrcWPTemp(4500);
		break;
	case MNU_VIDEO_WP_5000K:
		this->vbox->SetSrcWPTemp(5000);
		break;
	case MNU_VIDEO_WP_5500K:
		this->vbox->SetSrcWPTemp(5500);
		break;
	case MNU_VIDEO_WP_6000K:
		this->vbox->SetSrcWPTemp(6000);
		break;
	case MNU_VIDEO_WP_6500K:
		this->vbox->SetSrcWPTemp(6500);
		break;
	case MNU_VIDEO_WP_7000K:
		this->vbox->SetSrcWPTemp(7000);
		break;
	case MNU_VIDEO_WP_7500K:
		this->vbox->SetSrcWPTemp(7500);
		break;
	case MNU_VIDEO_WP_8000K:
		this->vbox->SetSrcWPTemp(8000);
		break;
	case MNU_VIDEO_WP_8500K:
		this->vbox->SetSrcWPTemp(8500);
		break;
	case MNU_VIDEO_WP_9000K:
		this->vbox->SetSrcWPTemp(9000);
		break;
	case MNU_VIDEO_WP_9500K:
		this->vbox->SetSrcWPTemp(9500);
		break;
	case MNU_VIDEO_WP_10000K:
		this->vbox->SetSrcWPTemp(10000);
		break;
	case MNU_VIDEO_WP_11000K:
		this->vbox->SetSrcWPTemp(11000);
		break;
	case MNU_VIDEO_WP_12000K:
		this->vbox->SetSrcWPTemp(12000);
		break;
	case MNU_VIDEO_WP_13000K:
		this->vbox->SetSrcWPTemp(13000);
		break;
	case MNU_VIDEO_WP_14000K:
		this->vbox->SetSrcWPTemp(14000);
		break;
	case MNU_VIDEO_WP_15000K:
		this->vbox->SetSrcWPTemp(15000);
		break;
	case MNU_VIDEO_WP_16000K:
		this->vbox->SetSrcWPTemp(16000);
		break;
	case MNU_VIDEO_WP_17000K:
		this->vbox->SetSrcWPTemp(17000);
		break;
	case MNU_VIDEO_WP_18000K:
		this->vbox->SetSrcWPTemp(18000);
		break;
	case MNU_VIDEO_WP_19000K:
		this->vbox->SetSrcWPTemp(19000);
		break;
	}
}
	
void SSWR::AVIRead::AVIRHQMPDSForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
}
