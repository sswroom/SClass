#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AVIRead/AVIRCoreWin.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "SSWR/AVIRead/AVIRSNBDongleForm.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	NotNullPtr<UI::GUICore> ui;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

//	MemSetBreakPoint(0x014746E8);
	MemSetLogFile(UTF8STRC("Memory.log"));
	sptr = IO::Path::GetProcessFileName(sbuff);
#ifdef _WIN64
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("SNBControl64.log"));
#else
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("SNBControl.log"));
#endif
	Manage::ExceptionRecorder exHdlr(CSTRP(sbuff, sptr), Manage::ExceptionRecorder::EA_CLOSE);
	if (progCtrl->CreateGUICore(progCtrl).SetTo(ui))
	{
		SSWR::AVIRead::AVIRCoreWin core(ui);
		SSWR::AVIRead::AVIRSNBDongleForm *snbFrm = 0;
		IO::LogTool log;
		{
			SSWR::AVIRead::AVIRSelStreamForm frm(0, ui, core, false, 0, log);
			frm.SetText(CSTR("Select SNB Dongle"));
			if (frm.ShowDialog(0) == UI::GUIForm::DR_OK)
			{
				NEW_CLASS(snbFrm, SSWR::AVIRead::AVIRSNBDongleForm(0, ui, core, frm.GetStream()));
				snbFrm->SetExitOnClose(true);
				snbFrm->Show();
			}
		}
		if (snbFrm)
		{
			ui->Run();
		}
		ui.Delete();
	}
	return 0;
}