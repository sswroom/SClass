#ifndef _SM_MAP_CSVMAPPARSER
#define _SM_MAP_CSVMAPPARSER
#include "IO/Stream.h"
#include "Map/IMapDrawLayer.h"
#include "Text/CString.h"

namespace Map
{
	class CSVMapParser
	{
	public:
		static Map::IMapDrawLayer *ParseAsPoint(IO::Stream *stm, UInt32 codePage, Text::CString layerName, Text::CString nameCol, Text::CString latCol, Text::CString lonCol, Math::CoordinateSystem *csys);
	};
}
#endif
