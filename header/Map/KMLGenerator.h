#ifndef _SM_MAP_KMLGENERATOR
#define _SM_MAP_KMLGENERATOR
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListStringNN.h"
#include "IO/Stream.h"

namespace Map
{
	class KMLGenerator
	{
	public:
		static Bool GenKMLPoints(NN<IO::Stream> fs, NN<Data::ArrayListStringNN> names, NN<Data::ArrayListNative<Double>> lats, NN<Data::ArrayListNative<Double>> lons);
	};
}
#endif
