#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/StmData/FileData.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRImageViewerForm.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Manage::ExceptionRecorder exHdlr(CSTR("Error.txt"), Manage::ExceptionRecorder::EA_CLOSE);
	SSWR::AVIRead::AVIRImageViewerForm *frm;
	SSWR::AVIRead::AVIRCore *core;
	UI::GUICore *ui;
	UTF8Char **argv;
	IO::StmData::FileData *fd;
	Bool succ;
	UOSInt argc;
	UInt32 i;
	ui = progCtrl->CreateGUICore(progCtrl);
	NEW_CLASS(core, SSWR::AVIRead::AVIRCoreWin(ui));
	NEW_CLASS(frm, SSWR::AVIRead::AVIRImageViewerForm(0, ui, core));
	frm->SetExitOnClose(true);
	frm->Show();
	argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc > 1)
	{
		i = 1;
		while (i < argc)
		{
			NEW_CLASS(fd, IO::StmData::FileData({argv[i], Text::StrCharCnt(argv[i])}, false));
			succ = frm->ParseFile(fd);
			DEL_CLASS(fd);
			if (succ)
			{
				break;
			}
			i++;
		}
	}
	ui->Run();
	DEL_CLASS(core);
	DEL_CLASS(ui);
	return 0;
}