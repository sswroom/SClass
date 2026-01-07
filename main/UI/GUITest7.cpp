#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUIButton.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<UI::GUICore> core;
	if (progCtrl->CreateGUICore(progCtrl).SetTo(core))
	{
		NN<UI::GUIButton> btn;
		NN<UI::GUIForm> frm;
		NN<UI::GUILabel> lbl;
		NEW_CLASSNN(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("Test 7 - Font and Color"));
		btn = core->NewButton(frm, CSTR("Button"));
		btn->SetRect(4, 4, 75, 23, false);
		btn->SetFont(CSTR("Arial"), 5, true);
		lbl = core->NewLabel(frm, CSTR("Big Text"));
		lbl->SetRect(4, 32, 100, 23, false);
		frm->SetFont(CSTR("Arial"), 32, false);
		frm->SetBGColor(0xff80ffc0);
		frm->SetExitOnClose(true);
		frm->Show();
		core->Run();
		core.Delete();
	}
	return 0;
}
