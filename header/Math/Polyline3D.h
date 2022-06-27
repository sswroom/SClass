#ifndef _SM_MATH_POLYLINE3D
#define _SM_MATH_POLYLINE3D
#include "Math/Polyline.h"

namespace Math
{
	class Polyline3D : public Math::Polyline
	{
	private:
		Double *altitudes;
	public:
		Polyline3D(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint);
		virtual ~Polyline3D();

		virtual Math::Vector2D *Clone() const;
		virtual Bool Support3D() const;
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Vector2D *vec) const;

		virtual Math::Polyline *SplitByPoint(Math::Coord2DDbl pt);
		Double *GetAltitudeList(UOSInt *nPoint);
	};
}
#endif
