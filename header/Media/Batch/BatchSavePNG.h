#ifndef _SM_MEDIA_BATCH_BATCHSAVEPNG
#define _SM_MEDIA_BATCH_BATCHSAVEPNG
#include "Exporter/GUIPNGExporter.h"
#include "Media/Batch/BatchSaver.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace Batch
	{
		class BatchSavePNG : public BatchSaver
		{
		private:
			Exporter::GUIPNGExporter exporter;
			Sync::Mutex mut;

		public:
			BatchSavePNG();
			virtual ~BatchSavePNG();
			
			virtual void ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN targetId);
		};
	}
}
#endif
