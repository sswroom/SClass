#ifndef _SM_MATH_MERCATORPROJECTEDCOORDINATESYSTEM
#define _SM_MATH_MERCATORPROJECTEDCOORDINATESYSTEM
#include "Math/ProjectedCoordinateSystem.h"

namespace Math
{
	class MercatorProjectedCoordinateSystem : public ProjectedCoordinateSystem
	{
	public:
		MercatorProjectedCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, UnitType unit);
		MercatorProjectedCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, UnitType unit);
		virtual ~MercatorProjectedCoordinateSystem();

		virtual NN<CoordinateSystem> Clone() const;
		virtual CoordinateSystemType GetCoordSysType() const;

		virtual Math::Coord2DDbl ToGeographicCoordinateRad(Math::Coord2DDbl projPos) const;
		virtual Math::Coord2DDbl FromGeographicCoordinateRad(Math::Coord2DDbl geoPos) const;
	};
}
#endif
