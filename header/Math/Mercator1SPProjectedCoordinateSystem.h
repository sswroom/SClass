#ifndef _SM_MATH_MERCATOR1SPPROJECTEDCOORDINATESYSTEM
#define _SM_MATH_MERCATOR1SPPROJECTEDCOORDINATESYSTEM
#include "Math/ProjectedCoordinateSystem.h"

namespace Math
{
	class Mercator1SPProjectedCoordinateSystem : public ProjectedCoordinateSystem
	{
	public:
		Mercator1SPProjectedCoordinateSystem(Text::String *sourceName, UInt32 srid, Text::CString projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit);
		Mercator1SPProjectedCoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit);
		virtual ~Mercator1SPProjectedCoordinateSystem();

		virtual CoordinateSystem *Clone();
		virtual CoordinateSystemType GetCoordSysType();

		virtual void ToGeographicCoordinate(Double projX, Double projY, Double *geoX, Double *geoY);
		virtual void FromGeographicCoordinate(Double geoX, Double geoY, Double *projX, Double *projY);
	};
}
#endif
