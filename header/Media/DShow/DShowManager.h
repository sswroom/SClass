#ifndef _SM_MEDIA_DSHOW_DSHOWMANAGER
#define _SM_MEDIA_DSHOW_DSHOWMANAGER
#include "Media/DShow/DShowVideoRecvFilter.h"

namespace Media
{
	namespace DShow
	{
		class DShowGraph;
		class DShowEVRFilter;
		class DShowVMR9Filter;

		class DShowManager
		{
		public:
			DShowManager();
			~DShowManager();

			DShowGraph *CreateGraph(void *hwnd);
			DShowEVRFilter *CreateEVR(void *hwnd);
			DShowVMR9Filter *CreateVMR9(void *hwnd);
			DShowVideoRecvFilter *CreateVideoRecvFilter(Media::DShow::DShowVideoRecvFilter::VFrame32Hdlr hdlr, void *userObj);
		};
	};
};
#endif
