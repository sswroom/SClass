#ifndef _SM_MAP_IMAPSEARCHLAYER
#define _SM_MAP_IMAPSEARCHLAYER
#include "Math/Coord2DDbl.h"

namespace Map
{
	class IMapSearchLayer
	{
	public:
		virtual ~IMapSearchLayer() {};

		virtual Bool IsError() = 0;
		virtual UTF8Char *GetPGLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex) = 0;
		virtual UTF8Char *GetPLLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex) = 0;
	};
}
#endif
