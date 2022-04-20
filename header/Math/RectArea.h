#ifndef _SM_MATH_RECTAREA
#define _SM_MATH_RECTAREA

namespace Math
{
	template <typename T> class RectArea
	{
	public:
		T left;
		T top;
		T width;
		T height;

		Bool ContainPt(T x, T y)
		{
			return (x >= left && x < (left + width) && y >= top && y < (top + height));
		}

		static void GetRectArea(RectArea<T> *area, T *points, UOSInt nPoints)
		{
			UOSInt i = nPoints - 1;
			T minX = points[(i << 1)];
			T minY = points[(i << 1) + 1];
			T maxX = minX;
			T maxY = minY;
			while (i-- > 0)
			{
				if (points[(i << 1)] < minX)
					minX = points[(i << 1)];
				if (points[(i << 1)] > maxX)
					maxX = points[(i << 1)];
				if (points[(i << 1) + 1] < minY)
					minY = points[(i << 1) + 1];
				if (points[(i << 1) + 1] > maxY)
					maxY = points[(i << 1) + 1];
			}
			area->left = minX;
			area->top = minY;
			area->width = maxX - minX;
			area->height = maxY - minY;
		}
	};
}
#endif
