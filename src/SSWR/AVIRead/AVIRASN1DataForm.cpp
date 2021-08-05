#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRASN1DataForm.h"
#include "Text/StringBuilderUTF8.h"

SSWR::AVIRead::AVIRASN1DataForm::AVIRASN1DataForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::ASN1Data *asn1) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"ASN1 Data");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->asn1 = asn1;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpDesc = this->tcMain->AddTabPage((const UTF8Char*)"Desc");
	NEW_CLASS(this->txtDesc, UI::GUITextBox(ui, this->tpDesc, (const UTF8Char*)"", true));
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpASN1 = this->tcMain->AddTabPage((const UTF8Char*)"ASN1");
	NEW_CLASS(this->txtASN1, UI::GUITextBox(ui, this->tpASN1, (const UTF8Char*)"", true));
	this->txtASN1->SetDockType(UI::GUIControl::DOCK_FILL);

	Text::StringBuilderUTF8 sb;
	this->asn1->ToString(&sb);
	this->txtDesc->SetText(sb.ToString());
	sb.ClearStr();
	this->asn1->ToASN1String(&sb);
	this->txtASN1->SetText(sb.ToString());
}

SSWR::AVIRead::AVIRASN1DataForm::~AVIRASN1DataForm()
{
	DEL_CLASS(this->asn1);
}

void SSWR::AVIRead::AVIRASN1DataForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
