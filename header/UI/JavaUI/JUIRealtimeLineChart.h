#ifndef _SM_UI_JAVAUI_JUIREALTIMELINECHART
#define _SM_UI_JAVAUI_JUIREALTIMELINECHART
#include "UI/GUIRealtimeLineChart.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIRealtimeLineChart : public GUIRealtimeLineChart
		{
		public:
			JUIRealtimeLineChart(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UIntOS lineCnt, UIntOS sampleCnt, UInt32 updateIntervalMS);
			virtual ~JUIRealtimeLineChart();
		};
	}
}
#endif
