#ifndef _SM_DOTNET_DNCOLORHANDLER
#define _SM_DOTNET_DNCOLORHANDLER
#include "Media/IColorHandler.h"
namespace DotNet
{
	public __gc __interface DNColorHandler
	{
	public:
		void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
		void RGBParamChanged(const Media::IColorHandler::RGBPARAM *rgbParam);
	};
};
#endif
