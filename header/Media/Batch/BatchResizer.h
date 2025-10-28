#ifndef _SM_MEDIA_BATCH_BATCHRESIZER
#define _SM_MEDIA_BATCH_BATCHRESIZER
#include "Media/ImageResizer.h"
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
				NN<Text::String> targetId;
			} TargetParam;
		private:
			NN<Media::ImageResizer> resizer;
			Optional<Media::Batch::BatchHandler> hdlr;
			Data::ArrayListNN<TargetParam> targetParam;
			Sync::Mutex resizeMut;

		public:
			BatchResizer(NN<Media::ImageResizer> resizer, Optional<Media::Batch::BatchHandler> hdlr);
			virtual ~BatchResizer();
			
			void AddTargetSize(UInt32 targetWidth, UInt32 targetHeight, NN<Text::String> targetId);
			void AddTargetDPI(UInt32 targetHDPI, UInt32 targetVDPI, NN<Text::String> targetId);
			void ClearTargetSizes();
			void SetHandler(Optional<Media::Batch::BatchHandler> hdlr);
		private:
			virtual void ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId);
		};
	}
}
#endif
