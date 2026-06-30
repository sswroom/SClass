#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/DiscDB/DiscDBEnv.h"
#include "SSWR/DiscDB/DiscDBMainForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Manage::ExceptionRecorder> exHdlr;
	NN<UI::GUICore> ui;

	MemSetLogFile(UTF8STRCPTR("Memory.log"));

	NEW_CLASSNN(exHdlr, Manage::ExceptionRecorder(CSTR("Error.log"), Manage::ExceptionRecorder::EA_RESTART));
	if (progCtrl->CreateGUICore(progCtrl).SetTo(ui))
	{
		SSWR::DiscDB::DiscDBEnv env;

		if (env.GetErrorType() == SSWR::DiscDB::DiscDBEnv::ERR_NONE)
		{
			NN<SSWR::DiscDB::DiscDBMainForm> frm;
			NEW_CLASSNN(frm, SSWR::DiscDB::DiscDBMainForm(ui, nullptr, &env));
			frm->SetExitOnClose(true);
			frm->Show();
			ui->Run();
		}
		else if (env.GetErrorType() == SSWR::DiscDB::DiscDBEnv::ERR_CONFIG)
		{
			ui->ShowMsgOK(CSTR("Please prepare the config file"), CSTR("Error"), nullptr);
		}
		else if (env.GetErrorType() == SSWR::DiscDB::DiscDBEnv::ERR_DB)
		{
			ui->ShowMsgOK(CSTR("Cannot connect to the database"), CSTR("Error"), nullptr);
		}
		else
		{
			ui->ShowMsgOK(CSTR("Unknown error occurs"), CSTR("Error"), nullptr);
		}
		ui.Delete();
	}
	exHdlr.Delete();

	return 0;
}