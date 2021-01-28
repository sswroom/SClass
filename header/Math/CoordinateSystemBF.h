#ifndef _SM_MATH_COORDINATESYSTEMBF
#define _SM_MATH_COORDINATESYSTEMBF
#include "Math/BigFloat.h"
#include "Math/CoordinateSystem.h"

namespace Math
{
	class CoordinateSystemBF
	{
	protected:
		CoordinateSystemBF(const WChar *name);
	public:
		virtual ~CoordinateSystemBF();

		virtual void CalSurfaceDistanceXY(Math::BigFloat *x1, Math::BigFloat *y1, Math::BigFloat *x2, Math::BigFloat *y2, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual void CalPLDistance(Math::Polyline *pl, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual void CalPLDistance3D(Math::Polyline3D *pl, Math::BigFloat *dist, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual CoordinateSystemBF *Clone() = 0;
		virtual Math::CoordinateSystem::CoordinateSystemType GetCoordSysType() = 0;

		Bool Equals(CoordinateSystemBF *csys);

		static void ConvertXY(Math::CoordinateSystemBF *srcCoord, Math::CoordinateSystemBF *destCoord, Math::BigFloat *srcX, Math::BigFloat *srcY, Math::BigFloat *destX, Math::BigFloat *destY);
	};
}
#endif
