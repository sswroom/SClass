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
				Int32 width;
				Int32 height;
				Int32 sizeType;
				const UTF8Char *targetId;
			} TargetParam;
		private:
			Media::IImgResizer *resizer;
			Media::Batch::BatchHandler *hdlr;
			Data::ArrayList<TargetParam*> *targetParam;
			Sync::Mutex *resizeMut;

		public:
			BatchResizer(Media::IImgResizer *resizer, Media::Batch::BatchHandler *hdlr);
			virtual ~BatchResizer();
			
			void AddTargetSize(Int32 targetWidth, Int32 targetHeight, const UTF8Char *targetId);
			void AddTargetDPI(Int32 targetHDPI, Int32 targetVDPI, const UTF8Char *targetId);
			void ClearTargetSizes();
			void SetHandler(Media::Batch::BatchHandler *hdlr);
		private:
			virtual void ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *subId);
		};
	};
};
#endif
