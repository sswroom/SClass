#ifndef _SM_MEDIA_BATCH_BATCHSAVEJPEGSIZE
#define _SM_MEDIA_BATCH_BATCHSAVEJPEGSIZE
#include "Exporter/GUIJPGExporter.h"
#include "Media/Batch/BatchSaver.h"
#include "Sync/Mutex.h"
namespace Media
{
	namespace Batch
	{
		class BatchSaveJPEGSize : public BatchSaver
		{
		private:
			Exporter::GUIJPGExporter exporter;
			Sync::Mutex mut;
			UInt32 sizePercent;

		public:
			BatchSaveJPEGSize(UInt32 sizePercent);
			virtual ~BatchSaveJPEGSize();
			
			virtual void ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN targetId);
		};
	}
}
#endif
