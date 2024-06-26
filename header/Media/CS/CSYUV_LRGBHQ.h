#ifndef _SM_MEDIA_CS_CSYUV_LRGBHQ
#define _SM_MEDIA_CS_CSYUV_LRGBHQ
#include "Media/CS/CSYUV_LRGB.h"

namespace Media
{
	namespace CS
	{
		class CSYUV_LRGBHQ : public Media::CS::CSYUV_LRGB
		{
		protected:
			UnsafeArray<Int64> yuv2rgb14;

		protected:
			void SetupYUV14_RGB13();

		protected:
			CSYUV_LRGBHQ(NN<const Media::ColorProfile> srcColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSYUV_LRGBHQ();

			virtual void UpdateTable();
		};
	}
}
#endif
