#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "UI/GUICore.h"
#include "UI/MessageDialog.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UI::MessageDialog::ShowDialog((const UTF8Char*)"Message Dialog Test", (const UTF8Char*)"Message Dialog", 0);
	return 0;
}