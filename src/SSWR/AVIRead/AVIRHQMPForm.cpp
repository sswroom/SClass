#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/PowerInfo.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/CS/TransferFunc.h"
#include "Net/MIME.h"
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
#include "Text/TextEnc/URIEncoding.h"
#include "UI/GUIVideoBoxDDLQ.h"
#include "UI/MessageDialog.h"

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

	MNU_PB_CHAPTERS = 1000
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRHQMPForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRHQMPForm *me = (SSWR::AVIRead::AVIRHQMPForm*)userObj;
	UOSInt i;

	me->player->StopPlayback();
	i = 0;
	while (i < nFiles)
	{
		if (me->OpenFile(files[i]))
			return;
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPForm *me = (SSWR::AVIRead::AVIRHQMPForm*)userObj;
	if (me->dbgFrm)
	{
		Text::StringBuilderUTF8 sb;
		UI::GUIVideoBoxDD::DebugValue dbg;
		Int32 currTime;
		Int32 v;
		NEW_CLASS(dbg.color, Media::ColorProfile());
		me->vbox->GetDebugValues(&dbg);
		sb.Append((const UTF8Char*)"Curr Time: ");
		sb.AppendI32(dbg.currTime);
		currTime = dbg.currTime;
		v = currTime / 3600000;
		sb.Append((const UTF8Char*)" (");
		sb.AppendI32(v);
		sb.Append((const UTF8Char*)":");
		currTime -= v * 3600000;
		v = currTime / 60000;
		if (v < 10)
		{
			sb.Append((const UTF8Char*)"0");
		}
		sb.AppendI32(v);
		sb.Append((const UTF8Char*)":");
		currTime -= v * 60000;
		v = currTime / 1000;
		if (v < 10)
		{
			sb.Append((const UTF8Char*)"0");
		}
		sb.AppendI32(v);
		sb.Append((const UTF8Char*)".");
		currTime -= v * 1000;
		if (currTime < 10)
		{
			sb.Append((const UTF8Char*)"00");
			sb.AppendI32(currTime);
		}
		else if (currTime < 100)
		{
			sb.Append((const UTF8Char*)"0");
			sb.AppendI32(currTime);
		}
		else
		{
			sb.AppendI32(currTime);
		}
		sb.Append((const UTF8Char*)")\r\n");
		sb.Append((const UTF8Char*)"Disp Frame Time: ");
		sb.AppendI32(dbg.dispFrameTime);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Disp Frame Num: ");
		sb.AppendI32(dbg.dispFrameNum);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Proc Delay: ");
		sb.AppendI32(dbg.procDelay);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Disp Delay: ");
		sb.AppendI32(dbg.dispDelay);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Disp Jitter: ");
		sb.AppendI32(dbg.dispJitter);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Video Delay: ");
		sb.AppendI32(dbg.videoDelay);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Source Delay: ");
		sb.AppendI32(dbg.srcDelay);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"A/V Offset: ");
		sb.AppendI32(dbg.avOfst);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Seek Count: ");
		sb.AppendOSInt(dbg.seekCnt);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Frame Displayed: ");
		sb.AppendI32(dbg.frameDispCnt);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Frame Skip before process: ");
		sb.AppendI32(dbg.frameSkip1);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Frame Skip after process: ");
		sb.AppendI32(dbg.frameSkip2);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"ProcTimes H: ");
		Text::SBAppendF64(&sb, dbg.hTime);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"ProcTimes V: ");
		Text::SBAppendF64(&sb, dbg.vTime);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"ProcTimes C: ");
		Text::SBAppendF64(&sb, dbg.csTime);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Buff: ");
		sb.AppendI32(dbg.buffProc);
		sb.Append((const UTF8Char*)",");
		sb.AppendI32(dbg.buffReady);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Src Size: ");
		sb.AppendOSInt(dbg.srcWidth);
		sb.Append((const UTF8Char*)" x ");
		sb.AppendOSInt(dbg.srcHeight);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Disp Size: ");
		sb.AppendOSInt(dbg.dispWidth);
		sb.Append((const UTF8Char*)" x ");
		sb.AppendOSInt(dbg.dispHeight);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"PAR: ");
		Text::SBAppendF64(&sb, dbg.par);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Decoder: ");
		if (dbg.decoderName)
		{
			sb.Append(dbg.decoderName);
		}
		else
		{
			sb.Append((const UTF8Char*)"-");
		}
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Format: ");
		sb.Append(Media::CS::CSConverter::GetFormatName(dbg.format));
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Output Bitdepth: ");
		sb.AppendI32(dbg.dispBitDepth);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Src YUV Type: ");
		sb.Append(Media::ColorProfile::GetNameYUVType(dbg.srcYUVType));
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Src R Transfer: ");
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(dbg.color->GetRTranParam()->GetTranType()));
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Src G Transfer: ");
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(dbg.color->GetGTranParam()->GetTranType()));
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Src B Transfer: ");
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(dbg.color->GetBTranParam()->GetTranType()));
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"Src Gamma: ");
		Text::SBAppendF64(&sb, dbg.color->GetRTranParam()->GetGamma());
		sb.Append((const UTF8Char*)"\r\n");
		Media::ColorProfile::ColorPrimaries *primaries = dbg.color->GetPrimaries(); 
		sb.Append((const UTF8Char*)"Src RGB Primary: ");
		sb.Append(Media::ColorProfile::GetNameColorType(primaries->colorType));
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"-Red:   ");
		Text::SBAppendF64(&sb, primaries->rx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->ry);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"-Green: ");
		Text::SBAppendF64(&sb, primaries->gx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->gy);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"-Blue:  ");
		Text::SBAppendF64(&sb, primaries->bx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->by);
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append((const UTF8Char*)"-White: ");
		Text::SBAppendF64(&sb, primaries->wx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->wy);
		sb.Append((const UTF8Char*)"\r\n");
		me->txtDebug->SetText(sb.ToString());
		DEL_CLASS(dbg.color);
	}
	if (me->player->IsPlaying())
	{
		me->ui->UseDevice(true, true);
	}
	if (me->pbEnd)
	{
		me->pbEnd = false;
		UTF8Char u8buff[512];
		OSInt i;
		OSInt j;
		UTF8Char *u8ptr;
		Int32 partNum;
		me->currFile->GetSourceName(u8buff);
		i = Text::StrLastIndexOf(u8buff, '.');
		j = Text::StrIndexOfICase(u8buff, (const UTF8Char*)"part");
		if (i > j && j >= 0)
		{
			u8buff[i] = 0;
			partNum = Text::StrToInt32(&u8buff[j + 4]);
			if (partNum > 0)
			{
				partNum++;
				if (partNum == 10)
				{
					Text::StrConcat(&u8buff[i + 2], &u8buff[i + 1]);
					u8ptr = Text::StrInt32(&u8buff[j + 4], partNum);
					*u8ptr++ = '.';
				}
				else
				{
					u8ptr = Text::StrInt32(&u8buff[j + 4], partNum);
					*u8ptr++ = '.';
				}

				if (me->OpenFile(u8buff))
				{
					if (!me->player->IsPlaying())
					{
						me->currPBC->StartPlayback();
					}
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPForm::OnDebugClosed(void *userObj, UI::GUIForm *frm)
{
	SSWR::AVIRead::AVIRHQMPForm *me = (SSWR::AVIRead::AVIRHQMPForm*)userObj;
	me->dbgFrm = 0;
}

void __stdcall SSWR::AVIRead::AVIRHQMPForm::OnVideoEnd(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPForm *me = (SSWR::AVIRead::AVIRHQMPForm*)userObj;
	me->pbEnd = true;
}

void __stdcall SSWR::AVIRead::AVIRHQMPForm::OnMouseAction(void *userObj, UI::GUIVideoBoxDD::MouseAction ma, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRHQMPForm *me = (SSWR::AVIRead::AVIRHQMPForm*)userObj;
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

Bool SSWR::AVIRead::AVIRHQMPForm::OpenFile(const UTF8Char *fileName)
{
	Parser::ParserList *parsers = this->core->GetParserList();
	IO::ParsedObject *pobj;
	IO::StmData::FileData *fd;

	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	pobj = parsers->ParseFileType(fd, IO::ParsedObject::PT_VIDEO_PARSER);
	if (pobj)
	{
		DEL_CLASS(fd);
		return OpenVideo((Media::MediaFile*)pobj);
	}
	else
	{
		DEL_CLASS(fd);
		return false;
	}
}

Bool SSWR::AVIRead::AVIRHQMPForm::OpenVideo(Media::MediaFile *mf)
{
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	OSInt i;
	OSInt j;

	this->player->LoadMedia(0);
	SDEL_CLASS(this->currFile);
	SDEL_CLASS(this->playlist);

	Bool hasAudio = false;
	Bool hasVideo = false;
	Media::IMediaSource *msrc;
	Media::MediaType mt;
	IO::Path::PathType pt;
	UInt64 fileSize;
	IO::Path::FindFileSession *sess;
	i = 0;
	while ((msrc = mf->GetStream(i++, 0)) != 0)
	{
		mt = msrc->GetMediaType();
		if (mt == Media::MEDIA_TYPE_VIDEO)
		{
			hasVideo = true;
		}
		else if (mt == Media::MEDIA_TYPE_AUDIO)
		{
			hasAudio = true;
		}
	}
	if (hasVideo && !hasAudio)
	{
		Text::StrConcat(sbuff, mf->GetSourceNameObj());
		i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
		if (i >= 0)
		{
			j = Text::StrLastIndexOf(&sbuff[i + 1], '.');
			if (j >= 0)
			{
				Text::StrConcat(&sbuff[i + j + 1], IO::Path::ALL_FILES);
				sess = IO::Path::FindFile(sbuff);
				if (sess)
				{
					Parser::ParserList *parsers = this->core->GetParserList();

					while (IO::Path::FindNextFile(&sbuff[i + 1], sess, 0, &pt, &fileSize))
					{
						j = Text::StrLastIndexOf(&sbuff[i + 1], '.');
						if (j >= 0)
						{
							Bool audFile = false;
							if (Text::StrEqualsICase(&sbuff[i + j + 2], (const UTF8Char*)"m4a"))
							{
								audFile = true;
							}
							else if (Text::StrEqualsICase(&sbuff[i + j + 2], (const UTF8Char*)"aac"))
							{
								audFile = true;
							}
							else if (Text::StrEqualsICase(&sbuff[i + j + 2], (const UTF8Char*)"ac3"))
							{
								audFile = true;
							}
							else if (Text::StrEqualsICase(&sbuff[i + j + 2], (const UTF8Char*)"wav"))
							{
								audFile = true;
							}

							if (audFile)
							{
								IO::StmData::FileData *fd;
								Media::MediaFile *audFile;
								NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
								audFile = (Media::MediaFile*)parsers->ParseFileType(fd, IO::ParsedObject::PT_VIDEO_PARSER);
								DEL_CLASS(fd);
								if (audFile)
								{
									Int32 syncTime;
									j = 0;
									while ((msrc = audFile->GetStream(j, &syncTime)) != 0)
									{
										audFile->KeepStream(j, true);
										mf->AddSource(msrc, syncTime);
										j++;
									}
									DEL_CLASS(audFile);
								}
							}
						}
					}
					IO::Path::FindFileClose(sess);
				}
			}
		}
	}
	
	this->currFile = mf;
#if defined(_WIN64)
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"HQMP3HQ64 - "), this->currFile->GetSourceNameObj());
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"HQMP3UQ64 - "), this->currFile->GetSourceNameObj());
	}
	else
	{
		Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"HQMP3_64 - "), this->currFile->GetSourceNameObj());
	}
