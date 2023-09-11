#ifndef _SM_MATH_MERCATOR1SPPROJECTEDCOORDINATESYSTEM
#define _SM_MATH_MERCATOR1SPPROJECTEDCOORDINATESYSTEM
#include "Math/ProjectedCoordinateSystem.h"

namespace Math
{
	class Mercator1SPProjectedCoordinateSystem : public ProjectedCoordinateSystem
	{
	public:
		Mercator1SPProjectedCoordinateSystem(NotNullPtr<Text::String> sourceName, UInt32 srid, Text::CString projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NotNullPtr<Math::GeographicCoordinateSystem> gcs, UnitType unit);
		Mercator1SPProjectedCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CString projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NotNullPtr<Math::GeographicCoordinateSystem> gcs, UnitType unit);
		virtual ~Mercator1SPProjectedCoordinateSystem();

		virtual NotNullPtr<CoordinateSystem> Clone() const;
		virtual CoordinateSystemType GetCoordSysType() const;

		virtual Math::Coord2DDbl ToGeographicCoordinateRad(Math::Coord2DDbl projPos) const;
		virtual Math::Coord2DDbl FromGeographicCoordinateRad(Math::Coord2DDbl geoPos) const;
	};
}
#endif
