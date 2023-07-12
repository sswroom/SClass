#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"
#include "UI/GUITreeView.h"
#include "UI/MessageDialog.h"

UI::GUITreeView *tv;

void __stdcall OnTVDblClick(void *userObj)
{
	UI::GUIForm *me = (UI::GUIForm*)userObj;
	UI::MessageDialog::ShowDialog(CSTR("Test"), CSTR("GUI Test 6 TV DblClick"), me);
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UI::GUICore *core = progCtrl->CreateGUICore(progCtrl);
	if (core)
	{
		UI::GUITreeView::TreeItem *item1;
		UI::GUITreeView::TreeItem *item2;
		UI::GUIForm *frm;
		NEW_CLASS(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("GUI Test 6"));
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
		DEL_CLASS(core);
	}
	return 0;
}
