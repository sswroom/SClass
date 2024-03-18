#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRDataModelForm.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRDataModelForm::OnPasteDataClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRDataModelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDataModelForm>();
	Text::StringBuilderUTF8 sb;
	Text::PString lines[2];
	if (UI::Clipboard::GetString(me->GetHandle(), sb))
	{
		lines[1] = sb;
		Data::Class cls();
		cls.
	}
}

void __stdcall SSWR::AVIRead::AVIRDataModelForm::OnCancelClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRDataModelForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDataModelForm>();

}

SSWR::AVIRead::AVIRDataModelForm::AVIRDataModelForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("Data Model"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvColumns = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvColumns->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvColumns->AddColumn(CSTR("Data Type"), 200);
	this->lvColumns->AddColumn(CSTR("NotNull"), 50);
	this->lvColumns->AddColumn(CSTR("Name"), 300);
	this->btnPasteData = ui->NewButton(this->pnlControl, CSTR("Paste Data"));
	this->btnPasteData->SetRect(4, 4, 75, 23, false);
	this->btnPasteData->HandleButtonClick(OnPasteDataClicked, this);
	this->btnCancel = ui->NewButton(this->pnlControl, CSTR("Cancel"));
	this->btnCancel->SetRect(84, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
}

SSWR::AVIRead::AVIRDataModelForm::~AVIRDataModelForm()
{
}

void SSWR::AVIRead::AVIRDataModelForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
