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
		GeographicCoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN csysName, const DatumData1 *datum, PrimemType primem, UnitType unit);
		GeographicCoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN csysName, const DatumData1 *datum, PrimemType primem, UnitType unit);
		virtual ~GeographicCoordinateSystem();

		virtual Double CalSurfaceDistance(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const;
		virtual Double CalLineStringDistance(NN<Math::Geometry::LineString> lineString, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const;
		virtual NN<Math::CoordinateSystem> Clone() const;
		virtual CoordinateSystemType GetCoordSysType() const;
		virtual Bool IsProjected() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;

		NN<Math::EarthEllipsoid> GetEllipsoid() const { return this->datum.spheroid.ellipsoid; }
		Text::CString GetDatumName() const;
		NN<const DatumData1> GetDatum() const;
		PrimemType GetPrimem() const;
		UnitType GetUnit() const;

		Math::Vector3 ToCartesianCoordRad(Math::Vector3 lonLatH) const;
		Math::Vector3 FromCartesianCoordRad(Math::Vector3 coord) const;
		Math::Vector3 ToCartesianCoordDeg(Math::Vector3 lonLatH) const { return ToCartesianCoordRad(lonLatH.MulXY(Math::PI / 180.0)); }
		Math::Vector3 FromCartesianCoordDeg(Math::Vector3 coord) const { return FromCartesianCoordRad(coord).MulXY(180.0 / Math::PI); }
	};
}
#endif
