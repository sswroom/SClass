#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUICore.h"
#include "UI/GUIForm.h"
#include "UI/GUIMainMenu.h"

typedef enum
{
	MNU_EXIT = 100,
	MNU_ITEM1,
	MNU_ITEM2,
	MNU_ITEM3
} MenuItems;

void __stdcall OnMenuEvent(AnyType userObj, UInt16 cmdId)
{
	NotNullPtr<UI::GUIForm> me = userObj.GetNN<UI::GUIForm>();
	switch (cmdId)
	{
	case MNU_EXIT:
		me->Close();
		break;
	case MNU_ITEM1:
		me->GetUI()->ShowMsgOK(CSTR("Item 1 Clicked"), CSTR("GUITest4"), me);
		break;
	case MNU_ITEM2:
		me->GetUI()->ShowMsgOK(CSTR("Item 2 Clicked"), CSTR("GUITest4"), me);
		break;
	case MNU_ITEM3:
		me->GetUI()->ShowMsgOK(CSTR("Item 3 Clicked"), CSTR("GUITest4"), me);
		break;
	}
}

void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files)
{
	NotNullPtr<UI::GUIForm> me = userObj.GetNN<UI::GUIForm>();
	UOSInt i;
	UOSInt nFiles = files.GetCount();
	Text::StringBuilderUTF8 sb;
	i = 0;
	while (i < nFiles)
	{
		if (i > 0)
			sb.AppendC(UTF8STRC("\r\n"));
		sb.Append(files[i]);
		i++;
	}
	me->GetUI()->ShowMsgOK(sb.ToCString(), CSTR("Drop Files"), me);
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	NotNullPtr<UI::GUICore> core;
	if (progCtrl->CreateGUICore(progCtrl).SetTo(core))
	{
		NotNullPtr<UI::GUIMainMenu> mainMenu;
		UI::GUIForm *frm;
		NEW_CLASS(frm, UI::GUIForm(0, 640, 480, core));
		frm->SetText(CSTR("Test 4 - Menu and file drop"));
		NEW_CLASSNN(mainMenu, UI::GUIMainMenu());
		NotNullPtr<UI::GUIMenu> mnu = mainMenu->AddSubMenu(CSTR("&File"));
		mnu->AddItem(CSTR("E&xit"), MNU_EXIT, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_X);
		mnu = mainMenu->AddSubMenu(CSTR("&Item"));
		mnu->AddItem(CSTR("Item &1"), MNU_ITEM1, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_1);
		mnu->AddItem(CSTR("Item &2"), MNU_ITEM2, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_2);
		mnu->AddItem(CSTR("Item &3"), MNU_ITEM3, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_3);
		frm->SetMenu(mainMenu);
		frm->HandleMenuClicked(OnMenuEvent, frm);
		frm->HandleDropFiles(OnFileDrop, frm);
		frm->SetExitOnClose(true);
		frm->Show();
		core->Run();
		core.Delete();
	}
	return 0;
}