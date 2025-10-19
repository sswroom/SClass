#ifndef _SM_MEDIA_BATCH_BATCHTO64BPP
#define _SM_MEDIA_BATCH_BATCHTO64BPP
#include "Media/Batch/BatchHandler.h"

namespace Media
{
	namespace Batch
	{
		class BatchTo64bpp : public BatchHandler
		{
		private:
			Optional<Media::Batch::BatchHandler> hdlr;

		public:
			BatchTo64bpp(Optional<Media::Batch::BatchHandler> hdlr);
			virtual ~BatchTo64bpp();
			
			void SetHandler(Optional<Media::Batch::BatchHandler> hdlr);
		private:
			virtual void ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId);
		};
	};
};
#endif
