#ifndef _SM_MEDIA_BATCH_BATCHWATERMARKER
#define _SM_MEDIA_BATCH_BATCHWATERMARKER
#include "Data/RandomOS.h"
#include "Media/DrawEngine.h"
#include "Media/ABlend/AlphaBlend8_8.h"
#include "Media/Batch/BatchHandler.h"

namespace Media
{
	namespace Batch
	{
		class BatchWatermarker : public BatchHandler
		{
		private:
			Data::RandomOS *rnd;
			Media::DrawEngine *deng;
			Media::ABlend::AlphaBlend8_8 *ablend;
			const UTF8Char *watermark;
			Media::Batch::BatchHandler *hdlr;

		public:
			BatchWatermarker(Media::DrawEngine *deng, Media::Batch::BatchHandler *hdlr);
			virtual ~BatchWatermarker();
			
			void SetWatermark(const UTF8Char *watermark);
			void SetHandler(Media::Batch::BatchHandler *hdlr);
		private:
			virtual void ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *subId);
		};
	};
};
#endif
