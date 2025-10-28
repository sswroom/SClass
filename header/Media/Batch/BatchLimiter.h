#ifndef _SM_MEDIA_BATCH_BATCHLIMITER
#define _SM_MEDIA_BATCH_BATCHLIMITER
#include "Media/Batch/BatchHandler.h"

namespace Media
{
	namespace Batch
	{
		class BatchLimiter : public BatchHandler
		{
		private:
			Optional<Media::Batch::BatchHandler> hdlr;

		public:
			BatchLimiter(Optional<Media::Batch::BatchHandler> hdlr);
			virtual ~BatchLimiter();
			
			void SetHandler(Optional<Media::Batch::BatchHandler> hdlr);
		private:
			virtual void ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId);
		};
	}
}
#endif
