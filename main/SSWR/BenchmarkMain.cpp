#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRBenchmarkForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	NN<UI::GUICore> ui;
	SSWR::AVIRead::AVIRBenchmarkForm *frm;
	NN<SSWR::AVIRead::AVIRCore> core;
	Manage::ExceptionRecorder *exHdlr;

	MemSetLogFile(UTF8STRC("Memory.log"));
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("SBenchmark.log"), Manage::ExceptionRecorder::EA_CLOSE));
	if (Core::IProgControl::CreateGUICore(progCtrl).SetTo(ui))
	{
		NEW_CLASSNN(core, SSWR::AVIRead::AVIRCoreWin(ui));
		NEW_CLASS(frm, SSWR::AVIRead::AVIRBenchmarkForm(0, ui, core));
		frm->SetExitOnClose(true);
		frm->Show();
		ui->Run();

		core.Delete();
		ui.Delete();
	}
	DEL_CLASS(exHdlr);
	return 0;
}