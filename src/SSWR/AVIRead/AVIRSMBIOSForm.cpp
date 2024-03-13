#include "Stdafx.h"
#include "IO/SMBIOSUtil.h"
#include "IO/FileAnalyse/SMBIOSFileAnalyse.h"
#include "SSWR/AVIRead/AVIRSMBIOSForm.h"

void __stdcall SSWR::AVIRead::AVIRSMBIOSForm::OnHexClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSMBIOSForm *me = (SSWR::AVIRead::AVIRSMBIOSForm*)userObj;
	NotNullPtr<IO::StreamData> fd = me->smbios->CreateStreamData();
	IO::FileAnalyse::SMBIOSFileAnalyse *fileAnalyse;
	NEW_CLASS(fileAnalyse, IO::FileAnalyse::SMBIOSFileAnalyse(fd));
	me->core->OpenHex(fd, fileAnalyse);
	fd.Delete();
}

SSWR::AVIRead::AVIRSMBIOSForm::AVIRSMBIOSForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("SMBIOS"));

	this->core = core;
	this->smbios = IO::SMBIOSUtil::GetSMBIOS();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	Text::StringBuilderUTF8 sb;
	if (this->smbios)
	{
		this->smbios->ToString(sb);
	}
	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnHex = ui->NewButton(this->pnlControl, CSTR("Hex"));
	this->btnHex->SetRect(4, 4, 75, 23, false);
	this->btnHex->SetEnabled(this->smbios != 0);
	this->btnHex->HandleButtonClick(OnHexClicked, this);
	this->txtSMBIOS = ui->NewTextBox(*this, sb.ToCString(), true);
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
