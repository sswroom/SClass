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
			RGBPARAM2 *rgbParam;
			TransferFunc *frFunc;
			TransferFunc *fgFunc;
			TransferFunc *fbFunc;
			TransferFunc *irFunc;
			TransferFunc *igFunc;
			TransferFunc *ibFunc;
			Media::ColorProfile *srcColor;
			Media::ColorProfile *destColor;
			Media::ColorProfile::YUVType yuvType;

		protected:
			Bool yuvUpdated;
			Bool rgbUpdated;
			UInt8 *rgbGammaCorr;
			Int64 *yuv2rgb;

			void SetupRGB13_8();
			void SetupYUV_RGB13();

		protected:
			CSYUV_RGB8(const Media::ColorProfile *srcColor, const Media::ColorProfile *destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			virtual ~CSYUV_RGB8();
			virtual void UpdateTable();

		public:
			virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuv);
			virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgb);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
