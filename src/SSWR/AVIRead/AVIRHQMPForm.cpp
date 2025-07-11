#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/PowerInfo.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/MediaPlayerWebInterface.h"
#include "Media/CS/TransferFunc.h"
#include "Net/MIME.h"
#include "Net/SSLEngineFactory.h"
#include "Net/URL.h"
#include "SSWR/AVIRead/AVIRCaptureDevForm.h"
#include "SSWR/AVIRead/AVIRColorSettingForm.h"
#include "SSWR/AVIRead/AVIRHQMPForm.h"
#include "SSWR/AVIRead/AVIRHQMPPlaylistForm.h"
#include "SSWR/AVIRead/AVIROpenFileForm.h"
#include "SSWR/AVIRead/AVIRSetAudioForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"
#include "UI/GUIVideoBoxDDLQ.h"

#include "IO/DebugWriter.h"

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
	MNU_FILE_HTTP_ENABLE,
	MNU_FILE_HTTP_DISABLE,
	MNU_PB_START,
	MNU_PB_STOP,
	MNU_PB_PAUSE,
	MNU_PB_FWD,
	MNU_PB_BWD,
	MNU_PB_FWD2,
	MNU_PB_BWD2,
	MNU_PB_CHAP_PREV,
	MNU_PB_CHAP_NEXT,
	MNU_PB_AVOFST_INC,
	MNU_PB_AVOFST_DEC,
	MNU_VIDEO_ORISIZE,
	MNU_VIDEO_FULLSCN,
	MNU_VIDEO_CROP,
	MNU_VIDEO_DEINT_AUTO,
	MNU_VIDEO_DEINT_PROG,
	MNU_VIDEO_DEINT_TFF,
	MNU_VIDEO_DEINT_BFF,
	MNU_VIDEO_DEINT_30P,
	MNU_VIDEO_PRESET_BT709,
	MNU_VIDEO_PRESET_BT2020,
	MNU_VIDEO_PRESET_BT2100,
	MNU_VIDEO_TRANT_sRGB,
	MNU_VIDEO_TRANT_BT709,
	MNU_VIDEO_TRANT_GAMMA,
	MNU_VIDEO_TRANT_SMPTE240,
	MNU_VIDEO_TRANT_LINEAR,
	MNU_VIDEO_TRANT_BT1361,
	MNU_VIDEO_TRANT_BT2100,
	MNU_VIDEO_TRANT_LOG100,
	MNU_VIDEO_TRANT_LOGSQRT10,
	MNU_VIDEO_TRANT_PROTUNE,
	MNU_VIDEO_TRANT_NTSC,
	MNU_VIDEO_TRANT_HLG,
	MNU_VIDEO_TRANT_SLOG,
	MNU_VIDEO_TRANT_SLOG1,
	MNU_VIDEO_TRANT_SLOG2,
	MNU_VIDEO_TRANT_SLOG3,
	MNU_VIDEO_TRANT_VLOG,
	MNU_VIDEO_TRANT_NLOG,
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
	MNU_VIDEO_PRIMARIES_GOPRO_PROTUNE,
	MNU_VIDEO_YUVT_BT601,
	MNU_VIDEO_YUVT_BT709,
	MNU_VIDEO_YUVT_FCC,
	MNU_VIDEO_YUVT_BT470BG,
	MNU_VIDEO_YUVT_SMPTE170M,
	MNU_VIDEO_YUVT_SMPTE240M,
	MNU_VIDEO_YUVT_BT2020,
	MNU_VIDEO_YUVT_F_BT601,
	MNU_VIDEO_YUVT_F_BT709,
	MNU_VIDEO_YUVT_F_FCC,
	MNU_VIDEO_YUVT_F_BT470BG,
	MNU_VIDEO_YUVT_F_SMPTE170M,
	MNU_VIDEO_YUVT_F_SMPTE240M,
	MNU_VIDEO_YUVT_F_BT2020,
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
	MNU_VIDEO_FILTER_BW,
	MNU_VIDEO_FILTER_BG,
	MNU_VIDEO_FILTER_BG_ABS,
	MNU_VIDEO_FILTER_BG_CREATE,
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
	MNU_VIDEO_FTIME_ENABLE,
	MNU_VIDEO_FTIME_DISABLE,
	MNU_VIDEO_SNAPSHOT,
	MNU_VIDEO_ROTATE_NONE,
	MNU_VIDEO_ROTATE_CW90,
	MNU_VIDEO_ROTATE_CW180,
	MNU_VIDEO_ROTATE_CW270,
	MNU_VIDEO_ROTATE_HFLIP,
	MNU_VIDEO_ROTATE_HFLIP_CW90,
	MNU_VIDEO_ROTATE_HFLIP_CW180,
	MNU_VIDEO_ROTATE_HFLIP_CW270,

	MNU_PB_CHAPTERS = 1000
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRHQMPForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRHQMPForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPForm>();
	UOSInt i;
	UOSInt nFiles = files.GetCount();
	NN<Media::MediaPlayer> player;

	if (me->player.SetTo(player)) player->StopPlayback();
	i = 0;
	while (i < nFiles)
	{
		if (me->OpenFile(files[i]->ToCString(), IO::ParserType::MediaFile))
			return;
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHQMPForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPForm>();
	NN<Media::MediaPlayer> player;
	NN<Media::PBControl> currPBC;
	if (me->dbgFrm.NotNull())
	{
		Text::StringBuilderUTF8 sb;
		Media::VideoRenderer::RendererStatus2 dbg;
		me->vbox->GetStatus(dbg);
		sb.AppendC(UTF8STRC("Curr Time: "));
		sb.AppendDur(dbg.currTime);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Disp Frame Time: "));
		sb.AppendDur(dbg.dispFrameTime);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Disp Frame Num: "));
		sb.AppendU32(dbg.dispFrameNum);
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
		sb.AppendDur(dbg.videoDelay);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Source Delay: "));
		sb.AppendI32(dbg.srcDelay);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("A/V Offset: "));
		sb.AppendI32(dbg.avOfst);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Seek Count: "));
		sb.AppendUOSInt(dbg.seekCnt);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Frame Displayed: "));
		sb.AppendU32(dbg.frameDispCnt);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Frame Skip before process: "));
		sb.AppendU32(dbg.frameSkipBefore);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Frame Skip after process: "));
		sb.AppendU32(dbg.frameSkipAfter);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("ProcTimes H: "));
		sb.AppendDouble(dbg.hTime);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("ProcTimes V: "));
		sb.AppendDouble(dbg.vTime);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("ProcTimes C: "));
		sb.AppendDouble(dbg.csTime);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Buff: "));
		sb.AppendI32(dbg.buffProc);
		sb.AppendC(UTF8STRC(","));
		sb.AppendI32(dbg.buffReady);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Src Size: "));
		sb.AppendUOSInt(dbg.srcSize.x);
		sb.AppendC(UTF8STRC(" x "));
		sb.AppendUOSInt(dbg.srcSize.y);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Disp Size: "));
		sb.AppendUOSInt(dbg.dispSize.x);
		sb.AppendC(UTF8STRC(" x "));
		sb.AppendUOSInt(dbg.dispSize.y);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("PAR: "));
		sb.AppendDouble(dbg.par);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Rotate: "));
		sb.AppendOpt(Media::RotateTypeGetName(dbg.rotateType));
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Decoder: "));
		if (dbg.decoderName.v.NotNull())
		{
			sb.AppendOpt(dbg.decoderName);
		}
		else
		{
			sb.AppendC(UTF8STRC("-"));
		}
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Format: "));
		sb.Append(Media::CS::CSConverter::GetFormatName(dbg.format));
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Output Bitdepth: "));
		sb.AppendU32(dbg.dispBitDepth);
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
		sb.AppendDouble(dbg.color.GetRTranParam()->GetGamma());
		sb.AppendC(UTF8STRC("\r\n"));
		NN<Media::ColorProfile::ColorPrimaries> primaries = dbg.color.GetPrimaries(); 
		sb.AppendC(UTF8STRC("Src RGB Primary: "));
		sb.Append(Media::ColorProfile::ColorTypeGetName(primaries->colorType));
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("-Red:   "));
		sb.AppendDouble(primaries->r.x);
		sb.AppendC(UTF8STRC(", "));
		sb.AppendDouble(primaries->r.y);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("-Green: "));
		sb.AppendDouble(primaries->g.x);
		sb.AppendC(UTF8STRC(", "));
		sb.AppendDouble(primaries->g.y);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("-Blue:  "));
		sb.AppendDouble(primaries->b.x);
		sb.AppendC(UTF8STRC(", "));
		sb.AppendDouble(primaries->b.y);
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("-White: "));
		sb.AppendDouble(primaries->w.x);
		sb.AppendC(UTF8STRC(", "));
		sb.AppendDouble(primaries->w.y);
		sb.AppendC(UTF8STRC("\r\n"));
		me->txtDebug->SetText(sb.ToCString());
	}
	if (me->player.SetTo(player) && player->IsPlaying())
	{
		me->ui->UseDevice(true, true);
	}
	if (me->pbEnd)
	{
		me->pbEnd = false;
		UTF8Char sbuff[512];
		UOSInt i;
		UOSInt j;
		UnsafeArray<UTF8Char> sptr;
		UnsafeArray<UTF8Char> sptrEnd;
		Int32 partNum;
		NN<Media::MediaFile> openedFile;
		if (me->GetOpenedFile().SetTo(openedFile))
		{
			sptrEnd = openedFile->GetSourceName(sbuff);
		}
		else
		{
			sptrEnd = Text::StrConcatC(sbuff, UTF8STRC("Untitled"));
		}
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptrEnd - sbuff), '.');
		j = Text::StrIndexOfICase(sbuff, (const UTF8Char*)"part");
		if (i > j && i != INVALID_INDEX && j != INVALID_INDEX)
		{
			sbuff[i] = 0;
			partNum = Text::StrToInt32(&sbuff[j + 4]);
			if (partNum > 0)
			{
				partNum++;
				if (partNum == 10)
				{
					sptrEnd = Text::StrConcat(&sbuff[i + 2], &sbuff[i + 1]);
					sptr = Text::StrInt32(&sbuff[j + 4], partNum);
					*sptr++ = '.';
				}
				else
				{
					sptr = Text::StrInt32(&sbuff[j + 4], partNum);
					*sptr++ = '.';
				}

				if (me->OpenFile(CSTRP(sbuff, sptrEnd), IO::ParserType::MediaFile))
				{
					if (me->player.SetTo(player) && me->currPBC.SetTo(currPBC) && !player->IsPlaying())
					{
						currPBC->StartPlayback();
					}
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPForm::OnDebugClosed(AnyType userObj, NN<UI::GUIForm> frm)
{
	NN<SSWR::AVIRead::AVIRHQMPForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPForm>();
	me->dbgFrm = 0;
}

void __stdcall SSWR::AVIRead::AVIRHQMPForm::OnVideoEnd(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHQMPForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPForm>();
	me->pbEnd = true;
}

void __stdcall SSWR::AVIRead::AVIRHQMPForm::OnMouseAction(AnyType userObj, UI::GUIVideoBoxDD::MouseAction ma, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRHQMPForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPForm>();
	if (ma == UI::GUIVideoBoxDD::MA_START)
	{
		me->PBStart();
	}
	else if (ma == UI::GUIVideoBoxDD::MA_PAUSE)
	{
		me->PBPause();
	}
	else if (ma == UI::GUIVideoBoxDD::MA_STOP)
	{
		me->PBStop();
	}
}

void SSWR::AVIRead::AVIRHQMPForm::OnMediaOpened()
{
	NN<Media::MediaFile> openedFile;
	if (!this->GetOpenedFile().SetTo(openedFile))
		return;
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	NN<Media::ChapterInfo> currChapInfo;
	UOSInt i;
	UOSInt j;
#if defined(_WIN64)
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		sptr = openedFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("HQMP3HQ64 - ")));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		sptr = openedFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("HQMP3UQ64 - ")));
	}
	else
	{
		sptr = openedFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("HQMP3_64 - ")));
	}
