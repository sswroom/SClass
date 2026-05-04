#include "Stdafx.h"
#include "Core/Core.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/AIDemo/AIDemoCore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Manage::ExceptionRecorder exHdlr(CSTR("Error.log"), Manage::ExceptionRecorder::EA_RESTART);
	SSWR::AIDemo::AIDemoCore core(progCtrl);
	if (core.IsError())
	{
		return 1;
	}
	core.Run();
	return 0;
}
