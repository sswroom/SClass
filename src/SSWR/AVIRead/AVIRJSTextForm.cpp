#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRJSTextForm.h"
#include "Text/JSText.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRJSTextForm::OnExtractClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRJSTextForm> me = userObj.GetNN<SSWR::AVIRead::AVIRJSTextForm>();
	Text::StringBuilderUTF8 sb;
	NN<Text::String> s;
	me->txtJSText->GetText(sb);
	if (Text::JSText::FromNewJSText(sb.ToString()).SetTo(s))
	{
		me->txtOriText->SetText(s->ToCString());
		s->Release();
	}
}

SSWR::AVIRead::AVIRJSTextForm::AVIRJSTextForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 160, ui)
{
	this->SetText(CSTR("JS Text"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblJSText = ui->NewLabel(*this, CSTR("JS Text"));
	this->lblJSText->SetRect(4, 4, 100, 23, false);
	this->txtJSText = ui->NewTextBox(*this, CSTR(""));
	this->txtJSText->SetRect(104, 4, 800, 23, false);
	this->btnExtract = ui->NewButton(*this, CSTR("Extract"));
	this->btnExtract->SetRect(104, 28, 75, 23, false);
	this->btnExtract->HandleButtonClick(OnExtractClicked, this);
	this->lblOriText = ui->NewLabel(*this, CSTR("Original Text"));
	this->lblOriText->SetRect(4, 56, 100, 23, false);
	this->txtOriText = ui->NewTextBox(*this, CSTR(""));
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
