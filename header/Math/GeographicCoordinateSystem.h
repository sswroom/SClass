#ifndef _SM_MATH_GEOGRAPHICCOORDINATESYSTEM
#define _SM_MATH_GEOGRAPHICCOORDINATESYSTEM
#include "Math/Unit/Angle.h"
#include "Math/CoordinateSystem.h"
#include "Math/EarthEllipsoid.h"
#include "Text/CString.h"

namespace Math
{
	class GeographicCoordinateSystem : public Math::CoordinateSystem
	{
	private:
		DatumData1 datum;
		PrimemType primem;
		UnitType unit;

	public:
		GeographicCoordinateSystem(Text::String *sourceName, UInt32 srid, Text::CString csysName, const DatumData1 *datum, PrimemType primem, UnitType unit);
		GeographicCoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString csysName, const DatumData1 *datum, PrimemType primem, UnitType unit);
		virtual ~GeographicCoordinateSystem();

		virtual Double CalSurfaceDistanceXY(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalPLDistance(Math::Geometry::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalPLDistance3D(Math::Geometry::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Math::CoordinateSystem *Clone() const;
		virtual CoordinateSystemType GetCoordSysType() const;
		virtual Bool IsProjected() const;
		virtual void ToString(Text::StringBuilderUTF8 *sb) const;

		Math::EarthEllipsoid *GetEllipsoid() const { return this->datum.spheroid.ellipsoid; }
		Text::CString GetDatumName() const;
		const DatumData1 *GetDatum() const;
		PrimemType GetPrimem() const;
		UnitType GetUnit() const;

		void ToCartesianCoordRad(Double lat, Double lon, Double h, Double *x, Double *y, Double *z) const;
		void FromCartesianCoordRad(Double x, Double y, Double z, Double *lat, Double *lon, Double *h) const;
		void ToCartesianCoordDeg(Double dlat, Double dlon, Double h, Double *x, Double *y, Double *z) const { ToCartesianCoordRad(dlat * Math::PI / 180.0, dlon * Math::PI / 180.0, h, x, y, z); }
		void FromCartesianCoordDeg(Double x, Double y, Double z, Double *dlat, Double *dlon, Double *h) const { FromCartesianCoordRad(x, y, z, dlat, dlon, h); *dlat = *dlat * 180.0 / Math::PI; *dlon = *dlon * 180.0 / Math::PI; }
	};
}
#endif
