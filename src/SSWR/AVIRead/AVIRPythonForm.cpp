#include "Stdafx.h"
#include "Python/PythonDict.h"
#include "Python/PythonModule.h"
#include "SSWR/AVIRead/AVIRPythonForm.h"

void __stdcall SSWR::AVIRead::AVIRPythonForm::OnModuleClicked(AnyType userObj)
{
	NN<AVIRPythonForm> me = userObj.GetNN<AVIRPythonForm>();
	Text::StringBuilderUTF8 sb;
	me->txtModuleName->GetText(sb);
	NN<Python::PythonModule> mod;
	UnsafeArray<const UTF8Char> s;
	if (sb.GetLength() > 0)
	{
		if (me->pyCore.ImportModule(sb.v).SetTo(mod))
		{
			me->pyModule.Delete();
			me->pyModule = mod;
			if (mod->GetFileName().SetTo(s))
			{
				me->txtModuleFile->SetText(Text::CStringNN::FromPtr(s));
			}
			else
			{
				me->txtModuleFile->SetText(CSTR("<Unknown>"));
			}
			me->lvModule->ClearItems();
			NN<Python::PythonDict> dict;
			if (mod->GetDict().SetTo(dict))
			{
				UnsafeArray<const UTF8Char> key;
				UIntOS i = 0;
				while (true)
				{
					if (!dict->GetKey(i).SetTo(key))
						break;
					me->lvModule->AddItem(Text::CStringNN::FromPtr(key), 0);
					NN<Python::PythonObject> val;
					if (dict->GetValue(i).SetTo(val))
					{
						sb.ClearStr();
						val->ToString(sb);
						me->lvModule->SetSubItem(i, 1, Python::ObjectTypeGetName(val->GetObjectType()));
						me->lvModule->SetSubItem(i, 2, sb.ToCString());
						val.Delete();
					}
					i++;
				}
				dict.Delete();
			}
		}
	}
}

SSWR::AVIRead::AVIRPythonForm::AVIRPythonForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Python"));

	this->core = core;
	this->pyModule = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->lblVersion = ui->NewLabel(this->tpInfo, CSTR("Version"));
	this->lblVersion->SetRect(4, 4, 100, 23, false);
	this->txtVersion = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtVersion->SetRect(104, 4, 300, 23, false);
	this->txtVersion->SetReadOnly(true);
	this->lblPlatform = ui->NewLabel(this->tpInfo, CSTR("Platform"));
	this->lblPlatform->SetRect(4, 28, 100, 23, false);
	this->txtPlatform = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtPlatform->SetRect(104, 28, 200, 23, false);
	this->txtPlatform->SetReadOnly(true);
	this->lblCompiler = ui->NewLabel(this->tpInfo, CSTR("Compiler"));
	this->lblCompiler->SetRect(4, 52, 100, 23, false);
	this->txtCompiler = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtCompiler->SetRect(104, 52, 200, 23, false);
	this->txtCompiler->SetReadOnly(true);
	this->lblBuildInfo = ui->NewLabel(this->tpInfo, CSTR("Build Info"));
	this->lblBuildInfo->SetRect(4, 76, 100, 23, false);
	this->txtBuildInfo = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBuildInfo->SetRect(104, 76, 200, 23, false);
	this->txtBuildInfo->SetReadOnly(true);
	this->lblCopyright = ui->NewLabel(this->tpInfo, CSTR("Copyright"));
	this->lblCopyright->SetRect(4, 100, 100, 23, false);
	this->txtCopyright = ui->NewTextBox(this->tpInfo, CSTR(""), true);
	this->txtCopyright->SetRect(104, 100, 400, 192, false);
	this->txtCopyright->SetReadOnly(true);

	this->tpModule = this->tcMain->AddTabPage(CSTR("Module"));
	this->pnlModule = ui->NewPanel(this->tpModule);
	this->pnlModule->SetRect(0, 0, 100, 52, true);
	this->pnlModule->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblModuleName = ui->NewLabel(this->pnlModule, CSTR("Module Name"));
	this->lblModuleName->SetRect(4, 4, 100, 23, false);
	this->txtModuleName = ui->NewTextBox(this->pnlModule, CSTR(""));
	this->txtModuleName->SetRect(104, 4, 200, 23, false);
	this->btnModule = ui->NewButton(this->pnlModule, CSTR("Load"));
	this->btnModule->SetRect(304, 4, 75, 23, false);
	this->btnModule->HandleButtonClick(OnModuleClicked, this);
	this->lblModuleFile = ui->NewLabel(this->pnlModule, CSTR("File Name"));
	this->lblModuleFile->SetRect(4, 28, 100, 23, false);
	this->txtModuleFile = ui->NewTextBox(this->pnlModule, CSTR(""));
	this->txtModuleFile->SetRect(104, 28, 400, 23, false);
	this->txtModuleFile->SetReadOnly(true);
	this->lvModule = ui->NewListView(this->tpModule, UI::ListViewStyle::Table, 3);
	this->lvModule->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvModule->AddColumn(CSTR("Key"), 200);
	this->lvModule->AddColumn(CSTR("Type"), 100);
	this->lvModule->AddColumn(CSTR("Value"), 400);
	this->lvModule->SetFullRowSelect(true);
	this->lvModule->SetShowGrid(true);

	UnsafeArray<const UTF8Char> s;
	if (this->pyCore.GetVersion().SetTo(s))
	{
		this->txtVersion->SetText(Text::CStringNN::FromPtr(s));
	}
	if (this->pyCore.GetPlatform().SetTo(s))
	{
		this->txtPlatform->SetText(Text::CStringNN::FromPtr(s));
	}
	if (this->pyCore.GetCopyright().SetTo(s))
	{
		this->txtCopyright->SetText(Text::CStringNN::FromPtr(s));
	}
	if (this->pyCore.GetCompiler().SetTo(s))
	{
		this->txtCompiler->SetText(Text::CStringNN::FromPtr(s));
	}
	if (this->pyCore.GetBuildInfo().SetTo(s))
	{
		this->txtBuildInfo->SetText(Text::CStringNN::FromPtr(s));
	}
}

SSWR::AVIRead::AVIRPythonForm::~AVIRPythonForm()
{
	this->pyModule.Delete();
}

void SSWR::AVIRead::AVIRPythonForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
