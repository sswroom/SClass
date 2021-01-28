#ifndef _SM_MATH_PROJECTEDCOORDINATESYSTEMBF
#define _SM_MATH_PROJECTEDCOORDINATESYSTEMBF
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

		virtual void CalSurfaceDistanceXY(Math::BigFloat *x1, Math::BigFloat *y1, Math::BigFloat *x2, Math::BigFloat *y2, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit);
		virtual void CalPLDistance(Math::Polyline *pl, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit);
		virtual void CalPLDistance3D(Math::Polyline3D *pl, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit);
		virtual CoordinateSystemBF *Clone();
		virtual Math::CoordinateSystem::CoordinateSystemType GetCoordSysType();

		Math::GeographicCoordinateSystemBF *GetGeographicCoordinateSystem();
		void ToGeographicCoordinate(Math::BigFloat *projX, Math::BigFloat *projY, Math::BigFloat *geoX, Math::BigFloat *geoY);
		void FromGeographicCoordinate(Math::BigFloat *geoX, Math::BigFloat *geoY, Math::BigFloat *projX, Math::BigFloat *projY);
		void CalcM(Math::BigFloat *rLat, Math::BigFloat *mVal);
		Bool SameProjection(Math::ProjectedCoordinateSystemBF *csys);

		static Math::ProjectedCoordinateSystemBF *CreateCoordinateSystem(Math::ProjectedCoordinateSystem::ProjCoordSysType pcst);
	};
}
#endif
