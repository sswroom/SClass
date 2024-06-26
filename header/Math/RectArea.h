#ifndef _SM_MATH_RECTAREA
#define _SM_MATH_RECTAREA
#include "Math/Coord2D.h"
#include "Math/Math.h"
#include "Math/Quadrilateral.h"
#include "Math/RectAreaDbl.h"
#include "Math/Size2D.h"

namespace Math
{
	template <typename T> class RectArea
	{
	public:
		Coord2D<T> min;
		Coord2D<T> max;

		RectArea() = default;

		RectArea(UOSInt *tmp)
		{
			this->min = Coord2D<T>(0, 0);
			this->max = Coord2D<T>(0, 0);
		}

		RectArea(T minX, T minY, T width, T height)
		{
			this->min = Coord2D<T>(minX, minY);
			this->max = Coord2D<T>(minX + width, minY + height);
		}

		RectArea(Coord2D<T> min, Coord2D<T> max)
		{
			this->min = min;
			this->max = max;
		}

		Bool ContainPt(T x, T y) const
		{
			return (x >= min.x && x < max.x && y >= min.y && y < max.y);
		}

		Math::Coord2D<T> GetMin() const
		{
			return this->min;
		}

		Math::Coord2D<T> GetMax() const
		{
			return this->max;
		}

		T GetWidth() const
		{
			return this->max.x - this->min.x;
		}

		T GetHeight() const
		{
			return this->max.y - this->min.y;
		}

		T GetArea() const
		{
			return this->GetWidth() * this->GetHeight();
		}

		Math::Size2D<T> GetSize() const
		{
			return this->max - this->min;
		}

		Math::Quadrilateral ToQuadrilateral() const
		{
			return Math::Quadrilateral(GetMin().ToDouble(), Math::Coord2DDbl((Double)max.x, (Double)min.y), GetMax().ToDouble(), Math::Coord2DDbl((Double)min.x, (Double)max.y));
		}

		Math::RectAreaDbl ToDouble() const
		{
			Math::RectAreaDbl rect;
			rect.min = Math::Coord2DDbl((Double)min.x, (Double)min.y);
			rect.max = Math::Coord2DDbl((Double)max.x, (Double)max.y);
			return rect;
		}

		Math::RectArea<T> Reorder() const
		{
			Math::RectArea<T> ret = *this;
			T tmp;
			if (ret.min.x > ret.max.x)
			{
				tmp = ret.min.x;
				ret.min.x = ret.max.x;
				ret.max.x = tmp;
			}
			if (ret.min.y > ret.max.y)
			{
				tmp = ret.min.y;
				ret.min.y = ret.max.y;
				ret.max.y = tmp;
			}
			return ret;
		}

		Bool operator==(Math::RectArea<T> &rect) const
		{
			return this->min == rect.min && this->max == rect.max;
		}

		Math::RectArea<T> operator*(T v) const
		{
			return Math::RectArea<T>(this->min * v, this->max * v);
		}

		Math::RectArea<T> operator/(T v) const
		{
			return Math::RectArea<T>(this->min / v, this->max / v);
		}

		Bool OverlapOrTouch(RectArea<T> rect) const
		{
			return rect.min.x <= this->max.x && rect.max.x >= this->min.x && rect.min.y <= this->max.y && rect.max.y >= this->min.y;	
		}

		static void GetRectArea(RectArea<T> *area, UnsafeArray<Coord2D<T>> points, UOSInt nPoints)
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
			area->min = Coord2D<T>(minX, minY);
			area->max = Coord2D<T>(maxX, maxY);
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
