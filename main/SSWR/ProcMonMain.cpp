#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "SSWR/ProcMonForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<UI::GUICore> ui;
	NN<SSWR::ProcMonForm> frm;
	if (progCtrl->CreateGUICore(progCtrl).SetTo(ui))
	{
		NEW_CLASSNN(frm, SSWR::ProcMonForm(nullptr, ui));
		frm->SetExitOnClose(true);
		frm->Show();
		ui->Run();
		ui.Delete();
	}
	return 0;
}
