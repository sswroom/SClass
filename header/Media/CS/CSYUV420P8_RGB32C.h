#ifndef _SM_MEDIA_CS_CSYUV420P8_RGB32C
#define _SM_MEDIA_CS_CSYUV420P8_RGB32C
#include "Media/CS/CSYUV420_RGB32C.h"

namespace Media
{
	namespace CS
	{
		class CSYUV420P8_RGB32C : public Media::CS::CSYUV420_RGB32C
		{
		public:
			CSYUV420P8_RGB32C(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess, Media::PixelFormat destPF);
			virtual ~CSYUV420P8_RGB32C();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
		};
	}
}
#endif
