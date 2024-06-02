#include "Stdafx.h"
#include "Core/Core.h"
#include "Map/MapSearch.h"

#include <stdio.h>

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Text::CStringNN mapSrchFile = CSTR("/home/sswroom/Progs/Temp/20220208 MapSearch/MapSearch.txt");
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Map::MapSearchManager srchMgr;
	Map::MapSearch mapSrch(mapSrchFile, &srchMgr);
	if (mapSrch.SearchName(sbuff, Math::Coord2DDbl(114.232824925755, 22.4716833405741)).SetTo(sptr))
	{
		printf("Result: %s\r\n", sbuff);
	}
	else
	{
		printf("Result: <null>\r\n");
	}
	return 0;
}
