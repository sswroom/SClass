#ifndef _SM_MEDIA_IMGREMAPPER_NEARESTNEIGHBOURREMAPPER
#define _SM_MEDIA_IMGREMAPPER_NEARESTNEIGHBOURREMAPPER
#include "Media/ImageRemapper.h"

namespace Media
{
	namespace ImgRemapper
	{
		class NearestNeighbourRemapper : public ImageRemapper
		{
		private:
			static UInt32 __stdcall GetPixel32_B8G8R8A8(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self);
			static UInt32 __stdcall GetPixel32_PAL8(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self);
			static UInt32 __stdcall GetPixel32_PAL4(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self);
			static UInt32 __stdcall GetPixel32_PAL1(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self);
			virtual GetPixel32Func GetPixel32();
		public:
			NearestNeighbourRemapper();
			virtual ~NearestNeighbourRemapper();
		};
	}
}

#endif
