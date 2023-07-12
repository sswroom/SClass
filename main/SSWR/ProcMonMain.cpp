#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "SSWR/ProcMonForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UI::GUICore *ui;
	SSWR::ProcMonForm *frm;
	ui = progCtrl->CreateGUICore(progCtrl);
	if (ui)
	{
		NEW_CLASS(frm, SSWR::ProcMonForm(0, ui));
		frm->SetExitOnClose(true);
		frm->Show();
		ui->Run();
		DEL_CLASS(ui);
	}
	return 0;
}
