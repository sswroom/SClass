#ifndef _SM_MATH_COORDINATESYSTEMCONVERTER
#define _SM_MATH_COORDINATESYSTEMCONVERTER
#include "Math/CoordinateConverter.h"
#include "Math/CoordinateSystem.h"

namespace Math
{
	class CoordinateSystemConverter : public CoordinateConverter
	{
	private:
		NN<const Math::CoordinateSystem> srcCsys;
		NN<const Math::CoordinateSystem> destCsys;
	public:
		CoordinateSystemConverter(NN<const Math::CoordinateSystem> srcCsys, NN<const Math::CoordinateSystem> destCsys);
		virtual ~CoordinateSystemConverter();

		virtual UInt32 GetSourceSRID() const;
		virtual UInt32 GetOutputSRID() const;
		virtual Math::Coord2DDbl Convert2D(Math::Coord2DDbl coord) const;
		virtual Math::Vector3 Convert3D(Math::Vector3 vec3) const;
		virtual void Convert2DArr(const Math::Coord2DDbl *srcArr, Math::Coord2DDbl *destArr, UOSInt nPoints) const;
	};
}
#endif
