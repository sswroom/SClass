#ifndef _SM_MAP_MAPSEARCHLAYER
#define _SM_MAP_MAPSEARCHLAYER
#include "Math/Coord2DDbl.h"
#include "Text/StringBuilderUTF8.h"

namespace Map
{
	class MapSearchLayer
	{
	public:
		virtual ~MapSearchLayer() {};

		virtual Bool IsError() const = 0;
		virtual Bool GetPGLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OptOut<Math::Coord2DDbl> outCoord, UIntOS strIndex) = 0;
		virtual Bool GetPLLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OutParam<Math::Coord2DDbl> outCoord, UIntOS strIndex) = 0;
	};
}
#endif
