#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IntOS i = 0;
	UTF8Char sbuff[10];

	i = 10 / i;
	Text::StrIntOS(sbuff, i);
	return 0;
}
