#ifndef _SM_MEDIA_BATCH_BATCHRESIZER
#define _SM_MEDIA_BATCH_BATCHRESIZER
#include "Media/IImgResizer.h"
#include "Media/Batch/BatchSaver.h"
#include "Sync/Mutex.h"
#include "Parser/ParserList.h"

namespace Media
{
	namespace Batch
	{
		class BatchResizer : public BatchHandler
		{
		private:
			typedef struct
			{
				UInt32 width;
				UInt32 height;
				Int32 sizeType;
				Text::String *targetId;
			} TargetParam;
		private:
			Media::IImgResizer *resizer;
			Media::Batch::BatchHandler *hdlr;
			Data::ArrayList<TargetParam*> *targetParam;
			Sync::Mutex *resizeMut;

		public:
			BatchResizer(Media::IImgResizer *resizer, Media::Batch::BatchHandler *hdlr);
			virtual ~BatchResizer();
			
			void AddTargetSize(UInt32 targetWidth, UInt32 targetHeight, Text::String *targetId);
			void AddTargetDPI(UInt32 targetHDPI, UInt32 targetVDPI, Text::String *targetId);
			void ClearTargetSizes();
			void SetHandler(Media::Batch::BatchHandler *hdlr);
		private:
			virtual void ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *subId);
		};
	}
}
#endif
