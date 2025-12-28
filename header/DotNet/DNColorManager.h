#ifndef _SM_DOTNET_DNCOLORMANAGER
#define _SM_DOTNET_DNCOLORMANAGER
#include "Media/ColorManager.h"
#include "DotNet/DNColorHandler.h"

namespace DotNet
{
	public __gc class DNColorManager
	{
	private:
		NN<Media::ColorManager> colorMgr;
		System::Collections::ArrayList ^hdlrs;

	public:
		DNColorManager(NN<Media::ColorManager> colorMgr);
		~DNColorManager();

		void AddHandler(NN<DNColorHandler> hdlr);
		void RemoveHandler(NN<DNColorHandler> hdlr);
		NN<Media::ColorManager> GetColorManager();

		void RGBUpdated();
		void YUVUpdated();
	};
}
#endif
