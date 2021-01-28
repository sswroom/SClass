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
//		const Media::IColorHandler::YUVPARAM *GetYUVParam();
		virtual const Media::IColorHandler::RGBPARAM2 *GetRGBParam() = 0;
		virtual Media::ColorProfile *GetDefVProfile() = 0;
		virtual Media::ColorProfile *GetDefPProfile() = 0;
//		Media::ColorProfile::YUVType GetDefYUVType();
//		Bool Get10BitColor();

//		void ChangeMonitor(void *hMon);

//		void RGBUpdated(const Media::IColorHandler::RGBPARAM2 *rgbParam);
//		void YUVUpdated(const Media::IColorHandler::YUVPARAM *yuvParam);
	};
};
#endif
