#ifndef _SM_MEDIA_CS_CSYV12_LRGBC
#define _SM_MEDIA_CS_CSYV12_LRGBC
#include "Media/CS/CSYUV420_LRGBC.h"

namespace Media
{
	namespace CS
	{
		class CSYV12_LRGBC : public Media::CS::CSYUV420_LRGBC
		{
		public:
			CSYV12_LRGBC(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSYV12_LRGBC();
			virtual void ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
		};
	}
}
#endif
