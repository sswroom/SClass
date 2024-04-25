#ifndef _SM_MEDIA_CS_CSYUV_RGB8HQ
#define _SM_MEDIA_CS_CSYUV_RGB8HQ
#include "Media/CS/CSYUV_RGB8.h"

namespace Media
{
	namespace CS
	{
		class CSYUV_RGB8HQ : public Media::CS::CSYUV_RGB8
		{
		protected:
			Int64 *yuv2rgb14;

		private:
			void SetupYUV14_RGB13();

		protected:
			CSYUV_RGB8HQ(NN<const Media::ColorProfile> srcColor, NN<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSYUV_RGB8HQ();
			virtual void UpdateTable();
		};
	};
};
#endif
