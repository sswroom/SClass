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
		class CSConverter : public Media::IColorHandler
		{
		protected:
			Media::ColorManagerSess *colorSess;
		public:
			CSConverter(Media::ColorManagerSess *colorSess);
			virtual ~CSConverter();

			virtual void ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destLineAdd, Media::FrameType ftype, Media::YCOffset ycOfst) = 0;
			virtual UOSInt GetSrcFrameSize(UOSInt width, UOSInt height) = 0;
			virtual UOSInt GetDestFrameSize(UOSInt width, UOSInt height) = 0;
			virtual void SetPalette(UInt8 *pal);

			static UOSInt GetSupportedCS(Data::ArrayListUInt32 *csList);
			static Bool IsSupported(UInt32 fourcc);
			static CSConverter *NewConverter(UInt32 srcFormat, UOSInt srcNBits, Media::PixelFormat srcPF, const Media::ColorProfile *srcProfile, UInt32 destFormat, UOSInt destNBits, Media::PixelFormat destPF, const Media::ColorProfile *destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess);
			static Text::CString GetFormatName(UInt32 format);
		};
	}
}
#endif
