#include "Stdafx.h"
#include "UI/MessageDialog.h"
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
	SSWR::AVIRead::AVIRSelPrinterForm *me = (SSWR::AVIRead::AVIRSelPrinterForm*)userObj;
	if (me->cboPrinter->GetSelectedItemText(sbuff))
	{
		SDEL_CLASS(me->currPrinter);
		NEW_CLASS(me->currPrinter, Media::Printer(sbuff));
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

SSWR::AVIRead::AVIRSelPrinterForm::AVIRSelPrinterForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 512, 104, ui)
{
	UTF8Char sbuff[512];
	UOSInt i;
	UOSInt j;

	this->SetText((const UTF8Char*)"Select Printer");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->currPrinter = 0;
	this->printer = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblPrinter, UI::GUILabel(ui, this, (const UTF8Char*)"Printer"));
	this->lblPrinter->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->cboPrinter, UI::GUIComboBox(ui, this, false));
	this->cboPrinter->SetRect(108, 8, 300, 23, false);
	this->cboPrinter->HandleSelectionChange(OnPrinterChg, this);
	NEW_CLASS(this->btnSetting, UI::GUIButton(ui, this, (const UTF8Char*)"&Setting"));
	this->btnSetting->SetRect(408, 8, 75, 23, false);
	this->btnSetting->HandleButtonClick(OnSettingClick, this);
	
	i = 0;
	j = Media::Printer::GetPrinterCount();
	while (i < j)
	{
		Media::Printer::GetPrinterName(sbuff, i);
		this->cboPrinter->AddItem(sbuff, 0);
		i++;
	}
	if (j > 0)
	{
		this->cboPrinter->SetSelectedIndex(0);
		NEW_CLASS(this->currPrinter, Media::Printer(sbuff));
		if (this->currPrinter->IsError())
		{
			DEL_CLASS(this->currPrinter);
			this->currPrinter = 0;
		}
	}

	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"&OK"));
	this->btnOK->SetRect(176, 40, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"&Cancel"));
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
