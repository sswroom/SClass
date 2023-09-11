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
		static Map::MapDrawLayer *ParseAsPoint(NotNullPtr<IO::Stream> stm, UInt32 codePage, Text::CStringNN layerName, Text::CString nameCol, Text::CString latCol, Text::CString lonCol, NotNullPtr<Math::CoordinateSystem> csys);
	};
}
#endif
