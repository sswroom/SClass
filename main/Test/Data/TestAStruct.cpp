#include "Stdafx.h"
#include "Core/Core.h"
#include "Math/Coord2DDbl.h"

#include <stdio.h>

ASTRUCT AlignedStruct
{
	Math::Coord2DDbl pos;
	UOSInt tmp;
};

Int32 MyMain(Core::IProgControl *progCtrl)
{
	printf("sizeof(Math::Coord2DDbl) = %d\r\n", sizeof(Math::Coord2DDbl));
	printf("sizeof(AlignedStruct) = %d\r\n", sizeof(AlignedStruct));
	if (sizeof(Math::Coord2DDbl) != 16)
	{
		return 1;
	}
	if (sizeof(AlignedStruct) & 15)
	{
		return 1;
	}
	return 0;
}
