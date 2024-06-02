#ifndef _SM_MAP_CSVMAPPARSER
#define _SM_MAP_CSVMAPPARSER
#include "IO/Stream.h"
#include "Map/MapDrawLayer.h"
#include "Text/CString.h"

namespace Map
{
	class CSVMapParser
	{
	public:
		static Map::MapDrawLayer *ParseAsPoint(NN<IO::Stream> stm, UInt32 codePage, Text::CStringNN layerName, Text::CStringNN nameCol, Text::CStringNN latCol, Text::CStringNN lonCol, NN<Math::CoordinateSystem> csys);
	};
}
#endif
