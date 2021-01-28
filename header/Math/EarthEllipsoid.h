#ifndef _SM_MATH_EARTHELLIPSOID
#define _SM_MATH_EARTHELLIPSOID
#include "Math/Polyline3D.h"
#include "Math/Unit/Distance.h"

namespace Math
{
	class EarthEllipsoid
	{
	public:
		typedef enum
		{
			EET_OTHER,
			EET_PLESSIS,
			EET_EVEREST1830,
			EET_EVEREST1830M,
			EET_EVEREST1830N,
			EET_AIRY1830,
			EET_AIRY1830M,
			EET_BESSEL1841,
			EET_CLARKE1866,
			EET_CLARKE1878,
			EET_CLARKE1880,
			EET_HELMERT1906,
			EET_HAYFORD1910,
			EET_INTL1924,
			EET_KRASSOVSKY1940,
			EET_WGS66,
			EET_AUSTRALIAN1966,
			EET_NEWINTL1967,
			EET_GPS67,
			EET_SAM1969,
			EET_WGS72,
			EET_GRS80,
			EET_WGS84,
			EET_WGS84_OGC,
			EET_IERS1989,
			EET_IERS2003
		} EarthEllipsoidType;

		typedef struct
		{
			EarthEllipsoidType eet;
			const Char *name;
			Int32 year;
			Double semiMajorAxis;
			Double inverseFlattening;
		} EarthEllipsoidInfo;
	private:
		Double semiMajorAxis;
		Double inverseFlattening;
		Double semiMinorAxis;
		Double eccentricity;
		EarthEllipsoidType eet;

		static EarthEllipsoidInfo refEllipsoids[];
	public:
		EarthEllipsoid(Double semiMajorAxis, Double inverseFlattening, EarthEllipsoidType eet);
		EarthEllipsoid(EarthEllipsoidType eet);
		EarthEllipsoid();
		~EarthEllipsoid();

		Double CalSurfaceDistance(Double dLat1, Double dLon1, Double dLat2, Double dLon2, Math::Unit::Distance::DistanceUnit unit);
		Double CalPLDistance(Math::Polyline *pl, Math::Unit::Distance::DistanceUnit unit);
		Double CalPLDistance3D(Math::Polyline3D *pl, Math::Unit::Distance::DistanceUnit unit);

		Double GetSemiMajorAxis();
		Double GetSemiMinorAxis();
		Double GetInverseFlattening();
		Double GetEccentricity();
		Double CalLonByDist(Double dLat, Double dLon, Double distM);
		Double CalLatByDist(Double dLat, Double distM);
		Double CalRadiusAtLat(Double lat);
		Bool Equals(EarthEllipsoid *ellipsoid);
		const UTF8Char *GetName();

		void operator=(const EarthEllipsoid &ellipsoid);
		void operator=(const EarthEllipsoid *ellipsoid);
		EarthEllipsoid *Clone();

		void ToCartesianCoord(Double dLat, Double dLon, Double h, Double *x, Double *y, Double *z);
		void FromCartesianCoord(Double x, Double y, Double z, Double *dLat, Double *dLon, Double *h);

		static const EarthEllipsoidInfo *GetEarthInfo(EarthEllipsoidType eet);
	};
}
#endif
