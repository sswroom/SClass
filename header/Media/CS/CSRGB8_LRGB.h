#ifndef _SM_MEDIA_CS_CSRGB8_LRGB
#define _SM_MEDIA_CS_CSRGB8_LRGB
#include "Media/CS/CSConverter.h"
#include "Sync/Event.h"

namespace Media
{
	namespace CS
	{
		class CSRGB8_LRGB : public Media::CS::CSConverter
		{
		private:
			UIntOS srcNBits;
			Media::ColorProfile srcProfile;
			Media::CS::TransferType currRGBType;
			Bool invert;

			Media::ColorHandler::RGBPARAM2 rgbParam;
			UInt8 *srcPal;
			UInt8 *destPal;
			UInt8 *rgbTable;
			Bool rgbUpdated;
		private:
			void UpdateRGBTable();
		public:
			CSRGB8_LRGB(UIntOS srcNBits, Bool invert, NN<Media::ColorProfile> srcProfile, Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSRGB8_LRGB();
			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst);
			virtual UIntOS GetSrcFrameSize(UIntOS width, UIntOS height);
			virtual UIntOS GetDestFrameSize(UIntOS width, UIntOS height);
			virtual void SetPalette(UInt8 *pal);

			virtual void YUVParamChanged(NN<const YUVPARAM> yuv);
			virtual void RGBParamChanged(NN<const RGBPARAM2> rgb);
		};
	}
}
#endif
