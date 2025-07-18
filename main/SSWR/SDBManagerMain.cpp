#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRDBManagerForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<UI::GUICore> ui;
	SSWR::AVIRead::AVIRDBManagerForm *frm;
	NN<SSWR::AVIRead::AVIRCore> core;
	Manage::ExceptionRecorder *exHdlr;

	MemSetLogFile(UTF8STRCPTR("Memory.log"));
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("SDBManager.log"), Manage::ExceptionRecorder::EA_RESTART));
	if (Core::ProgControl::CreateGUICore(progCtrl).SetTo(ui))
	{
		NEW_CLASSNN(core, SSWR::AVIRead::AVIRCoreWin(ui));
		NEW_CLASS(frm, SSWR::AVIRead::AVIRDBManagerForm(0, ui, core));
		frm->SetExitOnClose(true);
		frm->Show();
		ui->Run();

		core.Delete();
		ui.Delete();
	}
	DEL_CLASS(exHdlr);
	return 0;
}
