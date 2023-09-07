#ifndef _SM_MAP_OSM_OSMPARSER
#define _SM_MAP_OSM_OSMPARSER
#include "Map/MapDrawLayer.h"
#include "Text/CString.h"
#include "Text/XMLReader.h"

namespace Map
{
	namespace OSM
	{
		class OSMParser
		{
		public:
			static Map::MapDrawLayer *ParseLayerNode(NotNullPtr<Text::XMLReader> reader, Text::CStringNN fileName);
		};
	}
}
#endif
