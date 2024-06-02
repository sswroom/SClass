#ifndef _SM_MAP_GOOGLEMAP_GOOGLEMAPSUTIL
#define _SM_MAP_GOOGLEMAP_GOOGLEMAPSUTIL
#include "Math/Geometry/LineString.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleMapsUtil
		{
		public:
			static Optional<Math::Geometry::LineString> ParsePolylineText(UnsafeArray<const UTF8Char> polylineText);
		};
	}
}
#endif
