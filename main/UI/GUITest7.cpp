#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUIButton.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UI::GUICore *core = progCtrl->CreateGUICore(progCtrl);
	if (core)
	{
		UI::GUIButton *btn;
		UI::GUIForm *frm;
		UI::GUILabel *lbl;
		NEW_CLASS(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("GUI Test 7"));
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
		DEL_CLASS(core);
	}
	return 0;
}
