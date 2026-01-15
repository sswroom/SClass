#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRWIADevForm.h"
#include "Text/StringBuilder.hpp"

void __stdcall SSWR::AVIRead::AVIRWIADevForm::OnOKClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWIADevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWIADevForm>();
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

void __stdcall SSWR::AVIRead::AVIRWIADevForm::OnCancelClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWIADevForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWIADevForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRWIADevForm::AVIRWIADevForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("Select WIA Device"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lblDevice = ui->NewLabel(*this, CSTR("Capture Device"));
	this->lblDevice->SetRect(8, 8, 100, 23, false);
	this->lbDevice = ui->NewListBox(*this, false);
	this->lbDevice->SetRect(112, 8, 256, 112, false);

	this->btnOK = ui->NewButton(*this, CSTR("&OK"));
	this->btnOK->SetRect(112, 144, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(192, 144, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	NEW_CLASSNN(this->wiaMgr, Media::WIAManager());
	UIntOS i;
	UIntOS j;
	i = 0;
	j = this->wiaMgr->GetDeviceCount();
	while (i < j)
	{
		UnsafeArray<const UTF8Char> name = this->wiaMgr->GetDeviceName(i).Or(U8STR(""));
		this->lbDevice->AddItem({name, Text::StrCharCnt(name)}, (void*)i);
		i++;
	}
}

SSWR::AVIRead::AVIRWIADevForm::~AVIRWIADevForm()
{
	this->wiaMgr.Delete();
}

void SSWR::AVIRead::AVIRWIADevForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
