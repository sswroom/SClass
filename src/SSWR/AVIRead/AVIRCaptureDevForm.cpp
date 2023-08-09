#include "Stdafx.h"
#include "Media/CS/CSConverter.h"
#include "SSWR/AVIRead/AVIRCaptureDevForm.h"
#include "Text/MyStringFloat.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRCaptureDevForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRCaptureDevForm *me = (SSWR::AVIRead::AVIRCaptureDevForm*)userObj;
	if (me->currCapture == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select a device"), CSTR("Select Capture Device"), me);
		return;
	}
	CaptureFormat *fmt = (CaptureFormat*)me->cboFormat->GetItem((UOSInt)me->cboFormat->GetSelectedIndex());
	if (fmt == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select a format"), CSTR("Select Capture Device"), me);
		return;
	}

	me->currCapture->SetPreferSize(fmt->size, fmt->fourcc, fmt->bpp, fmt->frameRateNumer, fmt->frameRateDenom);
	me->capture = me->currCapture;
	me->currCapture = 0;

	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRCaptureDevForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRCaptureDevForm *me = (SSWR::AVIRead::AVIRCaptureDevForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRCaptureDevForm::OnDevChg(void *userObj)
{
	SSWR::AVIRead::AVIRCaptureDevForm *me = (SSWR::AVIRead::AVIRCaptureDevForm*)userObj;
	if (me->currCapture)
	{
		DEL_CLASS(me->currCapture);
	}
	me->ReleaseFormats();

	Media::VideoCaptureMgr::DeviceInfo *devInfo = (Media::VideoCaptureMgr::DeviceInfo*)me->lbDevice->GetSelectedItem();
	if (devInfo)
	{
		me->currCapture = me->captureMgr.CreateDevice(devInfo->devType, devInfo->devId);
	}
	else
	{
		me->currCapture = 0;
	}
	me->cboFormat->ClearItems();
	if (me->currCapture)
	{
		Data::ArrayListUInt32 supportedCS;
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		Text::StringBuilderUTF8 devInfo;
		Text::StringBuilderUTF8 sb;
		Media::IVideoCapture::VideoFormat fmts[80];
		SSWR::AVIRead::AVIRCaptureDevForm::CaptureFormat *cfmt;
		UOSInt bestSize = 0;
		UInt32 bestFmt = 0;
		UOSInt bestBPP = 0;
		UOSInt bestIndex = 0;
		UOSInt currSize;
		UOSInt fmtCnt;
		UOSInt i;
		Media::CS::CSConverter::GetSupportedCS(&supportedCS);

		devInfo.AppendC(UTF8STRC("Name: "));
		sptr = me->currCapture->GetSourceName(sbuff);
		devInfo.AppendP(sbuff, sptr);
		devInfo.AppendC(UTF8STRC("\r\n"));
		me->currCapture->GetInfo(devInfo);
		devInfo.AppendC(UTF8STRC("Supported Formats:\r\n"));
		fmtCnt = me->currCapture->GetSupportedFormats(fmts, 80);
		i = 0;
		while (i < fmtCnt)
		{
			cfmt = MemAlloc(SSWR::AVIRead::AVIRCaptureDevForm::CaptureFormat, 1);
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
	UOSInt i = this->currFormats.GetCount();
	while (i-- > 0)
	{
		MemFree(this->currFormats.RemoveAt(i));
	}
}

SSWR::AVIRead::AVIRCaptureDevForm::AVIRCaptureDevForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 768, 200, ui)
{
	this->SetText(CSTR("Select Capture Device"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->lblDevice, UI::GUILabel(ui, this, CSTR("Capture Device")));
	this->lblDevice->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->lbDevice, UI::GUIListBox(ui, this, false));
	this->lbDevice->SetRect(112, 8, 256, 88, false);
	this->lbDevice->HandleSelectionChange(OnDevChg, this);
	NEW_CLASS(this->lblFormat, UI::GUILabel(ui, this, CSTR("Capture Format")));
	this->lblFormat->SetRect(8, 96, 100, 23, false);
	NEW_CLASS(this->cboFormat, UI::GUIComboBox(ui, this, false));
	this->cboFormat->SetRect(112, 96, 264, 23, false);
	NEW_CLASS(this->txtDeviceInfo, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtDeviceInfo->SetReadOnly(true);
	this->txtDeviceInfo->SetRect(376, 8, 384, 168, false);

	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("&OK")));
	this->btnOK->SetRect(112, 144, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("&Cancel")));
	this->btnCancel->SetRect(192, 144, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	this->currCapture = 0;
	this->capture = 0;
	this->captureMgr.GetDeviceList(&this->devInfoList);
	UOSInt cnt = this->devInfoList.GetCount();
	if (cnt == 0)
	{
		return;
	}
	UOSInt i;
	i = 0;
	while (i < cnt)
	{
		Media::VideoCaptureMgr::DeviceInfo *dev = this->devInfoList.GetItem(i);
		this->lbDevice->AddItem({dev->devName, Text::StrCharCnt(dev->devName)}, dev);
		i++;
	}

}

SSWR::AVIRead::AVIRCaptureDevForm::~AVIRCaptureDevForm()
{
	this->ClearChildren();
	this->captureMgr.FreeDeviceList(&this->devInfoList);
	if (this->currCapture)
	{
		DEL_CLASS(this->currCapture);
		this->currCapture = 0;
	}
	this->ReleaseFormats();
}

void SSWR::AVIRead::AVIRCaptureDevForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
