#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUICore.h"
#include "UI/MessageDialog.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UI::MessageDialog::ShowDialog(CSTR("Message Dialog Test"), CSTR("Message Dialog"), 0);
	return 0;
}