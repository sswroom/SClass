#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "SSWR/AVIRead/AVIRTimedCaptureForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRTimedCaptureForm::OnDevChg(void *userObj)
{
	SSWR::AVIRead::AVIRTimedCaptureForm *me = (SSWR::AVIRead::AVIRTimedCaptureForm*)userObj;
	if (me->currCapture)
	{
		DEL_CLASS(me->currCapture);
	}
	me->ReleaseFormats();

	Media::VideoCaptureMgr::DeviceInfo *dev = (Media::VideoCaptureMgr::DeviceInfo*)me->lbDevice->GetSelectedItem();
	if (dev)
	{
		me->currCapture = me->captureMgr->CreateDevice(dev->devType, dev->devId);
	}
	else
	{
		me->currCapture = 0;
	}
	me->cboFormat->ClearItems();
	if (me->currCapture)
	{
		Data::ArrayListUInt32 supportedCS;
		Text::StringBuilderUTF8 devInfo;
		Text::StringBuilderUTF8 sb;
		Media::IVideoCapture::VideoFormat fmts[80];
		CaptureFormat *cfmt;
		UOSInt bestSize = 0;
		UInt32 bestFmt = 0;
		UOSInt bestBPP = 0;
		UOSInt bestIndex = 0;
		UOSInt currSize;
		UOSInt fmtCnt;
		UOSInt i;
		Media::CS::CSConverter::GetSupportedCS(&supportedCS);

		UTF8Char u8buff[128];
		UTF8Char *sptr;
		devInfo.AppendC(UTF8STRC("Name: "));
		sptr = me->currCapture->GetSourceName(u8buff);
		devInfo.AppendP(u8buff, sptr);
		devInfo.AppendC(UTF8STRC("\r\n"));
		devInfo.AppendC(UTF8STRC("Supported Formats:\r\n"));
		fmtCnt = me->currCapture->GetSupportedFormats(fmts, 80);
		i = 0;
		while (i < fmtCnt)
		{
			cfmt = MemAlloc(CaptureFormat, 1);
			cfmt->width = fmts[i].info.dispWidth;
			cfmt->height = fmts[i].info.dispHeight;
			cfmt->fourcc = fmts[i].info.fourcc;
			cfmt->bpp = fmts[i].info.storeBPP;
			cfmt->pf = fmts[i].info.pf;
			cfmt->frameRateNumer = fmts[i].frameRateNorm;
			cfmt->frameRateDenom = fmts[i].frameRateDenorm;
			currSize = cfmt->width * cfmt->height;
			if (currSize > bestSize)
			{
				bestSize = currSize;
				bestFmt = cfmt->fourcc;
				bestBPP = cfmt->bpp;
				bestIndex = i;
			}
			else if (currSize == bestSize)
			{
				if (cfmt->bpp > bestBPP)
				{
					if (supportedCS.SortedIndexOf(cfmt->fourcc) >= 0 || supportedCS.SortedIndexOf(bestFmt) < 0)
					{
						bestSize = currSize;
						bestFmt = cfmt->fourcc;
						bestBPP = cfmt->bpp;
						bestIndex = i;
					}
				}
			}
			
			sb.ClearStr();
			sb.AppendUOSInt(cfmt->width);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(cfmt->height);
			sb.AppendC(UTF8STRC(" ("));
			if (cfmt->fourcc)
			{
				UInt8 fcc[4];
				*(UInt32*)&fcc = cfmt->fourcc;
				sb.AppendC((const UTF8Char*)fcc, 4);
			}
			else
			{
				sb.AppendU32(cfmt->bpp);
				sb.AppendC(UTF8STRC(" bits RGB"));
			}
			sb.AppendC(UTF8STRC(")"));

			me->cboFormat->AddItem(sb.ToCString(), cfmt);
			me->currFormats->Add(cfmt);

			devInfo.AppendUOSInt(cfmt->width);
			devInfo.AppendC(UTF8STRC(" x "));
			devInfo.AppendUOSInt(cfmt->height);
			devInfo.AppendC(UTF8STRC(" ("));
			if (cfmt->fourcc)
			{
				UInt8 fcc[4];
				*(UInt32*)&fcc = cfmt->fourcc;
				devInfo.AppendC((const UTF8Char*)fcc, 4);
			}
			else
			{
				devInfo.AppendU32(cfmt->bpp);
				devInfo.AppendC(UTF8STRC(" bits RGB"));
			}
			devInfo.AppendC(UTF8STRC(")"));
			devInfo.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&devInfo, cfmt->frameRateNumer / (Double)cfmt->frameRateDenom);
			devInfo.AppendC(UTF8STRC(" fps\r\n"));

			i++;
		}
		if (fmtCnt > 0)
		{
			me->cboFormat->SetSelectedIndex(bestIndex);
		}
		me->txtDeviceInfo->SetText(devInfo.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRTimedCaptureForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimedCaptureForm *me = (SSWR::AVIRead::AVIRTimedCaptureForm*)userObj;
	if (me->isStarted)
	{
		me->StopCapture();
		me->lbDevice->SetEnabled(true);
		me->cboFormat->SetEnabled(true);
		me->txtInterval->SetReadOnly(false);
		me->txtJPGQuality->SetReadOnly(false);
		me->txtFileName->SetReadOnly(false);
	}
	else
	{
		CaptureFormat *cfmt = (CaptureFormat*)me->cboFormat->GetSelectedItem();
		if (cfmt && me->currCapture)
		{
			Text::StringBuilderUTF8 sb;
			me->txtInterval->GetText(&sb);
			if (!sb.ToUInt32(&me->interval))
			{
				return;
			}
			sb.ClearStr();
			me->txtJPGQuality->GetText(&sb);
			if (!sb.ToInt32(&me->jpgQuality))
			{
				return;
			}
			if (me->jpgQuality < 0 || me->jpgQuality > 100)
			{
				return;
			}
			me->currCapture->Init(OnVideoFrame, OnVideoChange, me);
			me->currCapture->SetPreferFrameType(Media::FT_NON_INTERLACE);
			me->currCapture->SetPreferSize(cfmt->width, cfmt->height, cfmt->fourcc, cfmt->bpp, cfmt->frameRateNumer, cfmt->frameRateDenom);

			sb.ClearStr();
			me->txtFileName->GetText(&sb);
			NEW_CLASS(me->timedImageList, Media::TimedImageList(sb.ToCString()));
			if (me->timedImageList->IsError())
			{
				DEL_CLASS(me->timedImageList);
				me->timedImageList = 0;
				return;
			}
			
			UInt32 norm;
			UInt32 denorm;
			UOSInt frameSize;
			me->currCapture->GetVideoInfo(&me->videoInfo, &norm, &denorm, &frameSize);
			me->lastSaveTime = (UInt32)-(Int32)me->interval;
			me->frameCnt = 0;
			me->saveCnt = 0;
			Media::ColorProfile dProfile(Media::ColorProfile::CPT_SRGB);
			me->csConv = Media::CS::CSConverter::NewConverter(cfmt->fourcc, cfmt->bpp, me->videoInfo.pf, me->videoInfo.color, 0, 32, Media::PF_B8G8R8A8, &dProfile, Media::ColorProfile::YUVT_UNKNOWN, 0);
			if (me->csConv == 0)
			{
				DEL_CLASS(me->timedImageList);
				me->timedImageList = 0;
				return;
			}
			if (me->currCapture->Start())
			{
				me->isStarted = true;
				me->lbDevice->SetEnabled(false);
				me->cboFormat->SetEnabled(false);
				me->txtInterval->SetReadOnly(true);
				me->txtJPGQuality->SetReadOnly(true);
				me->txtFileName->SetReadOnly(true);
				me->tcMain->SetSelectedIndex(1);
			}
			else
			{
				DEL_CLASS(me->csConv);
				me->csConv = 0;
				DEL_CLASS(me->timedImageList);
				me->timedImageList = 0;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTimedCaptureForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRTimedCaptureForm *me = (SSWR::AVIRead::AVIRTimedCaptureForm *)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrUInt32(sbuff, me->frameCnt);
	me->txtFrameCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, me->saveCnt);
	me->txtSaveCnt->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRTimedCaptureForm::OnVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	SSWR::AVIRead::AVIRTimedCaptureForm *me = (SSWR::AVIRead::AVIRTimedCaptureForm *)userData;
	me->frameCnt++;
	if (me->lastSaveTime + me->interval <= frameTime)
	{
		Media::ImageList *imgList;
		Media::StaticImage *simg;
		IO::MemoryStream *mstm;
		Data::DateTime dt;
		void *param;
		UInt8 *imgBuff;
		UOSInt imgSize;
		Media::ColorProfile sRGB(Media::ColorProfile::CPT_SRGB);
		dt.SetCurrTimeUTC();

		NEW_CLASS(simg, Media::StaticImage(me->videoInfo.dispWidth, me->videoInfo.dispHeight, 0, 32, Media::PF_B8G8R8A8, me->videoInfo.dispWidth * me->videoInfo.dispHeight << 2, &sRGB, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
		me->csConv->ConvertV2(imgData, simg->data, me->videoInfo.dispWidth, me->videoInfo.dispHeight, me->videoInfo.storeWidth, me->videoInfo.storeHeight, (OSInt)simg->GetDataBpl(), frameType, ycOfst);
		NEW_CLASS(imgList, Media::ImageList(CSTR("Temp")));
		imgList->AddImage(simg, 0);
		param = me->exporter->CreateParam(imgList);
		me->exporter->SetParamInt32(param, 0, me->jpgQuality);
		NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("SSWR.AVIRead.AVIRTimedCaptureForm.OnVidoeFrame.mstm")));
		me->exporter->ExportFile(mstm, CSTR("Temp"), imgList, param);
		imgBuff = mstm->GetBuff(&imgSize);
		me->timedImageList->AddImage(dt.ToTicks(), imgBuff, imgSize, Media::TimedImageList::IF_JPG);
		DEL_CLASS(mstm);
		me->exporter->DeleteParam(param);
		DEL_CLASS(imgList);
		me->saveCnt++;
		me->lastSaveTime = frameTime;
	}
}

void __stdcall SSWR::AVIRead::AVIRTimedCaptureForm::OnVideoChange(Media::IVideoSource::FrameChange frChg, void *userData)
{
//	SSWR::AVIRead::AVIRTimedCaptureForm *me = (SSWR::AVIRead::AVIRTimedCaptureForm *)userData;
}

void SSWR::AVIRead::AVIRTimedCaptureForm::StopCapture()
{
	if (this->isStarted)
	{
		this->currCapture->Stop();
		SDEL_CLASS(this->csConv);
		SDEL_CLASS(this->timedImageList);
		this->isStarted = false;
	}
}

void SSWR::AVIRead::AVIRTimedCaptureForm::ReleaseFormats()
{
	UOSInt i = this->currFormats->GetCount();
	while (i-- > 0)
	{
		MemFree(this->currFormats->RemoveAt(i));
	}
}

SSWR::AVIRead::AVIRTimedCaptureForm::AVIRTimedCaptureForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 652, 480, ui)
{
	this->SetText(CSTR("Timed Capture"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->csConv = 0;
	this->timedImageList = 0;
	this->isStarted = false;
	this->frameCnt = 0;
	this->saveCnt = 0;
	NEW_CLASS(this->exporter, Exporter::GUIJPGExporter());

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->lbDevice, UI::GUIListBox(ui, this, false));
	this->lbDevice->SetRect(0, 8, 256, 88, false);
	this->lbDevice->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevice->HandleSelectionChange(OnDevChg, this);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	NEW_CLASS(this->lblFormat, UI::GUILabel(ui, this->tpControl, CSTR("Capture Format")));
	this->lblFormat->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboFormat, UI::GUIComboBox(ui, this->tpControl, false));
	this->cboFormat->SetRect(104, 4, 264, 23, false);
	NEW_CLASS(this->lblInterval, UI::GUILabel(ui, this->tpControl, CSTR("Capture Interval(ms)")));
	this->lblInterval->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtInterval, UI::GUITextBox(ui, this->tpControl, CSTR("60000")));
	this->txtInterval->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblJPGQuality, UI::GUILabel(ui, this->tpControl, CSTR("JPG Quality(%)")));
	this->lblJPGQuality->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtJPGQuality, UI::GUITextBox(ui, this->tpControl, CSTR("95")));
	this->txtJPGQuality->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->lblFileName, UI::GUILabel(ui, this->tpControl, CSTR("File Name")));
	this->lblFileName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtFileName, UI::GUITextBox(ui, this->tpControl, CSTR("TimedCapture.til")));
	this->txtFileName->SetRect(104, 76, 260, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpControl, CSTR("Start")));
	this->btnStart->SetRect(104, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->txtDeviceInfo, UI::GUITextBox(ui, this->tpControl, CSTR(""), true));
	this->txtDeviceInfo->SetReadOnly(true);
	this->txtDeviceInfo->SetRect(4, 144, 384, 168, false);
	this->txtDeviceInfo->SetDockType(UI::GUIControl::DOCK_BOTTOM);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	NEW_CLASS(this->lblFrameCnt, UI::GUILabel(ui, this->tpStatus, CSTR("Frame Count")));
	this->lblFrameCnt->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFrameCnt, UI::GUITextBox(ui, this->tpStatus, CSTR("")));
	this->txtFrameCnt->SetReadOnly(true);
	this->txtFrameCnt->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblSaveCnt, UI::GUILabel(ui, this->tpStatus, CSTR("Save Count")));
	this->lblSaveCnt->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtSaveCnt, UI::GUITextBox(ui, this->tpStatus, CSTR("")));
	this->txtSaveCnt->SetReadOnly(true);
	this->txtSaveCnt->SetRect(104, 28, 100, 23, false);

	NEW_CLASS(this->captureMgr, Media::VideoCaptureMgr());
	NEW_CLASS(this->devInfoList, Data::ArrayList<Media::VideoCaptureMgr::DeviceInfo*>());
	NEW_CLASS(this->currFormats, Data::ArrayList<CaptureFormat*>());
	this->currCapture = 0;
	this->captureMgr->GetDeviceList(this->devInfoList);
	UOSInt cnt = this->devInfoList->GetCount();
	if (cnt == 0)
	{
		return;
	}
	Media::VideoCaptureMgr::DeviceInfo *dev;
	UOSInt i;
	i = 0;
	while (i < cnt)
	{
		dev = this->devInfoList->GetItem(i);
		this->lbDevice->AddItem({dev->devName, Text::StrCharCnt(dev->devName)}, dev);
		i++;
	}

	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRTimedCaptureForm::~AVIRTimedCaptureForm()
{
	this->StopCapture();
	if (this->currCapture)
	{
		DEL_CLASS(this->currCapture);
		this->currCapture = 0;
	}
	this->ReleaseFormats();
	DEL_CLASS(this->currFormats);
	this->captureMgr->FreeDeviceList(this->devInfoList);
	DEL_CLASS(this->devInfoList);
	DEL_CLASS(this->exporter);
	DEL_CLASS(this->captureMgr);
}

void SSWR::AVIRead::AVIRTimedCaptureForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
