#ifndef _SM_MATH_TRANSFORM
#define _SM_MATH_TRANSFORM
#include "Math/Matrix3D.h"

namespace Math
{
	class Transform
	{
	public:
		Matrix3D axis;
		Vector3D origin;

	public:
		Transform() = default;
		Transform(Matrix3D axis, Vector3D origin);

		Vector3D TransformPoint(const Vector3D &point) const;
		Vector3D TransformVector(const Vector3D &vector) const;
		Transform Compose(const Transform &other) const;
		Transform InversePlacement() const;
		void SetIdentity();
		static Transform Identity();
	};
}
#endif
