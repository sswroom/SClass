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
			PieArea
		};

	protected:
		UInt32 srid;
	public:
		Vector2D(UInt32 srid);
		virtual ~Vector2D();

		virtual VectorType GetVectorType() = 0;
		virtual Math::Coord2DDbl GetCenter() = 0;
		virtual Math::Vector2D *Clone() = 0;
		virtual void GetBounds(Math::RectAreaDbl *bounds) = 0;
		virtual Double CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY) = 0;
		virtual Bool JoinVector(Math::Vector2D *vec) = 0;
		virtual Bool Support3D() { return false; };
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys) = 0;
		virtual Bool Equals(Vector2D *vec) = 0;

		UInt32 GetSRID();
		void SetSRID(UInt32 srid);
	};
}
#endif
