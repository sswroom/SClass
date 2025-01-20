#ifndef _SM_MEDIA_COLORCONV
#define _SM_MEDIA_COLORCONV
#include "Media/ColorManager.h"

namespace Media
{
	class ColorConv
	{
	private:
		Optional<Media::ColorManagerSess> colorSess;
		Media::ColorProfile srcColor;
		Media::ColorProfile destColor;
		UnsafeArray<UInt8> rgbTable;

	public:
		ColorConv(NN<const Media::ColorProfile> srcColor, NN<const Media::ColorProfile> destColor, Optional<Media::ColorManagerSess> colorSess);
		~ColorConv();

		void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
		UInt32 ConvRGB8(UInt32 c);


		static UInt32 ConvARGB(NN<const Media::ColorProfile> srcColor, NN<const Media::ColorProfile> destColor, Optional<Media::ColorManagerSess> colorSess, UInt32 c);
	};
}
#endif
