#ifndef _SM_MEDIA_COLORSESS
#define _SM_MEDIA_COLORSESS
#include "Media/IColorHandler.h"

namespace Media
{
	class ColorSess
	{
	public:
		virtual void AddHandler(NN<Media::IColorHandler> hdlr) = 0;
		virtual void RemoveHandler(NN<Media::IColorHandler> hdlr) = 0;
//		NN<const Media::IColorHandler::YUVPARAM> GetYUVParam();
		virtual NN<const Media::IColorHandler::RGBPARAM2> GetRGBParam() = 0;
		virtual NN<Media::ColorProfile> GetDefVProfile() = 0;
		virtual NN<Media::ColorProfile> GetDefPProfile() = 0;
//		Media::ColorProfile::YUVType GetDefYUVType();
//		Bool Get10BitColor();

//		void ChangeMonitor(void *hMon);

//		void RGBUpdated(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);
//		void YUVUpdated(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
	};
};
#endif
