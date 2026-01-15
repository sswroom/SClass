#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSelPrinterForm.h"

void __stdcall SSWR::AVIRead::AVIRSelPrinterForm::OnSettingClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSelPrinterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSelPrinterForm>();
	NN<Media::Printer> currPrinter;
	if (me->currPrinter.SetTo(currPrinter))
	{
		currPrinter->ShowPrintSettings(nullptr);
	}
}

void __stdcall SSWR::AVIRead::AVIRSelPrinterForm::OnPrinterChg(AnyType userObj)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRSelPrinterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSelPrinterForm>();
	NN<Media::Printer> currPrinter;
	if (me->cboPrinter->GetSelectedItemText(sbuff).SetTo(sptr))
	{
		me->currPrinter.Delete();
		NEW_CLASSNN(currPrinter, Media::Printer(CSTRP(sbuff, sptr)));
		if (currPrinter->IsError())
		{
			currPrinter.Delete();
		}
		else
		{
			me->currPrinter = currPrinter;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSelPrinterForm::OnOKClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSelPrinterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSelPrinterForm>();
	if (me->currPrinter.NotNull())
	{
		me->printer = me->currPrinter;
		me->currPrinter = nullptr;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRSelPrinterForm::OnCancelClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSelPrinterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSelPrinterForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRSelPrinterForm::AVIRSelPrinterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 512, 104, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	UIntOS j;

	this->SetText(CSTR("Select Printer"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->currPrinter = nullptr;
	this->printer = nullptr;
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
		sbuff[0] = 0;
		sptr = Media::Printer::GetPrinterName(sbuff, i).Or(sbuff);
		this->cboPrinter->AddItem(CSTRP(sbuff, sptr), 0);
		i++;
	}
	if (j > 0)
	{
		sbuff[0] = 0;
		sptr = Media::Printer::GetPrinterName(sbuff, i).Or(sbuff);
		this->cboPrinter->SetSelectedIndex(0);
		NN<Media::Printer> currPrinter;
		NEW_CLASSNN(currPrinter, Media::Printer(CSTRP(sbuff, sptr)));
		if (currPrinter->IsError())
		{
			currPrinter.Delete();
		}
		else
		{
			this->currPrinter = currPrinter;
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
	this->currPrinter.Delete();
}

void SSWR::AVIRead::AVIRSelPrinterForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