#else
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"HQMP3HQ - "), this->currFile->GetSourceNameObj());
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"HQMP3UQ - "), this->currFile->GetSourceNameObj());
	}
	else
	{
		Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"HQMP3 - "), this->currFile->GetSourceNameObj());
	}
#endif
	this->SetText(sbuff);
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
					sptr = Text::StrConcat(sptr, (const UTF8Char*)"&");
				}
				sptr = Text::StrInt32(sptr, (Int32)j + 1);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)" ");
				sptr = Text::StrConcat(sptr, this->currChapInfo->GetChapterName(j));
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
	this->currFile->GetSourceName(sbuff);
	if (Text::StrIndexOfICase(sbuff, (const UTF8Char*)"sRGB") >= 0)
	{
		this->vbox->SetSrcRGBType(Media::CS::TRANT_sRGB);
	}
	else if (Text::StrIndexOfICase(sbuff, (const UTF8Char*)"BT709") >= 0)
	{
		this->vbox->SetSrcRGBType(Media::CS::TRANT_BT709);
	}
	return true;
}

void SSWR::AVIRead::AVIRHQMPForm::CloseFile()
{
	this->player->StopPlayback();
	this->player->LoadMedia(0);
	this->storeTime = -1;
	SDEL_CLASS(this->currFile);
	SDEL_CLASS(this->playlist);
	this->currPBC = this->player;
#if defined(_WIN64)
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		this->SetText((const UTF8Char*)"HQMP3HQ64");
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		this->SetText((const UTF8Char*)"HQMP3UQ64");
	}
	else
	{
		this->SetText((const UTF8Char*)"HQMP3_64");
	}
