#include "Stdafx.h"
#include "Core/Core.h"
#include "SSWR/ServerMonitor/ServerMonitorCore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	SSWR::ServerMonitor::ServerMonitorCore core;
	if (!core.IsError())
	{
		if (core.Run())
		{
			progCtrl->WaitForExit(progCtrl);
		}
	}
	return 0;
}