#include "Stdafx.h"
#include "Core/Core.h"
#include "Math/Coord2DDbl.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Math::Coord2DDbl coord1 = Math::Coord2DDbl(1, 1);
	Math::Coord2DDbl coord2 = Math::Coord2DDbl(2, 2);

	Math::Coord2DDbl v = coord1 + coord2;
	if (v.x != 3 || v.y != 3)
	{
		return 1;
	}
	v = coord2 - coord1;
	if (v.x != 1 || v.y != 1)
	{
		return 1;
	}
	if (v != coord1)
	{
		return 1;
	}
	v = coord2 * coord2;
	if (v.x != 4 || v.y != 4)
	{
		return 1;
	}
	v = v / coord1;
	if (v.x != 4 || v.y != 4)
	{
		return 1;
	}
	return 0;
}
