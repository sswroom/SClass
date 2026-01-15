#ifndef _SM_MEDIA_CS_CSYUV16_RGB32C
#define _SM_MEDIA_CS_CSYUV16_RGB32C
#include "Media/CS/CSYUV_LRGB.h"

namespace Media
{
	namespace CS
	{
		class CSYUV16_RGB32C : public Media::CS::CSConverter
		{
		protected:
			YUVPARAM yuvParam;
			RGBPARAM2 rgbParam;
			Media::ColorProfile::YUVType yuvType;
			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;

			Bool yuvUpdated;
			Bool rgbUpdated;
			UnsafeArray<Int64> yuv2rgb;
			UnsafeArray<Int64> yuv2rgb14;
			UnsafeArray<Int64> rgbGammaCorr;
			Media::PixelFormat destPF;

			void SetupRGB13_LR();
			void SetupYUV_RGB13();
			void SetupYUV14_RGB13();

		protected:
			CSYUV16_RGB32C(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess, Media::PixelFormat destPF);
			virtual ~CSYUV16_RGB32C();
			virtual void UpdateTable();

		public:
			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuv);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgb);
			virtual UIntOS GetDestFrameSize(UIntOS width, UIntOS height);
		};
	}
}
#endif
