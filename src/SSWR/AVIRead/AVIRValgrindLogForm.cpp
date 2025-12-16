#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRValgrindLogForm.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRValgrindLogForm::OnFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRValgrindLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRValgrindLogForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"ValgrindLog", false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg->GetFileName());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRValgrindLogForm::FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRValgrindLogForm> me = userObj.GetNN<SSWR::AVIRead::AVIRValgrindLogForm>();
	UOSInt i = 0;
	UOSInt j = files.GetCount();
	while (i < j)
	{
		if (me->LoadFile(files[i]))
		{
			break;
		}
		i++;
	}
}

Bool SSWR::AVIRead::AVIRValgrindLogForm::LoadFile(NN<Text::String> file)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<IO::ValgrindLog> log;
	if (IO::ValgrindLog::LoadFile(file->ToCString()).SetTo(log))
	{
		this->log.Delete();
		this->log = log;
		sptr = Text::StrUOSInt(sbuff, log->GetPPID());
		this->txtPPID->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUOSInt(sbuff, log->GetMainPID());
		this->txtPID->SetText(CSTRP(sbuff, sptr));
		this->txtVersion->SetText(Text::String::OrEmpty(log->GetVersion())->ToCString());
		this->txtCommandLine->SetText(Text::String::OrEmpty(log->GetCommandLine())->ToCString());
		sptr = Text::StrUInt32(sbuff, log->GetBlocksInUse());
		this->txtBlocksInUse->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt64(sbuff, log->GetBytesInUse());
		this->txtBytesInUse->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, log->GetBlocksAllocs());
		this->txtBlocksAllocs->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, log->GetBlocksFrees());
		this->txtBlocksFrees->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt64(sbuff, log->GetBytesAllocs());
		this->txtBytesAllocs->SetText(CSTRP(sbuff, sptr));
		return true;
	}
	return false;
}

SSWR::AVIRead::AVIRValgrindLogForm::AVIRValgrindLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Valgrind Log"));
	
	this->core = core;
	this->log = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlFile = ui->NewPanel(*this);
	this->pnlFile->SetRect(0, 0, 100, 31, false);
	this->pnlFile->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFile = ui->NewLabel(this->pnlFile, CSTR("Version"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	this->txtFile = ui->NewTextBox(this->pnlFile, CSTR(""));
	this->txtFile->SetReadOnly(true);
	this->txtFile->SetRect(104, 4, 400, 23, false);
	this->btnFile = ui->NewButton(this->pnlFile, CSTR("Browse"));
	this->btnFile->SetRect(504, 0, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->lblPPID = ui->NewLabel(this->tpInfo, CSTR("PPID"));
	this->lblPPID->SetRect(0, 0, 100, 23, false);
	this->txtPPID = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtPPID->SetRect(100, 0, 100, 23, false);
	this->txtPPID->SetReadOnly(true);
	this->lblPID = ui->NewLabel(this->tpInfo, CSTR("PID"));
	this->lblPID->SetRect(0, 24, 100, 23, false);
	this->txtPID = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtPID->SetRect(100, 24, 100, 23, false);
	this->txtPID->SetReadOnly(true);
	this->lblVersion = ui->NewLabel(this->tpInfo, CSTR("Version"));
	this->lblVersion->SetRect(0, 48, 100, 23, false);
	this->txtVersion = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtVersion->SetRect(100, 48, 100, 23, false);
	this->txtVersion->SetReadOnly(true);
	this->lblCommandLine = ui->NewLabel(this->tpInfo, CSTR("Command Line"));
	this->lblCommandLine->SetRect(0, 72, 100, 23, false);
	this->txtCommandLine = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtCommandLine->SetRect(100, 72, 400, 23, false);
	this->txtCommandLine->SetReadOnly(true);
	this->lblBlocksInUse = ui->NewLabel(this->tpInfo, CSTR("Blocks In Use"));
	this->lblBlocksInUse->SetRect(0, 96, 100, 23, false);
	this->txtBlocksInUse = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBlocksInUse->SetRect(100, 96, 100, 23, false);
	this->txtBlocksInUse->SetReadOnly(true);
	this->lblBytesInUse = ui->NewLabel(this->tpInfo, CSTR("Bytes In Use"));
	this->lblBytesInUse->SetRect(0, 120, 100, 23, false);
	this->txtBytesInUse = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBytesInUse->SetRect(100, 120, 100, 23, false);
	this->txtBytesInUse->SetReadOnly(true);
	this->lblBlocksAllocs = ui->NewLabel(this->tpInfo, CSTR("Blocks Allocs"));
	this->lblBlocksAllocs->SetRect(0, 144, 100, 23, false);
	this->txtBlocksAllocs = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBlocksAllocs->SetRect(100, 144, 100, 23, false);
	this->txtBlocksAllocs->SetReadOnly(true);
	this->lblBlocksFrees = ui->NewLabel(this->tpInfo, CSTR("Blocks Frees"));
	this->lblBlocksFrees->SetRect(0, 168, 100, 23, false);
	this->txtBlocksFrees = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBlocksFrees->SetRect(100, 168, 100, 23, false);
	this->txtBlocksFrees->SetReadOnly(true);
	this->lblBytesAllocs = ui->NewLabel(this->tpInfo, CSTR("Bytes Allocs"));
	this->lblBytesAllocs->SetRect(0, 192, 100, 23, false);
	this->txtBytesAllocs = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtBytesAllocs->SetRect(100, 192, 100, 23, false);
	this->txtBytesAllocs->SetReadOnly(true);

	this->HandleDropFiles(FileHandler, this);
}

SSWR::AVIRead::AVIRValgrindLogForm::~AVIRValgrindLogForm()
{
	this->log.Delete();
}

void SSWR::AVIRead::AVIRValgrindLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
