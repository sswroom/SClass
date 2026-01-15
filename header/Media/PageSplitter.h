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
		Math::Size2DDbl drawSize;

	public:
		PageSplitter();
		~PageSplitter();

		void SetDrawSize(Math::Size2DDbl size, Math::Unit::Distance::DistanceUnit unit);
		UIntOS SplitDrawings(Data::ArrayListA<Math::RectAreaDbl> *pages, Math::RectAreaDbl objectArea, Double objectBuffer, Double pageOverlapBuffer, Double scale);
	};
}
#endif
