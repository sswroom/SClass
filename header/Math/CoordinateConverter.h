#ifndef _SM_MATH_COORDINATECONVERTER
#define _SM_MATH_COORDINATECONVERTER
#include "Math/Coord2DDbl.h"
#include "Math/Vector3.h"

namespace Math
{
	class CoordinateConverter
	{
	public:
		virtual ~CoordinateConverter(){};

		virtual UInt32 GetSourceSRID() const = 0;
		virtual UInt32 GetOutputSRID() const = 0;
		virtual Math::Coord2DDbl Convert2D(Math::Coord2DDbl coord) const = 0;
		virtual Math::Vector3 Convert3D(Math::Vector3 vec3) const = 0;
		virtual void Convert2DArr(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints) const = 0;
	};
}
#endif
