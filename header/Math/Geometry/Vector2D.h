#ifndef _SM_MATH_GEOMETRY_VECTOR2D
#define _SM_MATH_GEOMETRY_VECTOR2D
#include "Data/ArrayListA.h"
#include "Math/Coord2DDbl.h"
#include "Math/RectAreaDbl.h"
#include "Text/CString.h"

namespace Math
{
	class CoordinateSystem;

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

				Image,
				String,
				Ellipse,
				PieArea
			};

		protected:
			UInt32 srid;
		public:
			Vector2D(UInt32 srid);
			virtual ~Vector2D();

			virtual VectorType GetVectorType() const = 0;
			virtual Math::Coord2DDbl GetCenter() const = 0;
			virtual Math::Geometry::Vector2D *Clone() const = 0;
			virtual void GetBounds(Math::RectAreaDbl *bounds) const = 0;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const = 0;
			virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
			virtual Bool JoinVector(Math::Geometry::Vector2D *vec) = 0;
			virtual Bool HasZ() const { return false; };
			virtual Bool HasM() const { return false; };
			virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys) = 0;
			virtual Bool Equals(Vector2D *vec) const = 0;
			virtual Bool EqualsNearly(Vector2D *vec) const = 0;
			virtual UOSInt GetCoordinates(Data::ArrayListA<Math::Coord2DDbl> *coordList) const = 0;
			virtual Bool InsideVector(Math::Coord2DDbl coord) const;
			virtual void SwapXY() = 0;

			UInt32 GetSRID() const;
			void SetSRID(UInt32 srid);
			Math::Coord2DDbl GetCentroid() const;
			Math::Coord2DDbl GetDistanceCenter() const;

			static Bool VectorTypeIsPoint(VectorType vecType);
			static Text::CString VectorTypeGetName(VectorType vecType);
		};
	}
}
#endif
