#include "Stdafx.h"
#include "SSWR/AVIRead/AVIREdgeAnalyseForm.h"

void __stdcall SSWR::AVIRead::AVIREdgeAnalyseForm::OnWebHookClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREdgeAnalyseForm> me = userObj.GetNN<SSWR::AVIRead::AVIREdgeAnalyseForm>();

}

void __stdcall SSWR::AVIRead::AVIREdgeAnalyseForm::OnWebHookDataSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREdgeAnalyseForm> me = userObj.GetNN<SSWR::AVIRead::AVIREdgeAnalyseForm>();

}

SSWR::AVIRead::AVIREdgeAnalyseForm::AVIREdgeAnalyseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("Edge Analyse Device"));
	this->SetFont(0, 0, 8.25, false);
	this->OnMonitorChanged();

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpWebHook = this->tcMain->AddTabPage(CSTR("Web Hook"));
	this->pnlWebHook = ui->NewPanel(this->tpWebHook);
	this->pnlWebHook->SetRect(0, 0, 100, 55, false);
	this->pnlWebHook->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblWebHookPort = ui->NewLabel(this->pnlWebHook, CSTR("Port"));
	this->lblWebHookPort->SetRect(4, 4, 100, 23, false);
	this->txtWebHookPort = ui->NewTextBox(this->pnlWebHook, CSTR("8080"));
	this->txtWebHookPort->SetRect(104, 4, 100, 23, false);
	this->btnWebHook = ui->NewButton(this->pnlWebHook, CSTR("Start"));
	this->btnWebHook->SetRect(104, 28, 75, 23, false);
	this->btnWebHook->HandleButtonClick(OnWebHookClicked, this);
	this->txtWebHookData = ui->NewTextBox(this->tpWebHook, CSTR(""), true);
	this->txtWebHookData->SetRect(0, 0, 100, 120, false);
	this->txtWebHookData->SetReadOnly(true);
	this->lvWebHookData = ui->NewListView(this->tpWebHook, UI::ListViewStyle::Table, 3);
	this->lvWebHookData->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvWebHookData->SetFullRowSelect(true);
	this->lvWebHookData->SetShowGrid(true);
	this->lvWebHookData->AddColumn(CSTR("Time"), 120);
	this->lvWebHookData->AddColumn(CSTR("Type"), 100);
	this->lvWebHookData->AddColumn(CSTR("Desc"), 300);
	this->lvWebHookData->HandleSelChg(OnWebHookDataSelChg, this);
}

SSWR::AVIRead::AVIREdgeAnalyseForm::~AVIREdgeAnalyseForm()
{
}

void SSWR::AVIRead::AVIREdgeAnalyseForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIREdgeAnalyseForm::AddRecord(Data::Timestamp ts, NN<Text::String> type, NN<Text::JSONBase> json)
{

}
