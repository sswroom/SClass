#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRHQMPForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	UI::GUICore *ui;
	SSWR::AVIRead::AVIRHQMPForm *frm;
	NN<SSWR::AVIRead::AVIRCore> core;

//	MemSetBreakPoint(0x01088d78);
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(L"Error.log", Manage::ExceptionRecorder::EA_CLOSE));
	ui = progCtrl->CreateGUICore(progCtrl);
	NEW_CLASS(core, SSWR::AVIRead::AVIRCoreWin(ui));
	NEW_CLASS(frm, SSWR::AVIRead::AVIRHQMPForm(0, ui, core, SSWR::AVIRead::AVIRHQMPForm::QM_LQ));
	frm->SetExitOnClose(true);

	Int32 argc;
	WChar **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	while (argc-- > 1)
	{
		if (frm->OpenFile(argv[argc]))
		{
			break;
		}
	}

	frm->Show();
	ui->Run();
	DEL_CLASS(core);
	DEL_CLASS(ui);
	DEL_CLASS(exHdlr);
	return 0;
}