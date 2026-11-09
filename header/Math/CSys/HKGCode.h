#ifndef _SM_MATH_CSYS_HKGCODE
#define _SM_MATH_CSYS_HKGCODE
#include "Math/RectAreaDbl.h"

namespace Math
{
	namespace CSys
	{
		class HKGCode
		{
		private:
			NN<Math::CSys::CoordinateSystem> csysHK80;
			NN<Math::CSys::CoordinateSystem> csysWGS84;

		public:
			HKGCode();
			~HKGCode();

			UnsafeArray<UTF8Char> WGS84_Grid(UnsafeArray<UTF8Char> gridStr, Double lat, Double lon);
			static UnsafeArray<UTF8Char> HK80_Grid(UnsafeArray<UTF8Char> gridStr, Math::Coord2DDbl pos);
			static Math::RectAreaDbl Grid_HK80(Text::CStringNN gridStr);
		};
	}
}
#endif
