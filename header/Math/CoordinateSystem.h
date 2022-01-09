#ifndef _SM_MATH_COORDINATESYSTEM
#define _SM_MATH_COORDINATESYSTEM
#include "IO/ParsedObject.h"
#include "Math/EarthEllipsoid.h"
#include "Math/Polyline3D.h"
#include "Math/Unit/Angle.h"
#include "Math/Unit/Distance.h"
#include "Text/StringBuilderUTF.h"

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
			const Char *name;
			Math::EarthEllipsoid *ellipsoid;
		};
		
		typedef struct
		{
			UInt32 srid;
			SpheroidData spheroid;
			const Char *name;
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

		CoordinateSystem(Text::String *sourceName, UInt32 srid, const UTF8Char *csysName);
		CoordinateSystem(const UTF8Char *sourceName, UInt32 srid, const UTF8Char *csysName);
	public:
		virtual ~CoordinateSystem();

		virtual Double CalSurfaceDistanceXY(Double x1, Double y1, Double x2, Double y2, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual Double CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual Double CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual CoordinateSystem *Clone() = 0;
		virtual CoordinateSystemType GetCoordSysType() = 0;
		virtual Bool IsProjected() = 0;
		virtual void ToString(Text::StringBuilderUTF *sb) = 0;

		virtual IO::ParserType GetParserType();

		virtual Bool Equals(CoordinateSystem *csys);
		Text::String *GetCSysName();
		UInt32 GetSRID();

		static void ConvertXYZ(Math::CoordinateSystem *srcCoord, Math::CoordinateSystem *destCoord, Double srcX, Double srcY, Double srcZ, Double *destX, Double *destY, Double *destZ);
	};
}
#endif
