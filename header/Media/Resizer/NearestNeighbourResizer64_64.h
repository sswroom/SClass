#ifndef _SM_MEDIA_RESIZER_NEARESTNEIGHBOURRESIZER64_64
#define _SM_MEDIA_RESIZER_NEARESTNEIGHBOURRESIZER64_64
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Media/IImgResizer.h"

namespace Media
{
	namespace Resizer
	{
		class NearestNeighbourResizer64_64 : public Media::IImgResizer
		{
		private:
			OSInt lastsbpl;
			Double lastswidth;
			Double lastsheight;
			UOSInt lastdwidth;
			UOSInt lastdheight;
			OSInt *xindex;
			OSInt *yindex;
		public:
			NearestNeighbourResizer64_64();
			virtual ~NearestNeighbourResizer64_64();

			virtual void Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
			virtual Bool Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg);
			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Media::StaticImage *ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);
		};
	}
}
#endif
