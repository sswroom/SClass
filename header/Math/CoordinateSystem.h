#ifndef _SM_MATH_COORDINATESYSTEM
#define _SM_MATH_COORDINATESYSTEM
#include "IO/ParsedObject.h"
#include "Math/Coord2DDbl.h"
#include "Math/EarthEllipsoid.h"
#include "Math/Vector3.h"
#include "Math/Unit/Angle.h"
#include "Math/Unit/Distance.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Math
{
	namespace Geometry
	{
		class LineString;
		class Polyline;
		class Vector2D;
	}

	class GeographicCoordinateSystem;
	class CoordinateSystem : public IO::ParsedObject
	{
	public:
		enum class CoordinateSystemType
		{
			Geographic,
			MercatorProjected,
			Mercator1SPProjected,
			PointMapping,
			GausskrugerProjected
		};

		typedef enum
		{
			PT_GREENWICH = 8901
		} PrimemType;

		typedef enum
		{
			UT_METRE = 9001,
			UT_DEGREE = 9122
		} UnitType;

		struct SpheroidData
		{
			UInt32 srid;
			UnsafeArray<const UTF8Char> name;
			UOSInt nameLen;
			NN<Math::EarthEllipsoid> ellipsoid;
		};
		
		typedef struct
		{
			UInt32 srid;
			SpheroidData spheroid;
			UnsafeArray<const UTF8Char> name;
			UOSInt nameLen;
			Double x0;
			Double y0;
			Double z0;
			Double cX;
			Double cY;
			Double cZ;
			Double xAngle;
			Double yAngle;
			Double zAngle;
			Double scale;
			Math::Unit::Angle::AngleUnit aunit;
		} DatumData1;
		
	protected:
		NN<Text::String> csysName;
		UInt32 srid;

		CoordinateSystem(NN<Text::String> sourceName, UInt32 srid, Text::CStringNN csysName);
		CoordinateSystem(Text::CStringNN sourceName, UInt32 srid, Text::CStringNN csysName);
	public:
		virtual ~CoordinateSystem();

		virtual Double CalSurfaceDistance(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const = 0;
		virtual Double CalLineStringDistance(NN<Math::Geometry::LineString> lineString, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const = 0;
		Double CalDistance(NN<Math::Geometry::Vector2D> vec, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const;
		virtual NN<CoordinateSystem> Clone() const = 0;
		virtual CoordinateSystemType GetCoordSysType() const = 0;
		virtual Bool IsProjected() const = 0;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;

		virtual IO::ParserType GetParserType() const;

		virtual Bool Equals(NN<const CoordinateSystem> csys) const;
		NN<Text::String> GetCSysName() const { return this->csysName; }
		UInt32 GetSRID() const { return this->srid; }
 
		static Math::Coord2DDbl Convert(NN<const Math::GeographicCoordinateSystem> srcCoord, NN<const Math::GeographicCoordinateSystem> destCoord, Math::Coord2DDbl coord);
		static Math::Vector3 Convert3D(NN<const Math::GeographicCoordinateSystem> srcCoord, NN<const Math::GeographicCoordinateSystem> destCoord, Math::Vector3 srcPos);
		static void ConvertArray(NN<const Math::GeographicCoordinateSystem> srcCoord, NN<const Math::GeographicCoordinateSystem> destCoord, const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints);
		static Math::Vector3 ConvertToCartesianCoord(NN<const Math::CoordinateSystem> srcCoord, Math::Vector3 srcPos);
		static void DatumData1ToString(NN<const DatumData1> datum, NN<Text::StringBuilderUTF8> sb);
		static Text::CStringNN CoordinateSystemTypeGetName(CoordinateSystemType csysType);
	};
}
#endif
