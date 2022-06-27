#ifndef _SM_MATH_MERCATORPROJECTEDCOORDINATESYSTEM
#define _SM_MATH_MERCATORPROJECTEDCOORDINATESYSTEM
#include "Math/ProjectedCoordinateSystem.h"

namespace Math
{
	class MercatorProjectedCoordinateSystem : public ProjectedCoordinateSystem
	{
	public:
		MercatorProjectedCoordinateSystem(Text::String *sourceName, UInt32 srid, Text::CString projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit);
		MercatorProjectedCoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit);
		virtual ~MercatorProjectedCoordinateSystem();

		virtual CoordinateSystem *Clone() const;
		virtual CoordinateSystemType GetCoordSysType() const;

		virtual void ToGeographicCoordinate(Double projX, Double projY, Double *geoX, Double *geoY) const;
		virtual void FromGeographicCoordinate(Double geoX, Double geoY, Double *projX, Double *projY) const;
		Double CalcM(Double rLat) const;
	};
}
#endif
