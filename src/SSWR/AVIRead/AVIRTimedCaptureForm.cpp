#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "SSWR/AVIRead/AVIRTimedCaptureForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRTimedCaptureForm::OnDevChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimedCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimedCaptureForm>();
	me->currCapture.Delete();
	me->ReleaseFormats();

	Media::VideoCaptureMgr::DeviceInfo *dev = (Media::VideoCaptureMgr::DeviceInfo*)me->lbDevice->GetSelectedItem().p;
	if (dev)
	{
		me->currCapture = me->captureMgr->CreateDevice(dev->devType, dev->devId);
	}
	else
	{
		me->currCapture = 0;
	}
	me->cboFormat->ClearItems();
	NN<Media::VideoCapturer> currCapture;
	if (me->currCapture.SetTo(currCapture))
	{
		Data::ArrayListUInt32 supportedCS;
		Text::StringBuilderUTF8 devInfo;
		Text::StringBuilderUTF8 sb;
		Media::VideoCapturer::VideoFormat fmts[80];
		NN<CaptureFormat> cfmt;
		UOSInt bestSize = 0;
		UInt32 bestFmt = 0;
		UOSInt bestBPP = 0;
		UOSInt bestIndex = 0;
		UOSInt currSize;
		UOSInt fmtCnt;
		UOSInt i;
		Media::CS::CSConverter::GetSupportedCS(supportedCS);

		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		devInfo.AppendC(UTF8STRC("Name: "));
		sptr = currCapture->GetSourceName(sbuff).Or(sbuff);
		devInfo.AppendP(sbuff, sptr);
		devInfo.AppendC(UTF8STRC("\r\n"));
		devInfo.AppendC(UTF8STRC("Supported Formats:\r\n"));
		fmtCnt = currCapture->GetSupportedFormats(fmts, 80);
		i = 0;
		while (i < fmtCnt)
		{
			cfmt = MemAllocNN(CaptureFormat);
			cfmt->size = fmts[i].info.dispSize;
			cfmt->fourcc = fmts[i].info.fourcc;
			cfmt->bpp = fmts[i].info.storeBPP;
			cfmt->pf = fmts[i].info.pf;
			cfmt->frameRateNumer = fmts[i].frameRateNorm;
			cfmt->frameRateDenom = fmts[i].frameRateDenorm;
			currSize = cfmt->size.CalcArea();
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
			sb.AppendUOSInt(cfmt->size.x);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendUOSInt(cfmt->size.y);
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
			me->currFormats.Add(cfmt);

			devInfo.AppendUOSInt(cfmt->size.x);
			devInfo.AppendC(UTF8STRC(" x "));
			devInfo.AppendUOSInt(cfmt->size.y);
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
			devInfo.AppendDouble(cfmt->frameRateNumer / (Double)cfmt->frameRateDenom);
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

void __stdcall SSWR::AVIRead::AVIRTimedCaptureForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimedCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimedCaptureForm>();
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
		NN<CaptureFormat> cfmt;
		NN<Media::VideoCapturer> currCapture;
		if (me->cboFormat->GetSelectedItem().GetOpt<CaptureFormat>().SetTo(cfmt) && me->currCapture.SetTo(currCapture))
		{
			Text::StringBuilderUTF8 sb;
			me->txtInterval->GetText(sb);
			if (!sb.ToUInt32(me->interval))
			{
				return;
			}
			sb.ClearStr();
			me->txtJPGQuality->GetText(sb);
			if (!sb.ToInt32(me->jpgQuality))
			{
				return;
			}
			if (me->jpgQuality < 0 || me->jpgQuality > 100)
			{
				return;
			}
			currCapture->Init(OnVideoFrame, OnVideoChange, me);
			currCapture->SetPreferFrameType(Media::FT_NON_INTERLACE);
			currCapture->SetPreferSize(cfmt->size, cfmt->fourcc, cfmt->bpp, cfmt->frameRateNumer, cfmt->frameRateDenom);

			sb.ClearStr();
			me->txtFileName->GetText(sb);
			NN<Media::TimedImageList> timedImageList;
			NEW_CLASSNN(timedImageList, Media::TimedImageList(sb.ToCString()));
			if (timedImageList->IsError())
			{
				timedImageList.Delete();
				return;
			}
			me->timedImageList = timedImageList;
			
			UInt32 norm;
			UInt32 denorm;
			UOSInt frameSize;
			currCapture->GetVideoInfo(me->videoInfo, norm, denorm, frameSize);
			me->lastSaveTime = (UInt32)-(Int32)me->interval;
			me->frameCnt = 0;
			me->saveCnt = 0;
			Media::ColorProfile dProfile(Media::ColorProfile::CPT_SRGB);
			me->csConv = Media::CS::CSConverter::NewConverter(cfmt->fourcc, cfmt->bpp, me->videoInfo.pf, me->videoInfo.color, 0, 32, Media::PF_B8G8R8A8, dProfile, Media::ColorProfile::YUVT_UNKNOWN, 0);
			if (me->csConv.IsNull() == 0)
			{
				timedImageList.Delete();
				me->timedImageList = 0;
				return;
			}
			if (currCapture->Start())
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
				me->csConv.Delete();
				timedImageList.Delete();
				me->timedImageList = 0;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRTimedCaptureForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimedCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimedCaptureForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUInt32(sbuff, me->frameCnt);
	me->txtFrameCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, me->saveCnt);
	me->txtSaveCnt->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRTimedCaptureForm::OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	NN<SSWR::AVIRead::AVIRTimedCaptureForm> me = userData.GetNN<SSWR::AVIRead::AVIRTimedCaptureForm>();
	me->frameCnt++;
	NN<Media::CS::CSConverter> csConv;
	NN<Media::TimedImageList> timedImageList;
	if (me->lastSaveTime + me->interval <= frameTime && me->csConv.SetTo(csConv) && me->timedImageList.SetTo(timedImageList))
	{
		NN<Media::ImageList> imgList;
		NN<Media::StaticImage> simg;
		Data::DateTime dt;
		Optional<IO::FileExporter::ParamData> param;
		UnsafeArray<UInt8> imgBuff;
		UOSInt imgSize;
		Media::ColorProfile sRGB(Media::ColorProfile::CPT_SRGB);
		dt.SetCurrTimeUTC();

		NEW_CLASSNN(simg, Media::StaticImage(me->videoInfo.dispSize, 0, 32, Media::PF_B8G8R8A8, me->videoInfo.dispSize.x * me->videoInfo.dispSize.y << 2, sRGB, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
		csConv->ConvertV2(imgData, simg->data, me->videoInfo.dispSize.x, me->videoInfo.dispSize.y, me->videoInfo.storeSize.x, me->videoInfo.storeSize.y, (OSInt)simg->GetDataBpl(), frameType, ycOfst);
		NEW_CLASSNN(imgList, Media::ImageList(CSTR("Temp")));
		imgList->AddImage(simg, 0);
		param = me->exporter->CreateParam(imgList);
		me->exporter->SetParamInt32(param, 0, me->jpgQuality);
		{
			IO::MemoryStream mstm;
			me->exporter->ExportFile(mstm, CSTR("Temp"), imgList, param);
			imgBuff = mstm.GetBuff(imgSize);
			timedImageList->AddImage(dt.ToTicks(), imgBuff, imgSize, Media::TimedImageList::IF_JPG);
		}
		me->exporter->DeleteParam(param);
		imgList.Delete();
		me->saveCnt++;
		me->lastSaveTime = frameTime;
	}
}

void __stdcall SSWR::AVIRead::AVIRTimedCaptureForm::OnVideoChange(Media::VideoSource::FrameChange frChg, AnyType userData)
{
//	NN<SSWR::AVIRead::AVIRTimedCaptureForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimedCaptureForm>();
}

void SSWR::AVIRead::AVIRTimedCaptureForm::StopCapture()
{
	if (this->isStarted)
	{
		NN<Media::VideoCapturer> currCapture;
		if (this->currCapture.SetTo(currCapture))
			currCapture->Stop();
		this->csConv.Delete();
		this->timedImageList.Delete();
		this->isStarted = false;
	}
}

void SSWR::AVIRead::AVIRTimedCaptureForm::ReleaseFormats()
{
	this->currFormats.MemFreeAll();
}

SSWR::AVIRead::AVIRTimedCaptureForm::AVIRTimedCaptureForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 652, 480, ui)
{
	this->SetText(CSTR("Timed Capture"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->csConv = 0;
	this->timedImageList = 0;
	this->isStarted = false;
	this->frameCnt = 0;
	this->saveCnt = 0;
	NEW_CLASSNN(this->exporter, Exporter::GUIJPGExporter());

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lbDevice = ui->NewListBox(*this, false);
	this->lbDevice->SetRect(0, 8, 256, 88, false);
	this->lbDevice->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevice->HandleSelectionChange(OnDevChg, this);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->lblFormat = ui->NewLabel(this->tpControl, CSTR("Capture Format"));
	this->lblFormat->SetRect(4, 4, 100, 23, false);
	this->cboFormat = ui->NewComboBox(this->tpControl, false);
	this->cboFormat->SetRect(104, 4, 264, 23, false);
	this->lblInterval = ui->NewLabel(this->tpControl, CSTR("Capture Interval(ms)"));
	this->lblInterval->SetRect(4, 28, 100, 23, false);
	this->txtInterval = ui->NewTextBox(this->tpControl, CSTR("60000"));
	this->txtInterval->SetRect(104, 28, 100, 23, false);
	this->lblJPGQuality = ui->NewLabel(this->tpControl, CSTR("JPG Quality(%)"));
	this->lblJPGQuality->SetRect(4, 52, 100, 23, false);
	this->txtJPGQuality = ui->NewTextBox(this->tpControl, CSTR("95"));
	this->txtJPGQuality->SetRect(104, 52, 100, 23, false);
	this->lblFileName = ui->NewLabel(this->tpControl, CSTR("File Name"));
	this->lblFileName->SetRect(4, 76, 100, 23, false);
	this->txtFileName = ui->NewTextBox(this->tpControl, CSTR("TimedCapture.til"));
	this->txtFileName->SetRect(104, 76, 260, 23, false);
	this->btnStart = ui->NewButton(this->tpControl, CSTR("Start"));
	this->btnStart->SetRect(104, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->txtDeviceInfo = ui->NewTextBox(this->tpControl, CSTR(""), true);
	this->txtDeviceInfo->SetReadOnly(true);
	this->txtDeviceInfo->SetRect(4, 144, 384, 168, false);
	this->txtDeviceInfo->SetDockType(UI::GUIControl::DOCK_BOTTOM);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->lblFrameCnt = ui->NewLabel(this->tpStatus, CSTR("Frame Count"));
	this->lblFrameCnt->SetRect(4, 4, 100, 23, false);
	this->txtFrameCnt = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtFrameCnt->SetReadOnly(true);
	this->txtFrameCnt->SetRect(104, 4, 100, 23, false);
	this->lblSaveCnt = ui->NewLabel(this->tpStatus, CSTR("Save Count"));
	this->lblSaveCnt->SetRect(4, 28, 100, 23, false);
	this->txtSaveCnt = ui->NewTextBox(this->tpStatus, CSTR(""));
	this->txtSaveCnt->SetReadOnly(true);
	this->txtSaveCnt->SetRect(104, 28, 100, 23, false);

	NEW_CLASSNN(this->captureMgr, Media::VideoCaptureMgr());
	this->currCapture = 0;
	this->captureMgr->GetDeviceList(this->devInfoList);
	UOSInt cnt = this->devInfoList.GetCount();
	if (cnt == 0)
	{
		return;
	}
	NN<Media::VideoCaptureMgr::DeviceInfo> dev;
	UOSInt i;
	i = 0;
	while (i < cnt)
	{
		dev = this->devInfoList.GetItemNoCheck(i);
		this->lbDevice->AddItem({dev->devName, Text::StrCharCnt(dev->devName)}, dev);
		i++;
	}

	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRTimedCaptureForm::~AVIRTimedCaptureForm()
{
	this->StopCapture();
	this->currCapture.Delete();
	this->ReleaseFormats();
	this->captureMgr->FreeDeviceList(this->devInfoList);
	this->exporter.Delete();
	this->captureMgr.Delete();
}

void SSWR::AVIRead::AVIRTimedCaptureForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
