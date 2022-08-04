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
		Math::Coord2DDbl pos;
		Double angleDegree;
		Double buffSize;
		Media::DrawEngine::DrawPos align;
		
	public:
		VectorString(UInt32 srid, Text::String *s, Math::Coord2DDbl pos, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align);
		VectorString(UInt32 srid, Text::CString s, Math::Coord2DDbl pos, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align);
		virtual ~VectorString();

		virtual VectorType GetVectorType() const;
		virtual Math::Coord2DDbl GetCenter() const;
		virtual Math::Vector2D *Clone() const;
		virtual void GetBounds(Math::RectAreaDbl *bounds) const;
		virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual Bool HasZ() const;
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Math::Vector2D *vec) const;

		Text::String *GetString() const;
		Double GetAngleDegree() const;
		Double GetBuffSize() const;
		Media::DrawEngine::DrawPos GetTextAlign() const;
	};
}
#endif
