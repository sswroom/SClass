#ifndef _SM_UI_WIN_WINREALTIMELINECHART
#define _SM_UI_WIN_WINREALTIMELINECHART
#include "UI/GUIRealtimeLineChart.h"

namespace UI
{
	namespace Win
	{
		class WinRealtimeLineChart : public GUIRealtimeLineChart
		{
		private:
			static Int32 useCnt;

			NN<Media::DrawEngine> eng;
			void *clsData;

		public:
			static OSInt __stdcall RLCWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
		private:
			void Init(InstanceHandle *hInst);
			void Deinit(InstanceHandle *hInst);
		public:
			WinRealtimeLineChart(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS);
			virtual ~WinRealtimeLineChart();
		};
	}
}
#endif
