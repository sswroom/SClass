#ifndef _SM_MATH_CSYS_COORDINATESYSTEMCONVERTER
#define _SM_MATH_CSYS_COORDINATESYSTEMCONVERTER
#include "Math/CoordinateConverter.h"
#include "Math/CSys/CoordinateSystem.h"

namespace Math
{
	namespace CSys
	{
		class CoordinateSystemConverter : public CoordinateConverter
		{
		private:
			NN<const CoordinateSystem> srcCsys;
			NN<const CoordinateSystem> destCsys;
		public:
			CoordinateSystemConverter(NN<const CoordinateSystem> srcCsys, NN<const CoordinateSystem> destCsys);
			virtual ~CoordinateSystemConverter();

			virtual UInt32 GetSourceSRID() const;
			virtual UInt32 GetOutputSRID() const;
			virtual Math::Coord2DDbl Convert2D(Math::Coord2DDbl coord) const;
			virtual Math::Vector3D Convert3D(Math::Vector3D vec3) const;
			virtual void Convert2DArr(UnsafeArray<const Math::Coord2DDbl> srcArr, UnsafeArray<Math::Coord2DDbl> destArr, UIntOS nPoints) const;
		};
	}
}
#endif
