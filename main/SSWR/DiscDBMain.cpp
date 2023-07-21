#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/DiscDB/DiscDBEnv.h"
#include "SSWR/DiscDB/DiscDBMainForm.h"
#include "UI/GUICore.h"
#include "UI/MessageDialog.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	NotNullPtr<UI::GUICore> ui;

	MemSetLogFile(UTF8STRC("Memory.log"));

	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("Error.log"), Manage::ExceptionRecorder::EA_RESTART));
	if (ui.Set(progCtrl->CreateGUICore(progCtrl)))
	{
		SSWR::DiscDB::DiscDBEnv env;

		if (env.GetErrorType() == SSWR::DiscDB::DiscDBEnv::ERR_NONE)
		{
			SSWR::DiscDB::DiscDBMainForm *frm;
			NEW_CLASS(frm, SSWR::DiscDB::DiscDBMainForm(ui, 0, &env));
			frm->SetExitOnClose(true);
			frm->Show();
			ui->Run();
		}
		else if (env.GetErrorType() == SSWR::DiscDB::DiscDBEnv::ERR_CONFIG)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please prepare the config file"), CSTR("Error"), 0);
		}
		else if (env.GetErrorType() == SSWR::DiscDB::DiscDBEnv::ERR_DB)
		{
			UI::MessageDialog::ShowDialog(CSTR("Cannot connect to the database"), CSTR("Error"), 0);
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("Unknown error occurs"), CSTR("Error"), 0);
		}
		ui.Delete();
	}
	DEL_CLASS(exHdlr);

	return 0;
}