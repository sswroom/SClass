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

		virtual void CalSurfaceDistanceXY(Math::BigFloat *x1, Math::BigFloat *y1, Math::BigFloat *x2, Math::BigFloat *y2, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit) const;
		virtual void CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) const;
		virtual void CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Math::CoordinateSystemBF *Clone() const;
		virtual Math::CoordinateSystem::CoordinateSystemType GetCoordSysType() const;

		Math::BigFloat *GetSemiMajorAxis() const;
		Math::BigFloat *GetSemiMinorAxis() const;
		Math::BigFloat *GetInverseFlattening() const;
		Math::BigFloat *GetEccentricity() const;
		void CalLonByDist(Math::BigFloat *inLat, Math::BigFloat *inLon, Math::BigFloat *distM, Math::BigFloat *outLon) const;
		void CalLatByDist(Math::BigFloat *inLat, Math::BigFloat *distM, Math::BigFloat *outLat) const;
		void CalRadiusAtLat(Math::BigFloat *lat, Math::BigFloat *outRadius) const;
		Bool Equals(Math::GeographicCoordinateSystemBF *gcs) const;

		void ToCartesianCoord(Math::BigFloat *lat, Math::BigFloat *lon, Math::BigFloat *h, Math::BigFloat *x, Math::BigFloat *y, Math::BigFloat *z) const;
		void FromCartesianCoord(Math::BigFloat *x, Math::BigFloat *y, Math::BigFloat *z, Math::BigFloat *lat, Math::BigFloat *lon, Math::BigFloat *h) const;

		static Optional<Math::GeographicCoordinateSystemBF> CreateCoordinateSystem(Math::GeographicCoordinateSystem::GeoCoordSysType gcst);
	};
}
#endif
