#ifndef _SM_MATH_COORD2DDBL
#define _SM_MATH_COORD2DDBL
#include "Math/Double2D.h"

namespace Math
{
	struct Coord2DDbl : public Double2D
	{
		Coord2DDbl() = default;
		Coord2DDbl(std::nullptr_t)
		{
			this->x = 0;
			this->y = 0;
		}

		Coord2DDbl(Double x, Double y) : Double2D(x, y)
		{
		}

		Coord2DDbl(Doublex2 vals) : Double2D(vals)
		{
		}

		Coord2DDbl(Double2D vals) : Double2D(vals)
		{
		}

		static Coord2DDbl FromLatLon(Double lat, Double lon)
		{
			return Coord2DDbl(lon, lat);
		}

		Double CalcLengTo(Coord2DDbl coord) const
		{
			Math::Coord2DDbl diff = *this - coord;
			diff = diff * diff;
			return Math_Sqrt(diff.x + diff.y);
		}

		void SetLat(Double lat)
		{
			this->y = lat;
		}

		void SetLon(Double lon)
		{
			this->x = lon;
		}

		Double GetLat() const
		{
			return this->y;
		}

		Double GetLon() const
		{
			return this->x;
		}

		Double &GetLatRef()
		{
			return this->y;
		}

		Double &GetLonRef()
		{
			return this->x;
		}
	};
}
#endif
