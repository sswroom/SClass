#ifndef _SM_MATH_GEOMETRY_VECTORSTRING
#define _SM_MATH_GEOMETRY_VECTORSTRING
#include "Math/Geometry/Vector2D.h"
#include "Media/DrawEngine.h"

namespace Math
{
	namespace Geometry
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
			virtual Math::Geometry::Vector2D *Clone() const;
			virtual void GetBounds(Math::RectAreaDbl *bounds) const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
			virtual Bool JoinVector(Math::Geometry::Vector2D *vec);
			virtual Bool HasZ() const;
			virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
			virtual Bool Equals(Math::Geometry::Vector2D *vec) const;

			Text::String *GetString() const;
			Double GetAngleDegree() const;
			Double GetBuffSize() const;
			Media::DrawEngine::DrawPos GetTextAlign() const;
		};
	}
}
#endif
