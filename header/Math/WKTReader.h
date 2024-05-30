#ifndef _SM_MATH_WKTREADER
#define _SM_MATH_WKTREADER
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	class WKTReader
	{
	private:
		UnsafeArrayOpt<const UTF8Char> lastError;
		UInt32 srid;

		static UnsafeArrayOpt<const UTF8Char> NextDouble(UnsafeArray<const UTF8Char> wkt, OutParam<Double> val);

		void SetLastError(const UTF8Char* lastError);
	public:
		WKTReader(UInt32 srid);
		~WKTReader();

		Optional<Math::Geometry::Vector2D> ParseWKT(UnsafeArray<const UTF8Char> wkt);
		UnsafeArrayOpt<const UTF8Char> GetLastError();
	};
}
#endif
