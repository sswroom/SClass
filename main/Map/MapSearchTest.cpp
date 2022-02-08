#include "Stdafx.h"
#include "Core/Core.h"
#include "Map/MapSearch.h"

#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::CString mapSrchFile = CSTR("/home/sswroom/Progs/Temp/20220208 MapSearch/MapSearch.txt");
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Map::MapSearchManager *srchMgr;
	Map::MapSearch *mapSrch;
	NEW_CLASS(srchMgr, Map::MapSearchManager());
	NEW_CLASS(mapSrch, Map::MapSearch(mapSrchFile, srchMgr));
	sptr = mapSrch->SearchName(sbuff, 22.4716833405741, 114.232824925755);
	if (sptr)
	{
		printf("Result: %s\r\n", sbuff);
	}
	else
	{
		printf("Result: <null>\r\n");
	}
	DEL_CLASS(mapSrch);
	DEL_CLASS(srchMgr);
	return 0;
}
