#ifndef _SM_MATH_VECTORSTRING
#define _SM_MATH_VECTORSTRING
#include "Math/Vector2D.h"
#include "Media/DrawEngine.h"

namespace Math
{
	class VectorString : public Vector2D
	{
	private:
		const UTF8Char *s;
		Double x;
		Double y;
		Double angleDegree;
		Double buffSize;
		Media::DrawEngine::DrawPos align;
		
	public:
		VectorString(UInt32 srid, const UTF8Char *s, Double x, Double y, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align);
		virtual ~VectorString();

		virtual VectorType GetVectorType();
		virtual void GetCenter(Double *x, Double *y);
		virtual Math::Vector2D *Clone();
		virtual void GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY);
		virtual Double CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY);
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual Bool Support3D();
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Math::Vector2D *vec);

		const UTF8Char *GetString();
		Double GetAngleDegree();
		Double GetBuffSize();
		Media::DrawEngine::DrawPos GetTextAlign();
	};
}
#endif
