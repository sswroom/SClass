#ifndef _SM_MEDIA_BATCH_BATCHHANDLER
#define _SM_MEDIA_BATCH_BATCHHANDLER
#include "Media/ImageList.h"

namespace Media
{
	namespace Batch
	{
		class BatchHandler
		{
		public:
			virtual void ImageOutput(NotNullPtr<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId) = 0;
		};
	}
}
#endif
