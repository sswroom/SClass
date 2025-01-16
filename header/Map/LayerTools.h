#ifndef _SM_MAP_LAYERTOOLS
#define _SM_MAP_LAYERTOOLS
#include "Map/VectorLayer.h"
#include "Data/ArrayList.h"

namespace Map
{
	class LayerTools
	{
	public:
		static Optional<Map::VectorLayer> CombineLayers(NN<Data::ArrayListNN<Map::MapDrawLayer>> layers, Optional<Text::String> name);
	};
}
#endif
