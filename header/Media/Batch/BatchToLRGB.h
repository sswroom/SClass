#ifndef _SM_MEDIA_BATCH_BATCHTOLRGB
#define _SM_MEDIA_BATCH_BATCHTOLRGB
#include "Media/Batch/BatchHandler.h"
#include "Media/CS/CSConverter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace Batch
	{
		class BatchToLRGB : public BatchHandler
		{
		private:
			Sync::Mutex mut;
			Optional<Media::CS::CSConverter> csconv;
			Media::Batch::BatchHandler *hdlr;
			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;
			UInt32 srcFCC;
			UInt32 srcBpp;
			Media::PixelFormat srcPF;

		public:
			BatchToLRGB(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::Batch::BatchHandler *hdlr);
			virtual ~BatchToLRGB();
			
			void SetHandler(Media::Batch::BatchHandler *hdlr);
		private:
			virtual void ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId);
		};
	}
}
#endif
