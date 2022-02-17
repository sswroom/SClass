#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRTableMsgForm.h"

SSWR::AVIRead::AVIRTableMsgForm::AVIRTableMsgForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Text::CString title, UOSInt colCnt, const UTF8Char **colNames) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(title);
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->colCnt = colCnt;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lvTable, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, this->colCnt));
	this->lvTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTable->SetFullRowSelect(true);
	this->lvTable->SetShowGrid(true);
	UOSInt i = 0;
	while (i < this->colCnt)
	{
		this->lvTable->AddColumn(colNames[i], 200);
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

void SSWR::AVIRead::AVIRTableMsgForm::AddRow(const UTF8Char **row)
{
	UOSInt k;
	UOSInt i = 1;
	UOSInt j = this->colCnt;
	k = this->lvTable->AddItem({row[0], Text::StrCharCnt(row[0])}, 0);
	while (i < j)
	{
		this->lvTable->SetSubItem(k, i, row[i]);
		i++;
	}
}
