#ifndef _SM_MEDIA_DSHOW_DSHOWFILTER
#define _SM_MEDIA_DSHOW_DSHOWFILTER

namespace Media
{
	namespace DShow
	{
		class DShowManager;

		class DShowFilter
		{
		protected:
			Media::DShow::DShowManager *mgr;
			void *filter;
		public:
			DShowFilter(Media::DShow::DShowManager *mgr);
			virtual ~DShowFilter();

			virtual void *GetFilter();
			virtual const WChar *GetName() = 0;
		};
	};
};

#endif
