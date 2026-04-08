#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ArrayListInt64.h"
#include "Map/CIPLayer2.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN filePath = CSTR("0_req/20100803/polyline");
	Map::CIPLayer2 *lyr;
	NN<Data::ArrayListInt64> outArr;
	Optional<Map::NameArray> nameArr;
	NEW_CLASS(lyr, Map::CIPLayer2(filePath));
	NEW_CLASSNN(outArr, Data::ArrayListInt64());
	lyr->GetObjectIdsMapXY(outArr, nameArr, Math::RectAreaDbl(Math::Coord2DDbl(120.55032138119826, 30.567941960883918), Math::Coord2DDbl(122.35654721587214, 31.922611336889336)), false);
	printf("Count = %d\r\n", (UInt32)outArr->GetCount());
	lyr->ReleaseNameArr(nameArr);
	outArr.Delete();
	DEL_CLASS(lyr);
	return 0;
}
