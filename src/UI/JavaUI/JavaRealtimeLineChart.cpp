#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/JavaUI/JavaRealtimeLineChart.h"

UI::JavaUI::JavaRealtimeLineChart::JavaRealtimeLineChart(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateInterval) : UI::GUIRealtimeLineChart(ui, parent, eng, lineCnt, sampleCnt)
{
}

UI::JavaUI::JavaRealtimeLineChart::~JavaRealtimeLineChart()
{
}
