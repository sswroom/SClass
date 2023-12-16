#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"

UI::GUIListBox *lb;

void __stdcall OnLBDblClick(void *userObj)
{
	UI::GUIForm *me = (UI::GUIForm*)userObj;
	NotNullPtr<Text::String> s;
	if (lb->GetSelectedItemTextNew().SetTo(s))
	{
		me->GetUI()->ShowMsgOK(s->ToCString(), CSTR("GUI Test 5 LB DblClick"), me);
		s->Release();
	}
	else
	{
		me->GetUI()->ShowMsgOK(CSTR("Error in getting the item value"), CSTR("GUI Test 5 LB DblClick"), me);
	}
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	NotNullPtr<UI::GUICore> core;
	if (core.Set(progCtrl->CreateGUICore(progCtrl)))
	{
		NotNullPtr<UI::GUIForm> frm;
		NEW_CLASSNN(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("Test 5 - ListBox"));
		NEW_CLASS(lb, UI::GUIListBox(core, frm, false));
		lb->SetDockType(UI::GUIControl::DOCK_FILL);
		lb->AddItem(CSTR("Item 1"), 0);
		lb->AddItem(CSTR("Item 2"), 0);
		lb->AddItem(CSTR("Item 3"), 0);
		lb->HandleDoubleClicked(OnLBDblClick, frm.Ptr());
		frm->SetExitOnClose(true);
		frm->Show();
		core->Run();
		core.Delete();
	}
	return 0;
}
