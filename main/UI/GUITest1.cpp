#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUICore.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	NN<UI::GUICore> ui;
	if (progCtrl->CreateGUICore(progCtrl).SetTo(ui))
	{
		ui->ShowMsgOK(CSTR("Message Dialog Test"), CSTR("Message Dialog"), 0);
	}
	return 0;
}