#ifndef _SM_MATH_VECTORSTRING
#define _SM_MATH_VECTORSTRING
#include "Math/Vector2D.h"
#include "Media/DrawEngine.h"

namespace Math
{
	class VectorString : public Vector2D
	{
	private:
		Text::String *s;
		Double x;
		Double y;
		Double angleDegree;
		Double buffSize;
		Media::DrawEngine::DrawPos align;
		
	public:
		VectorString(UInt32 srid, Text::String *s, Double x, Double y, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align);
		VectorString(UInt32 srid, Text::CString s, Double x, Double y, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align);
		virtual ~VectorString();

		virtual VectorType GetVectorType();
		virtual Math::Coord2DDbl GetCenter();
		virtual Math::Vector2D *Clone();
		virtual void GetBounds(Math::RectAreaDbl *bounds);
		virtual Double CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY);
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual Bool Support3D();
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Math::Vector2D *vec);

		Text::String *GetString();
		Double GetAngleDegree();
		Double GetBuffSize();
		Media::DrawEngine::DrawPos GetTextAlign();
	};
}
#endif
