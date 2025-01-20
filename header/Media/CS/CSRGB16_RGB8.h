#ifndef _SM_MEDIA_CS_CSRGB16_RGB8
#define _SM_MEDIA_CS_CSRGB16_RGB8
#include "Media/CS/CSConverter.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSRGB16_RGB8 : public Media::CS::CSConverter
		{
		private:
			UOSInt srcNBits;
			Media::PixelFormat srcPF;
			UOSInt destNBits;
			Media::PixelFormat destPF;
			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;
			Bool invert;

			Media::ColorHandler::RGBPARAM2 rgbParam;
			UInt8 *rgbTable;
			Bool rgbUpdated;
		private:
			void UpdateRGBTable();
		public:
			CSRGB16_RGB8(UOSInt srcNBits, Media::PixelFormat srcPF, UOSInt destNBits, Media::PixelFormat destPF, Bool invert, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSRGB16_RGB8();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);

			virtual void YUVParamChanged(NN<const YUVPARAM> yuv);
			virtual void RGBParamChanged(NN<const RGBPARAM2> rgb);
		};
	}
}
#endif
