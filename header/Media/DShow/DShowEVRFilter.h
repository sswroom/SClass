#ifndef _SM_MEDIA_DSHOW_DSHOWEVRFILTER
#define _SM_MEDIA_DSHOW_DSHOWEVRFILTER
#include "Media/DShow/DShowFilter.h"

namespace Media
{
	namespace DShow
	{
		class DShowEVRFilter : public DShowFilter
		{
		public:
			DShowEVRFilter(Media::DShow::DShowManager *mgr, void *hwnd);
			virtual ~DShowEVRFilter();

			virtual const WChar *GetName();
		};
	};
};

#endif
