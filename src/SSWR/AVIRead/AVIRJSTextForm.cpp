#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRJSTextForm.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRJSTextForm::OnExtractClicked(void *userObj)
{
	SSWR::AVIRead::AVIRJSTextForm *me = (SSWR::AVIRead::AVIRJSTextForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::String *s;
	me->txtJSText->GetText(&sb);
	s = Text::JSText::FromNewJSText(sb.ToString());
	if (s)
	{
		me->txtOriText->SetText(s->ToCString());
		s->Release();
	}
}

SSWR::AVIRead::AVIRJSTextForm::AVIRJSTextForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 160, ui)
{
	this->SetText(CSTR("JS Text"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblJSText, UI::GUILabel(ui, this, (const UTF8Char*)"JS Text"));
	this->lblJSText->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtJSText, UI::GUITextBox(ui, this, CSTR("")));
	this->txtJSText->SetRect(104, 4, 800, 23, false);
	NEW_CLASS(this->btnExtract, UI::GUIButton(ui, this, CSTR("Extract")));
	this->btnExtract->SetRect(104, 28, 75, 23, false);
	this->btnExtract->HandleButtonClick(OnExtractClicked, this);
	NEW_CLASS(this->lblOriText, UI::GUILabel(ui, this, (const UTF8Char*)"Original Text"));
	this->lblOriText->SetRect(4, 56, 100, 23, false);
	NEW_CLASS(this->txtOriText, UI::GUITextBox(ui, this, CSTR("")));
	this->txtOriText->SetRect(104, 56, 800, 23, false);
	this->txtOriText->SetReadOnly(true);
}

SSWR::AVIRead::AVIRJSTextForm::~AVIRJSTextForm()
{
}

void SSWR::AVIRead::AVIRJSTextForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
