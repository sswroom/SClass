#ifndef _SM_MEDIA_DSHOW_DSHOWVMR9FILTER
#define _SM_MEDIA_DSHOW_DSHOWVMR9FILTER
#include "Media/DShow/DShowFilter.h"

namespace Media
{
	namespace DShow
	{
		class DShowVMR9Filter : public DShowFilter
		{
		public:
			DShowVMR9Filter(Media::DShow::DShowManager *mgr, void *hwnd);
			virtual ~DShowVMR9Filter();

			virtual const WChar *GetName();
		};
	};
};

#endif
