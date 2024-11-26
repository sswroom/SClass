#ifndef _SM_MAP_ESRI_ESRICURVE
#define _SM_MAP_ESRI_ESRICURVE
#include "Data/ArrayList.h"
#include "Data/ArrayListA.h"
#include "Data/ArrayListNN.h"
#include "Math/Coord2DDbl.h"
#include "Math/Geometry/Vector2D.h"

namespace Map
{
	namespace ESRI
	{
		class ESRICurve
		{
		private:
			struct CurveInfo
			{
				UOSInt type;
				UOSInt startIndex;
			};
			struct ArcInfo : public CurveInfo
			{
				Math::Coord2DDbl center;
				UInt32 bits;
			};

			struct BezierCurveInfo : public CurveInfo
			{
				Math::Coord2DDbl point1;
				Math::Coord2DDbl point2;
			};
			
			struct EllipticArcInfo : public CurveInfo
			{
				Math::Coord2DDbl center;
				Double rotation;
				Double semiMajor;
				Double minorMajorRatio;
				UInt32 bits;
			};

		private:
			UInt32 srid;
			Data::ArrayListA<Math::Coord2DDbl> ptList;
			Data::ArrayList<UInt32> partList;
			Data::ArrayList<Double> zList;
			Data::ArrayList<Double> mList;
			Data::ArrayListNN<CurveInfo> curveList;
		public:
			ESRICurve(UInt32 srid, UnsafeArray<UInt32> ptOfstList, UOSInt nParts, UnsafeArray<Math::Coord2DDbl> ptArr, UOSInt nPoint, UnsafeArrayOpt<Double> zArr, UnsafeArrayOpt<Double> mArr);
			~ESRICurve();

			void AddArc(UOSInt index, Math::Coord2DDbl center, UInt32 bits);
			void AddBezier3Curve(UOSInt index, Math::Coord2DDbl point1, Math::Coord2DDbl point2);
			void AddEllipticArc(UOSInt index, Math::Coord2DDbl center, Double rotation, Double semiMajor, Double minorMajorRatio, UInt32 bits);
			NN<Math::Geometry::Vector2D> ToArea() const;
			NN<Math::Geometry::Vector2D> ToLine() const;
		};
	}
}
#endif
