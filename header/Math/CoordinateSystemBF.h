#ifndef _SM_MATH_COORDINATESYSTEMBF
#define _SM_MATH_COORDINATESYSTEMBF
#include "Math/BigFloat.h"
#include "Math/CoordinateSystem.h"

namespace Math
{
	class CoordinateSystemBF
	{
	protected:
		CoordinateSystemBF(Text::CStringNN name);
	public:
		virtual ~CoordinateSystemBF();

		virtual void CalSurfaceDistanceXY(NN<const Math::BigFloat> x1, NN<const Math::BigFloat> y1, NN<const Math::BigFloat> x2, NN<const Math::BigFloat> y2, NN<Math::BigFloat> dist, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual void CalPLDistance(NN<Math::Geometry::Polyline> pl, NN<Math::BigFloat> dist, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual void CalPLDistance3D(NN<Math::Geometry::Polyline> pl, NN<Math::BigFloat> dist, Math::Unit::Distance::DistanceUnit unit) = 0;
		virtual NN<CoordinateSystemBF> Clone() = 0;
		virtual Math::CoordinateSystem::CoordinateSystemType GetCoordSysType() = 0;

		Bool Equals(NN<const CoordinateSystemBF> csys);

		static void ConvertXY(NN<const Math::CoordinateSystemBF> srcCoord, NN<const Math::CoordinateSystemBF> destCoord, NN<const Math::BigFloat> srcX, NN<const Math::BigFloat> srcY, NN<Math::BigFloat> destX, NN<Math::BigFloat> destY);
	};
}
#endif
