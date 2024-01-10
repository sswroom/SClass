#ifndef _SM_MEDIA_RESIZER_NEARESTNEIGHBOURRESIZER32_32
#define _SM_MEDIA_RESIZER_NEARESTNEIGHBOURRESIZER32_32
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Media/IImgResizer.h"

namespace Media
{
	namespace Resizer
	{
		class NearestNeighbourResizer32_32 : public Media::IImgResizer
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
			NearestNeighbourResizer32_32();
			virtual ~NearestNeighbourResizer32_32();

			virtual void Resize(const UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
			virtual Bool Resize(NotNullPtr<const Media::StaticImage> srcImg, NotNullPtr<Media::StaticImage> destImg);
			virtual Bool IsSupported(NotNullPtr<const Media::FrameInfo> srcInfo);
			virtual Media::StaticImage *ProcessToNewPartial(NotNullPtr<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);
		};
	}
}
#endif