#else
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		sptr = openedFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("HQMP3HQ - ")));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		sptr = openedFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("HQMP3UQ - ")));
	}
	else
	{
		sptr = openedFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("HQMP3 - ")));
	}
#endif
	this->SetText(CSTRP(sbuff, sptr));

	this->uOfst = 0;
	this->vOfst = 0;
	this->vbox->SetUVOfst(this->uOfst, this->vOfst);

	this->currChapInfo = openedFile->GetChapterInfo();
	this->mnuChapters->ClearItems();
	if (this->currChapInfo.SetTo(currChapInfo))
	{
		i = currChapInfo->GetChapterCnt();
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
				sptr = Text::String::OrEmpty(currChapInfo->GetChapterName(j))->ConcatTo(sptr);
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
	openedFile->GetSourceName(sbuff);
	if (Text::StrIndexOfICase(sbuff, (const UTF8Char*)"sRGB") != INVALID_INDEX)
	{
		this->vbox->SetSrcRGBType(Media::CS::TRANT_sRGB);
	}
	else if (Text::StrIndexOfICase(sbuff, (const UTF8Char*)"BT709") != INVALID_INDEX)
	{
		this->vbox->SetSrcRGBType(Media::CS::TRANT_BT709);
	}
}

void SSWR::AVIRead::AVIRHQMPForm::OnMediaClosed()
{
	this->playlist.Delete();

#if defined(_WIN64)
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		this->SetText(CSTR("HQMP3HQ64"));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		this->SetText(CSTR("HQMP3UQ64"));
	}
	else
	{
		this->SetText(CSTR("HQMP3_64"));
	}
