#ifndef _SM_MEDIA_BATCH_BATCHSAVETIFF
#define _SM_MEDIA_BATCH_BATCHSAVETIFF
#include "Exporter/GUITIFExporter.h"
#include "Media/Batch/BatchSaver.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace Batch
	{
		class BatchSaveTIFF : public BatchSaver
		{
		private:
			Exporter::GUITIFExporter exporter;
			Sync::Mutex mut;
			Bool isCompressed;

		public:
			BatchSaveTIFF(Bool isCompressed);
			virtual ~BatchSaveTIFF();
			
			virtual void ImageOutput(NotNullPtr<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN targetId);
		};
	}
}
#endif
