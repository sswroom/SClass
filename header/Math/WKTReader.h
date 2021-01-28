#ifndef _SM_MATH_WKTREADER
#define _SM_MATH_WKTREADER
#include "Math/Vector2D.h"

namespace Math
{
	class WKTReader
	{
	private:
		const UTF8Char *lastError;
		Int32 srid;

		void SetLastError(const UTF8Char* lastError);
	public:
		WKTReader(Int32 srid);
		~WKTReader();

		Math::Vector2D *ParseWKT(const UTF8Char *wkt);
		const UTF8Char *GetLastError();
	};
}
#endif
