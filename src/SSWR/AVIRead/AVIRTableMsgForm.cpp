#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRTableMsgForm.h"

SSWR::AVIRead::AVIRTableMsgForm::AVIRTableMsgForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Text::CStringNN title, UOSInt colCnt, UnsafeArray<UnsafeArray<const UTF8Char>> colNames) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(title);
	this->SetFont(nullptr, 8.25, false);
	
	this->core = core;
	this->colCnt = colCnt;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lvTable = ui->NewListView(*this, UI::ListViewStyle::Table, this->colCnt);
	this->lvTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTable->SetFullRowSelect(true);
	this->lvTable->SetShowGrid(true);
	UOSInt i = 0;
	while (i < this->colCnt)
	{
		this->lvTable->AddColumn(Text::CStringNN::FromPtr(colNames[i]), 200);
		i++;
	}
}

SSWR::AVIRead::AVIRTableMsgForm::~AVIRTableMsgForm()
{
}

void SSWR::AVIRead::AVIRTableMsgForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRTableMsgForm::AddRow(UnsafeArray<UnsafeArray<const UTF8Char>> row)
{
	UOSInt k;
	UOSInt i = 1;
	UOSInt j = this->colCnt;
	k = this->lvTable->AddItem(Text::CStringNN::FromPtr(row[0]), 0);
	while (i < j)
	{
		this->lvTable->SetSubItem(k, i, Text::CStringNN::FromPtr(row[i]));
		i++;
	}
}
