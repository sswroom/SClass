#ifndef _SM_MEDIA_COLORSESS
#define _SM_MEDIA_COLORSESS
#include "Media/ColorHandler.h"

namespace Media
{
	class ColorSess
	{
	public:
		virtual void AddHandler(NN<Media::ColorHandler> hdlr) = 0;
		virtual void RemoveHandler(NN<Media::ColorHandler> hdlr) = 0;
//		NN<const Media::ColorHandler::YUVPARAM> GetYUVParam();
		virtual NN<const Media::ColorHandler::RGBPARAM2> GetRGBParam() = 0;
		virtual NN<Media::ColorProfile> GetDefVProfile() = 0;
		virtual NN<Media::ColorProfile> GetDefPProfile() = 0;
//		Media::ColorProfile::YUVType GetDefYUVType();
//		Bool Get10BitColor();

//		void ChangeMonitor(void *hMon);

//		void RGBUpdated(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
//		void YUVUpdated(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
	};
};
#endif
