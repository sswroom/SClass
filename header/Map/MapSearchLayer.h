#ifndef _SM_MAP_MAPSEARCHLAYER
#define _SM_MAP_MAPSEARCHLAYER
#include "Math/Coord2DDbl.h"

namespace Map
{
	class MapSearchLayer
	{
	public:
		virtual ~MapSearchLayer() {};

		virtual Bool IsError() const = 0;
		virtual UTF8Char *GetPGLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex) = 0;
		virtual UTF8Char *GetPLLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex) = 0;
	};
}
#endif
