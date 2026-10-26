#ifndef _SM_MATH_POLYCONICPROJECTEDCOORDINATESYSTEM
#define _SM_MATH_POLYCONICPROJECTEDCOORDINATESYSTEM
#include "Math/ProjectedCoordinateSystem.h"

namespace Math
{
	class PolyconicProjectedCoordinateSystem : public ProjectedCoordinateSystem
	{
	private:
		static Double MeridionalArc(Double a, Double e2, Double lat);
	public:
		PolyconicProjectedCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, UnitType unit);
		PolyconicProjectedCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, UnitType unit);
		virtual ~PolyconicProjectedCoordinateSystem();

		virtual NN<CoordinateSystem> Clone() const;
		virtual CoordinateSystemType GetCoordSysType() const;

		virtual Math::Coord2DDbl ToGeographicCoordinateRad(Math::Coord2DDbl projPos) const;
		virtual Math::Coord2DDbl FromGeographicCoordinateRad(Math::Coord2DDbl geoPos) const;
	};
}
#endif
