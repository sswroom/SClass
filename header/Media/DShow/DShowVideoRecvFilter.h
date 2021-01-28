#ifndef _SM_MEDIA_DSHOW_DSHOWVIDEORECVFILTER
#define _SM_MEDIA_DSHOW_DSHOWVIDEORECVFILTER
#include "Media/DShow/DShowFilter.h"

namespace Media
{
	namespace DShow
	{
		class DShowVideoRecvFilter : public DShowFilter
		{
		public:
			typedef void (__stdcall *VFrame32Hdlr)(void *userObj, UInt8 *frameBuff, Int32 frameTime, Int32 frameW, Int32 frmaeH);
		private:
			VFrame32Hdlr hdlr;
			void *userObj;
		public:
			DShowVideoRecvFilter(Media::DShow::DShowManager *mgr, VFrame32Hdlr hdlr, void *userObj);
			virtual ~DShowVideoRecvFilter();

			virtual const WChar *GetName();
		};
	};
};

#endif
