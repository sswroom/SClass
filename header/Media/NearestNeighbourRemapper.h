#ifndef _SM_MEDIA_NEARESTNEIGHBOURREMAPPER
#define _SM_MEDIA_NEARESTNEIGHBOURREMAPPER
#include "Media/ImageRemapper.h"

namespace Media
{
	class NearestNeighbourRemapper : public ImageRemapper
	{
	private:
		UInt32 GetPixel32(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord);
		virtual void DoRemap(UnsafeArray<const UInt8> srcImgPtr, UnsafeArray<UInt8> destImgPtr);
	public:
		NearestNeighbourRemapper();
		virtual ~NearestNeighbourRemapper();
	};
}

#endif
