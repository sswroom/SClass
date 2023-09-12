#ifndef _SM_MEDIA_CS_CSYUV_RGB8
#define _SM_MEDIA_CS_CSYUV_RGB8
#include "Media/CS/CSConverter.h"
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class CSYUV_RGB8 : public Media::CS::CSConverter
		{
		protected:
			YUVPARAM yuvParam;
			RGBPARAM2 rgbParam;
			NotNullPtr<TransferFunc> frFunc;
			NotNullPtr<TransferFunc> fgFunc;
			NotNullPtr<TransferFunc> fbFunc;
			NotNullPtr<TransferFunc> irFunc;
			NotNullPtr<TransferFunc> igFunc;
			NotNullPtr<TransferFunc> ibFunc;
			Media::ColorProfile srcColor;
			Media::ColorProfile destColor;
			Media::ColorProfile::YUVType yuvType;

		protected:
			Bool yuvUpdated;
			Bool rgbUpdated;
			UInt8 *rgbGammaCorr;
			Int64 *yuv2rgb;

			void SetupRGB13_8();
			void SetupYUV_RGB13();

		protected:
			CSYUV_RGB8(NotNullPtr<const Media::ColorProfile> srcColor, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSYUV_RGB8();
			virtual void UpdateTable();

		public:
			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuv);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgb);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
