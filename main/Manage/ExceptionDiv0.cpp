#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	OSInt i = 0;
	UTF8Char sbuff[10];

	i = 10 / i;
	Text::StrInt32(sbuff, i);
	return 0;
}
