#ifndef _SM_MATH_TRANSFORM
#define _SM_MATH_TRANSFORM
#include "Math/Matrix3.h"

namespace Math
{
	class Transform
	{
	public:
		Matrix3 axis;
		Vector3 origin;

	public:
		Transform() = default;
		Transform(Matrix3 axis, Vector3 origin);

		Vector3 TransformPoint(const Vector3 &point) const;
		Vector3 TransformVector(const Vector3 &vector) const;
		Transform Compose(const Transform &other) const;
		Transform InversePlacement() const;
		void SetIdentity();
		static Transform Identity();
	};
}
#endif
