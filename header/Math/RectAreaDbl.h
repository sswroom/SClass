#ifndef _SM_MATH_RECTAREADBL
#define _SM_MATH_RECTAREADBL
#include "Math/Coord2DDbl.h"
#include "Math/Math.h"
#include "Math/Quadrilateral.h"
#include "Math/Size2DDbl.h"

namespace Math
{
	class RectAreaDbl
	{
	public:
		Coord2DDbl min;
		Coord2DDbl max;

		RectAreaDbl() = default;

		RectAreaDbl(Double minX, Double minY, Double width, Double height)
		{
			this->min = Coord2DDbl(minX, minY);
			this->max = Coord2DDbl(minX + width, minY + height);
		}

		RectAreaDbl(Coord2DDbl min, Coord2DDbl max)
		{
			this->min = min;
			this->max = max;
		}

		Bool ContainPt(Double x, Double y) const
		{
			return (x >= min.x && x < max.x && y >= min.y && y < max.y);
		}

		Bool InsideOrTouch(Math::Coord2DDbl pt) const
		{
			return (pt.x >= min.x && pt.x <= max.x && pt.y >= min.y && pt.y <= max.y);
		}

		Math::Coord2DDbl GetMin() const
		{
			return this->min;
		}

		Math::Coord2DDbl GetMax() const
		{
			return this->max;
		}

		Math::Coord2DDbl GetCenter() const
		{
			return (this->min + this->max) * 0.5;
		}

		Double GetWidth() const
		{
			return this->max.x - this->min.x;
		}

		Double GetHeight() const
		{
			return this->max.y - this->min.y;
		}

		Math::Size2DDbl GetSize() const
		{
			return this->max - this->min;
		}

		Double GetArea() const
		{
			return this->GetWidth() * this->GetHeight();
		}

		Math::Quadrilateral ToQuadrilateral() const
		{
			return Math::Quadrilateral(GetMin(), Math::Coord2DDbl(max.x, min.y), GetMax(), Math::Coord2DDbl(min.x, max.y));
		}

		Math::RectAreaDbl Reorder() const
		{
			return Math::RectAreaDbl(this->min.Min(this->max), this->min.Max(this->max));
		}

		Math::RectAreaDbl Expand(Double size) const
		{
			return Math::RectAreaDbl(this->min - size, this->max + size);
		}

		Bool operator==(Math::RectAreaDbl v) const
		{
			return this->min == v.min && this->max == v.max;
		}

		Bool operator!=(Math::RectAreaDbl v) const
		{
			return this->min != v.min || this->max != v.max;
		}

		Math::RectAreaDbl operator*(Double v) const
		{
			return Math::RectAreaDbl(this->min * v, this->max * v);
		}

		Math::RectAreaDbl operator/(Double v) const
		{
			return Math::RectAreaDbl(this->min / v, this->max / v);
		}

		Bool OverlapOrTouch(RectAreaDbl rect) const
		{
			return rect.min.x <= this->max.x && rect.max.x >= this->min.x && rect.min.y <= this->max.y && rect.max.y >= this->min.y;	
		}

		Math::RectAreaDbl OverlapArea(Math::RectAreaDbl area) const
		{
			if (area.min.x <= this->min.x)
			{
				area.min.x = this->min.x;
			}
			if (area.min.y <= this->min.y)
			{
				area.min.y = this->min.y;
			}
			if (area.max.x >= this->max.x)
			{
				area.max.x = this->max.x;
			}
			if (area.max.y >= this->max.y)
			{
				area.max.y = this->max.y;
			}
			return area;
		}
		
		Math::RectAreaDbl MergeArea(Math::RectAreaDbl area) const
		{
			return Math::RectAreaDbl(this->min.Min(area.min), this->max.Max(area.max));
		}

		Math::RectAreaDbl MergePoint(Math::Coord2DDbl pt) const
		{
			return Math::RectAreaDbl(this->min.Min(pt), this->max.Max(pt));
		}

		Math::Quadrilateral Rotate(Double angleRad) const
		{
			Coord2DDbl center = (this->min + this->max) * 0.5;
			Coord2DDbl diff;
			Double sAng = Math_Sin(-angleRad);
			Double cAng = Math_Cos(-angleRad);
			diff = min - center;
			Coord2DDbl tl = center + Math::Coord2DDbl(diff.x * cAng + diff.y * sAng, -diff.x * sAng + diff.y * cAng);
			diff = Coord2DDbl(max.x, min.y) - center;
			Coord2DDbl tr = center + Math::Coord2DDbl(diff.x * cAng + diff.y * sAng, -diff.x * sAng + diff.y * cAng);
			diff = max - center;
			Coord2DDbl br = center + Math::Coord2DDbl(diff.x * cAng + diff.y * sAng, -diff.x * sAng + diff.y * cAng);
			diff = Coord2DDbl(min.x, max.y) - center;
			Coord2DDbl bl = center + Math::Coord2DDbl(diff.x * cAng + diff.y * sAng, -diff.x * sAng + diff.y * cAng);
			return Math::Quadrilateral(tl, tr, br, bl);
		}

		static RectAreaDbl GetRectArea(Coord2DDbl *points, UOSInt nPoints)
		{
			UOSInt i = nPoints - 1;
			Coord2DDbl min = points[i];
			Coord2DDbl max = min;
			while (i-- > 0)
			{
				min = min.Min(points[i]);
				max = max.Max(points[i]);
			}
			return Math::RectAreaDbl(min, max);
		}

		static RectAreaDbl FromQuadrilateral(Math::Quadrilateral quad)
		{
			Coord2DDbl min = quad.tl.Min(quad.tr).Min(quad.br).Min(quad.bl);
			Coord2DDbl max = quad.tl.Max(quad.tr).Max(quad.br).Max(quad.bl);
			return RectAreaDbl(min, max);
		}
	};
}
#endif
