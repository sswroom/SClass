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
		typedef enum
		{
			CST_GEOGRAPHIC,
			CST_MERCATORPROJECTED,
			CST_MERCATOR1SPPROJECTED,
			CST_POINTMAPPING,
			CST_GAUSSKRUGERPROJECTED
		} CoordinateSystemType;

		typedef enum
		{
			PT_GREENWICH = 8901
		} PrimemType;

		typedef enum
		{
			UT_METRE = 9001,
			UT_DEGREE = 9122
		} UnitType;

		typedef struct
		{
			Int32 srid;
			const Char *name;
			Math::EarthEllipsoid *ellipsoid;
		} SpheroidData;
		
		typedef struct
		{
			Int32 srid;
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
		const UTF8Char *csysName;
		Int32 srid;

		CoordinateSystem(const UTF8Char *sourceName, Int32 srid, const UTF8Char *csysName);
	public:
		virtual ~CoordinateSystem();

		virtual Double CalSurfaceDistanceXY(Double x1, Double y1, Double x2, Double y2, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual Double CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual Double CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual CoordinateSystem *Clone() = 0;
		virtual CoordinateSystemType GetCoordSysType() = 0;
		virtual Bool IsProjected() = 0;
		virtual void ToString(Text::StringBuilderUTF *sb) = 0;

		virtual IO::ParsedObject::ParserType GetParserType();

		virtual Bool Equals(CoordinateSystem *csys);
		const UTF8Char *GetCSysName();
		Int32 GetSRID();

		static void ConvertXYZ(Math::CoordinateSystem *srcCoord, Math::CoordinateSystem *destCoord, Double srcX, Double srcY, Double srcZ, Double *destX, Double *destY, Double *destZ);
	};
}
#endif
