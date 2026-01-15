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
			NN<Text::String> s;
			Math::Coord2DDbl pos;
			Double angleDegree;
			Double buffSize;
			Media::DrawEngine::DrawPos align;
			
		public:
			VectorString(UInt32 srid, NN<Text::String> s, Math::Coord2DDbl pos, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align);
			VectorString(UInt32 srid, Text::CStringNN s, Math::Coord2DDbl pos, Double angleDegree, Double buffSize, Media::DrawEngine::DrawPos align);
			virtual ~VectorString();

			virtual VectorType GetVectorType() const;
			virtual Math::Coord2DDbl GetCenter() const;
			virtual NN<Math::Geometry::Vector2D> Clone() const;
			virtual Math::RectAreaDbl GetBounds() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Double CalArea() const;
			virtual Bool JoinVector(NN<const Math::Geometry::Vector2D> vec);
			virtual Bool HasZ() const;
			virtual Bool GetZBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual Bool GetMBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual void Convert(NN<Math::CoordinateConverter> converter);
			virtual Bool Equals(NN<const Math::Geometry::Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal, Bool no3DGeometry) const;
			virtual UOSInt GetCoordinates(NN<Data::ArrayListA<Math::Coord2DDbl>> coordList) const;
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
			virtual void SwapXY();
			virtual void MultiplyCoordinatesXY(Double v);
			virtual UOSInt GetPointCount() const;
			virtual Bool HasArea() const;
			virtual UOSInt CalcHIntersacts(Double y, NN<Data::ArrayListNative<Double>> xList) const;
			virtual Math::Coord2DDbl GetDisplayCenter() const;

			NN<Text::String> GetString() const;
			Double GetAngleDegree() const;
			Double GetBuffSize() const;
			Media::DrawEngine::DrawPos GetTextAlign() const;
		};
	}
}
#endif
