#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	NotNullPtr<UI::GUICore> core;
	if (core.Set(progCtrl->CreateGUICore(progCtrl)))
	{
		UI::GUIForm *frm;
		NEW_CLASS(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("GUI Test 2"));
		frm->SetExitOnClose(true);
		frm->Show();
		core->Run();
		core.Delete();
	}
	return 0;
}