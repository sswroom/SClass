#ifndef _SM_MEDIA_CS_CSCONVERTER
#define _SM_MEDIA_CS_CSCONVERTER
#include "Data/ArrayListUInt32.h"
#include "Media/ColorManager.h"
#include "Media/FrameInfo.h"
#include "Text/CString.h"

namespace Media
{
	namespace CS
	{
		class CSConverter : public Media::ColorHandler
		{
		protected:
			Optional<Media::ColorManagerSess> colorSess;
		public:
			CSConverter(Optional<Media::ColorManagerSess> colorSess);
			virtual ~CSConverter();

			virtual void ConvertV2(UnsafeArray<const UnsafeArray<UInt8>> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS dispWidth, UIntOS dispHeight, UIntOS srcStoreWidth, UIntOS srcStoreHeight, IntOS destLineAdd, Media::FrameType ftype, Media::YCOffset ycOfst) = 0;
			virtual UIntOS GetSrcFrameSize(UIntOS width, UIntOS height) = 0;
			virtual UIntOS GetDestFrameSize(UIntOS width, UIntOS height) = 0;
			virtual void SetPalette(UnsafeArray<UInt8> pal);

			static UIntOS GetSupportedCS(NN<Data::ArrayListUInt32> csList);
			static Bool IsSupported(UInt32 fourcc);
			static Optional<CSConverter> NewConverter(UInt32 srcFormat, UIntOS srcNBits, Media::PixelFormat srcPF, NN<const Media::ColorProfile> srcProfile, UInt32 destFormat, UIntOS destNBits, Media::PixelFormat destPF, NN<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Optional<Media::ColorManagerSess> colorSess);
			static Text::CStringNN GetFormatName(UInt32 format);
		};
	}
}
#endif
