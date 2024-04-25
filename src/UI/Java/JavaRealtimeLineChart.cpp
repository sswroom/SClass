#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/Java/JavaRealtimeLineChart.h"

UI::Java::JavaRealtimeLineChart::JavaRealtimeLineChart(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateInterval) : UI::GUIRealtimeLineChart(ui, parent, eng, lineCnt, sampleCnt)
{
}

UI::Java::JavaRealtimeLineChart::~JavaRealtimeLineChart()
{
}