#else
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		this->SetText((const UTF8Char*)"HQMP3HQ");
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		this->SetText((const UTF8Char*)"HQMP3UQ");
	}
	else
	{
		this->SetText((const UTF8Char*)"HQMP3");
	}
#endif
	this->mnuChapters->ClearItems();
	this->mnuChapters->AddItem((const UTF8Char*)"No Chapters", MNU_PB_CHAPTERS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuChapters->SetItemEnabled(MNU_PB_CHAPTERS, false);
	this->UpdateMenu();
}

OSInt __stdcall SSWR::AVIRead::AVIRHQMPForm::VideoFileCompare(void *file1, void *file2)
{
	VideoFileInfo *vfile1 = (VideoFileInfo*)file1;
	VideoFileInfo *vfile2 = (VideoFileInfo*)file2;
	return Text::StrCompare(vfile1->fileName, vfile2->fileName);
}

SSWR::AVIRead::AVIRHQMPForm::AVIRHQMPForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, QualityMode qMode) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->qMode = qMode;
	this->pbEnd = false;
	this->listener = 0;
#if defined(_WIN64)
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		this->SetText((const UTF8Char*)"HQMP3HQ64");
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		this->SetText((const UTF8Char*)"HQMP3UQ64");
	}
	else
	{
		this->SetText((const UTF8Char*)"HQMP3_64");
	}
#else
	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		this->SetText((const UTF8Char*)"HQMP3HQ");
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		this->SetText((const UTF8Char*)"HQMP3UQ");
	}
	else
	{
		this->SetText((const UTF8Char*)"HQMP3");
	}
#endif
	this->playlist = 0;
	this->storeTime = -1;

	UI::GUIMenu *mnu;
	UI::GUIMenu *mnu2;
	UI::GUIMenu *mnu3;
	NEW_CLASS(this->mnu, UI::GUIMainMenu());
	mnu = this->mnu->AddSubMenu((const UTF8Char*)"&File");
	mnu->AddItem((const UTF8Char*)"&Open...", MNU_FILE_OPEN, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_O);
	mnu->AddItem((const UTF8Char*)"Open C&apture Device", MNU_FILE_CAPTURE_DEVICE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Create Playlist", MNU_FILE_PLAYLIST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"Set Monitor Color", MNU_FILE_MON_COLOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Set Audio Device", MNU_FILE_AUDIO_DEV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"HTTP Control (Port 8080)");
	mnu2->AddItem((const UTF8Char*)"Enable", MNU_FILE_HTTP_ENABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Disable", MNU_FILE_HTTP_DISABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Show &Info", MNU_FILE_INFO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"&Close", MNU_FILE_CLOSE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_L);//VK_F4);

	mnu = this->mnu->AddSubMenu((const UTF8Char*)"&Playback");
	mnu->AddItem((const UTF8Char*)"&Start", MNU_PB_START, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_SPACE);
	mnu->AddItem((const UTF8Char*)"S&top", MNU_PB_STOP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_S);
	mnu->AddItem((const UTF8Char*)"&Pause", MNU_PB_PAUSE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_Q);
	mnu->AddItem((const UTF8Char*)"&Forward 10 Seconds", MNU_PB_FWD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_ADD);
	mnu->AddItem((const UTF8Char*)"&Backward 10 Seconds", MNU_PB_BWD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_SUBTRACT);
	mnu->AddItem((const UTF8Char*)"&Forward 1 Minute", MNU_PB_FWD2, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_ADD);
	mnu->AddItem((const UTF8Char*)"&Backward 1 Minute", MNU_PB_BWD2, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_SUBTRACT);
