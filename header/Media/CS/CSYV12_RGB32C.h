#ifndef _SM_MEDIA_CS_CSYV12_RGB32C
#define _SM_MEDIA_CS_CSYV12_RGB32C
#include "Media/CS/CSYUV420_RGB32C.h"

namespace Media
{
	namespace CS
	{
		class CSYV12_RGB32C : public Media::CS::CSYUV420_RGB32C
		{
		public:
			CSYV12_RGB32C(const Media::ColorProfile *srcProfile, const Media::ColorProfile *destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess, Media::PixelFormat destPF);
			virtual ~CSYV12_RGB32C();
			virtual void ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
		};
	}
}
#endif
