#ifndef _SM_MEDIA_COLORCONV
#define _SM_MEDIA_COLORCONV
#include "Media/ColorManager.h"

namespace Media
{
	class ColorConv
	{
	private:
		Media::ColorManagerSess *colorSess;
		Media::ColorProfile *srcColor;
		Media::ColorProfile *destColor;
		UInt8 *rgbTable;

	public:
		ColorConv(const Media::ColorProfile *srcColor, const Media::ColorProfile *destColor, Media::ColorManagerSess *colorSess);
		~ColorConv();

		void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);
		Int32 ConvRGB8(Int32 c);


		static Int32 ConvARGB(Media::ColorProfile *srcColor,  Media::ColorProfile *destColor, Media::ColorManagerSess *colorSess, Int32 c);
	};
};
#endif
