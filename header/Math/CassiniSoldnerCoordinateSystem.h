#ifndef _SM_MATH_CASSINISOLDNERCOORDINATESYSTEM
#define _SM_MATH_CASSINISOLDNERCOORDINATESYSTEM
#include "Math/ProjectedCoordinateSystem.h"

namespace Math
{
	class CassiniSoldnerCoordinateSystem : public ProjectedCoordinateSystem
	{
	public:
		CassiniSoldnerCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, UnitType unit);
		CassiniSoldnerCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, UnitType unit);
		virtual ~CassiniSoldnerCoordinateSystem();

		virtual NN<CoordinateSystem> Clone() const;
		virtual CoordinateSystemType GetCoordSysType() const;

		virtual Math::Coord2DDbl ToGeographicCoordinateRad(Math::Coord2DDbl projPos) const;
		virtual Math::Coord2DDbl FromGeographicCoordinateRad(Math::Coord2DDbl geoPos) const;
	};
}
#endif
