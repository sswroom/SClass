#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"


Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Char *tmp = (Char*)"Testing";
	tmp[0] = 1;
	tmp[2] = 2;
	return 0;
}
