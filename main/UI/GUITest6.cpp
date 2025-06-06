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

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<UI::GUICore> core;
	if (progCtrl->CreateGUICore(progCtrl).SetTo(core))
	{
		NN<UI::GUITreeView::TreeItem> item1;
		NN<UI::GUITreeView::TreeItem> item2;
		NN<UI::GUIForm> frm;
		NEW_CLASSNN(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("Test 6 - TreeView"));
		NEW_CLASS(tv, UI::GUITreeView(core, frm));
		tv->SetDockType(UI::GUIControl::DOCK_FILL);
		tv->SetHasLines(true);
		tv->SetHasCheckBox(true);
		if (tv->InsertItem(0, 0, CSTR("Root Item"), 0).SetTo(item1))
		{
			if (tv->InsertItem(item1, 0, CSTR("Item 1"), 0).SetTo(item2))
			{
				tv->InsertItem(item2, 0, CSTR("Item 1-1"), 0);
				tv->ExpandItem(item2);
			}
			tv->ExpandItem(item1);
			if (tv->InsertItem(item1, 0, CSTR("Item 2"), 0).SetTo(item2))
			{
				tv->InsertItem(item2, 0, CSTR("Item 2-1"), 0);
				tv->InsertItem(item2, 0, CSTR("Item 2-2"), 0);
				tv->ExpandItem(item2);
			}
		}
		frm->SetExitOnClose(true);
		frm->Show();
		core->Run();
		core.Delete();
	}
	return 0;
}
