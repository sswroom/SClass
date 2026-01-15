#ifndef _SM_MEDIA_RESIZER_NEARESTNEIGHBOURRESIZER64_64
#define _SM_MEDIA_RESIZER_NEARESTNEIGHBOURRESIZER64_64
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Media/ImageResizer.h"

namespace Media
{
	namespace Resizer
	{
		class NearestNeighbourResizer64_64 : public Media::ImageResizer
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
			NearestNeighbourResizer64_64();
			virtual ~NearestNeighbourResizer64_64();

			virtual void Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight);
			virtual Bool Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg);
			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);
		};
	}
}
#endif
