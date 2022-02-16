#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/MessageDialog.h"

UI::GUIListBox *lb;

void __stdcall OnLBDblClick(void *userObj)
{
	UI::GUIForm *me = (UI::GUIForm*)userObj;
	Text::String *s = lb->GetSelectedItemTextNew();
	UI::MessageDialog::ShowDialog(s->v, (const UTF8Char*)"GUI Test 5 LB DblClick", me);
	s->Release();
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UI::GUICore *core = progCtrl->CreateGUICore(progCtrl);
	if (core)
	{
		UI::GUIForm *frm;
		NEW_CLASS(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("GUI Test 5"));
		NEW_CLASS(lb, UI::GUIListBox(core, frm, false));
		lb->SetDockType(UI::GUIControl::DOCK_FILL);
		lb->AddItem(CSTR("Item 1"), 0);
		lb->AddItem(CSTR("Item 2"), 0);
		lb->AddItem(CSTR("Item 3"), 0);
		lb->HandleDoubleClicked(OnLBDblClick, frm);
		frm->SetExitOnClose(true);
		frm->Show();
		core->Run();
		DEL_CLASS(core);
	}
	return 0;
}
