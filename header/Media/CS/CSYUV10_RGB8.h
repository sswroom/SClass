#ifndef _SM_MEDIA_CS_CSYUV10_RGB8
#define _SM_MEDIA_CS_CSYUV10_RGB8
#include "Media/CS/CSConverter.h"
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class CSYUV10_RGB8 : public Media::CS::CSConverter
		{
		private:
			Bool yuvUpdated;
			YUVPARAM yuvParam;
			Bool rgbUpdated;
			RGBPARAM2 rgbParam;
			TransferFunc *frFunc;
			TransferFunc *fgFunc;
			TransferFunc *fbFunc;
			TransferFunc *irFunc;
			TransferFunc *igFunc;
			TransferFunc *ibFunc;
			Media::ColorProfile srcColor;
			Media::ColorProfile destColor;
			Media::ColorProfile::YUVType yuvType;

		protected:
			UInt8 *rgbGammaCorr;
			Int64 *yuv2rgb;

		private:
			void SetupRGB13_8();
			void SetupYUV_RGB13();

		protected:
			CSYUV10_RGB8(NotNullPtr<const Media::ColorProfile> srcColor, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			void UpdateTable();
			void Release();

		public:
			virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuv);
			virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgb);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
