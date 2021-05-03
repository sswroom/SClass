#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"
#include "UI/GUIMainMenu.h"
#include "UI/MessageDialog.h"

typedef enum
{
	MNU_EXIT = 100,
	MNU_ITEM1,
	MNU_ITEM2,
	MNU_ITEM3
} MenuItems;

void __stdcall OnMenuEvent(void *userObj, UInt16 cmdId)
{
	UI::GUIForm *me = (UI::GUIForm*)userObj;
	switch (cmdId)
	{
	case MNU_EXIT:
		me->Close();
		break;
	case MNU_ITEM1:
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Item 1 Clicked", (const UTF8Char*)"GUITest4", me);
		break;
	case MNU_ITEM2:
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Item 2 Clicked", (const UTF8Char*)"GUITest4", me);
		break;
	case MNU_ITEM3:
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Item 3 Clicked", (const UTF8Char*)"GUITest4", me);
		break;
	}
}

void __stdcall OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	UI::GUIForm *me = (UI::GUIForm*)userObj;
	OSInt i;
	Text::StringBuilderUTF8 sb;
	i = 0;
	while (i < nFiles)
	{
		if (i > 0)
			sb.Append((const UTF8Char*)"\r\n");
		sb.Append(files[i]);
		i++;
	}
	UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"Drop Files", me);
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UI::GUICore *core = progCtrl->CreateGUICore(progCtrl);
	if (core)
	{
		UI::GUIMainMenu *mainMenu;
		UI::GUIForm *frm;
		NEW_CLASS(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText((const UTF8Char*)"GUI Test 4");
		NEW_CLASS(mainMenu, UI::GUIMainMenu());
		UI::GUIMenu *mnu = mainMenu->AddSubMenu((const UTF8Char*)"&File");
		mnu->AddItem((const UTF8Char*)"E&xit", MNU_EXIT, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_X);
		mnu = mainMenu->AddSubMenu((const UTF8Char*)"&Item");
		mnu->AddItem((const UTF8Char*)"Item &1", MNU_ITEM1, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_1);
		mnu->AddItem((const UTF8Char*)"Item &2", MNU_ITEM2, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_2);
		mnu->AddItem((const UTF8Char*)"Item &3", MNU_ITEM3, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_3);
		frm->SetMenu(mainMenu);
		frm->HandleMenuClicked(OnMenuEvent, frm);
		frm->HandleDropFiles(OnFileDrop, frm);
		frm->SetExitOnClose(true);
		frm->Show();
		core->Run();
		DEL_CLASS(core);
	}
	return 0;
}