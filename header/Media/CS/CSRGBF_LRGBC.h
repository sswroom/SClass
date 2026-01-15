#ifndef _SM_MEDIA_CS_CSRGBF_LRGBC
#define _SM_MEDIA_CS_CSRGBF_LRGBC
#include "Media/CS/CSConverter.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSRGBF_LRGBC : public Media::CS::CSConverter
		{
		private:
			UIntOS srcNBits;
			Media::PixelFormat srcPF;
			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;
			Bool invert;

			Media::ColorHandler::RGBPARAM2 rgbParam;
			UnsafeArrayOpt<UInt8> rgbTable;
			Bool rgbUpdated;
		private:
			void UpdateRGBTable();
		public:
			CSRGBF_LRGBC(UIntOS srcNBits, Media::PixelFormat srcPF, Bool invert, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSRGBF_LRGBC();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UIntOS GetSrcFrameSize(UIntOS width, UIntOS height);
			virtual UIntOS GetDestFrameSize(UIntOS width, UIntOS height);
			virtual void SetPalette(UnsafeArray<UInt8> pal);

			virtual void YUVParamChanged(NN<const YUVPARAM> yuv);
			virtual void RGBParamChanged(NN<const RGBPARAM2> rgb);
		};
	}
}
#endif
