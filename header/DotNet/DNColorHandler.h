#ifndef _SM_DOTNET_DNCOLORHANDLER
#define _SM_DOTNET_DNCOLORHANDLER
#include "Media/ColorHandler.h"
namespace DotNet
{
	public __gc __interface DNColorHandler
	{
	public:
		void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
		void RGBParamChanged(const Media::ColorHandler::RGBPARAM *rgbParam);
	};
};
#endif
