#ifndef _SM_MATH_RECTAREA
#define _SM_MATH_RECTAREA
#include "Math/Coord2D.h"
#include "Math/Math.h"
#include "Math/Quadrilateral.h"
#include "Math/RectAreaDbl.h"

namespace Math
{
	template <typename T> class RectArea
	{
	public:
		Coord2D<T> tl;
		Coord2D<T> br;

		RectArea() = default;

		RectArea(UOSInt *tmp)
		{
			this->tl = Coord2D<T>(0, 0);
			this->br = Coord2D<T>(0, 0);
		}

		RectArea(T left, T top, T width, T height)
		{
			this->tl = Coord2D<T>(left, top);
			this->br = Coord2D<T>(left + width, top + height);
		}

		RectArea(Coord2D<T> tl, Coord2D<T> br)
		{
			this->tl = tl;
			this->br = br;
		}

		Bool ContainPt(T x, T y)
		{
			return (x >= tl.x && x < br.x && y >= tl.y && y < br.y);
		}

		Math::Coord2D<T> GetTL()
		{
			return this->tl;
		}

		Math::Coord2D<T> GetTR()
		{
			return Math::Coord2D<T>(this->br.x, this->tl.y);
		}

		Math::Coord2D<T> GetBR()
		{
			return this->br;
		}

		Math::Coord2D<T> GetBL()
		{
			return Math::Coord2D<T>(this->tl.x, this->br.y);
		}

		T GetWidth()
		{
			return this->br.x - this->tl.x;
		}

		T GetHeight()
		{
			return this->br.y - this->tl.y;
		}

		T GetArea()
		{
			return this->GetWidth() * this->GetHeight();
		}

		Math::Quadrilateral ToQuadrilateral()
		{
			return Math::Quadrilateral(GetTL().ToDouble(), GetTR().ToDouble(), GetBR().ToDouble(), GetBL().ToDouble());
		}

		Math::RectAreaDbl ToDouble()
		{
			Math::RectAreaDbl rect;
			rect.tl = Math::Coord2DDbl((Double)tl.x, (Double)tl.y);
			rect.br = Math::Coord2DDbl((Double)br.x, (Double)br.y);
			return rect;
		}

		Math::RectArea<T> Reorder()
		{
			Math::RectArea<T> ret = *this;
			T tmp;
			if (ret.tl.x > ret.br.x)
			{
				tmp = ret.tl.x;
				ret.tl.x = ret.br.x;
				ret.br.x = tmp;
			}
			if (ret.tl.y > ret.br.y)
			{
				tmp = ret.tl.y;
				ret.tl.y = ret.br.y;
				ret.br.y = tmp;
			}
			return ret;
		}

		Bool operator==(Math::RectArea<T> &rect)
		{
			return this->tl == rect.tl && this->br == rect.br;
		}

		Math::RectArea<T> operator*(T v)
		{
			return Math::RectArea<T>(this->tl * v, this->br * v);
		}

		Math::RectArea<T> operator/(T v)
		{
			return Math::RectArea<T>(this->tl / v, this->br / v);
		}

		Bool OverlapOrTouch(RectArea<T> rect)
		{
			return rect.tl.x <= this->br.x && rect.br.x >= this->tl.x && rect.tl.y <= this->br.y && rect.br.y >= this->tl.y;	
		}

		static void GetRectArea(RectArea<T> *area, Coord2D<T> *points, UOSInt nPoints)
		{
			UOSInt i = nPoints - 1;
			T minX = points[i].x;
			T minY = points[i].y;
			T maxX = minX;
			T maxY = minY;
			while (i-- > 0)
			{
				if (points[i].x < minX)
					minX = points[i].x;
				if (points[i].x > maxX)
					maxX = points[i].x;
				if (points[i].y < minY)
					minY = points[i].y;
				if (points[i].y > maxY)
					maxY = points[i].y;
			}
			area->tl = Coord2D<T>(minX, minY);
			area->br = Coord2D<T>(maxX, maxY);
		}

		static RectArea<Double> FromQuadrilateral(Math::Quadrilateral quad)
		{
			Double minX = Math::Min(quad.tl.x, quad.tr.x, quad.br.x, quad.bl.x);
			Double minY = Math::Min(quad.tl.y, quad.tr.y, quad.br.y, quad.bl.y);
			Double maxX = Math::Max(quad.tl.x, quad.tr.x, quad.br.x, quad.bl.x);
			Double maxY = Math::Max(quad.tl.y, quad.tr.y, quad.br.y, quad.bl.y);
			return RectArea<Double>(minX, minY, maxX - minX, maxY - minY);
		}
	};
}
#endif
