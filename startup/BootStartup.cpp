#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"

Int32 MyMain(Core::IProgControl *progCtrl);

void __stdcall BootControl_WaitForExit(Core::IProgControl *progCtrl)
{
}

UI::GUICore *__stdcall Core::IProgControl::CreateGUICore(Core::IProgControl *progCtrl)
{
	return 0;
}

UTF8Char **__stdcall BootControl_GetCommandLines(Core::IProgControl *progCtrl, OSInt *cmdCnt)
{
	*cmdCnt = 1;
	return (UTF8Char**)&"Boot";
}

void BootControl_Create(Core::IProgControl *ctrl)
{
	ctrl->WaitForExit = BootControl_WaitForExit;
	ctrl->GetCommandLines = BootControl_GetCommandLines;
}

void BootControl_Destroy(Core::IProgControl *ctrl)
{
}

int main()
{
	Core::IProgControl conCtrl;
	Core::CoreStart();
	BootControl_Create(&conCtrl);
	while (true)
	{
		MyMain(&conCtrl);
	}
	BootControl_Destroy(&conCtrl);
	Core::CoreEnd();
	return 0;
}

