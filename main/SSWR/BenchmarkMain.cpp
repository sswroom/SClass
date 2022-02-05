#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRBenchmarkForm.h"
#include "UI/GUICore.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UI::GUICore *ui;
	SSWR::AVIRead::AVIRBenchmarkForm *frm;
	SSWR::AVIRead::AVIRCore *core;
	Manage::ExceptionRecorder *exHdlr;

	MemSetLogFile(UTF8STRC("Memory.log"));
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("SBenchmark.log"), Manage::ExceptionRecorder::EA_CLOSE));
	ui = Core::IProgControl::CreateGUICore(progCtrl);
	NEW_CLASS(core, SSWR::AVIRead::AVIRCoreWin(ui));
	NEW_CLASS(frm, SSWR::AVIRead::AVIRBenchmarkForm(0, ui, core));
	frm->SetExitOnClose(true);
	frm->Show();
	ui->Run();

	DEL_CLASS(core);
	DEL_CLASS(ui);
	DEL_CLASS(exHdlr);
	return 0;
}