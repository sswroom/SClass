#ifndef _SM_MATH_MERCATORPROJECTEDCOORDINATESYSTEM
#define _SM_MATH_MERCATORPROJECTEDCOORDINATESYSTEM
#include "Math/ProjectedCoordinateSystem.h"

namespace Math
{
	class MercatorProjectedCoordinateSystem : public ProjectedCoordinateSystem
	{
	public:
		MercatorProjectedCoordinateSystem(const UTF8Char *sourceName, UInt32 srid, const UTF8Char *projName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, UnitType unit);
		virtual ~MercatorProjectedCoordinateSystem();

		virtual CoordinateSystem *Clone();
		virtual CoordinateSystemType GetCoordSysType();

		virtual void ToGeographicCoordinate(Double projX, Double projY, Double *geoX, Double *geoY);
		virtual void FromGeographicCoordinate(Double geoX, Double geoY, Double *projX, Double *projY);
		Double CalcM(Double rLat);
	};
}
#endif
