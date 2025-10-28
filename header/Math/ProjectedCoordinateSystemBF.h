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
		NN<Math::BigFloat> falseEasting;
		NN<Math::BigFloat> falseNorthing;
		NN<Math::BigFloat> centralMeridian;
		NN<Math::BigFloat> latitudeOfOrigin;
		NN<Math::BigFloat> scaleFactor;
		NN<Math::BigFloat> latAdj;
		NN<Math::BigFloat> lonAdj;
		NN<Math::GeographicCoordinateSystemBF> gcs;

	public:
		ProjectedCoordinateSystemBF(Text::CStringNN name, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Double latAdj, Double lonAdj, NN<Math::GeographicCoordinateSystemBF> gcs);
		virtual ~ProjectedCoordinateSystemBF();

		virtual void CalSurfaceDistanceXY(NN<const Math::BigFloat> x1, NN<const Math::BigFloat> y1, NN<const Math::BigFloat> x2, NN<const Math::BigFloat> y2, NN<Math::BigFloat> dist, Math::Unit::Distance::DistanceUnit unit) const;
		virtual void CalPLDistance(NN<Math::Geometry::Polyline> pl, NN<Math::BigFloat> dist, Math::Unit::Distance::DistanceUnit unit) const;
		virtual void CalPLDistance3D(NN<Math::Geometry::Polyline> pl, NN<Math::BigFloat> dist, Math::Unit::Distance::DistanceUnit unit) const;
		virtual NN<CoordinateSystemBF> Clone() const;
		virtual Math::CoordinateSystem::CoordinateSystemType GetCoordSysType() const;

		NN<Math::GeographicCoordinateSystemBF> GetGeographicCoordinateSystem() const;
		void ToGeographicCoordinate(NN<const Math::BigFloat> projX, NN<const Math::BigFloat> projY, NN<Math::BigFloat> geoX, NN<Math::BigFloat> geoY) const;
		void FromGeographicCoordinate(NN<const Math::BigFloat> geoX, NN<const Math::BigFloat> geoY, NN<Math::BigFloat> projX, NN<Math::BigFloat> projY) const;
		void CalcM(NN<const Math::BigFloat> rLat, NN<Math::BigFloat> mVal) const;
		Bool SameProjection(NN<Math::ProjectedCoordinateSystemBF> csys) const;

		static Optional<Math::ProjectedCoordinateSystemBF> CreateCoordinateSystem(Math::CoordinateSystemManager::ProjCoordSysType pcst);
	};
}
#endif
