#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	OSInt i = 0;
	UTF8Char sbuff[10];

	i = 10 / i;
	Text::StrOSInt(sbuff, i);
	return 0;
}
