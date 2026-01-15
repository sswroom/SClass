#include "Stdafx.h"
#include "SSWR/AVIRead/AVIROpenFileForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIROpenFileForm::OnBrowseClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenFileForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenFileForm>();
	Text::StringBuilderUTF8 sb;
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"OpenFile", false);
	me->txtName->GetText(sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	me->core->GetParserList()->PrepareSelector(dlg, me->t);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtName->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIROpenFileForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenFileForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenFileForm>();
	Text::StringBuilderUTF8 sb;
	me->txtName->GetText(sb);
	me->fileName = Text::String::New(sb.ToString(), sb.GetLength());
	me->parserType = (IO::ParserType)me->cboType->GetSelectedItem().GetUIntOS();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIROpenFileForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenFileForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenFileForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIROpenFileForm::FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIROpenFileForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenFileForm>();
	if (files.GetCount() > 0)
	{
		me->txtName->SetText(files[0]->ToCString());
	}
}

SSWR::AVIRead::AVIROpenFileForm::AVIROpenFileForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, IO::ParserType t) : UI::GUIForm(parent, 640, 120, ui)
{
	this->SetText(CSTR("Open File"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->fileName = nullptr;
	this->parserType = IO::ParserType::Unknown;
	this->core = core;
	this->t = t;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->lblName = ui->NewLabel(*this, CSTR("File Name"));
	this->lblName->SetRect(8, 16, 100, 23, false);
	this->txtName = ui->NewTextBox(*this, CSTR(""));
	this->txtName->SetRect(108, 16, 450, 23, false);
	this->btnBrowse = ui->NewButton(*this, CSTR("B&rowse"));
	this->btnBrowse->SetRect(550, 16, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	this->lblType = ui->NewLabel(*this, CSTR("Type"));
	this->lblType->SetRect(8, 40, 100, 23, false);
	this->cboType = ui->NewComboBox(*this, false);
	this->cboType->SetRect(108, 40, 200, 23, false);
	this->btnOK = ui->NewButton(*this, CSTR("&Ok"));
	this->btnOK->SetRect(240, 76, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(325, 76, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->txtName->Focus();
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	this->cboType->AddItem(CSTR("-- Any Type --"), (void*)IO::ParserType::Unknown);
	UIntOS i = (UIntOS)IO::ParserType::Unknown;
	while (i < (UIntOS)IO::ParserType::LastType)
	{
		i++;
		this->cboType->AddItem(IO::ParserTypeGetName((IO::ParserType)i), (void*)i);
	}
	this->cboType->SetSelectedIndex(0);

	this->HandleDropFiles(FileHandler, this);
}

SSWR::AVIRead::AVIROpenFileForm::~AVIROpenFileForm()
{
	OPTSTR_DEL(this->fileName);
}

void SSWR::AVIRead::AVIROpenFileForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

NN<Text::String> SSWR::AVIRead::AVIROpenFileForm::GetFileName() const
{
	return Text::String::OrEmpty(this->fileName);
}

IO::ParserType SSWR::AVIRead::AVIROpenFileForm::GetParserType()
{
	return this->parserType;
}
