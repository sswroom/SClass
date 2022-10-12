#ifndef _SM_MATH_COORDINATESYSTEM
#define _SM_MATH_COORDINATESYSTEM
#include "IO/ParsedObject.h"
#include "Math/Coord2DDbl.h"
#include "Math/EarthEllipsoid.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Unit/Angle.h"
#include "Math/Unit/Distance.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Math
{
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
			const UTF8Char *name;
			UOSInt nameLen;
			Math::EarthEllipsoid *ellipsoid;
		};
		
		typedef struct
		{
			UInt32 srid;
			SpheroidData spheroid;
			const UTF8Char *name;
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
		Text::String *csysName;
		UInt32 srid;

		CoordinateSystem(Text::String *sourceName, UInt32 srid, Text::CString csysName);
		CoordinateSystem(Text::CString sourceName, UInt32 srid, Text::CString csysName);
	public:
		virtual ~CoordinateSystem();

		virtual Double CalSurfaceDistanceXY(Math::Coord2DDbl pos1, Math::Coord2DDbl pos2, Math::Unit::Distance::DistanceUnit unit) const = 0;
		virtual Double CalPLDistance(Math::Geometry::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) const = 0;
		virtual Double CalPLDistance3D(Math::Geometry::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) const = 0;
		virtual CoordinateSystem *Clone() const = 0;
		virtual CoordinateSystemType GetCoordSysType() const = 0;
		virtual Bool IsProjected() const = 0;
		virtual void ToString(Text::StringBuilderUTF8 *sb) const = 0;

		virtual IO::ParserType GetParserType() const;

		virtual Bool Equals(CoordinateSystem *csys) const;
		Text::String *GetCSysName() const { return this->csysName; }
		UInt32 GetSRID() const { return this->srid; }

		static void ConvertXYZ(Math::CoordinateSystem *srcCoord, Math::CoordinateSystem *destCoord, Double srcX, Double srcY, Double srcZ, Double *destX, Double *destY, Double *destZ);
		static void ConvertXYArray(Math::CoordinateSystem *srcCoord, Math::CoordinateSystem *destCoord, const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints);
		static void ConvertToCartesianCoord(Math::CoordinateSystem *srcCoord, Double srcX, Double srcY, Double srcZ, Double *destX, Double *destY, Double *destZ);
		static void DatumData1ToString(const DatumData1 *datum, Text::StringBuilderUTF8 *sb);
		static Text::CString CoordinateSystemTypeGetName(CoordinateSystemType csysType);
	};
}
#endif