//	mnu->AddItem((const UTF8Char*)"Store Curr Time", MNU_PB_STORE_TIME, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_Q);
//	mnu->AddItem((const UTF8Char*)"Resume Stored Time", MNU_PB_RESUME_TIME, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_W);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"&Previous Chapter", MNU_PB_CHAP_PREV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_LEFT);
	mnu->AddItem((const UTF8Char*)"&Next Chapter", MNU_PB_CHAP_NEXT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_RIGHT);
	mnu->AddItem((const UTF8Char*)"A/V Offset Dec", MNU_PB_AVOFST_DEC, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_OEM_4); //'['
	mnu->AddItem((const UTF8Char*)"A/V Offset Inc", MNU_PB_AVOFST_INC, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_OEM_6); //']'
	this->mnuChapters = mnu->AddSubMenu((const UTF8Char*)"&Chapters");

	mnu = this->mnu->AddSubMenu((const UTF8Char*)"&Video");
	mnu->AddItem((const UTF8Char*)"&Original Size", MNU_VIDEO_ORISIZE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_1);
	mnu->AddItem((const UTF8Char*)"Switch &Fullscreen", MNU_VIDEO_FULLSCN, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_ENTER);
	mnu->AddSeperator();
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Preset");
	mnu2->AddItem((const UTF8Char*)"BT.709", MNU_VIDEO_PRESET_BT709, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"BT.2020", MNU_VIDEO_PRESET_BT2020, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"BT.2100(HDR10)", MNU_VIDEO_PRESET_BT2100, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Transfer Func");
	mnu2->AddItem((const UTF8Char*)"sRGB", MNU_VIDEO_TRANT_sRGB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"BT.709", MNU_VIDEO_TRANT_BT709, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Constant Gamma", MNU_VIDEO_TRANT_GAMMA, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"SMPTE 240M", MNU_VIDEO_TRANT_SMPTE240, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Linear RGB", MNU_VIDEO_TRANT_LINEAR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"BT.1361", MNU_VIDEO_TRANT_BT1361, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"BT.2100(HDR10)", MNU_VIDEO_TRANT_BT2100, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Log100", MNU_VIDEO_TRANT_LOG100, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"LogSqrt10", MNU_VIDEO_TRANT_LOGSQRT10, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"GoPro Protune", MNU_VIDEO_TRANT_PROTUNE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"NTSC", MNU_VIDEO_TRANT_NTSC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"HLG", MNU_VIDEO_TRANT_HLG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Sony S-Log", MNU_VIDEO_TRANT_SLOG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Sony S-Log1", MNU_VIDEO_TRANT_SLOG1, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Sony S-Log2", MNU_VIDEO_TRANT_SLOG2, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Sony S-Log3", MNU_VIDEO_TRANT_SLOG3, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Panasonic V-Log", MNU_VIDEO_TRANT_VLOG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Nikon N-Log", MNU_VIDEO_TRANT_NLOG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"YUV Type");
	mnu3 = mnu2->AddSubMenu((const UTF8Char*)"TV Range (16-235)");
	mnu3->AddItem((const UTF8Char*)"BT.601", MNU_VIDEO_YUVT_BT601, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"BT.709", MNU_VIDEO_YUVT_BT709, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"FCC", MNU_VIDEO_YUVT_FCC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"BT.470BG", MNU_VIDEO_YUVT_BT470BG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"SMPTE170M", MNU_VIDEO_YUVT_SMPTE170M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"SMPTE240M", MNU_VIDEO_YUVT_SMPTE240M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"BT.2020", MNU_VIDEO_YUVT_BT2020, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu((const UTF8Char*)"PC Range (0-255)");
	mnu3->AddItem((const UTF8Char*)"BT.601", MNU_VIDEO_YUVT_F_BT601, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"BT.709", MNU_VIDEO_YUVT_F_BT709, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"FCC", MNU_VIDEO_YUVT_F_FCC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"BT.470BG", MNU_VIDEO_YUVT_F_BT470BG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"SMPTE170M", MNU_VIDEO_YUVT_F_SMPTE170M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"SMPTE240M", MNU_VIDEO_YUVT_F_SMPTE240M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"BT.2020", MNU_VIDEO_YUVT_F_BT2020, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Color Primaries");
	mnu2->AddItem((const UTF8Char*)"From Video", MNU_VIDEO_PRIMARIES_SOURCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"sRGB", MNU_VIDEO_PRIMARIES_SRGB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Bt.470M", MNU_VIDEO_PRIMARIES_BT470M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Bt.470BG", MNU_VIDEO_PRIMARIES_BT470BG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"SMPTE 170M", MNU_VIDEO_PRIMARIES_SMPTE170M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"SMPTE 240M", MNU_VIDEO_PRIMARIES_SMPTE240M, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Generic Film", MNU_VIDEO_PRIMARIES_GENERIC_FILM, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Bt.2020", MNU_VIDEO_PRIMARIES_BT2020, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Adobe RGB", MNU_VIDEO_PRIMARIES_ADOBE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Apple RGB", MNU_VIDEO_PRIMARIES_APPLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"CIE RGB", MNU_VIDEO_PRIMARIES_CIERGB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"ColorMatch RGB", MNU_VIDEO_PRIMARIES_COLORMATCH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Wide", MNU_VIDEO_PRIMARIES_WIDE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Sony S-Gamut3 (S-Gamut)", MNU_VIDEO_PRIMARIES_SGAMUT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Sony S-Gamut3.Cine", MNU_VIDEO_PRIMARIES_SGAMUTCINE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"DCI-P3", MNU_VIDEO_PRIMARIES_DCI_P3, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"ACES-Gamut", MNU_VIDEO_PRIMARIES_ACESGAMUT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"ALEXA Wide", MNU_VIDEO_PRIMARIES_ALEXAWIDE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Panasonic V-Gamut", MNU_VIDEO_PRIMARIES_VGAMUT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu((const UTF8Char*)"Custom White Point");
	mnu3->AddItem((const UTF8Char*)"D50", MNU_VIDEO_WP_D50, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"D65", MNU_VIDEO_WP_D65, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"2000K", MNU_VIDEO_WP_2000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"2500K", MNU_VIDEO_WP_2500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"3000K", MNU_VIDEO_WP_3000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"3500K", MNU_VIDEO_WP_3500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"4000K", MNU_VIDEO_WP_4000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"4500K", MNU_VIDEO_WP_4500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"5000K", MNU_VIDEO_WP_5000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"5500K", MNU_VIDEO_WP_5500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"6000K", MNU_VIDEO_WP_6000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"6500K", MNU_VIDEO_WP_6500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"7000K", MNU_VIDEO_WP_7000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"7500K", MNU_VIDEO_WP_7500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"8000K", MNU_VIDEO_WP_8000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"8500K", MNU_VIDEO_WP_8500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"9000K", MNU_VIDEO_WP_9000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"9500K", MNU_VIDEO_WP_9500K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"10000K", MNU_VIDEO_WP_10000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"11000K", MNU_VIDEO_WP_11000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"12000K", MNU_VIDEO_WP_12000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"13000K", MNU_VIDEO_WP_13000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"14000K", MNU_VIDEO_WP_14000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"15000K", MNU_VIDEO_WP_15000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"16000K", MNU_VIDEO_WP_16000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"17000K", MNU_VIDEO_WP_17000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"18000K", MNU_VIDEO_WP_18000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"19000K", MNU_VIDEO_WP_19000K, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
//	mnu->AddItem((const UTF8Char*)"&Crop Detect", MNU_VIDEO_CROP, 0, UI::GUIControl::GK_R);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"&Deinterlace");
	mnu2->AddItem((const UTF8Char*)"&From Video", MNU_VIDEO_DEINT_AUTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Force &Progressive", MNU_VIDEO_DEINT_PROG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Force Interlaced (&TFF)", MNU_VIDEO_DEINT_TFF, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Force Interlaced (&BFF)", MNU_VIDEO_DEINT_BFF, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"30P Mode", MNU_VIDEO_DEINT_30P, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Source PAR");
	mnu2->AddItem((const UTF8Char*)"From Video", MNU_VIDEO_SPAR_AUTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Square Pixel", MNU_VIDEO_SPAR_SQR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"1.333", MNU_VIDEO_SPAR_1333, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"0.889 (4:3 on DVD)", MNU_VIDEO_SPAR_DVD4_3, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"1.185 (16:9 on DVD)", MNU_VIDEO_SPAR_DVD16_9, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Monitor PAR");
	mnu2->AddItem((const UTF8Char*)"Square Pixel", MNU_VIDEO_MPAR_SQR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"1:2", MNU_VIDEO_MPAR_1_2, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"Filter");
	mnu3 = mnu2->AddSubMenu((const UTF8Char*)"IVTC");
	mnu3->AddItem((const UTF8Char*)"Enable", MNU_VIDEO_IVTC_ENABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"Disable", MNU_VIDEO_IVTC_DISABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu((const UTF8Char*)"UV Offset");
	mnu3->AddItem((const UTF8Char*)"Move Left", MNU_VIDEO_UVOFST_LEFT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_OEM_4);
	mnu3->AddItem((const UTF8Char*)"Move Right", MNU_VIDEO_UVOFST_RIGHT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_OEM_6);
	mnu3->AddItem((const UTF8Char*)"Reset", MNU_VIDEO_UVOFST_RESET, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu((const UTF8Char*)"Auto Crop");
	mnu3->AddItem((const UTF8Char*)"Enable", MNU_VIDEO_CROP_ENABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_R);
	mnu3->AddItem((const UTF8Char*)"Disable", MNU_VIDEO_CROP_DISABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"B/W", MNU_VIDEO_FILTER_BW, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu((const UTF8Char*)"Background");
	mnu3->AddItem((const UTF8Char*)"Enable", MNU_VIDEO_FILTER_BG, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"Absolute", MNU_VIDEO_FILTER_BG_ABS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"Create", MNU_VIDEO_FILTER_BG_CREATE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu((const UTF8Char*)"Ignore Frame Time");
	mnu3->AddItem((const UTF8Char*)"Enable", MNU_VIDEO_FTIME_ENABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem((const UTF8Char*)"Disable", MNU_VIDEO_FTIME_DISABLE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu->AddItem((const UTF8Char*)"Take Snapshot", MNU_VIDEO_SNAPSHOT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	mnu = this->mnu->AddSubMenu((const UTF8Char*)"&Audio");

	this->SetMenu(this->mnu);

	if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_HQ)
	{
		NEW_CLASS(this->vbox, UI::GUIVideoBoxDD(ui, this, this->colorSess, 6, 2));
	}
	else if (this->qMode == SSWR::AVIRead::AVIRHQMPForm::QM_UQ)
	{
		NEW_CLASS(this->vbox, UI::GUIVideoBoxDD(ui, this, this->colorSess, 6, 2));
	}
	else
	{
		NEW_CLASS(this->vbox, UI::GUIVideoBoxDDLQ(ui, this, this->colorSess, 6, 2));
	}

	this->vbox->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vbox->SetUserFSMode(VFSMODE?(UI::GUIDDrawControl::SM_VFS):(UI::GUIDDrawControl::SM_FS));
	this->vbox->HandleMouseActon(OnMouseAction, this);
	this->HandleDropFiles(OnFileDrop, this);
	this->uOfst = 0;
	this->vOfst = 0;

	NEW_CLASS(this->bgFilter, Media::ImageFilter::BGImgFilter());
	this->vbox->AddImgFilter(this->bgFilter);
	NEW_CLASS(this->bwFilter, Media::ImageFilter::BWImgFilter(false));
	this->vbox->AddImgFilter(this->bwFilter);

	NEW_CLASS(this->player, SSWR::AVIRead::AVIRMediaPlayer(this->vbox, this->core));
	this->player->SetEndHandler(OnVideoEnd, this);
	this->currFile = 0;
	CloseFile();

	this->dbgFrm = 0;
	this->AddTimer(30, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHQMPForm::~AVIRHQMPForm()
{
	SDEL_CLASS(this->listener);
	this->CloseFile();
	DEL_CLASS(this->player);
	if (this->dbgFrm)
		this->dbgFrm->Close();
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRHQMPForm::EventMenuClicked(UInt16 cmdId)
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
			SSWR::AVIRead::AVIROpenFileForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIROpenFileForm(0, this->ui, this->core, IO::ParsedObject::PT_VIDEO_PARSER));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				const UTF8Char *fname = dlg->GetFileName();
				OSInt i = Text::StrIndexOf(fname, ':');
				if (i == 1 || i < 0)
				{
					this->OpenFile(dlg->GetFileName());
				}
				else
				{
					IO::ParsedObject *pobj = Net::URL::OpenObject(fname, 0, this->core->GetSocketFactory());
					if (pobj == 0)
					{
						UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in loading file", (const UTF8Char *)"HQMP", this);
					}
					else
					{
						if (pobj->GetParserType() == IO::ParsedObject::PT_VIDEO_PARSER)
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
			DEL_CLASS(dlg);
		}
		break;
	case MNU_FILE_CAPTURE_DEVICE:
		{
			SSWR::AVIRead::AVIRCaptureDevForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRCaptureDevForm(0, this->ui, this->core));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				UTF8Char sbuff[256];
				Media::MediaFile *mf;
				dlg->capture->GetSourceName(sbuff);
				NEW_CLASS(mf, Media::MediaFile(sbuff));
				mf->AddSource(dlg->capture, 0);
				this->OpenVideo(mf);
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_FILE_PLAYLIST:
		{
			SSWR::AVIRead::AVIRHQMPPlaylistForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRHQMPPlaylistForm(0, this->ui, this->core, this->playlist));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->currPBC->StopPlayback();
				SDEL_CLASS(this->playlist);
				this->playlist = dlg->GetPlaylist();
				this->playlist->SetPlayer(this->player);
				this->currPBC = this->playlist;
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_FILE_MON_COLOR:
		{
			SSWR::AVIRead::AVIRColorSettingForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRColorSettingForm(0, this->ui, this->core, this->GetHMonitor()));
			dlg->ShowDialog(this);
			DEL_CLASS(dlg);
		}
		break;
	case MNU_FILE_AUDIO_DEV:
		{
			SSWR::AVIRead::AVIRSetAudioForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRSetAudioForm(0, this->ui, this->core));
			dlg->ShowDialog(this);
			DEL_CLASS(dlg);
			this->player->SwitchAudio(0);
		}
		break;
	case MNU_FILE_INFO:
		if (this->dbgFrm == 0)
		{
			NEW_CLASS(this->dbgFrm, UI::GUIForm(0, 320, 444, ui));
			NEW_CLASS(this->txtDebug, UI::GUITextBox(ui, this->dbgFrm, (const UTF8Char*)"", true));
			this->txtDebug->SetReadOnly(true);
			this->txtDebug->SetDockType(UI::GUIControl::DOCK_FILL);
			this->dbgFrm->SetFont(0, 8.25, false);
			this->dbgFrm->SetText((const UTF8Char*)"Info");
			this->dbgFrm->Show();
			this->dbgFrm->HandleFormClosed(OnDebugClosed, this);
		}
		break;
	case MNU_FILE_CLOSE:
		this->CloseFile();
		break;
	case MNU_FILE_HTTP_ENABLE:
		if (this->listener == 0)
		{
			NEW_CLASS(this->listener, Net::WebServer::WebListener(this->core->GetSocketFactory(), this, 8080, 10, 2, (const UTF8Char*)"HQMP/1.0", false, true));
			if (this->listener->IsError())
			{
				DEL_CLASS(this->listener);
				this->listener = 0;
				UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in listening to the port 8080", (const UTF8Char *)"HQMP Error", this);
			}
		}
		break;
	case MNU_FILE_HTTP_DISABLE:
		SDEL_CLASS(this->listener);
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
	case MNU_PB_CHAP_PREV:
		this->currPBC->PrevChapter();
		break;
	case MNU_PB_CHAP_NEXT:
		this->currPBC->NextChapter();
		break;
	case MNU_PB_AVOFST_DEC:
		this->vbox->SetAVOfst(this->vbox->GetAVOfst() - 10);
		break;
	case MNU_PB_AVOFST_INC:
		this->vbox->SetAVOfst(this->vbox->GetAVOfst() + 10);
		break;
	case MNU_VIDEO_ORISIZE:
		{
			UOSInt vw;
			UOSInt vh;
			if (this->player->GetVideoSize(&vw, &vh))
			{
				UOSInt w1;
				UOSInt h1;
				UOSInt w2;
				UOSInt h2;

				if (this->vbox->IsFullScreen())
				{
					this->vbox->SwitchFullScreen(false, false);
				}
				this->vbox->GetSizeP(&w1, &h1);
				this->GetSizeP(&w2, &h2);

				this->SetFormState(UI::GUIForm::FS_NORMAL);
				if (w1 == vw && h1 == vh)
				{
					this->vbox->OnSizeChanged(false);
				}
				else
				{
					this->SetSizeP(w2 - w1 + vw, h2 - h1 + vh);
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
	}
}
	
void SSWR::AVIRead::AVIRHQMPForm::OnMonitorChanged()
{
	void *hMon = this->GetHMonitor();
	this->colorSess->ChangeMonitor(hMon);
	this->vbox->ChangeMonitor(hMon);
}

void SSWR::AVIRead::AVIRHQMPForm::BrowseRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	const UTF8Char *fname = req->GetQueryValue((const UTF8Char*)"fname");
	if (this->currFile == 0)
	{
		resp->RedirectURL(req, (const UTF8Char*)"/", 0);
		return;
	}
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[1024];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Text::StrConcat(sbuff, this->currFile->GetSourceNameObj());
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];

	if (fname)
	{
		Text::StrConcat(sptr, fname);
		if (this->OpenFile(sbuff))
		{
			this->EventMenuClicked(MNU_PB_START);
			resp->RedirectURL(req, (const UTF8Char*)"/", 0);
			return;
		}
	}
	IO::MemoryStream *mstm;
	IO::Writer *writer;
	IO::Path::PathType pt;
	IO::Path::FindFileSession *sess;
	UInt8 *buff;
	const UTF8Char *u8ptr;
	UOSInt size;
	UInt64 fileSize;

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SP.GPSWeb.GPSWebHandler.LoginFunc"));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));

	writer->WriteLine((const UTF8Char*)"<html>");
	writer->WriteLine((const UTF8Char*)"<head><title>HQMP Control</title>");
	writer->WriteLine((const UTF8Char*)"</head>");
	writer->WriteLine((const UTF8Char*)"<body>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/\">Back</a><br/><br/>");
	writer->Write((const UTF8Char*)"<b>Current File: </b>");
	u8ptr = Text::XML::ToNewHTMLText(this->currFile->GetSourceNameObj());
	writer->Write(u8ptr);
	Text::XML::FreeNewText(u8ptr);
	writer->WriteLine((const UTF8Char*)"<hr/>");

	writer->WriteLine((const UTF8Char*)"<table border=\"0\">");
	writer->WriteLine((const UTF8Char*)"<tr><td>Name</td><td>Size</td><td>MIME Type</td></tr>");

	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		Data::ArrayList<VideoFileInfo *> fileList;
		VideoFileInfo *vfile;

		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize))
		{
			if (pt == IO::Path::PT_FILE)
			{
				vfile = MemAlloc(VideoFileInfo, 1);
				vfile->fileName = Text::StrCopyNew(sptr);
				vfile->fileSize = fileSize;
				fileList.Add(vfile);
			}
		}
		IO::Path::FindFileClose(sess);

		void **arr = (void**)fileList.GetArray(&j);
		ArtificialQuickSort_SortCmp(arr, VideoFileCompare, 0, j - 1);

		i = 0;
		j = fileList.GetCount();
		while (i < j)
		{
			vfile = fileList.GetItem(i);

			writer->Write((const UTF8Char*)"<tr><td>");
			writer->Write((const UTF8Char*)"<a href=\"/browse?fname=");
			Text::TextEnc::URIEncoding::URIEncode(sbuff2, vfile->fileName);
			u8ptr = Text::XML::ToNewXMLText(sbuff2);
			writer->Write(u8ptr);
			Text::XML::FreeNewText(u8ptr);
			writer->Write((const UTF8Char*)"\">");

			u8ptr = Text::XML::ToNewHTMLText(vfile->fileName);
			writer->Write(u8ptr);
			Text::XML::FreeNewText(u8ptr);
			writer->Write((const UTF8Char*)"</a></td><td>");
			Text::StrInt64(sbuff2, vfile->fileSize);
			writer->Write(sbuff2);
			writer->Write((const UTF8Char*)"</td><td>");

			IO::Path::GetFileExt(sbuff2, vfile->fileName);
			u8ptr = Net::MIME::GetMIMEFromExt(sbuff2);
			writer->Write(u8ptr);
			writer->WriteLine((const UTF8Char*)"</td></tr>");

			Text::StrDelNew(vfile->fileName);
			MemFree(vfile);
			i++;
		}
	}
	writer->WriteLine((const UTF8Char*)"</table>");

	writer->WriteLine((const UTF8Char*)"</body>");
	writer->WriteLine((const UTF8Char*)"</html>");
	DEL_CLASS(writer);

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	buff = mstm->GetBuff(&size);
	resp->AddContentLength(size);
	resp->AddContentType((const UTF8Char*)"text/html;charset=UTF-8");
	resp->Write(buff, size);
	DEL_CLASS(mstm);
}

void SSWR::AVIRead::AVIRHQMPForm::WebRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	const UTF8Char *reqURI = req->GetRequestURI();
	if (Text::StrEquals(reqURI, (const UTF8Char*)"/browse") || Text::StrStartsWith(reqURI, (const UTF8Char*)"/browse?"))
	{
		this->BrowseRequest(req, resp);
		return;
	}
	if (Text::StrEquals(reqURI, (const UTF8Char*)"/start"))
	{
		this->EventMenuClicked(MNU_PB_START);
	}
	else if (Text::StrEquals(reqURI, (const UTF8Char*)"/stop"))
	{
		this->EventMenuClicked(MNU_PB_STOP);
	}
	else if (Text::StrEquals(reqURI, (const UTF8Char*)"/pause"))
	{
		this->EventMenuClicked(MNU_PB_PAUSE);
	}
	else if (Text::StrEquals(reqURI, (const UTF8Char*)"/backward60"))
	{
		this->EventMenuClicked(MNU_PB_BWD2);
	}
	else if (Text::StrEquals(reqURI, (const UTF8Char*)"/backward10"))
	{
		this->EventMenuClicked(MNU_PB_BWD);
	}
	else if (Text::StrEquals(reqURI, (const UTF8Char*)"/forward10"))
	{
		this->EventMenuClicked(MNU_PB_FWD);
	}
	else if (Text::StrEquals(reqURI, (const UTF8Char*)"/forward60"))
	{
		this->EventMenuClicked(MNU_PB_FWD2);
	}
	else if (Text::StrEquals(reqURI, (const UTF8Char*)"/prevchap"))
	{
		this->EventMenuClicked(MNU_PB_CHAP_PREV);
	}
	else if (Text::StrEquals(reqURI, (const UTF8Char*)"/nextchap"))
	{
		this->EventMenuClicked(MNU_PB_CHAP_NEXT);
	}
	else if (Text::StrEquals(reqURI, (const UTF8Char*)"/avofstdec"))
	{
		this->EventMenuClicked(MNU_PB_AVOFST_DEC);
	}
	else if (Text::StrEquals(reqURI, (const UTF8Char*)"/avofstinc"))
	{
		this->EventMenuClicked(MNU_PB_AVOFST_INC);
	}
	IO::MemoryStream *mstm;
	IO::Writer *writer;
	UInt8 *buff;
	const UTF8Char *u8ptr;
	UOSInt size;

	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"SP.GPSWeb.GPSWebHandler.LoginFunc"));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));

	writer->WriteLine((const UTF8Char*)"<html>");
	writer->WriteLine((const UTF8Char*)"<head><title>HQMP Control</title>");
	writer->WriteLine((const UTF8Char*)"</head>");
	writer->WriteLine((const UTF8Char*)"<body>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/\">Refresh</a><br/><br/>");
	writer->Write((const UTF8Char*)"<b>Current File: </b>");
	if (this->currFile)
	{
		u8ptr = Text::XML::ToNewHTMLText(this->currFile->GetSourceNameObj());
		writer->Write(u8ptr);
		Text::XML::FreeNewText(u8ptr);

		writer->Write((const UTF8Char*)" <a href=\"/browse\">Browse</a>");
	}
	else
	{
		writer->Write((const UTF8Char*)"-");
	}
	writer->WriteLine((const UTF8Char*)"<br/>");

	writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Stop\" onclick=\"document.location.replace('/stop')\"/>");
	writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Start\" onclick=\"document.location.replace('/start')\"/>");
	writer->WriteLine((const UTF8Char*)"<input type=\"button\" value=\"Pause\" onclick=\"document.location.replace('/pause')\"/>");
	writer->WriteLine((const UTF8Char*)"<br/>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/backward60\">Backward 1 Minute</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/backward10\">Backward 10 Seconds</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/forward10\">Forward 10 Seconds</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/forward60\">Forward 1 Minute</a>");
	writer->WriteLine((const UTF8Char*)"<br/>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/prevchap\">Previous Chapter</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/nextchap\">Next Chapter</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/avofstdec\">A/V Offset Decrease</a>");
	writer->WriteLine((const UTF8Char*)"<a href=\"/avofstinc\">A/V Offset Increase</a>");
	{
		Text::StringBuilderUTF8 sb;
		UI::GUIVideoBoxDD::DebugValue dbg;
		Int32 currTime;
		Int32 v;

		writer->WriteLine((const UTF8Char*)"<hr/>");
		NEW_CLASS(dbg.color, Media::ColorProfile());
		this->vbox->GetDebugValues(&dbg);
		sb.Append((const UTF8Char*)"Curr Time: ");
		sb.AppendI32(dbg.currTime);
		currTime = dbg.currTime;
		v = currTime / 3600000;
		sb.Append((const UTF8Char*)" (");
		sb.AppendI32(v);
		sb.Append((const UTF8Char*)":");
		currTime -= v * 3600000;
		v = currTime / 60000;
		if (v < 10)
		{
			sb.Append((const UTF8Char*)"0");
		}
		sb.AppendI32(v);
		sb.Append((const UTF8Char*)":");
		currTime -= v * 60000;
		v = currTime / 1000;
		if (v < 10)
		{
			sb.Append((const UTF8Char*)"0");
		}
		sb.AppendI32(v);
		sb.Append((const UTF8Char*)".");
		currTime -= v * 1000;
		if (currTime < 10)
		{
			sb.Append((const UTF8Char*)"00");
			sb.AppendI32(currTime);
		}
		else if (currTime < 100)
		{
			sb.Append((const UTF8Char*)"0");
			sb.AppendI32(currTime);
		}
		else
		{
			sb.AppendI32(currTime);
		}
		sb.Append((const UTF8Char*)")<br/>\r\n");
		sb.Append((const UTF8Char*)"Disp Frame Time: ");
		sb.AppendI32(dbg.dispFrameTime);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Disp Frame Num: ");
		sb.AppendI32(dbg.dispFrameNum);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Proc Delay: ");
		sb.AppendI32(dbg.procDelay);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Disp Delay: ");
		sb.AppendI32(dbg.dispDelay);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Disp Jitter: ");
		sb.AppendI32(dbg.dispJitter);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Video Delay: ");
		sb.AppendI32(dbg.videoDelay);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Source Delay: ");
		sb.AppendI32(dbg.srcDelay);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"A/V Offset: ");
		sb.AppendI32(dbg.avOfst);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Seek Count: ");
		sb.AppendOSInt(dbg.seekCnt);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Frame Displayed: ");
		sb.AppendI32(dbg.frameDispCnt);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Frame Skip before process: ");
		sb.AppendI32(dbg.frameSkip1);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Frame Skip after process: ");
		sb.AppendI32(dbg.frameSkip2);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"ProcTimes H: ");
		Text::SBAppendF64(&sb, dbg.hTime);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"ProcTimes V: ");
		Text::SBAppendF64(&sb, dbg.vTime);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"ProcTimes C: ");
		Text::SBAppendF64(&sb, dbg.csTime);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Buff: ");
		sb.AppendI32(dbg.buffProc);
		sb.Append((const UTF8Char*)",");
		sb.AppendI32(dbg.buffReady);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src Size: ");
		sb.AppendUOSInt(dbg.srcWidth);
		sb.Append((const UTF8Char*)" x ");
		sb.AppendUOSInt(dbg.srcHeight);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Disp Size: ");
		sb.AppendUOSInt(dbg.dispWidth);
		sb.Append((const UTF8Char*)" x ");
		sb.AppendUOSInt(dbg.dispHeight);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"PAR: ");
		Text::SBAppendF64(&sb, dbg.par);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Decoder: ");
		if (dbg.decoderName)
		{
			sb.Append(dbg.decoderName);
		}
		else
		{
			sb.Append((const UTF8Char*)"-");
		}
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Format: ");
		sb.Append(Media::CS::CSConverter::GetFormatName(dbg.format));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Output Bitdepth: ");
		sb.AppendI32(dbg.dispBitDepth);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src YUV Type: ");
		sb.Append(Media::ColorProfile::GetNameYUVType(dbg.srcYUVType));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src R Transfer: ");
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(dbg.color->GetRTranParam()->GetTranType()));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src G Transfer: ");
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(dbg.color->GetGTranParam()->GetTranType()));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src B Transfer: ");
		sb.Append(Media::CS::TransferFunc::GetTransferFuncName(dbg.color->GetBTranParam()->GetTranType()));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Src Gamma: ");
		Text::SBAppendF64(&sb, dbg.color->GetRTranParam()->GetGamma());
		sb.Append((const UTF8Char*)"<br/>\r\n");
		Media::ColorProfile::ColorPrimaries *primaries = dbg.color->GetPrimaries(); 
		sb.Append((const UTF8Char*)"Src RGB Primary: ");
		sb.Append(Media::ColorProfile::GetNameColorType(primaries->colorType));
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"-Red:   ");
		Text::SBAppendF64(&sb, primaries->rx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->ry);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"-Green: ");
		Text::SBAppendF64(&sb, primaries->gx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->gy);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"-Blue:  ");
		Text::SBAppendF64(&sb, primaries->bx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->by);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"-White: ");
		Text::SBAppendF64(&sb, primaries->wx);
		sb.Append((const UTF8Char*)", ");
		Text::SBAppendF64(&sb, primaries->wy);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		DEL_CLASS(dbg.color);
		writer->Write(sb.ToString());
	}

	writer->WriteLine((const UTF8Char*)"</body>");
	writer->WriteLine((const UTF8Char*)"</html>");
	DEL_CLASS(writer);

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	buff = mstm->GetBuff(&size);
	resp->AddContentLength(size);
	resp->AddContentType((const UTF8Char*)"text/html;charset=UTF-8");
	resp->Write(buff, size);
	DEL_CLASS(mstm);
}

void SSWR::AVIRead::AVIRHQMPForm::Release()
{
}

void SSWR::AVIRead::AVIRHQMPForm::PBStart()
{
	if (!this->player->IsPlaying())
	{
		this->currPBC->StartPlayback();
		if (this->storeTime != -1)
		{
			this->player->SeekTo(this->storeTime);
			this->storeTime = -1;
		}
	}
}

void SSWR::AVIRead::AVIRHQMPForm::PBStop()
{
	this->currPBC->StopPlayback();
	this->storeTime = -1;
}

void SSWR::AVIRead::AVIRHQMPForm::PBPause()
{
	if (this->player->IsPlaying())
	{
		this->storeTime = this->player->GetCurrTime();
		this->currPBC->StopPlayback();
	}
	else if (this->storeTime != -1)
	{
		this->currPBC->StartPlayback();
		this->player->SeekTo(this->storeTime);
		this->storeTime = -1;
	}
}
