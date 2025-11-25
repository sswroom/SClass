#ifndef _SM_MATH_GEOMETRY_VECTOR2D
#define _SM_MATH_GEOMETRY_VECTOR2D
#include "Data/ArrayList.hpp"
#include "Data/ArrayListA.hpp"
#include "Math/Coord2DDbl.h"
#include "Math/CoordinateConverter.h"
#include "Math/RectAreaDbl.h"
#include "Text/CString.h"

namespace Math
{
	namespace Geometry
	{
		class Vector2D
		{
		public:
			enum class VectorType
			{
				Unknown,
				Point,
				LineString,
				Polygon,
				MultiPoint,
				Polyline, //MultiLineString
				MultiPolygon,
				GeometryCollection,
				CircularString,
				CompoundCurve,
				CurvePolygon,
				MultiCurve,
				MultiSurface,
				Curve,
				Surface,
				PolyhedralSurface,
				Tin,
				Triangle,
				LinearRing,

				Image,
				String,
				Ellipse,
				PieArea
			};

		protected:
			UInt32 srid;
		public:
			Vector2D(UInt32 srid) {	this->srid = srid; }
			virtual ~Vector2D() {};

			virtual VectorType GetVectorType() const = 0;
			virtual Math::Coord2DDbl GetCenter() const = 0;
			virtual NN<Math::Geometry::Vector2D> Clone() const = 0;
			virtual Math::RectAreaDbl GetBounds() const = 0;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const = 0;
			virtual Double CalSqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Double CalArea() const = 0;
			virtual Bool JoinVector(NN<const Vector2D> vec) = 0;
			virtual Bool HasZ() const { return false; };
			virtual Bool HasM() const { return false; };
			virtual Bool GetZBounds(OutParam<Double> min, OutParam<Double> max) const = 0;
			virtual Bool GetMBounds(OutParam<Double> min, OutParam<Double> max) const = 0;
			virtual void Convert(NN<Math::CoordinateConverter> converter) = 0;
			virtual Bool Equals(NN<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal, Bool no3DGeometry) const = 0;
			virtual UOSInt GetCoordinates(NN<Data::ArrayListA<Math::Coord2DDbl>> coordList) const = 0;
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const = 0;
			virtual void SwapXY() = 0;
			virtual void MultiplyCoordinatesXY(Double v) = 0;
			virtual UOSInt GetPointCount() const = 0;
			virtual Bool HasArea() const = 0;
			virtual UOSInt CalcHIntersacts(Double y, NN<Data::ArrayList<Double>> xList) const = 0;
			virtual Math::Coord2DDbl GetDisplayCenter() const = 0;
			virtual Bool HasCurve() const { return false; }
			virtual Optional<Vector2D> ToSimpleShape() const { return this->Clone(); }
			Bool Contains(NN<Math::Geometry::Vector2D> vec) const;

			UInt32 GetSRID() const { return this->srid; }
			virtual void SetSRID(UInt32 srid) { this->srid = srid; }
			Math::Coord2DDbl GetCentroid() const;
			Math::Coord2DDbl GetDistanceCenter() const;

			static Bool VectorTypeIsPoint(VectorType vecType) { return vecType == VectorType::Point || vecType == VectorType::MultiPoint; }
			static Text::CStringNN VectorTypeGetName(VectorType vecType);
		};
	}
}
#endif