#else
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		this->SetText(CSTR("HQMP3HQ"));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		this->SetText(CSTR("HQMP3UQ"));
	}
	else
	{
		this->SetText(CSTR("HQMP3"));
	}
#endif
	this->mnuChapters->ClearItems();
	this->mnuChapters->AddItem(CSTR("No Chapters"), MNU_PB_CHAPTERS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuChapters->SetItemEnabled(MNU_PB_CHAPTERS, false);
	this->UpdateMenu();
}

SSWR::AVIRead::AVIRHQMPForm::AVIRHQMPForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, QualityMode qMode) : UI::GUIForm(parent, 1024, 768, ui), Media::MediaPlayerInterface(core->GetParserList())
{
	this->core = core;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->qMode = qMode;
	this->pbEnd = false;
	this->listener = 0;
#if defined(_WIN64)
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		this->SetText(CSTR("HQMP3HQ64"));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		this->SetText(CSTR("HQMP3UQ64"));
	}
	else
	{
		this->SetText(CSTR("HQMP3_64"));
	}
#else
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		this->SetText(CSTR("HQMP3HQ"));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		this->SetText(CSTR("HQMP3UQ"));
	}
	else
	{
		this->SetText(CSTR("HQMP3"));
	}
#endif
	this->playlist = 0;

	NN<UI::GUIMenu> mnu;
	NN<UI::GUIMenu> mnu2;
	NN<UI::GUIMenu> mnu3;
	NEW_CLASSNN(this->mnu, UI::GUIMainMenu());
	mnu = this->mnu->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("&Open..."), MNU_FILE_OPEN, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_O);
	mnu->AddItem(CSTR("Open C&apture Device"), MNU_FILE_CAPTURE_DEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Create Playlist"), MNU_FILE_PLAYLIST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("Set Monitor Color"), MNU_FILE_MON_COLOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Set Audio Device"), MNU_FILE_AUDIO_DEV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu2 = mnu->AddSubMenu(CSTR("HTTP Control (Port 8080)"));
	mnu2->AddItem(CSTR("Enable"), MNU_FILE_HTTP_ENABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Disable"), MNU_FILE_HTTP_DISABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Show &Info"), MNU_FILE_INFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Close"), MNU_FILE_CLOSE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_L);//VK_F4);

	mnu = this->mnu->AddSubMenu(CSTR("&Playback"));
	mnu->AddItem(CSTR("&Start"), MNU_PB_START, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_SPACE);
	mnu->AddItem(CSTR("S&top"), MNU_PB_STOP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_S);
	mnu->AddItem(CSTR("&Pause"), MNU_PB_PAUSE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_Q);
	mnu->AddItem(CSTR("&Forward 10 Seconds"), MNU_PB_FWD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_OEM_PERIOD);
	mnu->AddItem(CSTR("&Backward 10 Seconds"), MNU_PB_BWD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_OEM_COMMA);
	mnu->AddItem(CSTR("&Forward 1 Minute"), MNU_PB_FWD2, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_OEM_PERIOD);
	mnu->AddItem(CSTR("&Backward 1 Minute"), MNU_PB_BWD2, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_OEM_COMMA);
