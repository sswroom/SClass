#ifndef _SM_MATH_PROJECTEDCOORDINATESYSTEM
#define _SM_MATH_PROJECTEDCOORDINATESYSTEM
#include "Math/GeographicCoordinateSystem.h"

namespace Math
{
	class ProjectedCoordinateSystem : public CoordinateSystem
	{
	public:
		typedef enum
		{
			PCST_HK80,
			PCST_UK_NATIONAL_GRID,
			PCST_IRISH_NATIONAL_GRID,
			PCST_MACAU_GRID,
			PCST_TWD67,
			PCST_TWD97,

			PCST_FIRST = PCST_HK80,
			PCST_LAST = PCST_TWD97
		} ProjCoordSysType;

	protected:
		Math::GeographicCoordinateSystem *gcs;
		Double falseEasting;
		Double falseNorthing;
		Double centralMeridian;
		Double latitudeOfOrigin;
		Double scaleFactor;
		Math::CoordinateSystem::UnitType unit;

	public:
		ProjectedCoordinateSystem(const UTF8Char *sourceName, Int32 srid, const UTF8Char *csysName, Double falseEasting, Double falseNorthing, Double centralMeridian, Double latitudeOfOrigin, Double scaleFactor, Math::GeographicCoordinateSystem *gcs, Math::CoordinateSystem::UnitType unit);
		virtual ~ProjectedCoordinateSystem();

		virtual Double CalSurfaceDistanceXY(Double x1, Double y1, Double x2, Double y2, Math::Unit::Distance::DistanceUnit unit);
		virtual Double CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit);
		virtual Double CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit);
		virtual CoordinateSystem *Clone() = 0;
		virtual CoordinateSystemType GetCoordSysType() = 0;
		virtual Bool IsProjected();
		virtual void ToString(Text::StringBuilderUTF *sb);

		Math::GeographicCoordinateSystem *GetGeographicCoordinateSystem();
		virtual void ToGeographicCoordinate(Double projX, Double projY, Double *geoX, Double *geoY) = 0;
		virtual void FromGeographicCoordinate(Double geoX, Double geoY, Double *projX, Double *projY) = 0;
		Bool SameProjection(Math::ProjectedCoordinateSystem *csys);

		Double GetLatitudeOfOrigin();
		Double GetCentralMeridian();
		Double GetScaleFactor();
		Double GetFalseEasting();
		Double GetFalseNorthing();
		UnitType GetUnit();

		static const UTF8Char *GetCoordinateSystemName(ProjCoordSysType pcst);
	};
}
#endif
