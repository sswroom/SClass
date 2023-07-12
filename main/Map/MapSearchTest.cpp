#include "Stdafx.h"
#include "Core/Core.h"
#include "Map/MapSearch.h"

#include <stdio.h>

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Text::CString mapSrchFile = CSTR("/home/sswroom/Progs/Temp/20220208 MapSearch/MapSearch.txt");
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Map::MapSearchManager srchMgr;
	Map::MapSearch mapSrch(mapSrchFile, &srchMgr);
	sptr = mapSrch.SearchName(sbuff, Math::Coord2DDbl(114.232824925755, 22.4716833405741));
	if (sptr)
	{
		printf("Result: %s\r\n", sbuff);
	}
	else
	{
		printf("Result: <null>\r\n");
	}
	return 0;
}
