#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSMBIOSForm.h"
#include "Win32/SMBIOSUtil.h"

SSWR::AVIRead::AVIRSMBIOSForm::AVIRSMBIOSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("SMBIOS"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	Text::StringBuilderUTF8 sb;
	Win32::SMBIOS *smbios = Win32::SMBIOSUtil::GetSMBIOS();
	if (smbios)
	{
		smbios->ToString(&sb);
		DEL_CLASS(smbios);
	}
	NEW_CLASS(this->txtSMBIOS, UI::GUITextBox(ui, this, sb.ToCString(), true));
	this->txtSMBIOS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSMBIOS->SetReadOnly(true);
}

SSWR::AVIRead::AVIRSMBIOSForm::~AVIRSMBIOSForm()
{
}

void SSWR::AVIRead::AVIRSMBIOSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
