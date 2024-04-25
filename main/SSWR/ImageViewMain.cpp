#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/StmData/FileData.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRImageViewerForm.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Manage::ExceptionRecorder exHdlr(CSTR("Error.txt"), Manage::ExceptionRecorder::EA_CLOSE);
	SSWR::AVIRead::AVIRImageViewerForm *frm;
	NN<SSWR::AVIRead::AVIRCore> core;
	NN<UI::GUICore> ui;
	UTF8Char **argv;
	Bool succ;
	UOSInt argc;
	UInt32 i;
	if (progCtrl->CreateGUICore(progCtrl).SetTo(ui))
	{
		NEW_CLASSNN(core, SSWR::AVIRead::AVIRCoreWin(ui));
		NEW_CLASS(frm, SSWR::AVIRead::AVIRImageViewerForm(0, ui, core));
		frm->SetExitOnClose(true);
		frm->Show();
		argv = progCtrl->GetCommandLines(progCtrl, argc);
		if (argc > 1)
		{
			i = 1;
			while (i < argc)
			{
				IO::StmData::FileData fd({argv[i], Text::StrCharCnt(argv[i])}, false);
				succ = frm->ParseFile(fd);
				if (succ)
				{
					break;
				}
				i++;
			}
		}
		ui->Run();
		core.Delete();
		ui.Delete();
	}
	return 0;
}