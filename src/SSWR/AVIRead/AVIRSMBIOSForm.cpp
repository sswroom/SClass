#include "Stdafx.h"
#include "IO/SMBIOSUtil.h"
#include "IO/FileAnalyse/SMBIOSFileAnalyse.h"
#include "SSWR/AVIRead/AVIRSMBIOSForm.h"

void __stdcall SSWR::AVIRead::AVIRSMBIOSForm::OnHexClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSMBIOSForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSMBIOSForm>();
	NN<IO::SMBIOS> smbios;
	if (me->smbios.SetTo(smbios))
	{
		NN<IO::StreamData> fd = smbios->CreateStreamData();
		IO::FileAnalyse::SMBIOSFileAnalyse *fileAnalyse;
		NEW_CLASS(fileAnalyse, IO::FileAnalyse::SMBIOSFileAnalyse(fd));
		me->core->OpenHex(fd, fileAnalyse);
		fd.Delete();
	}
}

SSWR::AVIRead::AVIRSMBIOSForm::AVIRSMBIOSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("SMBIOS"));

	this->core = core;
	this->smbios = IO::SMBIOSUtil::GetSMBIOS();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	Text::StringBuilderUTF8 sb;
	NN<IO::SMBIOS> smbios;
	if (this->smbios.SetTo(smbios))
	{
		smbios->ToString(sb);
	}
	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnHex = ui->NewButton(this->pnlControl, CSTR("Hex"));
	this->btnHex->SetRect(4, 4, 75, 23, false);
	this->btnHex->SetEnabled(this->smbios.NotNull());
	this->btnHex->HandleButtonClick(OnHexClicked, this);
	this->txtSMBIOS = ui->NewTextBox(*this, sb.ToCString(), true);
	this->txtSMBIOS->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSMBIOS->SetReadOnly(true);
}

SSWR::AVIRead::AVIRSMBIOSForm::~AVIRSMBIOSForm()
{
	this->smbios.Delete();
}

void SSWR::AVIRead::AVIRSMBIOSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
