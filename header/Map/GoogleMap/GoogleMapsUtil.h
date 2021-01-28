#ifndef _SM_MAP_GOOGLEMAP_GOOGLEMAPSUTIL
#define _SM_MAP_GOOGLEMAP_GOOGLEMAPSUTIL
#include "Math/Polyline.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleMapsUtil
		{
		public:
			static Math::Polyline *ParsePolylineText(const UTF8Char *polylineText);
		};
	}
}
#endif
