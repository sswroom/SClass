#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISOSMDataForm.h"

SSWR::AVIRead::AVIRGISOSMDataForm::AVIRGISOSMDataForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::OSM::OSMData> osmData, NN<SSWR::AVIRead::AVIRMapNavigator> nav) : UI::GUIForm(parent, 416, 408, ui)
{
	this->core = core;
	this->osmData = osmData;
	this->nav = nav;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("OSM Data - "));
	sb.Append(osmData->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(nullptr, 8.25, false);

	this->tcMain = this->ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpRelation = this->tcMain->AddTabPage(CSTR("Relations"));
	this->lbRelation = this->ui->NewListBox(this->tpRelation, false);
	this->lbRelation->SetRect(0, 0, 100, 100, false);
	this->lbRelation->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lvRelation = this->ui->NewListView(this->tpRelation, UI::ListViewStyle::Table, 2);
	this->lvRelation->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvRelation->AddColumn(CSTR("k"), 100);
	this->lvRelation->AddColumn(CSTR("v"), 200);
}

SSWR::AVIRead::AVIRGISOSMDataForm::~AVIRGISOSMDataForm()
{
}

void SSWR::AVIRead::AVIRGISOSMDataForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
