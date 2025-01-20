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
			NN<TransferFunc> frFunc;
			NN<TransferFunc> fgFunc;
			NN<TransferFunc> fbFunc;
			NN<TransferFunc> irFunc;
			NN<TransferFunc> igFunc;
			NN<TransferFunc> ibFunc;
			Media::ColorProfile srcColor;
			Media::ColorProfile destColor;
			Media::ColorProfile::YUVType yuvType;

		protected:
			UnsafeArray<UInt8> rgbGammaCorr;
			UnsafeArray<Int64> yuv2rgb;

		private:
			void SetupRGB13_8();
			void SetupYUV_RGB13();

		protected:
			CSYUV10_RGB8(NN<const Media::ColorProfile> srcColor, NN<const Media::ColorProfile> destColor, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			void UpdateTable();
			void Release();

		public:
			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuv);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgb);
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height);
		};
	}
}
#endif
