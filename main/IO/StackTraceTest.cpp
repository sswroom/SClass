#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/DebugTool.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::DebugTool::PrintStackTrace();
	return 0;
}
