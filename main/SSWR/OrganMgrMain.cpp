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
	Manage::ExceptionRecorder *exHdlr;
	UI::GUICore *ui;
	SSWR::OrganMgr::OrganMainForm *frm;
	SSWR::OrganMgr::OrganSelCategoryForm *cateFrm;
	SSWR::OrganMgr::OrganEnv *env;

	MemSetLogFile(UTF8STRC("Memory.log"));

	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("Error.log"), Manage::ExceptionRecorder::EA_RESTART));
	if (Manage::Process::IsAlreadyStarted())
	{
	}
	else
	{
		ui = progCtrl->CreateGUICore(progCtrl);
		NEW_CLASS(env, SSWR::OrganMgr::OrganEnvDB());

		if (env->GetErrorType() == SSWR::OrganMgr::OrganEnv::ERR_NONE)
		{
			NEW_CLASS(cateFrm, SSWR::OrganMgr::OrganSelCategoryForm(0, ui, env));
			UI::GUIForm::DialogResult dr = cateFrm->ShowDialog(0);
			DEL_CLASS(cateFrm);

			if (dr == UI::GUIForm::DR_OK)
			{
				NEW_CLASS(frm, SSWR::OrganMgr::OrganMainForm(ui, 0, env));
				frm->SetExitOnClose(true);
				frm->Show();
				ui->Run();
			}
		}
		else if (env->GetErrorType() == SSWR::OrganMgr::OrganEnv::ERR_CONFIG)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please prepare the config file", (const UTF8Char*)"Error", 0);
		}
		else if (env->GetErrorType() == SSWR::OrganMgr::OrganEnv::ERR_DB)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Cannot connect to the database", (const UTF8Char*)"Error", 0);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Unknown error occurs", (const UTF8Char*)"Error", 0);
		}
		DEL_CLASS(env);
		DEL_CLASS(ui);
	}
	DEL_CLASS(exHdlr);

	return 0;
}