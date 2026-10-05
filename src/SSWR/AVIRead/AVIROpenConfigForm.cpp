#include "Stdafx.h"
#include "IO/IniFile.h"
#include "IO/Java/JavaProperties.h"
#include "SSWR/AVIRead/AVIROpenConfigForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIROpenConfigForm::OnBrowseClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenConfigForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenConfigForm>();
	Text::StringBuilderUTF8 sb;
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"OpenConfig", false);
	me->txtName->GetText(sb);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	dlg->AddFilter(CSTR("*.cfg"), CSTR("Config Files"));
	dlg->AddFilter(CSTR("*.ini"), CSTR("INI Files"));
	dlg->AddFilter(CSTR("*.properties"), CSTR("Properties Files"));
	dlg->AddFilter(CSTR("*.yml"), CSTR("YAML Files"));
	dlg->AddFilter(CSTR("*.yaml"), CSTR("YAML Files"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->SetFile(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIROpenConfigForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenConfigForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenConfigForm>();
	Text::StringBuilderUTF8 sb;
	me->txtName->GetText(sb);
	UIntOS typeIndex = me->cboType->GetSelectedIndex();
	if (typeIndex == 0)
	{
		me->cfg = IO::IniFile::Parse(sb.ToCString(), 0);
	}
	else if (typeIndex == 1)
	{
		me->cfg = IO::Java::JavaProperties::Parse(sb.ToCString());
	}
	else if (typeIndex == 2)
	{
		me->cfg = nullptr; //IO::YAMLFile::ParseFile(sb.ToCString());
	}
	if (me->cfg.NotNull())
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Failed to parse config file."), CSTR("Open Config"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIROpenConfigForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenConfigForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenConfigForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIROpenConfigForm::FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIROpenConfigForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenConfigForm>();
	if (files.GetCount() > 0)
	{
		UIntOS i = 0;
		UIntOS j = files.GetCount();
		while (i < j)
		{
			if (me->SetFile(files[i]->ToCString()))
			{
				break;
			}
			i++;
		}
	}
}

Bool SSWR::AVIRead::AVIROpenConfigForm::SetFile(Text::CStringNN fileName)
{
	if (fileName.EndsWith(CSTR(".ini")))
	{
		this->txtName->SetText(fileName);
		this->cboType->SetSelectedIndex(0);
		return true;
	}
	else if (fileName.EndsWith(CSTR(".cfg")))
	{
		this->txtName->SetText(fileName);
		this->cboType->SetSelectedIndex(0);
		return true;
	}
	else if (fileName.EndsWith(CSTR(".properties")))
	{
		this->txtName->SetText(fileName);
		this->cboType->SetSelectedIndex(1);
		return true;
	}
	else if (fileName.EndsWith(CSTR(".yml")) || fileName.EndsWith(CSTR(".yaml")))
	{
		this->txtName->SetText(fileName);
		this->cboType->SetSelectedIndex(2);
		return true;
	}
	return false;
}

SSWR::AVIRead::AVIROpenConfigForm::AVIROpenConfigForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 120, ui)
{
	this->SetText(CSTR("Open Config"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->cfg = nullptr;
	this->core = core;
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

	this->cboType->AddItem(CSTR("INI File"), (void*)0);
	this->cboType->AddItem(CSTR("Java Properties File"), (void*)1);
	this->cboType->AddItem(CSTR("YAML File"), (void*)2);
	this->cboType->SetSelectedIndex(0);

	this->HandleDropFiles(FileHandler, this);
}

SSWR::AVIRead::AVIROpenConfigForm::~AVIROpenConfigForm()
{
}

void SSWR::AVIRead::AVIROpenConfigForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<IO::ConfigFile> SSWR::AVIRead::AVIROpenConfigForm::GetConfigFile() const
{
	return this->cfg;
}
