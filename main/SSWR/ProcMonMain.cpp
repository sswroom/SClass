#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "SSWR/ProcMonForm.h"
#include "UI/GUICore.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	NotNullPtr<UI::GUICore> ui;
	SSWR::ProcMonForm *frm;
	if (ui.Set(progCtrl->CreateGUICore(progCtrl)))
	{
		NEW_CLASS(frm, SSWR::ProcMonForm(0, ui));
		frm->SetExitOnClose(true);
		frm->Show();
		ui->Run();
		ui.Delete();
	}
	return 0;
}
