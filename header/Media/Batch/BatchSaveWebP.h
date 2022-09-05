#ifndef _SM_MEDIA_BATCH_BATCHSAVEWEBP
#define _SM_MEDIA_BATCH_BATCHSAVEWEBP
#include "Exporter/WebPExporter.h"
#include "Media/Batch/BatchSaver.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace Batch
	{
		class BatchSaveWebP : public BatchSaver
		{
		private:
			Exporter::WebPExporter exporter;
			Sync::Mutex mut;
			UInt32 quality;

		public:
			BatchSaveWebP(UInt32 quality);
			virtual ~BatchSaveWebP();
			
			virtual void ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *targetId);
		};
	}
}
#endif