//	mnu->AddItem(CSTR("Store Curr Time"), MNU_PB_STORE_TIME, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_Q);
//	mnu->AddItem(CSTR("Resume Stored Time"), MNU_PB_RESUME_TIME, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_W);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("&Previous Chapter"), MNU_PB_CHAP_PREV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_LEFT);
	mnu->AddItem(CSTR("&Next Chapter"), MNU_PB_CHAP_NEXT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_RIGHT);
	mnu->AddItem(CSTR("A/V Offset Dec"), MNU_PB_AVOFST_DEC, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_OEM_4); //'['
	mnu->AddItem(CSTR("A/V Offset Inc"), MNU_PB_AVOFST_INC, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_OEM_6); //']'
	this->mnuChapters = mnu->AddSubMenu(CSTR("&Chapters"));

	mnu = this->mnu->AddSubMenu(CSTR("&Video"));
	mnu->AddItem(CSTR("&Original Size"), MNU_VIDEO_ORISIZE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_1);
	mnu->AddItem(CSTR("Switch &Fullscreen"), MNU_VIDEO_FULLSCN, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_ENTER);
	mnu->AddSeperator();
	mnu2 = mnu->AddSubMenu(CSTR("Preset"));
	mnu2->AddItem(CSTR("BT.709"), MNU_VIDEO_PRESET_BT709, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("BT.2020"), MNU_VIDEO_PRESET_BT2020, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("BT.2100(HDR10)"), MNU_VIDEO_PRESET_BT2100, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("Transfer Func"));
	mnu2->AddItem(CSTR("sRGB"), MNU_VIDEO_TRANT_sRGB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("BT.709"), MNU_VIDEO_TRANT_BT709, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Constant Gamma"), MNU_VIDEO_TRANT_GAMMA, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SMPTE 240M"), MNU_VIDEO_TRANT_SMPTE240, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Linear RGB"), MNU_VIDEO_TRANT_LINEAR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("BT.1361"), MNU_VIDEO_TRANT_BT1361, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("BT.2100(HDR10)"), MNU_VIDEO_TRANT_BT2100, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Log100"), MNU_VIDEO_TRANT_LOG100, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("LogSqrt10"), MNU_VIDEO_TRANT_LOGSQRT10, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("GoPro Protune"), MNU_VIDEO_TRANT_PROTUNE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("NTSC"), MNU_VIDEO_TRANT_NTSC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("HLG"), MNU_VIDEO_TRANT_HLG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Sony S-Log"), MNU_VIDEO_TRANT_SLOG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Sony S-Log1"), MNU_VIDEO_TRANT_SLOG1, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Sony S-Log2"), MNU_VIDEO_TRANT_SLOG2, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Sony S-Log3"), MNU_VIDEO_TRANT_SLOG3, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Panasonic V-Log"), MNU_VIDEO_TRANT_VLOG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Nikon N-Log"), MNU_VIDEO_TRANT_NLOG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("YUV Type"));
	mnu3 = mnu2->AddSubMenu(CSTR("TV Range (16-235)"));
	mnu3->AddItem(CSTR("BT.601"), MNU_VIDEO_YUVT_BT601, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("BT.709"), MNU_VIDEO_YUVT_BT709, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("FCC"), MNU_VIDEO_YUVT_FCC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("BT.470BG"), MNU_VIDEO_YUVT_BT470BG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("SMPTE170M"), MNU_VIDEO_YUVT_SMPTE170M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("SMPTE240M"), MNU_VIDEO_YUVT_SMPTE240M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("BT.2020"), MNU_VIDEO_YUVT_BT2020, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu(CSTR("PC Range (0-255)"));
	mnu3->AddItem(CSTR("BT.601"), MNU_VIDEO_YUVT_F_BT601, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("BT.709"), MNU_VIDEO_YUVT_F_BT709, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("FCC"), MNU_VIDEO_YUVT_F_FCC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("BT.470BG"), MNU_VIDEO_YUVT_F_BT470BG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("SMPTE170M"), MNU_VIDEO_YUVT_F_SMPTE170M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("SMPTE240M"), MNU_VIDEO_YUVT_F_SMPTE240M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("BT.2020"), MNU_VIDEO_YUVT_F_BT2020, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
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
	mnu2->AddItem(CSTR("GoPro Protune"), MNU_VIDEO_PRIMARIES_GOPRO_PROTUNE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
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
	mnu2 = mnu->AddSubMenu(CSTR("Rotate"));
	mnu2->AddItem(CSTR("No Rotate"), MNU_VIDEO_ROTATE_NONE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("CW 90"), MNU_VIDEO_ROTATE_CW90, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("CW 180"), MNU_VIDEO_ROTATE_CW180, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("CW 270"), MNU_VIDEO_ROTATE_CW270, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("H-Flip"), MNU_VIDEO_ROTATE_HFLIP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("H-Flip CW 90"), MNU_VIDEO_ROTATE_HFLIP_CW90, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("H-Flip CW 180"), MNU_VIDEO_ROTATE_HFLIP_CW180, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("H-Flip CW 270"), MNU_VIDEO_ROTATE_HFLIP_CW270, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu->AddSeperator();
//	mnu->AddItem(CSTR("&Crop Detect"), MNU_VIDEO_CROP, 0, UI::GUIControl::GK_R);
	mnu2 = mnu->AddSubMenu(CSTR("&Deinterlace"));
	mnu2->AddItem(CSTR("&From Video"), MNU_VIDEO_DEINT_AUTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Force &Progressive"), MNU_VIDEO_DEINT_PROG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Force Interlaced (&TFF)"), MNU_VIDEO_DEINT_TFF, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Force Interlaced (&BFF)"), MNU_VIDEO_DEINT_BFF, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("30P Mode"), MNU_VIDEO_DEINT_30P, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
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
	mnu2->AddItem(CSTR("B/W"), MNU_VIDEO_FILTER_BW, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu(CSTR("Background"));
	mnu3->AddItem(CSTR("Enable"), MNU_VIDEO_FILTER_BG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Absolute"), MNU_VIDEO_FILTER_BG_ABS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Create"), MNU_VIDEO_FILTER_BG_CREATE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu(CSTR("Ignore Frame Time"));
	mnu3->AddItem(CSTR("Enable"), MNU_VIDEO_FTIME_ENABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Disable"), MNU_VIDEO_FTIME_DISABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("Take Snapshot"), MNU_VIDEO_SNAPSHOT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnu->AddSubMenu(CSTR("&Audio"));

	this->SetMenu(this->mnu);

	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		NEW_CLASSNN(this->vbox, UI::GUIVideoBoxDD(ui, *this, this->colorSess, 6, 2));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		NEW_CLASSNN(this->vbox, UI::GUIVideoBoxDD(ui, *this, this->colorSess, 6, 2));
	}
	else
	{
		NEW_CLASSNN(this->vbox, UI::GUIVideoBoxDDLQ(ui, *this, this->colorSess, 6, 2));
	}

	this->vbox->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vbox->SetUserFSMode(VFSMODE?(UI::GUIDDrawControl::SM_VFS):(UI::GUIDDrawControl::SM_FS));
	this->vbox->HandleMouseActon(OnMouseAction, this);
	this->HandleDropFiles(OnFileDrop, this);
	this->uOfst = 0;
	this->vOfst = 0;

	NEW_CLASSNN(this->bgFilter, Media::ImgFilter::BGImgFilter());
	this->vbox->AddImgFilter(this->bgFilter);
	NEW_CLASSNN(this->bwFilter, Media::ImgFilter::BWImgFilter(false));
	this->vbox->AddImgFilter(this->bwFilter);

	NN<Media::MediaPlayer> player;
	NEW_CLASSNN(player, Media::MediaPlayer(this->vbox, this->core->GetAudioDevice()));
	this->SetPlayer(player);
	player->SetEndHandler(OnVideoEnd, this);
	this->CloseFile();

	this->dbgFrm = 0;
	this->AddTimer(30, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHQMPForm::~AVIRHQMPForm()
{
	NN<UI::GUIForm> dbgFrm;
	this->listener.Delete();
	if (this->dbgFrm.SetTo(dbgFrm))
		dbgFrm->Close();
	this->ClearChildren();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRHQMPForm::EventMenuClicked(UInt16 cmdId)
{
	NN<Media::MediaPlayer> player;
	NN<Media::Playlist> playlist;
	NN<Media::PBControl> currPBC;
	UOSInt i;
	if (cmdId >= MNU_PB_CHAPTERS)
	{
		i = (UOSInt)(cmdId - MNU_PB_CHAPTERS);
		if (this->currChapInfo.NotNull() && this->player.SetTo(player))
		{
			player->GotoChapter(i);
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
				NN<Text::String> fname = dlg.GetFileName();
				UOSInt i = fname->IndexOf(':');
				if (i == 1 || i == INVALID_INDEX)
				{
					this->OpenFile(fname->ToCString(), dlg.GetParserType());
				}
				else
				{
					NN<IO::ParsedObject> pobj;
					if (!Net::URL::OpenObject(fname->ToCString(), CSTR_NULL, this->core->GetTCPClientFactory(), this->ssl, 30000, this->core->GetLog()).SetTo(pobj))
					{
						this->ui->ShowMsgOK(CSTR("Error in loading file"), CSTR("HQMP"), this);
					}
					else
					{
						if (pobj->GetParserType() == IO::ParserType::MediaFile)
						{
							this->OpenVideo(NN<Media::MediaFile>::ConvertFrom(pobj));
						}
						else
						{
							pobj.Delete();
						}
					}
				}
			}
		}
		break;
	case MNU_FILE_CAPTURE_DEVICE:
		{
			SSWR::AVIRead::AVIRCaptureDevForm dlg(0, this->ui, this->core);
			NN<Media::VideoCapturer> capture;
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK && dlg.capture.SetTo(capture))
			{
				UTF8Char sbuff[256];
				UnsafeArray<UTF8Char> sptr;
				NN<Media::MediaFile> mf;
				sbuff[0] = 0;
				sptr = capture->GetSourceName(sbuff).Or(sbuff);
				NEW_CLASSNN(mf, Media::MediaFile(CSTRP(sbuff, sptr)));
				mf->AddSource(capture, 0);
				this->OpenVideo(mf);
			}
		}
		break;
	case MNU_FILE_PLAYLIST:
		if (this->currPBC.SetTo(currPBC))
		{
			SSWR::AVIRead::AVIRHQMPPlaylistForm dlg(0, this->ui, this->core, this->playlist);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				currPBC->StopPlayback();
				this->playlist.Delete();
				this->playlist = playlist = dlg.GetPlaylist();
				playlist->SetPlayer(this->player);
				this->currPBC = playlist;
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
		if (this->player.SetTo(player))
		{
			SSWR::AVIRead::AVIRSetAudioForm dlg(0, this->ui, this->core);
			dlg.ShowDialog(this);
			player->SwitchAudio(0);
		}
		break;
	case MNU_FILE_INFO:
		if (this->dbgFrm.IsNull())
		{
			NN<UI::GUIForm> frm;
			NEW_CLASSNN(frm, UI::GUIForm(0, 320, 444, ui));
			this->dbgFrm = frm;
			this->txtDebug = ui->NewTextBox(frm, CSTR(""), true);
			this->txtDebug->SetReadOnly(true);
			this->txtDebug->SetDockType(UI::GUIControl::DOCK_FILL);
			frm->SetFont(0, 0, 8.25, false);
			frm->SetText(CSTR("Info"));
			frm->Show();
			frm->HandleFormClosed(OnDebugClosed, this);
		}
		break;
	case MNU_FILE_CLOSE:
		this->CloseFile();
		break;
	case MNU_FILE_HTTP_ENABLE:
		if (this->listener.IsNull())
		{
			NN<Media::MediaPlayerWebInterface> hdlr;
			NN<Net::WebServer::WebListener> listener;
			NEW_CLASSNN(hdlr, Media::MediaPlayerWebInterface(*this, true));
			NEW_CLASSNN(listener, Net::WebServer::WebListener(this->core->GetTCPClientFactory(), 0, hdlr, 8080, 10, 1, 2, CSTR("HQMP/1.0"), false, Net::WebServer::KeepAlive::Default, true));
			if (listener->IsError())
			{
				listener.Delete();
				this->ui->ShowMsgOK(CSTR("Error in listening to the port 8080"), CSTR("HQMP Error"), this);
			}
			else
			{
				this->listener = listener;
			}
		}
		break;
	case MNU_FILE_HTTP_DISABLE:
		this->listener.Delete();
		break;
	case MNU_PB_START:
		this->PBStart();
		break;
	case MNU_PB_STOP:
		this->PBStop();
		break;
	case MNU_PB_PAUSE:
		this->PBPause();
		break;
	case MNU_PB_FWD:
		this->PBJumpOfst(10000);
		break;
	case MNU_PB_BWD:
		this->PBJumpOfst(-10000);
		break;
	case MNU_PB_FWD2:
		this->PBJumpOfst(60000);
		break;
	case MNU_PB_BWD2:
		this->PBJumpOfst(-60000);
		break;
	case MNU_PB_CHAP_PREV:
		this->PBPrevChapter();
		break;
	case MNU_PB_CHAP_NEXT:
		this->PBNextChapter();
		break;
	case MNU_PB_AVOFST_DEC:
		this->PBDecAVOfst();
		break;
	case MNU_PB_AVOFST_INC:
		this->PBIncAVOfst();
		break;
	case MNU_VIDEO_ORISIZE:
		if (this->player.SetTo(player))
		{
			Math::Size2D<UOSInt> vSize;
			IO::DebugWriter debug;
			Text::StringBuilderUTF8 sb;
			sb.AppendTSNoZone(Data::Timestamp::UtcNow());
			sb.AppendC(UTF8STRC(" a"));
			debug.WriteLine(sb.ToCString());
			if (this->vbox->IsFullScreen())
			{
				this->vbox->SwitchFullScreen(false, false);
			}
			sb.ClearStr();
			sb.AppendTSNoZone(Data::Timestamp::UtcNow());
			sb.AppendC(UTF8STRC(" b"));
			debug.WriteLine(sb.ToCString());
			if (player->GetVideoSize(vSize.x, vSize.y))
			{
				Math::Size2D<UOSInt> sz1;
				Math::Size2D<UOSInt> sz2;

				sb.ClearStr();
				sb.AppendTSNoZone(Data::Timestamp::UtcNow());
				sb.AppendC(UTF8STRC(" c"));
				debug.WriteLine(sb.ToCString());
				sz1 = this->vbox->GetSizeP();
				sb.ClearStr();
				sb.AppendTSNoZone(Data::Timestamp::UtcNow());
				sb.AppendC(UTF8STRC(" d"));
				debug.WriteLine(sb.ToCString());
				sz2 = this->GetSizeP();

				sb.ClearStr();
				sb.AppendTSNoZone(Data::Timestamp::UtcNow());
				sb.AppendC(UTF8STRC(" e"));
				debug.WriteLine(sb.ToCString());
				this->SetFormState(UI::GUIForm::FS_NORMAL);
				if (sz1 == vSize)
				{
					sb.ClearStr();
					sb.AppendTSNoZone(Data::Timestamp::UtcNow());
					sb.AppendC(UTF8STRC(" f"));
					debug.WriteLine(sb.ToCString());
					this->vbox->OnSizeChanged(false);
				}
				else
				{
					sb.ClearStr();
					sb.AppendTSNoZone(Data::Timestamp::UtcNow());
					sb.AppendC(UTF8STRC(" g"));
					debug.WriteLine(sb.ToCString());
					this->SetSizeP(sz2 - sz1 + vSize);
				}
				sb.ClearStr();
				sb.AppendTSNoZone(Data::Timestamp::UtcNow());
				sb.AppendC(UTF8STRC(" h"));
				debug.WriteLine(sb.ToCString());
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
	case MNU_VIDEO_PRESET_BT709:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_BT709);
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_SRGB);
		this->vbox->SetSrcYUVType(Media::ColorProfile::YUVT_BT709);
		break;
	case MNU_VIDEO_PRESET_BT2020:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_BT709);
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_BT2020);
		this->vbox->SetSrcYUVType(Media::ColorProfile::YUVT_BT2020);
		break;
	case MNU_VIDEO_PRESET_BT2100:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_BT2100);
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_BT2020);
		this->vbox->SetSrcYUVType(Media::ColorProfile::YUVT_BT2020);
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
	case MNU_VIDEO_TRANT_BT2100:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_BT2100);
		break;
	case MNU_VIDEO_TRANT_LOG100:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_LOG100);
		break;
	case MNU_VIDEO_TRANT_LOGSQRT10:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_LOGSQRT10);
		break;
	case MNU_VIDEO_TRANT_PROTUNE:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_PROTUNE);
		break;
	case MNU_VIDEO_TRANT_NTSC:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_NTSC);
		break;
	case MNU_VIDEO_TRANT_HLG:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_HLG);
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
	case MNU_VIDEO_TRANT_NLOG:
		this->vbox->SetSrcRGBType(Media::CS::TRANT_NLOG);
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
	case MNU_VIDEO_PRIMARIES_GOPRO_PROTUNE:
		this->vbox->SetSrcPrimaries(Media::ColorProfile::CT_GOPRO_PROTUNE);
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
	case MNU_VIDEO_YUVT_BT2020:
		this->vbox->SetSrcYUVType(Media::ColorProfile::YUVT_BT2020);
		break;
	case MNU_VIDEO_YUVT_F_BT601:
		this->vbox->SetSrcYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_BT601));
		break;
	case MNU_VIDEO_YUVT_F_BT709:
		this->vbox->SetSrcYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_BT709));
		break;
	case MNU_VIDEO_YUVT_F_FCC:
		this->vbox->SetSrcYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_FCC));
		break;
	case MNU_VIDEO_YUVT_F_BT470BG:
		this->vbox->SetSrcYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_BT470BG));
		break;
	case MNU_VIDEO_YUVT_F_SMPTE170M:
		this->vbox->SetSrcYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_SMPTE170M));
		break;
	case MNU_VIDEO_YUVT_F_SMPTE240M:
		this->vbox->SetSrcYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_SMPTE240M));
		break;
	case MNU_VIDEO_YUVT_F_BT2020:
		this->vbox->SetSrcYUVType((Media::ColorProfile::YUVType)(Media::ColorProfile::YUVT_FLAG_YUV_0_255 | Media::ColorProfile::YUVT_BT2020));
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
	case MNU_VIDEO_FILTER_BW:
		this->bwFilter->SetEnabled(!this->bwFilter->IsEnabled());
		break;
	case MNU_VIDEO_FILTER_BG:
		this->bgFilter->SetEnabled(!this->bgFilter->IsEnabled());
		break;
	case MNU_VIDEO_FILTER_BG_ABS:
		this->bgFilter->SetAbsolute(!this->bgFilter->IsAbsolute());
		break;
	case MNU_VIDEO_FILTER_BG_CREATE:
		this->bgFilter->ToCreateBGImg();
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
	case MNU_VIDEO_FTIME_ENABLE:
		this->vbox->SetIgnoreFrameTime(true);
		break;
	case MNU_VIDEO_FTIME_DISABLE:
		this->vbox->SetIgnoreFrameTime(false);
		break;
	case MNU_VIDEO_SNAPSHOT:
		this->vbox->Snapshot();
		break;
	case MNU_VIDEO_ROTATE_NONE:
		this->vbox->SetRotateType(Media::RotateType::None);
		break;
	case MNU_VIDEO_ROTATE_CW90:
		this->vbox->SetRotateType(Media::RotateType::CW_90);
		break;
	case MNU_VIDEO_ROTATE_CW180:
		this->vbox->SetRotateType(Media::RotateType::CW_180);
		break;
	case MNU_VIDEO_ROTATE_CW270:
		this->vbox->SetRotateType(Media::RotateType::CW_270);
		break;
	case MNU_VIDEO_ROTATE_HFLIP:
		this->vbox->SetRotateType(Media::RotateType::HFLIP);
		break;
	case MNU_VIDEO_ROTATE_HFLIP_CW90:
		this->vbox->SetRotateType(Media::RotateType::HFLIP_CW_90);
		break;
	case MNU_VIDEO_ROTATE_HFLIP_CW180:
		this->vbox->SetRotateType(Media::RotateType::HFLIP_CW_180);
		break;
	case MNU_VIDEO_ROTATE_HFLIP_CW270:
		this->vbox->SetRotateType(Media::RotateType::HFLIP_CW_270);
		break;
	}
}
	
void SSWR::AVIRead::AVIRHQMPForm::OnMonitorChanged()
{
	Optional<MonitorHandle> hMon = this->GetHMonitor();
	this->colorSess->ChangeMonitor(hMon);
	this->vbox->ChangeMonitor(hMon);
}

void SSWR::AVIRead::AVIRHQMPForm::DestroyObject()
{
	this->CloseFile();
}
