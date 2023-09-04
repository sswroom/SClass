#ifndef _SM_MEDIA_COLORCONV
#define _SM_MEDIA_COLORCONV
#include "Media/ColorManager.h"

namespace Media
{
	class ColorConv
	{
	private:
		Media::ColorManagerSess *colorSess;
		Media::ColorProfile srcColor;
		Media::ColorProfile destColor;
		UInt8 *rgbTable;

	public:
		ColorConv(NotNullPtr<const Media::ColorProfile> srcColor, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorManagerSess *colorSess);
		~ColorConv();

		void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);
		UInt32 ConvRGB8(UInt32 c);


		static UInt32 ConvARGB(NotNullPtr<const Media::ColorProfile> srcColor, NotNullPtr<const Media::ColorProfile> destColor, Media::ColorManagerSess *colorSess, UInt32 c);
	};
}
#endif
