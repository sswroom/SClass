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
			Optional<Math::CoordinateSystem> csys;
			UInt32 srid;
		};
	public:
		static Optional<Map::MapDrawLayer> ParseFeatureCollection(NN<Text::XMLReader> reader, Text::CStringNN fileName);
	private:
		static Optional<Math::Geometry::Vector2D> ParseGeometry(NN<Text::XMLReader> reader, NN<ParseEnv> env);
	};
}
#endif
