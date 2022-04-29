#ifndef _SM_MATH_RECTAREADBL
#define _SM_MATH_RECTAREADBL
#include "Math/Coord2DDbl.h"
#include "Math/Math.h"
#include "Math/Quadrilateral.h"

namespace Math
{
	class RectAreaDbl
	{
	public:
		Coord2DDbl tl;
		Coord2DDbl br;

		RectAreaDbl() = default;

		RectAreaDbl(Double left, Double top, Double width, Double height)
		{
			this->tl = Coord2DDbl(left, top);
			this->br = Coord2DDbl(left + width, top + height);
		}

		RectAreaDbl(Coord2DDbl tl, Coord2DDbl br)
		{
			this->tl = tl;
			this->br = br;
		}

		Bool ContainPt(Double x, Double y)
		{
			return (x >= tl.x && x < br.x && y >= tl.y && y < br.y);
		}

		Math::Coord2DDbl GetTL()
		{
			return this->tl;
		}

		Math::Coord2DDbl GetTR()
		{
			return Math::Coord2DDbl(this->br.x, this->tl.y);
		}

		Math::Coord2DDbl GetBR()
		{
			return this->br;
		}

		Math::Coord2DDbl GetBL()
		{
			return Math::Coord2DDbl(this->tl.x, this->br.y);
		}

		Math::Coord2DDbl GetCenter() const
		{
			return (this->tl + this->br) * 0.5;
		}

		Double GetWidth()
		{
			return this->br.x - this->tl.x;
		}

		Double GetHeight()
		{
			return this->br.y - this->tl.y;
		}

		Math::Coord2DDbl GetSize()
		{
			return this->br - this->tl;
		}

		Double GetArea()
		{
			return this->GetWidth() * this->GetHeight();
		}

		Math::Quadrilateral ToQuadrilateral()
		{
			return Math::Quadrilateral(GetTL(), GetTR(), GetBR(), GetBL());
		}

		Math::RectAreaDbl Reorder()
		{
			return Math::RectAreaDbl(this->tl.Min(this->br), this->tl.Max(this->br));
		}

		Math::RectAreaDbl operator*(Double v)
		{
			return Math::RectAreaDbl(this->tl * v, this->br * v);
		}

		Math::RectAreaDbl operator/(Double v)
		{
			return Math::RectAreaDbl(this->tl / v, this->br / v);
		}

		Bool OverlapOrTouch(RectAreaDbl rect)
		{
			return rect.tl.x <= this->br.x && rect.br.x >= this->tl.x && rect.tl.y <= this->br.y && rect.br.y >= this->tl.y;	
		}

		static void GetRectArea(RectAreaDbl *area, Coord2DDbl *points, UOSInt nPoints)
		{
			UOSInt i = nPoints - 1;
			Coord2DDbl min = points[i];
			Coord2DDbl max = min;
			while (i-- > 0)
			{
				min = min.Min(points[i]);
				max = max.Max(points[i]);
			}
			area->tl = min;
			area->br = max;
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
