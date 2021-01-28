#ifndef _SM_MATH_GEOGRAPHICCOORDINATESYSTEMBF
#define _SM_MATH_GEOGRAPHICCOORDINATESYSTEMBF
#include "Math/CoordinateSystemBF.h"
#include "Math/GeographicCoordinateSystem.h"

namespace Math
{
	class GeographicCoordinateSystemBF : public Math::CoordinateSystemBF
	{
	private:
		Math::BigFloat *semiMajorAxis;
		Math::BigFloat *inverseFlattening;
		Math::BigFloat *semiMinorAxis;
		Math::BigFloat *eccentricity;
		Math::GeographicCoordinateSystem::GeoCoordSysType gcst;

	public:
		GeographicCoordinateSystemBF(const WChar *name, Math::BigFloat *semiMajorAxisMeter, Math::BigFloat *inverseFlattening, Math::GeographicCoordinateSystem::GeoCoordSysType gcst);
		virtual ~GeographicCoordinateSystemBF();

		virtual void CalSurfaceDistanceXY(Math::BigFloat *x1, Math::BigFloat *y1, Math::BigFloat *x2, Math::BigFloat *y2, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit);
		virtual void CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit);
		virtual void CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit);
		virtual Math::CoordinateSystemBF *Clone();
		virtual Math::CoordinateSystem::CoordinateSystemType GetCoordSysType();

		Math::BigFloat *GetSemiMajorAxis();
		Math::BigFloat *GetSemiMinorAxis();
		Math::BigFloat *GetInverseFlattening();
		Math::BigFloat *GetEccentricity();
		void CalLonByDist(Math::BigFloat *inLat, Math::BigFloat *inLon, Math::BigFloat *distM, Math::BigFloat *outLon);
		void CalLatByDist(Math::BigFloat *inLat, Math::BigFloat *distM, Math::BigFloat *outLat);
		void CalRadiusAtLat(Math::BigFloat *lat, Math::BigFloat *outRadius);
		Bool Equals(Math::GeographicCoordinateSystemBF *gcs);

		void ToCartesianCoord(Math::BigFloat *lat, Math::BigFloat *lon, Math::BigFloat *h, Math::BigFloat *x, Math::BigFloat *y, Math::BigFloat *z);
		void FromCartesianCoord(Math::BigFloat *x, Math::BigFloat *y, Math::BigFloat *z, Math::BigFloat *lat, Math::BigFloat *lon, Math::BigFloat *h);

		static Math::GeographicCoordinateSystemBF *CreateCoordinateSystem(Math::GeographicCoordinateSystem::GeoCoordSysType gcst);
	};
}
#endif
