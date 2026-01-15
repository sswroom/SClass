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

			Optional<Media::ColorSess> colorSess;
			void *clsData;

		public:
			static IntOS __stdcall RLCWndProc(void *hWnd, UInt32 msg, UInt32 wParam, IntOS lParam);
		private:
			void Init(Optional<InstanceHandle> hInst);
			void Deinit(Optional<InstanceHandle> hInst);
		public:
			WinRealtimeLineChart(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Optional<Media::ColorSess> colorSess, UIntOS lineCnt, UIntOS sampleCnt, UInt32 updateIntervalMS);
			virtual ~WinRealtimeLineChart();
		};
	}
}
#endif
