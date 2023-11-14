#ifndef _SM_TEXT_JSONUTIL
#define _SM_TEXT_JSONUTIL
#include "Map/GPSTrack.h"
#include "Text/JSONBuilder.h"

namespace Text
{
	class JSONUtil
	{
	public:
		static void ArrayGPSTrack(NotNullPtr<JSONBuilder> json, NotNullPtr<Map::GPSTrack> trk);
	};
}
#endif
