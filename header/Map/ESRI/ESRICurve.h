#ifndef _SM_MAP_ESRI_ESRICURVE
#define _SM_MAP_ESRI_ESRICURVE
#include "Data/ArrayListA.hpp"
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListNN.hpp"
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
				UIntOS type;
				UIntOS startIndex;
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
			Data::ArrayListNative<UInt32> partList;
			Data::ArrayListNative<Double> zList;
			Data::ArrayListNative<Double> mList;
			Data::ArrayListNN<CurveInfo> curveList;
		public:
			ESRICurve(UInt32 srid, UnsafeArray<UInt32> ptOfstList, UIntOS nParts, UnsafeArray<Math::Coord2DDbl> ptArr, UIntOS nPoint, UnsafeArrayOpt<Double> zArr, UnsafeArrayOpt<Double> mArr);
			~ESRICurve();

			void AddArc(UIntOS index, Math::Coord2DDbl center, UInt32 bits);
			void AddBezier3Curve(UIntOS index, Math::Coord2DDbl point1, Math::Coord2DDbl point2);
			void AddEllipticArc(UIntOS index, Math::Coord2DDbl center, Double rotation, Double semiMajor, Double minorMajorRatio, UInt32 bits);
			NN<Math::Geometry::Vector2D> CreatePolygon() const;
			NN<Math::Geometry::Vector2D> CreatePolyline() const;
		};
	}
}
#endif
