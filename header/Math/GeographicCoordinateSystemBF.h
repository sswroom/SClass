#ifndef _SM_MATH_GEOGRAPHICCOORDINATESYSTEMBF
#define _SM_MATH_GEOGRAPHICCOORDINATESYSTEMBF
#include "Math/CoordinateSystemBF.h"
#include "Math/GeographicCoordinateSystem.h"

namespace Math
{
	class GeographicCoordinateSystemBF : public Math::CoordinateSystemBF
	{
	private:
		NN<Math::BigFloat> semiMajorAxis;
		NN<Math::BigFloat> inverseFlattening;
		NN<Math::BigFloat> semiMinorAxis;
		NN<Math::BigFloat> eccentricity;
		Math::CoordinateSystemManager::GeoCoordSysType gcst;

	public:
		GeographicCoordinateSystemBF(Text::CStringNN name, NN<const Math::BigFloat> semiMajorAxisMeter, NN<const Math::BigFloat> inverseFlattening, Math::CoordinateSystemManager::GeoCoordSysType gcst);
		virtual ~GeographicCoordinateSystemBF();

		virtual void CalSurfaceDistanceXY(NN<const Math::BigFloat> x1, NN<const Math::BigFloat> y1, NN<const Math::BigFloat> x2, NN<const Math::BigFloat> y2, NN<Math::BigFloat> dist, Math::Unit::Distance::DistanceUnit unit) const;
		virtual void CalPLDistance(NN<Math::Geometry::Polyline> pl, NN<Math::BigFloat> dist, Math::Unit::Distance::DistanceUnit unit) const;
		virtual void CalPLDistance3D(NN<Math::Geometry::Polyline> pl, NN<Math::BigFloat> dist, Math::Unit::Distance::DistanceUnit unit) const;
		virtual NN<Math::CoordinateSystemBF> Clone() const;
		virtual Math::CoordinateSystem::CoordinateSystemType GetCoordSysType() const;

		NN<const Math::BigFloat> GetSemiMajorAxis() const;
		NN<const Math::BigFloat> GetSemiMinorAxis() const;
		NN<const Math::BigFloat> GetInverseFlattening() const;
		NN<const Math::BigFloat> GetEccentricity() const;
		void CalLonByDist(NN<const Math::BigFloat> inLat, NN<const Math::BigFloat> inLon, NN<const Math::BigFloat> distM, NN<Math::BigFloat> outLon) const;
		void CalLatByDist(NN<const Math::BigFloat> inLat, NN<const Math::BigFloat> distM, NN<Math::BigFloat> outLat) const;
		void CalRadiusAtLat(NN<const Math::BigFloat> lat, NN<Math::BigFloat> outRadius) const;
		Bool Equals(NN<const Math::GeographicCoordinateSystemBF> gcs) const;

		void ToCartesianCoord(NN<const Math::BigFloat> lat, NN<const Math::BigFloat> lon, NN<const Math::BigFloat> h, NN<Math::BigFloat> x, NN<Math::BigFloat> y, NN<Math::BigFloat> z) const;
		void FromCartesianCoord(NN<const Math::BigFloat> x, NN<const Math::BigFloat> y, NN<const Math::BigFloat> z, NN<Math::BigFloat> lat, NN<Math::BigFloat> lon, NN<Math::BigFloat> h) const;

		static Optional<Math::GeographicCoordinateSystemBF> CreateCoordinateSystem(Math::CoordinateSystemManager::GeoCoordSysType gcst);
	};
}
#endif
