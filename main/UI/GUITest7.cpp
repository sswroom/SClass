#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUIButton.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	NotNullPtr<UI::GUICore> core;
	if (core.Set(progCtrl->CreateGUICore(progCtrl)))
	{
		UI::GUIButton *btn;
		NotNullPtr<UI::GUIForm> frm;
		UI::GUILabel *lbl;
		NEW_CLASSNN(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("Test 7 - Font and Color"));
		NEW_CLASS(btn, UI::GUIButton(core, frm, CSTR("Button")));
		btn->SetRect(4, 4, 75, 23, false);
		btn->SetFont(UTF8STRC("Arial"), 5, true);
		NEW_CLASS(lbl, UI::GUILabel(core, frm, CSTR("Big Text")));
		lbl->SetRect(4, 32, 100, 23, false);
		frm->SetFont(UTF8STRC("Arial"), 32, false);
		frm->SetBGColor(0xff80ffc0);
		frm->SetExitOnClose(true);
		frm->Show();
		core->Run();
		core.Delete();
	}
	return 0;
}
