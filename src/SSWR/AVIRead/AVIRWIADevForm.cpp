#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "Text/StringBuilder.h"
#include "SSWR/AVIRead/AVIRWIADevForm.h"

void __stdcall SSWR::AVIRead::AVIRWIADevForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRWIADevForm *me = (SSWR::AVIRead::AVIRWIADevForm*)userObj;
/*	if (me->currCapture == 0)
	{
		me->ui->ShowMsgOK(L"Please select a device", L"Select Capture Device");
		return;
	}
	CaptureFormat *fmt = (CaptureFormat*)me->cboFormat->GetItem(me->cboFormat->GetSelectedIndex());
	if (fmt == 0)
	{
		me->ui->ShowMsgOK(L"Please select a format", L"Select Capture Device");
		return;
	}

	me->currCapture->SetPreferSize(fmt->width, fmt->height, fmt->fourcc, fmt->bpp);
	me->capture = me->currCapture;
	me->currCapture = 0;*/

	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRWIADevForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRWIADevForm *me = (SSWR::AVIRead::AVIRWIADevForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRWIADevForm::AVIRWIADevForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("Select WIA Device"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lblDevice = ui->NewLabel(*this, CSTR("Capture Device"));
	this->lblDevice->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->lbDevice, UI::GUIListBox(ui, *this, false));
	this->lbDevice->SetRect(112, 8, 256, 112, false);

	this->btnOK = ui->NewButton(*this, CSTR("&OK"));
	this->btnOK->SetRect(112, 144, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(192, 144, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	NEW_CLASS(this->wiaMgr, Media::WIAManager());
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->wiaMgr->GetDeviceCount();
	while (i < j)
	{
		const UTF8Char *name = this->wiaMgr->GetDeviceName(i);
		this->lbDevice->AddItem({name, Text::StrCharCnt(name)}, (void*)i);
		i++;
	}
}

SSWR::AVIRead::AVIRWIADevForm::~AVIRWIADevForm()
{
	DEL_CLASS(this->wiaMgr);
}

void SSWR::AVIRead::AVIRWIADevForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
