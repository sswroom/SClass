#ifndef _SM_MATH_EARTHELLIPSOID
#define _SM_MATH_EARTHELLIPSOID
#include "Math/Vector3.h"
#include "Math/Unit/Distance.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Math
{
	namespace Geometry
	{
		class LineString;
		class Polyline;
	}

	class EarthEllipsoid
	{
	public:
		typedef enum
		{
			EET_OTHER,
			EET_PLESSIS,
			EET_EVEREST1830,
			EET_EVEREST1830A,
			EET_EVEREST1830M,
			EET_EVEREST1830N,
			EET_AIRY1830,
			EET_AIRY1830M,
			EET_AIRYM1849,
			EET_BESSEL1841,
			EET_BESSELMOD,
			EET_BESSELNAMIBIA,
			EET_CLARKE1858,
			EET_CLARKE1866,
			EET_CLARKE1866M,
			EET_CLARKE1878,
			EET_CLARKE1880A,
			EET_CLARKE1880B,
			EET_CLARKE1880I,
			EET_CLARKE1880R,
			EET_CLARKE1880S,
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
			Text::CStringNN name;
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

		Double CalSurfaceDistance(Double dLat1, Double dLon1, Double dLat2, Double dLon2, Math::Unit::Distance::DistanceUnit unit) const;
		Double CalLineStringDistance(NotNullPtr<Math::Geometry::LineString> lineString, Bool include3D, Math::Unit::Distance::DistanceUnit unit) const;
		Double CalPLDistance(NotNullPtr<Math::Geometry::Polyline> pl, Math::Unit::Distance::DistanceUnit unit) const;
		Double CalPLDistance3D(NotNullPtr<Math::Geometry::Polyline> pl, Math::Unit::Distance::DistanceUnit unit) const;

		Double GetSemiMajorAxis() const { return this->semiMajorAxis; }
		Double GetSemiMinorAxis() const { return this->semiMinorAxis; }
		Double GetInverseFlattening() const { return this->inverseFlattening; }
		Double GetEccentricity() const { return this->eccentricity; }
		Double CalLonByDist(Double dLat, Double dLon, Double distM) const;
		Double CalLatByDist(Double dLat, Double distM) const;
		Double CalRadiusAtLat(Double lat) const { return CalRadiusAtRLat(lat * Math::PI / 180.0); }
		Double CalRadiusAtRLat(Double rlat) const { Double ec = Math_Cos(rlat) * this->eccentricity; return this->semiMajorAxis / Math_Sqrt(1.0 - ec * ec); }
		Bool Equals(NotNullPtr<const EarthEllipsoid> ellipsoid) const { return ellipsoid->semiMajorAxis == this->semiMajorAxis && ellipsoid->inverseFlattening == this->inverseFlattening; }
		Text::CStringNN GetName() const;
		void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;

		void operator=(const EarthEllipsoid &ellipsoid);
		void operator=(const EarthEllipsoid *ellipsoid);
		NotNullPtr<EarthEllipsoid> Clone() const;

		Math::Vector3 ToCartesianCoordRad(Math::Vector3 lonLatH) const;
		Math::Vector3 FromCartesianCoordRad(Math::Vector3 coord) const;
		Math::Vector3 ToCartesianCoordDeg(Math::Vector3 lonLatH) const { return ToCartesianCoordRad(lonLatH.MulXY(Math::PI / 180.0)); }
		Math::Vector3 FromCartesianCoordDeg(Math::Vector3 coord) const { return FromCartesianCoordRad(coord).MulXY(180.0 / Math::PI); }

		static NotNullPtr<const EarthEllipsoidInfo> GetEarthInfo(EarthEllipsoidType eet);
	};
}
#endif
