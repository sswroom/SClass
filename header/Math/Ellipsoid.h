#ifndef _SM_MATH_ELLIPSOID
#define _SM_MATH_ELLIPSOID

namespace Math
{
	class Ellipsoid
	{
	private:
		Double radiusX;
		Double radiusY;
		Double radiusZ;

	public:
		static Double CalcPolarAxis(Double equator, Double flattening);

		Ellipsoid(Double radiusX, Double radiusY, Double radiusZ);
		Ellipsoid(Double equatorRadius, Double flattening);
		~Ellipsoid();

		Bool GetX(Double y, Double z, OutParam<Double> x1, OutParam<Double> x2); //true = success
		Bool GetY(Double x, Double z, OutParam<Double> y1, OutParam<Double> y2); //true = success
		Bool GetZ(Double x, Double y, OutParam<Double> z1, OutParam<Double> z2); //true = success

		Double GetVolume();
		Double GetSurfaceArea();
	};
};

#endif
