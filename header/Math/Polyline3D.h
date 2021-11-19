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

		virtual Math::Vector2D *Clone();
		virtual Bool Support3D();
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Vector2D *vec);

		virtual Math::Polyline *SplitByPoint(Double x, Double y);
		Double *GetAltitudeList(UOSInt *nPoint);
	};
}
#endif
