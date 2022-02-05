#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "SSWR/AVIRead/AVIRSNBDongleForm.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UI::GUICore *ui;
	SSWR::AVIRead::AVIRCore *core;
	Manage::ExceptionRecorder *exHdlr;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

//	MemSetBreakPoint(0x014746E8);
	MemSetLogFile(UTF8STRC("Memory.log"));
	IO::Path::GetProcessFileName(sbuff);
#ifdef _WIN64
	sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"SNBControl64.log");
#else
	sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"SNBControl.log");
#endif
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder({sbuff, (UOSInt)(sptr - sbuff)}, Manage::ExceptionRecorder::EA_CLOSE));
	ui = progCtrl->CreateGUICore(progCtrl);
	NEW_CLASS(core, SSWR::AVIRead::AVIRCoreWin(ui));
	SSWR::AVIRead::AVIRSNBDongleForm *snbFrm = 0;
	SSWR::AVIRead::AVIRSelStreamForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRSelStreamForm(0, ui, core, false));
	frm->SetText((const UTF8Char*)"Select SNB Dongle");
	if (frm->ShowDialog(0) == UI::GUIForm::DR_OK)
	{
		NEW_CLASS(snbFrm, SSWR::AVIRead::AVIRSNBDongleForm(0, ui, core, frm->stm));
		snbFrm->SetExitOnClose(true);
		snbFrm->Show();
	}
	DEL_CLASS(frm);
	if (snbFrm)
	{
		ui->Run();
	}

	DEL_CLASS(core);
	DEL_CLASS(ui);
	DEL_CLASS(exHdlr);
	return 0;
}