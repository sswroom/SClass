/////////////Not finish
#ifndef _SM_MATH_PROJECTIONCONVERT
#define _SM_MATH_PROJECTIONCONVERT
#include "Math/BigFloat.h"

namespace Math
{
	class ProjectionConvert
	{
	private:
		NN<Math::BigFloat> originNorthing;
		NN<Math::BigFloat> originEasting;
		NN<Math::BigFloat> originLatitude;
		NN<Math::BigFloat> originLongitude;
		NN<Math::BigFloat> meridianScale;
		NN<Math::BigFloat> meridianOrigin;
		NN<Math::BigFloat> curvPrimeVertical;
		NN<Math::BigFloat> curvMerdian;
		NN<Math::BigFloat> refEllipsoid;
		NN<Math::BigFloat> eccentricity;
	public:
		ProjectionConvert();
		~ProjectionConvert();

		NN<Math::BigFloat> CalMeridian(NN<Math::BigFloat> meridianDist, NN<Math::BigFloat> latitude);
	};
}
#endif
