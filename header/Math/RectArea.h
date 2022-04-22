#ifndef _SM_MATH_RECTAREA
#define _SM_MATH_RECTAREA
#include "Math/Coord2D.h"

namespace Math
{
	template <typename T> class RectArea
	{
	public:
		Coord2D<T> tl;
		T width;
		T height;

		RectArea()
		{
		}

		RectArea(T left, T top, T width, T height)
		{
			this->tl = Coord2D<T>(left, top);
			this->width = width;
			this->height = height;
		}

		Bool ContainPt(T x, T y)
		{
			return (x >= tl.x && x < (tl.x + width) && y >= tl.y && y < (tl.y + height));
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
			area->width = maxX - minX;
			area->height = maxY - minY;
		}
	};
}
#endif
