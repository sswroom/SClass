#ifndef _SM_MEDIA_CS_CSI420_RGB32C
#define _SM_MEDIA_CS_CSI420_RGB32C
#include "Media/CS/CSYUV420_RGB32C.h"

namespace Media
{
	namespace CS
	{
		class CSI420_RGB32C : public Media::CS::CSYUV420_RGB32C
		{
		public:
			CSI420_RGB32C(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess, Media::PixelFormat destPF);
			virtual ~CSI420_RGB32C();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
		};
	}
}
#endif
