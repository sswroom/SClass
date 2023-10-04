#include "Stdafx.h"
#include "SSWR/AVIRead/AVIROpenFileForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"

void __stdcall SSWR::AVIRead::AVIROpenFileForm::OnBrowseClicked(void *userObj)
{
	SSWR::AVIRead::AVIROpenFileForm *me = (SSWR::AVIRead::AVIROpenFileForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"OpenFile", false);
	me->txtName->GetText(sb);
	if (sb.GetLength() > 0)
	{
		dlg.SetFileName(sb.ToCString());
	}
	me->core->GetParserList()->PrepareSelector(dlg, me->t);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->txtName->SetText(dlg.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIROpenFileForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIROpenFileForm *me = (SSWR::AVIRead::AVIROpenFileForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtName->GetText(sb);
	me->fileName = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
	me->parserType = (IO::ParserType)(UOSInt)me->cboType->GetSelectedItem();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIROpenFileForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIROpenFileForm *me = (SSWR::AVIRead::AVIROpenFileForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIROpenFileForm::AVIROpenFileForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::ParserType t) : UI::GUIForm(parent, 640, 120, ui)
{
	this->SetText(CSTR("Open File"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->fileName = 0;
	this->parserType = IO::ParserType::Unknown;
	this->core = core;
	this->t = t;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this, CSTR("File Name")));
	this->lblName->SetRect(8, 16, 100, 23, false);
	NEW_CLASS(this->txtName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtName->SetRect(108, 16, 450, 23, false);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this, CSTR("B&rowse")));
	this->btnBrowse->SetRect(550, 16, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->lblType, UI::GUILabel(ui, this, CSTR("Type")));
	this->lblType->SetRect(8, 40, 100, 23, false);
	NEW_CLASS(this->cboType, UI::GUIComboBox(ui, this, false));
	this->cboType->SetRect(108, 40, 200, 23, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("&Ok")));
	this->btnOK->SetRect(240, 76, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("&Cancel")));
	this->btnCancel->SetRect(325, 76, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->txtName->Focus();
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	this->cboType->AddItem(CSTR("-- Any Type --"), (void*)IO::ParserType::Unknown);
	UOSInt i = (UOSInt)IO::ParserType::Unknown;
	while (i < (UOSInt)IO::ParserType::LastType)
	{
		i++;
		this->cboType->AddItem(IO::ParserTypeGetName((IO::ParserType)i), (void*)i);
	}
	this->cboType->SetSelectedIndex(0);
}

SSWR::AVIRead::AVIROpenFileForm::~AVIROpenFileForm()
{
	SDEL_STRING(this->fileName);
}

void SSWR::AVIRead::AVIROpenFileForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

NotNullPtr<Text::String> SSWR::AVIRead::AVIROpenFileForm::GetFileName() const
{
	return Text::String::OrEmpty(this->fileName);
}

IO::ParserType SSWR::AVIRead::AVIROpenFileForm::GetParserType()
{
	return this->parserType;
}
