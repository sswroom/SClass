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
			Media::Batch::BatchHandler *hdlr;

		public:
			BatchLimiter(Media::Batch::BatchHandler *hdlr);
			virtual ~BatchLimiter();
			
			void SetHandler(Media::Batch::BatchHandler *hdlr);
		private:
			virtual void ImageOutput(NotNullPtr<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId);
		};
	}
}
#endif
