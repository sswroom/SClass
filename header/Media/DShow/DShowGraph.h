#ifndef _SM_MEDIA_DSHOW_DSHOWGRAPH
#define _SM_MEDIA_DSHOW_DSHOWGRAPH

namespace Media
{
	namespace DShow
	{
		class DShowManager;
		class DShowFilter;

		class DShowGraph
		{
		private:
			Media::DShow::DShowManager *mgr;
			void *hwnd;
			void *graphBuilder;
			void *pControl;
			void *pEvent;
			void *pSeek;
			Bool completed;

			UInt32 lastHR;
		public:
			DShowGraph(Media::DShow::DShowManager *mgr, void *hwnd);
			~DShowGraph();

			Bool AddFilter(DShowFilter *filter);
			Bool RenderFile(const WChar *fileName);
			Bool Run();
			Bool IsCompleted();
			Bool SeekToTime(Int32 time);
			Bool CheckStatus();
		};
	};
};

#endif
