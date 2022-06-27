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
		Double centralMeridian;
		Double latitudeOfOrigin;
		Double scaleFactor;
		Math::CoordinateSystem::UnitType unit;

	public:
		ProjectedCoordinateSystem(Text::String *sourceName, UInt32 srid, Text::CString csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, Math::CoordinateSystem::UnitType unit);
		ProjectedCoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, Math::CoordinateSystem::UnitType unit);
		virtual ~ProjectedCoordinateSystem();

		virtual Double CalSurfaceDistanceXY(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit) const;
		virtual CoordinateSystem *Clone() const = 0;
		virtual CoordinateSystemType GetCoordSysType() const = 0;
		virtual Bool IsProjected() const;
		virtual void ToString(Text::StringBuilderUTF8 *sb) const;

		Math::GeographicCoordinateSystem *GetGeographicCoordinateSystem() const { return this->gcs; }
		virtual void ToGeographicCoordinate(Double projX, Double projY, Double *geoX, Double *geoY) const = 0;
		virtual void FromGeographicCoordinate(Double geoX, Double geoY, Double *projX, Double *projY) const = 0;
		Bool SameProjection(Math::ProjectedCoordinateSystem *csys) const;

		Double GetLatitudeOfOrigin() const { return this->latitudeOfOrigin; }
		Double GetCentralMeridian() const { return this->centralMeridian; }
		Double GetScaleFactor() const { return this->scaleFactor; }
		Double GetFalseEasting() const { return this->falseEasting; }
		Double GetFalseNorthing() const { return this->falseNorthing; }
		UnitType GetUnit() const { return this->unit; }
	};
}
#endif
