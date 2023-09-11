#ifndef _SM_MAP_GMLXML
#define _SM_MAP_GMLXML
#include "Map/MapDrawLayer.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/XMLReader.h"

namespace Map
{
	class GMLXML
	{
	private:
		struct ParseEnv
		{
			Math::CoordinateSystem *csys;
			UInt32 srid;
		};
	public:
		static Map::MapDrawLayer *ParseFeatureCollection(NotNullPtr<Text::XMLReader> reader, Text::CStringNN fileName);
	private:
		static Math::Geometry::Vector2D *ParseGeometry(NotNullPtr<Text::XMLReader> reader, NotNullPtr<ParseEnv> env);
	};
}
#endif
