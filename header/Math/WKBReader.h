#ifndef _SM_MATH_WKBREADER
#define _SM_MATH_WKBREADER
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	class WKBReader
	{
	public:
		enum WKBGeometryType
		{
			wkbPoint = 1,
			wkbLineString = 2,
			wkbPolygon = 3,
			wkbMultiPoint = 4,
			wkbMultiLineString = 5,
			wkbMultiPolygon = 6,
			wkbGeometryCollection = 7,
			wkbPolyhedralSurface = 15,
			wkbTIN = 16,
			wkbTriangle = 17,
			wkbPointZ = 1001,
			wkbLineStringZ = 1002,
			wkbPolygonZ = 1003,
			wkbMultiPointZ = 1004,
			wkbMultiLineStringZ = 1005,
			wkbMultiPolygonZ = 1006,
			wkbGeometryCollectionZ = 1007,
			wkbPolyhedralSurfaceZ = 1015,
			wkbTINZ = 1016,
			wkbTrianglez = 1017,
			wkbPointM = 2001,
			wkbLineStringM = 2002,
			wkbPolygonM = 2003,
			wkbMultiPointM = 2004,
			wkbMultiLineStringM = 2005,
			wkbMultiPolygonM = 2006,
			wkbGeometryCollectionM = 2007,
			wkbPolyhedralSurfaceM = 2015,
			wkbTINM = 2016,
			wkbTriangleM = 2017,
			wkbPointZM = 3001,
			wkbLineStringZM = 3002,
			wkbPolygonZM = 3003,
			wkbMultiPointZM = 3004,
			wkbMultiLineStringZM = 3005,
			wkbMultiPolygonZM = 3006,
			wkbGeometryCollectionZM = 3007,
			wkbPolyhedralSurfaceZM = 3015,
			wkbTinZM = 3016,
			wkbTriangleZM = 3017
		};
	private:
		typedef Double (*FReadDouble)(const UInt8 *buff);
		typedef UInt32 (*FReadUInt32)(const UInt8 *buff);
		UInt32 srid;

		static Double NDRReadDouble(const UInt8 *buff);
		static UInt32 NDRReadUInt32(const UInt8 *buff);
		static Double XDRReadDouble(const UInt8 *buff);
		static UInt32 XDRReadUInt32(const UInt8 *buff);
	public:
		WKBReader(UInt32 srid);
		~WKBReader();

		Optional<Math::Geometry::Vector2D> ParseWKB(UnsafeArray<const UInt8> wkb, UIntOS wkbLen, OptOut<UIntOS> sizeUsed);
	};
}
#endif
