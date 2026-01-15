#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl);

void __stdcall BootControl_WaitForExit(NN<Core::ProgControl> progCtrl)
{
}

Optional<UI::GUICore> __stdcall Core::ProgControl::CreateGUICore(NN<Core::ProgControl> progCtrl)
{
	return 0;
}

UTF8Char **__stdcall BootControl_GetCommandLines(NN<Core::ProgControl> progCtrl, OutParam<UIntOS> cmdCnt)
{
	cmdCnt.Set(1);
	return (UTF8Char**)&"Boot";
}

void BootControl_Create(Core::ProgControl *ctrl)
{
	ctrl->WaitForExit = BootControl_WaitForExit;
	ctrl->GetCommandLines = BootControl_GetCommandLines;
	ctrl->SignalExit = BootControl_WaitForExit;
	ctrl->SignalRestart = BootControl_WaitForExit;
}

void BootControl_Destroy(Core::ProgControl *ctrl)
{
}

int main()
{
	Core::ProgControl conCtrl;
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

