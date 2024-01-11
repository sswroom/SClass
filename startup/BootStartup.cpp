#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl);

void __stdcall BootControl_WaitForExit(NotNullPtr<Core::IProgControl> progCtrl)
{
}

Optional<UI::GUICore> __stdcall Core::IProgControl::CreateGUICore(NotNullPtr<Core::IProgControl> progCtrl)
{
	return 0;
}

UTF8Char **__stdcall BootControl_GetCommandLines(NotNullPtr<Core::IProgControl> progCtrl, OutParam<UOSInt> cmdCnt)
{
	cmdCnt.Set(1);
	return (UTF8Char**)&"Boot";
}

void BootControl_Create(Core::IProgControl *ctrl)
{
	ctrl->WaitForExit = BootControl_WaitForExit;
	ctrl->GetCommandLines = BootControl_GetCommandLines;
	ctrl->SignalExit = BootControl_WaitForExit;
	ctrl->SignalRestart = BootControl_WaitForExit;
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
		MyMain(conCtrl);
	}
	BootControl_Destroy(&conCtrl);
	Core::CoreEnd();
	return 0;
}

