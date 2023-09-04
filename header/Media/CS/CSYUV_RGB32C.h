#ifndef _SM_MEDIA_CS_CSYUV_RGB32C
#define _SM_MEDIA_CS_CSYUV_RGB32C
#include "Media/CS/CSYUV_LRGB.h"

namespace Media
{
	namespace CS
	{
		class CSYUV_RGB32C : public Media::CS::CSConverter
		{
		protected:
			YUVPARAM yuvParam;
			RGBPARAM2 rgbParam;
			Media::ColorProfile::YUVType yuvType;
			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;

			Bool yuvUpdated;
			Bool rgbUpdated;
			Int64 *yuv2rgb;
			Int64 *yuv2rgb14;
			Int64 *rgbGammaCorr;
			Media::PixelFormat destPF;

			void SetupRGB13_LR();
			void SetupYUV_RGB13();
			void SetupYUV14_RGB13();

		protected:
			CSYUV_RGB32C(NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess, Media::PixelFormat destPF);
			virtual ~CSYUV_RGB32C();
			virtual void UpdateTable();

		public:
			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuv);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgb);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
