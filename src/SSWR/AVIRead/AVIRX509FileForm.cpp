#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRX509FileForm.h"
#include "Text/StringBuilderUTF8.h"

SSWR::AVIRead::AVIRX509FileForm::AVIRX509FileForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Crypto::X509File *file) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"X509 File");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->file = file;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpASN1 = this->tcMain->AddTabPage((const UTF8Char*)"ASN1");
	NEW_CLASS(this->txtASN1, UI::GUITextBox(ui, this->tpASN1, (const UTF8Char*)"", true));
	this->txtASN1->SetDockType(UI::GUIControl::DOCK_FILL);

	Text::StringBuilderUTF8 sb;
	this->file->ToASN1String(&sb);
	this->txtASN1->SetText(sb.ToString());
}

SSWR::AVIRead::AVIRX509FileForm::~AVIRX509FileForm()
{
	DEL_CLASS(this->file);
}

void SSWR::AVIRead::AVIRX509FileForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
