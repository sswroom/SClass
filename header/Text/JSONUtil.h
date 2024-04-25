#ifndef _SM_TEXT_JSONUTIL
#define _SM_TEXT_JSONUTIL
#include "Map/GPSTrack.h"
#include "Text/JSONBuilder.h"

namespace Text
{
	class JSONUtil
	{
	public:
		static void ArrayGPSTrack(NN<JSONBuilder> json, NN<Map::GPSTrack> trk);
	};
}
#endif
