#ifndef _SM_MAP_GOOGLEMAP_GOOGLEMAPSUTIL
#define _SM_MAP_GOOGLEMAP_GOOGLEMAPSUTIL
#include "Math/Geometry/Polyline.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleMapsUtil
		{
		public:
			static Math::Geometry::Polyline *ParsePolylineText(const UTF8Char *polylineText);
		};
	}
}
#endif
