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
			virtual void ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *subId) = 0;
		};
	};
};
#endif
