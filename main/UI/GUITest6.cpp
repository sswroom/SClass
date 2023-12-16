#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"
#include "UI/GUITreeView.h"

UI::GUITreeView *tv;

void __stdcall OnTVDblClick(void *userObj)
{
	UI::GUIForm *me = (UI::GUIForm*)userObj;
	me->GetUI()->ShowMsgOK(CSTR("Test"), CSTR("GUI Test 6 TV DblClick"), me);
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	NotNullPtr<UI::GUICore> core;
	if (core.Set(progCtrl->CreateGUICore(progCtrl)))
	{
		UI::GUITreeView::TreeItem *item1;
		UI::GUITreeView::TreeItem *item2;
		NotNullPtr<UI::GUIForm> frm;
		NEW_CLASSNN(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("Test 6 - TreeView"));
		NEW_CLASS(tv, UI::GUITreeView(core, frm));
		tv->SetDockType(UI::GUIControl::DOCK_FILL);
		tv->SetHasLines(true);
		tv->SetHasCheckBox(true);
		item1 = tv->InsertItem(0, 0, CSTR("Root Item"), 0);
		item2 = tv->InsertItem(item1, 0, CSTR("Item 1"), 0);
		tv->InsertItem(item2, 0, CSTR("Item 1-1"), 0);
		tv->ExpandItem(item1);
		tv->ExpandItem(item2);
		item2 = tv->InsertItem(item1, 0, CSTR("Item 2"), 0);
		tv->InsertItem(item2, 0, CSTR("Item 2-1"), 0);
		tv->InsertItem(item2, 0, CSTR("Item 2-2"), 0);
		tv->ExpandItem(item2);
		frm->SetExitOnClose(true);
		frm->Show();
		core->Run();
		core.Delete();
	}
	return 0;
}
