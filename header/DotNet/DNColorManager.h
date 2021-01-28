#ifndef _SM_DOTNET_DNCOLORMANAGER
#define _SM_DOTNET_DNCOLORMANAGER
#include "Media/ColorManager.h"
#include "DotNet/DNColorHandler.h"

namespace DotNet
{
	public __gc class DNColorManager
	{
	private:
		Media::ColorManager *colorMgr;
		System::Collections::ArrayList *hdlrs;

	public:
		DNColorManager(Media::ColorManager *colorMgr);
		~DNColorManager();

		void AddHandler(DNColorHandler *hdlr);
		void RemoveHandler(DNColorHandler *hdlr);
		Media::ColorManager *GetColorMgr();

		void RGBUpdated();
		void YUVUpdated();
	};
};
#endif
