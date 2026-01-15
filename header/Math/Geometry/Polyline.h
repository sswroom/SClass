#ifndef _SM_MATH_GEOMETRY_POLYLINE
#define _SM_MATH_GEOMETRY_POLYLINE
#include "Math/Geometry/MultiGeometry.hpp"
#include "Math/Geometry/LineString.h"

namespace Math
{
	namespace Geometry
	{
		class Polyline : public Math::Geometry::MultiGeometry<LineString>
		{
		protected:
			Int32 flags;
			UInt32 color;
		public:
			Polyline(UInt32 srid) : MultiGeometry<LineString>(srid) { this->flags = 0; this->color = 0; }
			virtual ~Polyline() {}

			virtual VectorType GetVectorType() const { return Math::Geometry::Vector2D::VectorType::Polyline; }
			virtual NN<Math::Geometry::Vector2D> Clone() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Bool JoinVector(NN<const Math::Geometry::Vector2D> vec);

			void AddFromPtOfst(UnsafeArray<UInt32> ptOfstList, UIntOS nPtOfst, UnsafeArray<Math::Coord2DDbl> pointList, UIntOS nPoint, UnsafeArrayOpt<Double> zList, UnsafeArrayOpt<Double> mList);
			Double CalcHLength() const;
			Double Calc3DLength() const;
			UIntOS FillPointOfstList(UnsafeArray<Math::Coord2DDbl> pointList, UnsafeArray<UInt32> ptOfstList, UnsafeArrayOpt<Double> zList, UnsafeArrayOpt<Double> mList) const;
			Math::Coord2DDbl CalcPosAtDistance(Double dist) const;

			Optional<Math::Geometry::Polyline> SplitByPoint(Math::Coord2DDbl pt);
//			virtual void OptimizePolyline();
			IntOS GetPointNo(Math::Coord2DDbl pt, OptOut<Bool> isPoint, OptOut<Math::Coord2DDbl> calPt, OptOut<Double> calZ, OptOut<Double> calM);

//			Optional<Math::Geometry::Polygon> CreatePolygonByDist(Double dist) const;
			Bool HasColor() const { return (this->flags & 1) != 0; }
			UInt32 GetColor() const { return this->color; }
			void SetColor(UInt32 color) { this->color = color; this->flags |= 1; }
		};
	}
}
#endif
