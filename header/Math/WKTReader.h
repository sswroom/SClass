#ifndef _SM_MATH_WKTREADER
#define _SM_MATH_WKTREADER
#include "Math/Vector2D.h"

namespace Math
{
	class WKTReader
	{
	private:
		const UTF8Char *lastError;
		UInt32 srid;

		static const UTF8Char *NextDouble(const UTF8Char *wkt, Double *val);

		void SetLastError(const UTF8Char* lastError);
	public:
		WKTReader(UInt32 srid);
		~WKTReader();

		Math::Vector2D *ParseWKT(const UTF8Char *wkt);
		const UTF8Char *GetLastError();
	};
}
#endif
