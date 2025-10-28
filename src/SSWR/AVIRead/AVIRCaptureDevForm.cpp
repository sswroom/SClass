#include "Stdafx.h"
#include "Media/CS/CSConverter.h"
#include "SSWR/AVIRead/AVIRCaptureDevForm.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRCaptureDevForm::OnOKClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCaptureDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCaptureDevForm>();
	NN<Media::VideoCapturer> currCapture;
	if (!me->currCapture.SetTo(currCapture))
	{
		me->ui->ShowMsgOK(CSTR("Please select a device"), CSTR("Select Capture Device"), me);
		return;
	}
	NN<CaptureFormat> fmt;
	if (!me->cboFormat->GetItem((UOSInt)me->cboFormat->GetSelectedIndex()).GetOpt<CaptureFormat>().SetTo(fmt))
	{
		me->ui->ShowMsgOK(CSTR("Please select a format"), CSTR("Select Capture Device"), me);
		return;
	}

	currCapture->SetPreferSize(fmt->size, fmt->fourcc, fmt->bpp, fmt->frameRateNumer, fmt->frameRateDenom);
	me->capture = currCapture;
	me->currCapture = 0;

	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRCaptureDevForm::OnCancelClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCaptureDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCaptureDevForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRCaptureDevForm::OnDevChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCaptureDevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCaptureDevForm>();
	me->currCapture.Delete();
	me->ReleaseFormats();

	Media::VideoCaptureMgr::DeviceInfo *devInfo = (Media::VideoCaptureMgr::DeviceInfo*)me->lbDevice->GetSelectedItem().p;
	if (devInfo)
	{
		me->currCapture = me->captureMgr.CreateDevice(devInfo->devType, devInfo->devId);
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
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		Text::StringBuilderUTF8 devInfo;
		Text::StringBuilderUTF8 sb;
		Media::VideoCapturer::VideoFormat fmts[80];
		NN<SSWR::AVIRead::AVIRCaptureDevForm::CaptureFormat> cfmt;
		UOSInt bestSize = 0;
		UInt32 bestFmt = 0;
		UOSInt bestBPP = 0;
		UOSInt bestIndex = 0;
		UOSInt currSize;
		UOSInt fmtCnt;
		UOSInt i;
		Media::CS::CSConverter::GetSupportedCS(supportedCS);

		devInfo.AppendC(UTF8STRC("Name: "));
		sptr = currCapture->GetSourceName(sbuff).Or(sbuff);
		devInfo.AppendP(sbuff, sptr);
		devInfo.AppendC(UTF8STRC("\r\n"));
		currCapture->GetInfo(devInfo);
		devInfo.AppendC(UTF8STRC("Supported Formats:\r\n"));
		fmtCnt = currCapture->GetSupportedFormats(fmts, 80);
		i = 0;
		while (i < fmtCnt)
		{
			cfmt = MemAllocNN(SSWR::AVIRead::AVIRCaptureDevForm::CaptureFormat);
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
			sb.AppendC(UTF8STRC(" "));
			sb.AppendDouble(cfmt->frameRateNumer / (Double)cfmt->frameRateDenom);
			sb.AppendC(UTF8STRC(" fps"));

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

void SSWR::AVIRead::AVIRCaptureDevForm::ReleaseFormats()
{
	this->currFormats.MemFreeAll();
}

SSWR::AVIRead::AVIRCaptureDevForm::AVIRCaptureDevForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 768, 200, ui)
{
	this->SetText(CSTR("Select Capture Device"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lblDevice = ui->NewLabel(*this, CSTR("Capture Device"));
	this->lblDevice->SetRect(8, 8, 100, 23, false);
	this->lbDevice = ui->NewListBox(*this, false);
	this->lbDevice->SetRect(112, 8, 256, 88, false);
	this->lbDevice->HandleSelectionChange(OnDevChg, this);
	this->lblFormat = ui->NewLabel(*this, CSTR("Capture Format"));
	this->lblFormat->SetRect(8, 96, 100, 23, false);
	this->cboFormat = ui->NewComboBox(*this, false);
	this->cboFormat->SetRect(112, 96, 264, 23, false);
	this->txtDeviceInfo = ui->NewTextBox(*this, CSTR(""), true);
	this->txtDeviceInfo->SetReadOnly(true);
	this->txtDeviceInfo->SetRect(376, 8, 384, 168, false);

	this->btnOK = ui->NewButton(*this, CSTR("&OK"));
	this->btnOK->SetRect(112, 144, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(192, 144, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	this->currCapture = 0;
	this->capture = 0;
	this->captureMgr.GetDeviceList(this->devInfoList);
	UOSInt cnt = this->devInfoList.GetCount();
	if (cnt == 0)
	{
		return;
	}
	UOSInt i;
	i = 0;
	while (i < cnt)
	{
		NN<Media::VideoCaptureMgr::DeviceInfo> dev = this->devInfoList.GetItemNoCheck(i);
		this->lbDevice->AddItem({dev->devName, Text::StrCharCnt(dev->devName)}, dev);
		i++;
	}

}

SSWR::AVIRead::AVIRCaptureDevForm::~AVIRCaptureDevForm()
{
	this->ClearChildren();
	this->captureMgr.FreeDeviceList(this->devInfoList);
	this->currCapture.Delete();
	this->ReleaseFormats();
}

void SSWR::AVIRead::AVIRCaptureDevForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
