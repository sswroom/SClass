#ifndef _SM_MEDIA_RESIZER_NEARESTNEIGHBOURRESIZER32_32
#define _SM_MEDIA_RESIZER_NEARESTNEIGHBOURRESIZER32_32
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Media/ImageResizer.h"

namespace Media
{
	namespace Resizer
	{
		class NearestNeighbourResizer32_32 : public Media::ImageResizer
		{
		private:
			IntOS lastsbpl;
			Double lastswidth;
			Double lastsheight;
			UIntOS lastdwidth;
			UIntOS lastdheight;
			UnsafeArrayOpt<IntOS> xindex;
			UnsafeArrayOpt<IntOS> yindex;
		public:
			NearestNeighbourResizer32_32();
			virtual ~NearestNeighbourResizer32_32();

			virtual void Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight);
			virtual Bool Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg);
			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);
		};
	}
}
#endif
