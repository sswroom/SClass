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
			struct ArcInfo
			{
				UOSInt type;
				UOSInt startIndex;
				Math::Coord2DDbl center;
				UInt32 bits;
			};
			
		private:
			UInt32 srid;
			Data::ArrayListA<Math::Coord2DDbl> ptList;
			Data::ArrayList<UInt32> partList;
			Data::ArrayList<Double> zList;
			Data::ArrayList<Double> mList;
			Data::ArrayListNN<ArcInfo> arcList;
		public:
			ESRICurve(UInt32 srid, UnsafeArray<UInt32> ptOfstList, UOSInt nParts, UnsafeArray<Math::Coord2DDbl> ptArr, UOSInt nPoint, UnsafeArrayOpt<Double> zArr, UnsafeArrayOpt<Double> mArr);
			~ESRICurve();

			void AddArc(UOSInt index, Math::Coord2DDbl center, UInt32 bits);
			NN<Math::Geometry::Vector2D> ToArea() const;
			NN<Math::Geometry::Vector2D> ToLine() const;
		};
	}
}
#endif
