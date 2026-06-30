#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRBenchmarkForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<UI::GUICore> ui;
	NN<SSWR::AVIRead::AVIRBenchmarkForm> frm;
	NN<SSWR::AVIRead::AVIRCore> core;
	NN<Manage::ExceptionRecorder> exHdlr;

	MemSetLogFile(UTF8STRCPTR("Memory.log"));
	NEW_CLASSNN(exHdlr, Manage::ExceptionRecorder(CSTR("SBenchmark.log"), Manage::ExceptionRecorder::EA_CLOSE));
	if (Core::ProgControl::CreateGUICore(progCtrl).SetTo(ui))
	{
		NEW_CLASSNN(core, SSWR::AVIRead::AVIRCoreWin(ui));
		NEW_CLASSNN(frm, SSWR::AVIRead::AVIRBenchmarkForm(nullptr, ui, core));
		frm->SetExitOnClose(true);
		frm->Show();
		ui->Run();

		core.Delete();
		ui.Delete();
	}
	exHdlr.Delete();
	return 0;
}