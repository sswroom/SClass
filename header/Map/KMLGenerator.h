#ifndef _SM_MAP_KMLGENERATOR
#define _SM_MAP_KMLGENERATOR
#include "IO/Stream.h"
#include "Data/ArrayList.hpp"

namespace Map
{
	class KMLGenerator
	{
	public:
		static Bool GenKMLPoints(IO::Stream *fs, Data::ArrayList<const UTF8Char*> *names, Data::ArrayList<Double> *lats, Data::ArrayList<Double> *lons);
	};
}
#endif
