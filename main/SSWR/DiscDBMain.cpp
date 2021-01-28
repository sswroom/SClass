#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/DiscDB/DiscDBEnv.h"
#include "SSWR/DiscDB/DiscDBMainForm.h"
#include "UI/GUICore.h"
#include "UI/MessageDialog.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	UI::GUICore *ui;
	SSWR::DiscDB::DiscDBEnv *env;

	MemSetLogFile((const UTF8Char*)"Memory.log");

	NEW_CLASS(exHdlr, Manage::ExceptionRecorder((const UTF8Char*)"Error.log", Manage::ExceptionRecorder::EA_RESTART));
	ui = progCtrl->CreateGUICore(progCtrl);
	NEW_CLASS(env, SSWR::DiscDB::DiscDBEnv());

	if (env->GetErrorType() == SSWR::DiscDB::DiscDBEnv::ERR_NONE)
	{
		SSWR::DiscDB::DiscDBMainForm *frm;
		NEW_CLASS(frm, SSWR::DiscDB::DiscDBMainForm(ui, 0, env));
		frm->SetExitOnClose(true);
		frm->Show();
		ui->Run();
	}
	else if (env->GetErrorType() == SSWR::DiscDB::DiscDBEnv::ERR_CONFIG)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please prepare the config file", (const UTF8Char*)"Error", 0);
	}
	else if (env->GetErrorType() == SSWR::DiscDB::DiscDBEnv::ERR_DB)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Cannot connect to the database", (const UTF8Char*)"Error", 0);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Unknown error occurs", (const UTF8Char*)"Error", 0);
	}
	DEL_CLASS(env);
	DEL_CLASS(ui);
	DEL_CLASS(exHdlr);

	return 0;
}