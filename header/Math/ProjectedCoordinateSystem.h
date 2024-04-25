#ifndef _SM_MATH_PROJECTEDCOORDINATESYSTEM
#define _SM_MATH_PROJECTEDCOORDINATESYSTEM
#include "Math/GeographicCoordinateSystem.h"

namespace Math
{
	class ProjectedCoordinateSystem : public CoordinateSystem
	{
	protected:
		NN<Math::GeographicCoordinateSystem> gcs;
		Double falseEasting;
		Double falseNorthing;
		Double rcentralMeridian;
		Double rlatitudeOfOrigin;
		Double scaleFactor;
		Math::CoordinateSystem::UnitType unit;

	public:
		ProjectedCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CString csysName, Double falseEasting, Double falseNorthing, Double dcentralMeridian, Double dlatitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, Math::CoordinateSystem::UnitType unit);
		ProjectedCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CString csysName, Double falseEasting, Double falseNorthing, Double dcentralMeridian, Double dlatitudeOfOrigin, Double scaleFactor, NN<Math::GeographicCoordinateSystem> gcs, Math::CoordinateSystem::UnitType unit);
		virtual ~ProjectedCoordinateSystem();

		virtual Double CalSurfaceDistance(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalLineStringDistance(NN<Math::Geometry::LineString> lineString, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const;
		virtual NN<CoordinateSystem> Clone() const = 0;
		virtual CoordinateSystemType GetCoordSysType() const = 0;
		virtual Bool IsProjected() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;

		NN<Math::GeographicCoordinateSystem> GetGeographicCoordinateSystem() const { return this->gcs; }
		virtual Math::Coord2DDbl ToGeographicCoordinateRad(Math::Coord2DDbl projPos) const = 0;
		virtual Math::Coord2DDbl FromGeographicCoordinateRad(Math::Coord2DDbl geoPos) const = 0;
		Math::Coord2DDbl ToGeographicCoordinateDeg(Math::Coord2DDbl projPos) const { return ToGeographicCoordinateRad(projPos) * (180 / Math::PI); }
		Math::Coord2DDbl FromGeographicCoordinateDeg(Math::Coord2DDbl geoPos) const { return FromGeographicCoordinateRad(geoPos * (Math::PI / 180.0)); }
		Bool SameProjection(NN<const Math::ProjectedCoordinateSystem> csys) const;

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
