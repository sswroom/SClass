#include "Stdafx.h"
#include "SSWR/AVIRead/AVIROpenFileForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"

void __stdcall SSWR::AVIRead::AVIROpenFileForm::OnBrowseClicked(void *userObj)
{
	SSWR::AVIRead::AVIROpenFileForm *me = (SSWR::AVIRead::AVIROpenFileForm*)userObj;
	UI::FileDialog *dlg;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"OpenFile", false));
	me->txtName->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	me->core->GetParserList()->PrepareSelector(dlg, me->t);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtName->SetText(dlg->GetFileName());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIROpenFileForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIROpenFileForm *me = (SSWR::AVIRead::AVIROpenFileForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtName->GetText(&sb);
	me->fileName = Text::String::New(sb.ToString(), sb.GetLength());
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIROpenFileForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIROpenFileForm *me = (SSWR::AVIRead::AVIROpenFileForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIROpenFileForm::AVIROpenFileForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::ParserType t) : UI::GUIForm(parent, 640, 120, ui)
{
	this->SetText((const UTF8Char*)"Open File");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->fileName = 0;
	this->core = core;
	this->t = t;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this, (const UTF8Char*)"File Name"));
	this->lblName->SetRect(8, 16, 100, 23, false);
	NEW_CLASS(this->txtName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtName->SetRect(108, 16, 450, 23, false);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this, (const UTF8Char*)"B&rowse"));
	this->btnBrowse->SetRect(550, 16, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"&Ok"));
	this->btnOK->SetRect(240, 52, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"&Cancel"));
	this->btnCancel->SetRect(325, 52, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->txtName->Focus();
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
}

SSWR::AVIRead::AVIROpenFileForm::~AVIROpenFileForm()
{
	SDEL_STRING(this->fileName);
}

void SSWR::AVIRead::AVIROpenFileForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Text::String *SSWR::AVIRead::AVIROpenFileForm::GetFileName()
{
	return this->fileName;
}
