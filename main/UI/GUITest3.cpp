#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	NN<UI::GUICore> core;
	if (progCtrl->CreateGUICore(progCtrl).SetTo(core))
	{
		UI::GUIForm *frm;
		NEW_CLASS(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("Test 3 - Fix sized form"));
		frm->SetNoResize(true);
		frm->SetExitOnClose(true);
		frm->Show();
		core->Run();
		core.Delete();
	}
	return 0;
}