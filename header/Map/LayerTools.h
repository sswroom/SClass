#ifndef _SM_MAP_LAYERTOOLS
#define _SM_MAP_LAYERTOOLS
#include "Data/ArrayListNN.hpp"
#include "Map/VectorLayer.h"

namespace Map
{
	class LayerTools
	{
	public:
		static Optional<Map::VectorLayer> CombineLayers(NN<Data::ArrayListNN<Map::MapDrawLayer>> layers, Optional<Text::String> name);
	};
}
#endif
