#ifndef _SM_MEDIA_CS_CSRGB8_RGB8
#define _SM_MEDIA_CS_CSRGB8_RGB8
#include "Media/CS/CSConverter.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSRGB8_RGB8 : public Media::CS::CSConverter
		{
		private:
			UIntOS srcNBits;
//			Media::PixelFormat srcPF;
			UIntOS destNBits;
//			Media::PixelFormat destPF;
			Media::ColorProfile srcColor;
			Media::ColorProfile destColor;
			Bool invert;

			Media::ColorHandler::RGBPARAM2 rgbParam;
			UnsafeArrayOpt<UInt8> srcPal;
			UnsafeArrayOpt<UInt8> destPal;
			UnsafeArrayOpt<UInt8> rgbTable;
			Bool rgbUpdated;
		private:
			void UpdateRGBTable();
		public:
			CSRGB8_RGB8(UIntOS srcNBits, Media::PixelFormat srcPF, UIntOS destNBits, Media::PixelFormat destPF, Bool invert, NN<const Media::ColorProfile> srcColor, NN<const Media::ColorProfile> destColor, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSRGB8_RGB8();
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
