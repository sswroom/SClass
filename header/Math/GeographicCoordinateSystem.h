#ifndef _SM_MATH_GEOGRAPHICCOORDINATESYSTEM
#define _SM_MATH_GEOGRAPHICCOORDINATESYSTEM
#include "Math/Unit/Angle.h"
#include "Math/CoordinateSystem.h"
#include "Math/EarthEllipsoid.h"

namespace Math
{
	class GeographicCoordinateSystem : public Math::CoordinateSystem
	{
	public:
		typedef enum
		{
			GCST_WGS84,
			GCST_CGCS2000,
			GCST_MACAU2009,
			GCST_HK1980,

			GCST_FIRST = GCST_WGS84,
			GCST_LAST = GCST_HK1980
		} GeoCoordSysType;

	private:
		const UTF8Char *csysName;
		DatumData1 datum;
		PrimemType primem;
		UnitType unit;

	public:
		GeographicCoordinateSystem(const UTF8Char *sourceName, Int32 srid, const UTF8Char *csysName, const DatumData1 *datum, PrimemType primem, UnitType unit);
		virtual ~GeographicCoordinateSystem();

		virtual Double CalSurfaceDistanceXY(Double x1, Double y1, Double x2, Double y2, Math::Unit::Distance::DistanceUnit unit);
		virtual Double CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit);
		virtual Double CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit);
		virtual Math::CoordinateSystem *Clone();
		virtual CoordinateSystemType GetCoordSysType();
		virtual Bool IsProjected();
		virtual void ToString(Text::StringBuilderUTF *sb);

		Math::EarthEllipsoid *GetEllipsoid();
		const UTF8Char *GetDatumName();
		const DatumData1 *GetDatum();
		PrimemType GetPrimem();
		UnitType GetUnit();

		void ToCartesianCoord(Double lat, Double lon, Double h, Double *x, Double *y, Double *z);
		void FromCartesianCoord(Double x, Double y, Double z, Double *lat, Double *lon, Double *h);

		static const UTF8Char *GetCoordinateSystemName(GeoCoordSysType gcst);
	};
}
#endif
