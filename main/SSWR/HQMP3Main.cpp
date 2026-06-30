#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRHQMPForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Manage::ExceptionRecorder> exHdlr;
	NN<UI::GUICore> ui;
	NN<SSWR::AVIRead::AVIRHQMPForm> frm;
	NN<SSWR::AVIRead::AVIRCore> core;

//	MemSetBreakPoint(0x01088d78);
	NEW_CLASSNN(exHdlr, Manage::ExceptionRecorder(CSTR("Error.log"), Manage::ExceptionRecorder::EA_CLOSE));
	if (progCtrl->CreateGUICore(progCtrl).SetTo(ui))
	{
		NEW_CLASSNN(core, SSWR::AVIRead::AVIRCoreWin(ui));
		NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHQMPForm(nullptr, ui, core, SSWR::AVIRead::AVIRHQMPForm::QM_LQ));
		frm->SetExitOnClose(true);

		UIntOS argc;
		UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
		while (argc-- > 1)
		{
			if (frm->OpenFile(Text::CStringNN::FromPtr(argv[argc]), IO::ParserType::MediaFile))
			{
				break;
			}
		}

		frm->Show();
		ui->Run();
		core.Delete();
		ui.Delete();
	}
	exHdlr.Delete();
	return 0;
}