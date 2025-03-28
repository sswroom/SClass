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
			Data::RandomOS rnd;
			NN<Media::DrawEngine> deng;
			Media::ABlend::AlphaBlend8_8 ablend;
			Optional<Text::String> watermark;
			Media::Batch::BatchHandler *hdlr;

		public:
			BatchWatermarker(NN<Media::DrawEngine> deng, Media::Batch::BatchHandler *hdlr);
			virtual ~BatchWatermarker();
			
			void SetWatermark(Text::CString watermark);
			void SetHandler(Media::Batch::BatchHandler *hdlr);
		private:
			virtual void ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId);
		};
	}
}
#endif
