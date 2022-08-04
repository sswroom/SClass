#ifndef _SM_MATH_VECTOR2D
#define _SM_MATH_VECTOR2D
#include "Math/Coord2DDbl.h"
#include "Math/RectAreaDbl.h"

namespace Math
{
	class CoordinateSystem;
	class Vector2D
	{
	public:
		enum class VectorType
		{
			Unknown,
			Point,
			Multipoint,
			Polyline,
			Polygon,
			Image,
			String,
			Ellipse,
			PieArea,
			Multipolygon
		};

	protected:
		UInt32 srid;
	public:
		Vector2D(UInt32 srid);
		virtual ~Vector2D();

		virtual VectorType GetVectorType() const = 0;
		virtual Math::Coord2DDbl GetCenter() const = 0;
		virtual Math::Vector2D *Clone() const = 0;
		virtual void GetBounds(Math::RectAreaDbl *bounds) const = 0;
		virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const = 0;
		virtual Bool JoinVector(Math::Vector2D *vec) = 0;
		virtual Bool HasZ() const { return false; };
		virtual Bool HasM() const { return false; };
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys) = 0;
		virtual Bool Equals(Vector2D *vec) const = 0;

		UInt32 GetSRID() const;
		void SetSRID(UInt32 srid);
	};
}
#endif
