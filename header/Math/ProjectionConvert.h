/////////////Not finish
#ifndef _SM_MATH_PROJECTIONCONVERT
#define _SM_MATH_PROJECTIONCONVERT
#include "Math/BigFloat.h"

namespace Math
{
	class ProjectionConvert
	{
	private:
		Math::BigFloat *originNorthing;
		Math::BigFloat *originEasting;
		Math::BigFloat *originLatitude;
		Math::BigFloat *originLongitude;
		Math::BigFloat *meridianScale;
		Math::BigFloat *meridianOrigin;
		Math::BigFloat *curvPrimeVertical;
		Math::BigFloat *curvMerdian;
		Math::BigFloat *refEllipsoid;
		Math::BigFloat *eccentricity;

	public:
		ProjectionConvert();
		~ProjectionConvert();

		Math::BigFloat *CalMeridian(Math::BigFloat *meridianDist, Math::BigFloat *latitude);
	};
}
#endif
