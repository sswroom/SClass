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

			NotNullPtr<Media::DrawEngine> eng;
			void *clsData;

		public:
			static OSInt __stdcall RLCWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
		private:
			void Init(InstanceHandle *hInst);
			void Deinit(InstanceHandle *hInst);
		public:
			WinRealtimeLineChart(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS);
			virtual ~WinRealtimeLineChart();
		};
	}
}
#endif
