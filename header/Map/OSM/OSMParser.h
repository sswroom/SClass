#ifndef _SM_MAP_OSM_OSMPARSER
#define _SM_MAP_OSM_OSMPARSER
#include "Map/MapDrawLayer.h"
#include "Map/OSM/OSMData.h"
#include "Text/CString.h"
#include "Text/XMLReader.h"

namespace Map
{
	namespace OSM
	{
		class OSMParser
		{
		public:
			static NN<Map::MapDrawLayer> ParseLayerNode(NN<Text::XMLReader> reader, Text::CStringNN fileName);
			static NN<Map::OSM::OSMData> ParseOSMNode(NN<Text::XMLReader> reader, Text::CStringNN fileName);
		};
	}
}
#endif
