#ifndef _SM_MEDIA_BATCH_BATCHTO32BPP
#define _SM_MEDIA_BATCH_BATCHTO32BPP
#include "Media/Batch/BatchHandler.h"

namespace Media
{
	namespace Batch
	{
		class BatchTo32bpp : public BatchHandler
		{
		private:
			Optional<Media::Batch::BatchHandler> hdlr;

		public:
			BatchTo32bpp(Optional<Media::Batch::BatchHandler> hdlr);
			virtual ~BatchTo32bpp();
			
			void SetHandler(Optional<Media::Batch::BatchHandler> hdlr);
		private:
			virtual void ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId);
		};
	}
}
#endif
