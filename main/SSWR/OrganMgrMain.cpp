#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "Manage/Process.h"
#include "SSWR/OrganMgr/OrganEnvDB.h"
#include "SSWR/OrganMgr/OrganMainForm.h"
#include "SSWR/OrganMgr/OrganSelCategoryForm.h"
#include "UI/GUICore.h"
#include "UI/MessageDialog.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	MemSetLogFile(UTF8STRC("Memory.log"));

	Manage::ExceptionRecorder exHdlr(CSTR("Error.log"), Manage::ExceptionRecorder::EA_RESTART);
	if (Manage::Process::IsAlreadyStarted())
	{
	}
	else
	{
		UI::GUICore *ui;
		ui = progCtrl->CreateGUICore(progCtrl);
		SSWR::OrganMgr::OrganEnvDB env;

		if (env.GetErrorType() == SSWR::OrganMgr::OrganEnv::ERR_NONE)
		{
			UI::GUIForm::DialogResult dr;
			{
				SSWR::OrganMgr::OrganSelCategoryForm cateFrm(0, ui, &env);
				dr = cateFrm.ShowDialog(0);
			}

			if (dr == UI::GUIForm::DR_OK)
			{
				SSWR::OrganMgr::OrganMainForm *frm;
				NEW_CLASS(frm, SSWR::OrganMgr::OrganMainForm(ui, 0, &env));
				frm->SetExitOnClose(true);
				frm->Show();
				ui->Run();
			}
		}
		else if (env.GetErrorType() == SSWR::OrganMgr::OrganEnv::ERR_CONFIG)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please prepare the config file"), CSTR("Error"), 0);
		}
		else if (env.GetErrorType() == SSWR::OrganMgr::OrganEnv::ERR_DB)
		{
			UI::MessageDialog::ShowDialog(CSTR("Cannot connect to the database"), CSTR("Error"), 0);
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("Unknown error occurs"), CSTR("Error"), 0);
		}
		DEL_CLASS(ui);
	}
	return 0;
}