#ifndef _SM_UI_JAVA_JAVAREALTIMELINECHART
#define _SM_UI_JAVA_JAVAREALTIMELINECHART
#include "UI/GUIRealtimeLineChart.h"

namespace UI
{
	namespace Java
	{
		class JavaRealtimeLineChart : public GUIRealtimeLineChart
		{
		public:
			JavaRealtimeLineChart(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS);
			virtual ~JavaRealtimeLineChart();
		};
	}
}
#endif
