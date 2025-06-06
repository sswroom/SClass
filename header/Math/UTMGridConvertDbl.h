#ifndef _SM_MATH_UTMGRIDCONVERTDBL
#define _SM_MATH_UTMGRIDCONVERTDBL

namespace Math
{
	class UTMGridConvertDbl
	{
	private:
		Double eccentricity;
		Double radius;

	public:
		UTMGridConvertDbl();
		~UTMGridConvertDbl();

		UnsafeArray<UTF8Char> WGS84_Grid(UnsafeArray<UTF8Char> gridStr, Int32 digitCnt, OptOut<Int32> lonZone, OptOut<Char> latZone, OptOut<Double> northing, OptOut<Double> easting, Double lat, Double lon);
		Bool Grid_WGS84(OutParam<Double> lat, OutParam<Double> lon, UnsafeArray<const UTF8Char> grid);
		Bool Grid_WGS84(OutParam<Double> lat, OutParam<Double> lon, Int32 lonZone, Char latZone, Double northing, Double easting);
	};
}
#endif
