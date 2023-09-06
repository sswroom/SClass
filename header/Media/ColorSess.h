#ifndef _SM_MEDIA_COLORSESS
#define _SM_MEDIA_COLORSESS
#include "Media/IColorHandler.h"

namespace Media
{
	class ColorSess
	{
	public:
		virtual void AddHandler(Media::IColorHandler *hdlr) = 0;
		virtual void RemoveHandler(Media::IColorHandler *hdlr) = 0;
//		NotNullPtr<const Media::IColorHandler::YUVPARAM> GetYUVParam();
		virtual NotNullPtr<const Media::IColorHandler::RGBPARAM2> GetRGBParam() = 0;
		virtual NotNullPtr<Media::ColorProfile> GetDefVProfile() = 0;
		virtual NotNullPtr<Media::ColorProfile> GetDefPProfile() = 0;
//		Media::ColorProfile::YUVType GetDefYUVType();
//		Bool Get10BitColor();

//		void ChangeMonitor(void *hMon);

//		void RGBUpdated(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);
//		void YUVUpdated(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
	};
};
#endif
