#ifndef _SM_MATH_PROJECTEDCOORDINATESYSTEM
#define _SM_MATH_PROJECTEDCOORDINATESYSTEM
#include "Math/GeographicCoordinateSystem.h"

namespace Math
{
	class ProjectedCoordinateSystem : public CoordinateSystem
	{
	protected:
		Math::GeographicCoordinateSystem *gcs;
		Double falseEasting;
		Double falseNorthing;
		Double rcentralMeridian;
		Double rlatitudeOfOrigin;
		Double scaleFactor;
		Math::CoordinateSystem::UnitType unit;

	public:
		ProjectedCoordinateSystem(Text::String *sourceName, UInt32 srid, Text::CString csysName, Double falseEasting, Double falseNorthing, Double dcentralMeridian, Double dlatitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, Math::CoordinateSystem::UnitType unit);
		ProjectedCoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString csysName, Double falseEasting, Double falseNorthing, Double dcentralMeridian, Double dlatitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, Math::CoordinateSystem::UnitType unit);
		virtual ~ProjectedCoordinateSystem();

		virtual Double CalSurfaceDistanceXY(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit) const;
		virtual CoordinateSystem *Clone() const = 0;
		virtual CoordinateSystemType GetCoordSysType() const = 0;
		virtual Bool IsProjected() const;
		virtual void ToString(Text::StringBuilderUTF8 *sb) const;

		Math::GeographicCoordinateSystem *GetGeographicCoordinateSystem() const { return this->gcs; }
		virtual void ToGeographicCoordinateRad(Double projX, Double projY, Double *geoX, Double *geoY) const = 0;
		virtual void FromGeographicCoordinateRad(Double geoX, Double geoY, Double *projX, Double *projY) const = 0;
		void ToGeographicCoordinateDeg(Double projX, Double projY, Double *geoX, Double *geoY) const { ToGeographicCoordinateRad(projX, projY, geoX, geoY); *geoX = *geoX * 180 / Math::PI; *geoY = *geoY * 180 / Math::PI; }
		void FromGeographicCoordinateDeg(Double geoX, Double geoY, Double *projX, Double *projY) const { FromGeographicCoordinateRad(geoX * Math::PI / 180.0, geoY * Math::PI / 180.0, projX, projY); }
		Bool SameProjection(Math::ProjectedCoordinateSystem *csys) const;

		Double GetLatitudeOfOriginDegree() const { return this->rlatitudeOfOrigin * 180 / Math::PI; }
		Double GetCentralMeridianDegree() const { return this->rcentralMeridian * 180 / Math::PI; }
		Double GetLatitudeOfOriginRadian() const { return this->rlatitudeOfOrigin; }
		Double GetCentralMeridianRadian() const { return this->rcentralMeridian; }
		Double GetScaleFactor() const { return this->scaleFactor; }
		Double GetFalseEasting() const { return this->falseEasting; }
		Double GetFalseNorthing() const { return this->falseNorthing; }
		UnitType GetUnit() const { return this->unit; }
	};
}
#endif
