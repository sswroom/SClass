#ifndef _SM_MATH_PROJECTEDCOORDINATESYSTEMBF
#define _SM_MATH_PROJECTEDCOORDINATESYSTEMBF
#include "Math/CoordinateSystemManager.h"
#include "Math/GeographicCoordinateSystemBF.h"
#include "Math/ProjectedCoordinateSystem.h"

namespace Math
{
	class ProjectedCoordinateSystemBF : public CoordinateSystemBF
	{
	private:
		Math::BigFloat *falseEasting;
		Math::BigFloat *falseNorthing;
		Math::BigFloat *centralMeridian;
		Math::BigFloat *latitudeOfOrigin;
		Math::BigFloat *scaleFactor;
		Math::BigFloat *latAdj;
		Math::BigFloat *lonAdj;
		Math::GeographicCoordinateSystemBF *gcs;

	public:
		ProjectedCoordinateSystemBF(const WChar *name, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Double latAdj, Double lonAdj, Math::GeographicCoordinateSystemBF *gcs);
		virtual ~ProjectedCoordinateSystemBF();

		virtual void CalSurfaceDistanceXY(Math::BigFloat *x1, Math::BigFloat *y1, Math::BigFloat *x2, Math::BigFloat *y2, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit) const;
		virtual void CalPLDistance(Math::Geometry::Polyline *pl, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit) const;
		virtual void CalPLDistance3D(Math::Geometry::Polyline *pl, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit) const;
		virtual CoordinateSystemBF *Clone() const;
		virtual Math::CoordinateSystem::CoordinateSystemType GetCoordSysType() const;

		Math::GeographicCoordinateSystemBF *GetGeographicCoordinateSystem() const;
		void ToGeographicCoordinate(Math::BigFloat *projX, Math::BigFloat *projY, Math::BigFloat *geoX, Math::BigFloat *geoY) const;
		void FromGeographicCoordinate(Math::BigFloat *geoX, Math::BigFloat *geoY, Math::BigFloat *projX, Math::BigFloat *projY) const;
		void CalcM(Math::BigFloat *rLat, Math::BigFloat *mVal) const;
		Bool SameProjection(NN<Math::ProjectedCoordinateSystemBF> csys) const;

		static Optional<Math::ProjectedCoordinateSystemBF> CreateCoordinateSystem(Math::CoordinateSystemManager::ProjCoordSysType pcst);
	};
}
#endif
