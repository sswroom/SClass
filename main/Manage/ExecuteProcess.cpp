#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Manage/Process.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Manage::Process proc((const UTF8Char*)"shutdown -r now");
	return 0;
}
