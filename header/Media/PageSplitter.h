#ifndef _SM_MEDIA_PAGESPLITTER
#define _SM_MEDIA_PAGESPLITTER
#include "Data/ArrayListA.h"
#include "Math/RectAreaDbl.h"
#include "Math/Size2D.h"
#include "Math/Unit/Distance.h"

namespace Media
{
	class PageSplitter
	{
	public:
		Math::Size2D<Double> drawSize;

	public:
		PageSplitter();
		~PageSplitter();

		void SetDrawSize(Math::Size2D<Double> size, Math::Unit::Distance::DistanceUnit unit);
		UOSInt SplitDrawings(Data::ArrayListA<Math::RectAreaDbl> *pages, Math::RectAreaDbl objectArea, Double objectBuffer, Double pageOverlapBuffer, Double scale);
	};
}
#endif
