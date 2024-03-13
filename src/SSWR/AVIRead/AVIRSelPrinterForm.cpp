#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSelPrinterForm.h"

void __stdcall SSWR::AVIRead::AVIRSelPrinterForm::OnSettingClick(void *userObj)
{
	SSWR::AVIRead::AVIRSelPrinterForm *me = (SSWR::AVIRead::AVIRSelPrinterForm*)userObj;
	if (me->currPrinter)
	{
		me->currPrinter->ShowPrintSettings(0);
	}
}

void __stdcall SSWR::AVIRead::AVIRSelPrinterForm::OnPrinterChg(void *userObj)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	SSWR::AVIRead::AVIRSelPrinterForm *me = (SSWR::AVIRead::AVIRSelPrinterForm*)userObj;
	if ((sptr = me->cboPrinter->GetSelectedItemText(sbuff)) != 0)
	{
		SDEL_CLASS(me->currPrinter);
		NEW_CLASS(me->currPrinter, Media::Printer(CSTRP(sbuff, sptr)));
		if (me->currPrinter->IsError())
		{
			DEL_CLASS(me->currPrinter);
			me->currPrinter = 0;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSelPrinterForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRSelPrinterForm *me = (SSWR::AVIRead::AVIRSelPrinterForm*)userObj;
	if (me->currPrinter)
	{
		me->printer = me->currPrinter;
		me->currPrinter = 0;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRSelPrinterForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRSelPrinterForm *me = (SSWR::AVIRead::AVIRSelPrinterForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRSelPrinterForm::AVIRSelPrinterForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 512, 104, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr = 0;
	UOSInt i;
	UOSInt j;

	this->SetText(CSTR("Select Printer"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->currPrinter = 0;
	this->printer = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblPrinter = ui->NewLabel(*this, CSTR("Printer"));
	this->lblPrinter->SetRect(8, 8, 100, 23, false);
	this->cboPrinter = ui->NewComboBox(*this, false);
	this->cboPrinter->SetRect(108, 8, 300, 23, false);
	this->cboPrinter->HandleSelectionChange(OnPrinterChg, this);
	this->btnSetting = ui->NewButton(*this, CSTR("&Setting"));
	this->btnSetting->SetRect(408, 8, 75, 23, false);
	this->btnSetting->HandleButtonClick(OnSettingClick, this);
	
	i = 0;
	j = Media::Printer::GetPrinterCount();
	while (i < j)
	{
		sptr = Media::Printer::GetPrinterName(sbuff, i);
		this->cboPrinter->AddItem(CSTRP(sbuff, sptr), 0);
		i++;
	}
	if (j > 0)
	{
		this->cboPrinter->SetSelectedIndex(0);
		NEW_CLASS(this->currPrinter, Media::Printer(CSTRP(sbuff, sptr)));
		if (this->currPrinter->IsError())
		{
			DEL_CLASS(this->currPrinter);
			this->currPrinter = 0;
		}
	}

	this->btnOK = ui->NewButton(*this, CSTR("&OK"));
	this->btnOK->SetRect(176, 40, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(260, 40, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
}

SSWR::AVIRead::AVIRSelPrinterForm::~AVIRSelPrinterForm()
{
	SDEL_CLASS(this->currPrinter);
}

void SSWR::AVIRead::AVIRSelPrinterForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
