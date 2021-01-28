#ifndef _SM_MAP_OSM_OSMPARSER
#define _SM_MAP_OSM_OSMPARSER
#include "Map/IMapDrawLayer.h"
#include "Text/XMLReader.h"

namespace Map
{
	namespace OSM
	{
		class OSMParser
		{
		public:
			static Map::IMapDrawLayer *ParseLayerNode(Text::XMLReader *reader, const UTF8Char *fileName);
		};
	}
}
#endif
