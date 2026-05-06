#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRPythonForm.h"

SSWR::AVIRead::AVIRPythonForm::AVIRPythonForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Python"));

	this->core = core;
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
}

void SSWR::AVIRead::AVIRPythonForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
