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
			NotNullPtr<Text::String> s;
			Math::Coord2DDbl pos;
			Double angleDegree;
			Double buffSize;
			Media::DrawEngine::DrawPos align;
			
		public:
			VectorString(UInt32 srid, NotNullPtr<Text::String> s, Math::Coord2DDbl pos, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align);
			VectorString(UInt32 srid, Text::CStringNN s, Math::Coord2DDbl pos, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align);
			virtual ~VectorString();

			virtual VectorType GetVectorType() const;
			virtual Math::Coord2DDbl GetCenter() const;
			virtual NotNullPtr<Math::Geometry::Vector2D> Clone() const;
			virtual Math::RectAreaDbl GetBounds() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Bool JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec);
			virtual Bool HasZ() const;
			virtual void ConvCSys(NotNullPtr<const Math::CoordinateSystem> srcCSys, NotNullPtr<const Math::CoordinateSystem> destCSys);
			virtual Bool Equals(NotNullPtr<const Math::Geometry::Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const;
			virtual UOSInt GetCoordinates(NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> coordList) const;
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
			virtual void SwapXY();
			virtual void MultiplyCoordinatesXY(Double v);
			virtual UOSInt GetPointCount() const;

			NotNullPtr<Text::String> GetString() const;
			Double GetAngleDegree() const;
			Double GetBuffSize() const;
			Media::DrawEngine::DrawPos GetTextAlign() const;
		};
	}
}
#endif
