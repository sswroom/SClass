#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUICore.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	NotNullPtr<UI::GUICore> ui;
	if (ui.Set(progCtrl->CreateGUICore(progCtrl)))
	{
		ui->ShowMsgOK(CSTR("Message Dialog Test"), CSTR("Message Dialog"), 0);
	}
	return 0;
}