#ifndef _SM_MEDIA_BATCH_BATCHSAVEJPEG
#define _SM_MEDIA_BATCH_BATCHSAVEJPEG
#include "Exporter/GUIJPGExporter.h"
#include "Media/Batch/BatchSaver.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace Batch
	{
		class BatchSaveJPEG : public BatchSaver
		{
		private:
			Exporter::GUIJPGExporter exporter;
			Sync::Mutex mut;
			UInt32 quality;

		public:
			BatchSaveJPEG(UInt32 quality);
			virtual ~BatchSaveJPEG();
			
			virtual void ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN targetId);
		};
	}
}
#endif
