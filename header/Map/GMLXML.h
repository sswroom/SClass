#ifndef _SM_MAP_GMLXML
#define _SM_MAP_GMLXML
#include "Map/IMapDrawLayer.h"
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
		static Map::IMapDrawLayer *ParseFeatureCollection(Text::XMLReader *reader, Text::CString fileName);
	private:
		static Math::Geometry::Vector2D *ParseGeometry(Text::XMLReader *reader, ParseEnv *env);
	};
}
#endif
