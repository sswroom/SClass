#include "Stdafx.h"
#include "Core/Core.h"
#include "Math/Coord2DDbl.h"

#include <stdio.h>

ASTRUCT AlignedStruct
{
	Math::Coord2DDbl pos;
	UIntOS tmp;
};

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
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
