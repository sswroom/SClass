#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "Manage/Process.h"
#include "SSWR/OrganMgr/OrganEnvDB.h"
#include "SSWR/OrganMgr/OrganMainForm.h"
#include "SSWR/OrganMgr/OrganSelCategoryForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	MemSetLogFile(UTF8STRCPTR("Memory.log"));

	Manage::ExceptionRecorder exHdlr(CSTR("Error.log"), Manage::ExceptionRecorder::EA_RESTART);
	if (Manage::Process::IsAlreadyStarted())
	{
	}
	else
	{
		NN<UI::GUICore> ui;
		if (progCtrl->CreateGUICore(progCtrl).SetTo(ui))
		{
			SSWR::OrganMgr::OrganEnvDB env;

			if (env.GetErrorType() == SSWR::OrganMgr::OrganEnv::ERR_NONE)
			{
				UI::GUIForm::DialogResult dr;
				{
					SSWR::OrganMgr::OrganSelCategoryForm cateFrm(nullptr, ui, env);
					dr = cateFrm.ShowDialog(nullptr);
				}

				if (dr == UI::GUIForm::DR_OK)
				{
					SSWR::OrganMgr::OrganMainForm *frm;
					NEW_CLASS(frm, SSWR::OrganMgr::OrganMainForm(ui, nullptr, env));
					frm->SetExitOnClose(true);
					frm->Show();
					ui->Run();
				}
			}
			else if (env.GetErrorType() == SSWR::OrganMgr::OrganEnv::ERR_CONFIG)
			{
				ui->ShowMsgOK(CSTR("Please prepare the config file"), CSTR("Error"), nullptr);
			}
			else if (env.GetErrorType() == SSWR::OrganMgr::OrganEnv::ERR_DB)
			{
				ui->ShowMsgOK(CSTR("Cannot connect to the database"), CSTR("Error"), nullptr);
			}
			else
			{
				ui->ShowMsgOK(CSTR("Unknown error occurs"), CSTR("Error"), nullptr);
			}
			ui.Delete();
		}
	}
	return 0;
}