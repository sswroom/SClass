#include "Stdafx.h"
#include "IO/SMBIOSUtil.h"
#include "SSWR/AVIRead/AVIRSMBIOSForm.h"

void __stdcall SSWR::AVIRead::AVIRSMBIOSForm::OnHexClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSMBIOSForm *me = (SSWR::AVIRead::AVIRSMBIOSForm*)userObj;
	IO::IStreamData *fd = me->smbios->CreateStreamData();
	me->core->OpenHex(fd);
	DEL_CLASS(fd);
}

SSWR::AVIRead::AVIRSMBIOSForm::AVIRSMBIOSForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("SMBIOS"));

	this->core = core;
	this->smbios = IO::SMBIOSUtil::GetSMBIOS();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	Text::StringBuilderUTF8 sb;
	if (this->smbios)
	{
		this->smbios->ToString(&sb);
	}
	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnHex, UI::GUIButton(ui, this->pnlControl, CSTR("Hex")));
	this->btnHex->SetRect(4, 4, 75, 23, false);
	this->btnHex->SetEnabled(this->smbios != 0);
	this->btnHex->HandleButtonClick(OnHexClicked, this);
	NEW_CLASS(this->txtSMBIOS, UI::GUITextBox(ui, this, sb.ToCString(), true));
	this->txtSMBIOS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSMBIOS->SetReadOnly(true);
}

SSWR::AVIRead::AVIRSMBIOSForm::~AVIRSMBIOSForm()
{
	SDEL_CLASS(smbios);
}

void SSWR::AVIRead::AVIRSMBIOSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
