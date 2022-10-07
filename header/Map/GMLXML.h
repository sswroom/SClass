#ifndef _SM_MAP_GMLXML
#define _SM_MAP_GMLXML
#include "Map/IMapDrawLayer.h"
#include "Text/XMLReader.h"

namespace Map
{
	class GMLXML
	{
	public:
		static Map::IMapDrawLayer *ParseFeatureCollection(Text::XMLReader *reader, Text::CString fileName);
	};
}
#endif
