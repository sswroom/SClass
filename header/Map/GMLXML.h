#ifndef _SM_MAP_GMLXML
#define _SM_MAP_GMLXML
#include "DB/ColDef.h"
#include "Map/MapDrawLayer.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/XMLReader.h"

namespace Map
{
	class GMLXML
	{
	public:
		enum class SRSType
		{
			EPSG, // EPSG:xxxx
			URI, // http://www.opengis.net/gml/srs/epsg.xml#xxxx
			OGC, // urn:ogc:def:crs:EPSG::xxx
			XOGC // urn:x-ogc:def:crs:EPSG:xxxx
		};
	private:
		struct ParseEnv
		{
			Optional<Math::CoordinateSystem> csys;
			UInt32 srid;
		};
	public:
		static Optional<Map::MapDrawLayer> ParseFeatureCollection(NN<Text::XMLReader> reader, Text::CStringNN fileName);
		static Text::CStringNN GeometryType2GMLType(DB::ColDef::GeometryType geomType);
		static void AppendGeometry(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::Vector2D> vec, SRSType srsType);
		static Text::CStringNN GetSRSPrefix(SRSType srsType);
	private:
		static Optional<Math::Geometry::Vector2D> ParseGeometry(NN<Text::XMLReader> reader, NN<ParseEnv> env);
	};
}
#endif
